#include "common.h"


static struct cmd_map{
    u16_t cmd_id;
    char cmd_name[16];
};

static struct cmd_map cmd_list[] = {
    {0x0100, "sys_info"},
    {0x0101, "set_date"},
    {0x0102, "set_wifi"},
    {0x0103, "add_user"},
    {0x0104, "set_video_info"},
    {0x0105, "set_brightness"},
    {0x0106, "get_electricity"},
    {0x0107, "power_off"},
    {0x0108, "reboot"},
    {0x0109, "get_ip"},
    {0x010a, "set_ip"},
    {0x0200, "start_record"},
    {0x0201, "stop_record"},
    {0x0202, "preview_on"},
    {0x0203, "preview_off"},
};

static char *get_cmd_name(u16_t cmd_id)
{
    int i = 0;
    for(i = 0; i < sizeof(cmd_list)/sizeof(cmd_list[0]); i++){
        if(cmd_list[i].cmd_id == cmd_id){
            return cmd_list[i].cmd_name;
        }
    }
    return NULL;
}

int sl_check_data(unsigned char *pucBufHead)
{
    u16_t crc;
    u16_t data_len = 0;
    u16_t data_crc = 0;
    if(pucBufHead[0] != 0x22 || pucBufHead[1] != 0x53){
        logger_error("cmd frame head mismatch.\n");
        return -1;
    }
    data_len = pucBufHead[2] << 8 | pucBufHead[3];
    // 数据校验位
    data_crc = pucBufHead[data_len] << 8 | pucBufHead[data_len + 1];
    // 计算校验位
    crc = uhf_crc16_ccitt_tab(pucBufHead, data_len);
    // 比较校验结果
    if(crc != data_crc){
        logger_error("cmd crc check error.\n");
        return -1;
    }
    
    return 0;
}


void sl_parse(void *fd, unsigned char *pucBufHead)
{
    int ret = 0;
    char *cmd_name = NULL;
    st_cmd_frame cmd;
    cmd.fd = (*(int *)fd);
    cmd.len = pucBufHead[2] << 8 | pucBufHead[3];
    cmd.frame_id = pucBufHead[4] << 8 | pucBufHead[5];
    cmd.cmd_id = pucBufHead[6] << 8 | pucBufHead[7];
    cmd_name = get_cmd_name(cmd.cmd_id);
    if(cmd_name != NULL){
        strcpy(cmd.cmd_name, cmd_name);
    }
    printf("push %s\n", cmd.cmd_name);
    cmd.device_id = pucBufHead[9] << 24 | pucBufHead[10] << 16 | pucBufHead[11] << 8 | pucBufHead[12];
    cmd.cmd_len = cmd.len - 13; // 指令内容的长度 = 数据长度 - 帧序号2字节 - 命令号2字节 - 长度2字节 - 校验2字节 - 方向1字节 - 设备编号4字节
    printf("cmd.len = %d \n", cmd.len);
    memcpy(cmd.data, pucBufHead + 13, cmd.cmd_len);
    printf("sl_parse done\n");
    ret = push_cmd_queue(cmd);
    if(ret){
        printf("PushElement error\n");
    }
}





static T_ProtocolSet g_tSLProtocol = {
	.name          = "SL",
    .check_data    = sl_check_data,
    .parse         = sl_parse,
};

int sl_protocolRegister(void)
{
    printf("sl_protocolRegister\n");
    return registerProtocol(&g_tSLProtocol);
}


