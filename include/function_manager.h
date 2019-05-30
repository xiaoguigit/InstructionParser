#ifndef __FUNCTION_MANAGER_H__
#define __FUNCTION_MANAGER_H__

#include "media.h"


typedef struct cmdList {
	char *name;
    char *usage;
    char *help_info;
	u16_t cmd_id;
    int (*do_cmd)(unsigned char *pucBufHead, u16_t len);
    int (*do_text_cmd)(char **pucBufHead, u16_t len);
    int (*get_reply)(unsigned char *reply);
	struct cmdList *ptNext;
}T_CmdList, *PT_CmdList;

int do_cmd_list(st_cmd_frame *cmd, unsigned char *reply);
int do_text_cmd(st_cmd_frame *cmd, unsigned char *reply);
int registerCmd(PT_CmdList ptCmd);
int cmd_register_enter(void);
int register_reboot_cmd(void);
int register_network_cmd(void);
int register_help_cmd(void);
int register_system_cmd(void);
int text_do_help(st_cmd_frame *cmd);


#endif  /* __FUNCTION_MANAGER_H__ */