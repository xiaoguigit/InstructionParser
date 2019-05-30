#include "common.h"

extern clilist_t clilist;
static pthread_t thrd_server_id;    // 指令服务器线程ID
static pthread_t thrd_cmd_id;       // 指令监听线程ID


/* 协议处理 */
void handle(void * clisockfd)
{	
    PT_ProtocolSet protocol = NULL;
	unsigned char line[255] = {0};
	int n = op_read(clisockfd, line, 255);

	if(n<-1){
		return;
	}else if(n > 0){
        protocol = do_protocol_handle(line);
        if(protocol){
            printf("use protocol : %s\n", protocol->name);
            protocol->parse(clisockfd, line);
        }else{
            printf("NO PROTOCOL SUPPORT\n");
        }
    }
    
#if 0	
	n = op_write(clisockfd, line, strlen(line));
	if(n < -1)
	{
		return;
	}	
	memset(line, 0, 255);

	for(i=0; i<clilist.tailindex; i++)
	{
		op_write(&clilist.clilist[i].fd, "helloworld", strlen("helloworld"));
	}
	printf("%d\n", clilist.tailindex);
#endif
}



int do_response(st_cmd_frame *cmd, unsigned char *reply, int len)
{
    int ret = 0;
    u16_t crc = 0;
    int buf_len = 13 + len;
    unsigned char *buf = malloc(sizeof(unsigned char) * buf_len);
    if(NULL == buf){
        logger_error("No memory\n");
        return -1;
    }
    buf[0] = 0x22;      // 帧头
    buf[1] = 0x53;      // 帧头
    buf[2] = ((buf_len - 2) & 0xff00) >> 8;     // 数据长度
    buf[3] = ((buf_len - 2) & 0x00ff);
    buf[4] = (cmd->frame_id & 0xff00) >> 8;     // 帧序号
    buf[5] = ((cmd->frame_id) & 0x00ff);
    
    buf[6] = (cmd->cmd_id & 0xff00) >> 8;       // 指令ID
    buf[7] = ((cmd->cmd_id) & 0x00ff);
    buf[8] = 0xff;                              // 方向
    buf[9] = (cmd->device_id & 0xff00) >> 8;    // 设备ID
    buf[10] = ((cmd->device_id) & 0x00ff);
    memcpy(&buf[11], reply, len);                // 数据
    crc = uhf_crc16_ccitt_tab(buf, 11+len);      // 计算校验
    buf[11+len] = (crc & 0xff00) >> 8;           // crc校验
    buf[12+len] = (crc & 0x00ff);
    printf("llllen : %d\n", buf_len);
    ret = op_write(&(cmd->fd), buf, buf_len);
	if(ret < -1)
	{
		return ret;
	}	
    free(buf);
    return 0;
}



int do_text_response(st_cmd_frame *cmd, unsigned char *reply, int len)
{
    int ret = 0;
    char respone[512];
    sprintf(respone, "[%d] %s %s  ...done.\n", cmd->frame_id, cmd->cmd_name, reply);
    ret = op_write(&(cmd->fd), respone, strlen(respone));
	if(ret < -1)
	{
		return ret;
	}	
    return 0;
}


int do_text_response_error(st_cmd_frame *cmd, int code)
{
    int ret = 0;
    char respone[128];
    sprintf(respone, "[%d] ERROR [%d]\n", cmd->frame_id, code);
    ret = op_write(&(cmd->fd), respone, strlen(respone));
	if(ret < -1)
	{
		return ret;
	}	
    return 0;
}

 

/* 查询指令缓存队列，并提取指令 */
void *get_cmd(void *arg)
{
    st_cmd_frame cmd;
    unsigned char reply[512];
    int ret = 0;
    while(1){
        if(pop_cmd_queue(&cmd) == 0){
            // 一般协议必须满足cmd_id != 0
            if(cmd.cmd_id != 0 ){
                ret = do_cmd_list(&cmd, reply);
                if(ret < 0){
                    printf("do_cmd_list error\n");
                    continue;
                }
                // 大于0表示需要回复消息
                if(ret > 0){
                    do_response(&cmd, reply, ret);
                }
            }else{  // 否则当文本协议处理
                printf("do text cmd : %s\n", cmd.cmd_name);
                printf("do text cmd : %d\n", cmd.cmd_len);
                printf("do text cmd : %s\n", cmd.cmd_data[0]);
                ret = do_text_cmd(&cmd, reply);
                if(ret < 0){
                    printf("do_text_cmd error [%d]\n", ret);
                    do_text_response_error(&cmd, ret);
                    continue;
                }
                if(ret >= 0){
                    do_text_response(&cmd, reply, ret);
                }
            }
        }
        usleep(1);
    }
    
}



int start_cmd_thread(void)
{
    int ret = 0;
    // 初始化指令缓存队列
    if(init_cmd_queue()){   
        printf("init_cmd_queue error\n");
        return -1;
    }
    
    // 创建指令监听线程
    ret = pthread_create(&thrd_cmd_id,NULL,get_cmd,NULL);
    if(ret != 0)
    {
        printf("get_cmd pthread_create error");
        return ret;
    }
    return ret;
}



void *server_start(void *arg)
{
    int ret = 0;
    // 创建指令服务器
    ret = start_server("0.0.0.0", 8001);
    if(ret){
        printf("start_server faild\n");
    }
    return (void*)0;
}


int start_server_thread(void)
{
    int ret = 0;
    
    // 创建指令服务器线程
    ret = pthread_create(&thrd_server_id,NULL,server_start,NULL);
    if(ret != 0)
    {
        printf("thrd_server_id pthread_create error");
        return ret;
    }
    return ret;
}


