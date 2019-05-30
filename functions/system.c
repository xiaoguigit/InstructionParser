#include "common.h"
#include <time.h>
#include <sys/time.h>

static struct timeval nowTus;
static struct tm nowTm2;
static int mode = 0;
static int result = 0;

int set_system_date(unsigned char *pucBufHead, u16_t len)
{
    u16_t year;
    u8_t mon;
    u8_t day;
    u8_t hour;
    u8_t min;
    u8_t sec;
    //u32_t usec;
    char cmd[64] = {0};
    int ret = 0;
    
    mode = 0;
    
    // 查询时间
    if(pucBufHead[0] != 1){
        gettimeofday( &nowTus, NULL ); 
        localtime_r( &nowTus.tv_sec, &nowTm2 );
    }else{      // 设置时间
        mode = 1;
        year = pucBufHead[1] << 8 | pucBufHead[2];
        mon =  pucBufHead[3];
        day =  pucBufHead[4];
        hour =  pucBufHead[5];
        min =  pucBufHead[6];
        sec =  pucBufHead[7];
        //usec =  pucBufHead[8] << 24 |  pucBufHead[8] << 16 | pucBufHead[8] << 8 | pucBufHead[8];
        sprintf(cmd, "hisi_hwclock -s time %d/%d/%d/%d/%d/%d", year, mon, day, hour, min, sec);
        ret = system(cmd);
        if(ret == 0){
            memset(cmd, 0, sizeof(cmd));
            sprintf(cmd, "date -s \"%d-%d-%d %d:%d:%d\"", year, mon, day, hour, min, sec);
            ret = system(cmd);
            return ret;
        }
    }
    return 0;
}

int text_set_system_date(char **pucBufHead, u16_t len)
{
    logger_info("set system date.\n");
    return 0;
}


int set_date_reply(unsigned char *reply)
{
    if(mode == 0){
        reply[0] = (unsigned char)((nowTm2.tm_year + 1900)>>8);
        reply[1] = (unsigned char)(nowTm2.tm_year + 1900);
        reply[2] = (unsigned char)(nowTm2.tm_mon + 1);
        reply[3] = (unsigned char)(nowTm2.tm_mday);
        reply[4] = (unsigned char)(nowTm2.tm_hour);
        reply[5] = (unsigned char)(nowTm2.tm_min);
        reply[6] = (unsigned char)(nowTm2.tm_sec);
        reply[7] = (unsigned char)(nowTus.tv_usec >> 24);
        reply[8] = (unsigned char)(nowTus.tv_usec >> 16);
        reply[9] = (unsigned char)(nowTus.tv_usec >> 8);
        reply[10] = (unsigned char)(nowTus.tv_usec);
        return 11;
    }else{
        reply[0] = 0x0;
        return 1;
    }
}


static T_CmdList g_tSetDateCmd = {
	.name          = "set_date",
    .usage         = "[id] set_date 2019-12-12 09:08:07",
    .help_info     = "\tUpdate time info to system.",
	.cmd_id        = 0x0101,
	.do_cmd        = set_system_date,
    .do_text_cmd   = text_set_system_date,
    .get_reply     = set_date_reply,
};



int add_system_user(unsigned char *pucBufHead, u16_t len)
{
    char user_name[64] = {0};
    char user_passwd[64] = {0};
    char cmd[128] = {0};
    int user_name_len = 0;
    int user_passwd_len = 0;
    int ret = 0;
    // 查询账号
    if(pucBufHead[0] != 1){
;
    }else{      // 添加账号

        user_name_len = *(pucBufHead+1);
        user_passwd_len = *(pucBufHead + 2 + user_name_len);
        strncpy(user_name, (const char *)pucBufHead+2, user_name_len);
        strncpy(user_passwd, (const char *)pucBufHead + user_name_len + 3, user_passwd_len);
        printf("user_name : %s\n", user_name);
        printf("user_passwd : %s\n", user_passwd);
        sprintf(cmd, "/usr/sbin/add_user %s %s", user_name, user_passwd);
        ret = system(cmd);
        if(ret){
            result = 1;
        }
    }
    return 0;
}


int text_add_system_user(char **pucBufHead, u16_t len)
{
    logger_info("set system date.\n");
    return 0;
}

int add_system_user_reply(unsigned char *reply)
{
    if(result == 1){
        result = 0;
        reply[0] = 0x1;
        return 1;
    }else{
        reply[0] = 0x0;
        return 1;
    }
}



int get_system_info(unsigned char *pucBufHead, u16_t len)
{
    return 0;
}

int text_get_system_info(char **pucBufHead, u16_t len)
{
    logger_info("set system date.\n");
    return 0;
}

int get_system_info_reply(unsigned char *reply)
{
    reply[0] = 0x01;
    reply[1] = 0x02;
    reply[2] = 0x03;
    reply[3] = 0x04;
    
    reply[4] = 0x01;
    reply[5] = 0x01;
    
    //07 e2 08 03 03 17 10 00 09 9a 43
    reply[6] = 0x07;
    reply[7] = 0xe2;
    reply[8] = 0x08;
    reply[9] = 0x03;
    reply[10] = 0x03;
    reply[11] = 0x17;
    reply[12] = 0x10;
    reply[13] = 0x00;
    reply[14] = 0x09;
    reply[15] = 0x9a;
    reply[16] = 0x43;
    
    // 电量
    reply[17] = 0x36;
    
    // 亮度
    reply[18] = 0x03;
    
    // 总容量
    reply[19] = 0x17;
    reply[20] = 0x10;
    
    // 剩余容量
    reply[21] = 0x10;
    reply[22] = 0x08;
    
    // 工作状态
    reply[23] = 0x0;
    
    // 设备名称
    // 63 61 70 73 75 6C 65
    reply[24] = 0x7;
    reply[25] = 0x63;
    reply[26] = 0x61;
    reply[27] = 0x70;
    reply[28] = 0x73;
    reply[29] = 0x75;
    reply[30] = 0x6c;
    reply[31] = 0x65;
    
    return 32;
}



static T_CmdList g_tAddUserCmd = {
	.name          = "add_user",
    .usage         = "[id] add_user name passwd ",
    .help_info     = "\tAdd System User.",
	.cmd_id        = 0x0103,
	.do_cmd        = add_system_user,
    .do_text_cmd   = text_add_system_user,
    .get_reply     = add_system_user_reply,
};



static T_CmdList g_tSysInfoCmd = {
	.name          = "sys_info",
    .usage         = "[id] sys_info",
    .help_info     = "\trequest system informations.",
	.cmd_id        = 0x0100,
	.do_cmd        = get_system_info,
    .do_text_cmd   = text_get_system_info,
    .get_reply     = get_system_info_reply,
};


int register_system_cmd(void)
{
    int ret = 0;
    ret += registerCmd(&g_tSetDateCmd);
    ret += registerCmd(&g_tAddUserCmd);
    ret += registerCmd(&g_tSysInfoCmd);
    return ret;
}
