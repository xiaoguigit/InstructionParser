#ifndef __HANDLE_H__
#define __HANDLE_H__

// 指令帧格式，高位在前，低位在后
typedef struct cmd_frame{
    int fd;                 // 客户端fd
    u16_t len;              // 数据长度
    u16_t frame_id;         // 帧序号
    u16_t cmd_id;           // 指令号
    u16_t cmd_len;          // 指令长度
    u32_t device_id;        // 设备ID
    char cmd_name[16];      // 指令名称
    u8_t data[1024];        // 数据缓存区
    char cmd_data[5][64];   // 指令缓存区
}st_cmd_frame;




void handle(void * clisockfd);
int init_cmd_queue(void);
int push_cmd_queue(st_cmd_frame cmd);
int pop_cmd_queue(st_cmd_frame *cmd);
int get_cmd_head(st_cmd_frame *cmd);
int start_cmd_thread(void);
int start_server_thread(void);

#endif  /* __HANDLE_H__ */