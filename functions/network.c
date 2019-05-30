#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <string.h>
#include "common.h"


typedef struct ipinfo{
    unsigned char name[6];
    unsigned char ipaddr[4];
}T_IPInfo;


typedef struct ipinfo_text{
    char name[6];
    char ipaddr[16];
}T_IPInfo_Text;

static T_IPInfo ip_info;
static T_IPInfo_Text ip_info_text;
static int flag = 0;        // =0非文本协议  =1文本协议

void ip_to_hex_str(char* ip, unsigned char *new_ip)
{
    char *ptr;
    int i = 0;
    ptr = strtok(ip, ".");
	while (ptr != NULL) {
        new_ip[i] = atoi(ptr);
		ptr = strtok(NULL, ".");
        i++;
	}
}


/*************************************************************
* 函数功能：	通过正则表达式检测是否为IP地址
* 参数类型：	需要检测的IP地址
* 返回类型：	成功返回0，失败返回-1
**************************************************************/
static int check_right_ip(const char *ip)
{
    char *p;
    int i = 4;
    p = strtok((char *)ip,".");
    while(p != NULL)
	{
        if(atoi(ip) > 255 || atoi(ip) < 0){
            break;
        }
        i--;
        p = strtok(NULL,".");
	}
    
	return (i==0)? 0:-1;
}
 
/*************************************************************
* 函数功能：	获取IP地址
* 参数类型：	IP地址存放位置
* 返回类型：	
**************************************************************/
static char *get_ip(char *device, char *ip)
{
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
	char *temp_ip = NULL;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket");
        return NULL;                
    }
    strncpy(ifr.ifr_name, device, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        return NULL;
    }
 
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	temp_ip = inet_ntoa(sin.sin_addr);
	strcpy(ip,temp_ip);
	return ip;
}
 
/*************************************************************
* 函数功能：	手动设置IP地址
* 参数类型：	要设置的IP地址
* 返回类型：	成功返回0，失败返回-1
**************************************************************/
static int set_hand_ip(char *device, const char *ipaddr)
{
	int sock_set_ip;     
    struct sockaddr_in sin_set_ip;     
    struct ifreq ifr_set_ip;     
 
    memset( &ifr_set_ip,0,sizeof(ifr_set_ip));     
    if( ipaddr == NULL )     
    {
        return -ERR_PARAMS;     
    }
   
	sock_set_ip = socket( AF_INET, SOCK_STREAM, 0 );
	//printf("sock_set_ip=====%d\n",sock_set_ip);
    if(sock_set_ip<0)  
    {     
        perror("socket create failse...SetLocalIp!");     
        return -1;     
    }     
 
    memset( &sin_set_ip, 0, sizeof(sin_set_ip));     
    strncpy(ifr_set_ip.ifr_name, device, sizeof(ifr_set_ip.ifr_name)-1);        
 
    sin_set_ip.sin_family = AF_INET;     
    sin_set_ip.sin_addr.s_addr = inet_addr(ipaddr);     
    memcpy( &ifr_set_ip.ifr_addr, &sin_set_ip, sizeof(sin_set_ip));     
    if( ioctl( sock_set_ip, SIOCSIFADDR, &ifr_set_ip) < 0 )     
    {     
        perror( "Not setup interface");     
        return -ERR_PARAMS;     
    }     
 
    //设置激活标志      
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;      
 
    //get the status of the device      
    if( ioctl( sock_set_ip, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )     
    {     
         perror("SIOCSIFFLAGS");     
         return -1;     
    }     
    
    close( sock_set_ip );     
	return 0;
}



int get_device_ip(char *device, char *ip_buff)
{
    if(get_ip(device, ip_buff) == NULL){
        return -NO_DEV;
    }
    return 0;
}

int set_device_ip(char *device, char *ip_addr)
{
    char *ip = malloc(strlen(ip_addr));
    if(ip == NULL){
        printf("no memory");
        return -NO_MEM;
    }
    strncpy(ip, ip_addr, strlen(ip_addr));
    if(check_right_ip(ip_addr)){
        return -ERR_PARAMS;
    }
    return set_hand_ip(device, ip);
}

/* pucBufHead 前6个字节是网卡设备 */
int do_get_device_ip(unsigned char *pucBufHead, u16_t len)
{
    int ret;
    int str_len;
    char ip[32] = {0};
    if(len > 6){
        str_len = 6;
    }else{
        str_len = len;
    }
    
    memset(&ip_info, 0, sizeof(ip_info));
    strncpy((char *)&(ip_info.name), (char *)pucBufHead, str_len);

    ret = get_device_ip((char *)&(ip_info.name), ip);
    if(ret){
        return ret;
    }
    ip_to_hex_str(ip, ip_info.ipaddr);
    flag = 0;
    return 0;
}


// 用于文本协议
int do_text_get_device_ip(char **pucBufHead, u16_t len)
{
    int ret;
    char (*p)[64] = (char (*)[64])pucBufHead;

    memset(&ip_info_text, 0, sizeof(ip_info_text));
    strncpy(ip_info_text.name, p[0], strlen(p[0]));
    ret = get_device_ip(ip_info_text.name, ip_info_text.ipaddr);
    if(ret){
        return ret;
    }

    flag = 1;
    return 0;
}




int get_ip_reply(unsigned char *reply)
{
    if(flag){
        sprintf((char *)reply, "%s:%s", ip_info_text.name, ip_info_text.ipaddr);
        return strlen((char *)reply);
    }else{
        memcpy(reply, &ip_info, sizeof(T_IPInfo));
        return sizeof(T_IPInfo);
    }
}

/* pucBufHead 前6个字节是网卡设备, 后面是ip地址 */
int do_set_device_ip(unsigned char *pucBufHead, u16_t len)
{
    int str_len;
    if(len > 6){
        str_len = 6;
    }else{
        str_len = len;
    }
    memset(&ip_info, 0, sizeof(ip_info));
    strncpy((char *)&(ip_info.name), (char *)pucBufHead, str_len);
    strncpy((char *)&(ip_info.ipaddr), (char *)pucBufHead + str_len, len - str_len);
    return set_device_ip((char *)ip_info.name, (char *)ip_info.ipaddr);
}


int do_text_set_device_ip(char **pucBufHead, u16_t len)
{
    T_IPInfo_Text ip_info_text;
    memset(&ip_info_text, 0, sizeof(ip_info_text));
    char (*p)[64] = (char (*)[64])pucBufHead;
    strncpy(ip_info_text.name, p[0], strlen(p[0]));
    strncpy(ip_info_text.ipaddr, p[1], strlen(p[1]));
    return set_device_ip(ip_info_text.name, ip_info_text.ipaddr);
}



static T_CmdList g_tGetIPCmd = {
	.name          = "get_ip",
    .usage         = "[id] get_ip dev",
    .help_info     = "\tTo get dev ip addr. Which dev is eth0/ra0.",
	.cmd_id        = 0x000a,
	.do_cmd        = do_get_device_ip,
    .do_text_cmd   = do_text_get_device_ip,
    .get_reply     = get_ip_reply,
};



static T_CmdList g_tSetIPCmd = {
	.name          = "set_ip",
    .usage         = "[id] set_ip dev ipaddr",
    .help_info     = "To set dev ip addr. Which dev is eth0/ra0.",
	.cmd_id        = 0x000b,
	.do_cmd        = do_set_device_ip,
    .do_text_cmd   = do_text_set_device_ip,
};


int register_network_cmd(void)
{
    int ret = 0;
    ret += registerCmd(&g_tGetIPCmd);
    ret += registerCmd(&g_tSetIPCmd);
    
    return ret;
}


