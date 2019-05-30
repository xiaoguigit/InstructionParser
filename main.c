#include <stdio.h>
#include <unistd.h>
#include "common.h"


int main(int argc, char** argv)
{
    int ret;
    // 记录版本信息
    ret = config_file("/etc/os-release", "CMD_SERVER_VER", "V1.0.00");
    if(ret){
        return ret;
    }
    // 初始化日志系统
    ret = logger_init("zlog.conf");
    if(ret){
        return ret;
    }
    // 启动服务器
    start_server_thread();
    
    //启动协议解析器
    start_cmd_thread();

    // 注册协议
    ret = ht_protocolRegister();
    if(ret){
        printf("ht_protocolRegister error \n");
    }
    
    ret = sl_protocolRegister();
    if(ret){
        printf("sl_protocolRegister error \n");
    }
    
    ret = text_protocolRegister();
    if(ret){
        printf("text_protocolRegister error \n");
    }
    
    // 注册指令
    ret = cmd_register_enter();
    if(ret){
        printf("cmd_register_enter error\n");
    }

    
    while(1){
        sleep(1);
    }
        
    logger_exit();
    return 0;
}


