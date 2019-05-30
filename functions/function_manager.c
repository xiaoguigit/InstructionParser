#include "common.h"


static PT_CmdList g_ptCmdListHead;

int registerCmd(PT_CmdList ptCmd)
{
	PT_CmdList ptTmp;

	if (!g_ptCmdListHead)
	{
		g_ptCmdListHead   = ptCmd;
		ptCmd->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptCmdListHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptCmd;
		ptCmd->ptNext = NULL;
	}

	return 0;
}



int do_cmd_list(st_cmd_frame *cmd, unsigned char *reply)
{
    int ret = 0;
	PT_CmdList ptTmp = g_ptCmdListHead;
	while (ptTmp)
	{	
		//if (ptTmp->cmd_id == cmd_id){
        if(strcmp(ptTmp->name, cmd->cmd_name) == 0){
            printf("do %s\n", ptTmp->name);
            if(ptTmp->do_cmd){
                ret = ptTmp->do_cmd(cmd->data, cmd->cmd_len);
                if(ret){
                    return ret;     // 命令执行失败返回
                }
            }

            if(ptTmp->get_reply){
                return ptTmp->get_reply(reply);
            }else{
                return 0;
            }
		}else{
			ptTmp = ptTmp->ptNext;
        }
	}
	return -1;
}


int do_text_cmd(st_cmd_frame *cmd, unsigned char *reply)
{
    int ret = 0;
	PT_CmdList ptTmp = g_ptCmdListHead;
	while (ptTmp)
	{	
        if(strcmp(ptTmp->name, cmd->cmd_name) == 0){
            if(strcmp("help", cmd->cmd_name) == 0){
                text_do_help(cmd);
            }else if(ptTmp->do_text_cmd){
                ret = ptTmp->do_text_cmd((char **)cmd->cmd_data, cmd->cmd_len);
                if(ret){
                    return ret;
                }
            }
            
            if(ptTmp->get_reply){
                return ptTmp->get_reply(reply);
            }else{
                return 0;
            }
        }else{
			ptTmp = ptTmp->ptNext;
        }
	}
	return -1;
}






int text_do_help(st_cmd_frame *cmd)
{
    int i = 0;
    int ret;
    PT_CmdList ptTmp = g_ptCmdListHead;
    char help[256];
    while (ptTmp)
	{	
        printf("%s\t\t%s\n", ptTmp->usage, ptTmp->help_info);
        sprintf(help, "%s\t\t%s\n", ptTmp->usage, ptTmp->help_info);
        ret = op_write(&(cmd->fd), help, strlen(help));
        if(ret < -1)
        {
            return ret;
        }
        i++;
		ptTmp = ptTmp->ptNext;
	}
    return 0;
}





static T_CmdList g_tHelpCmd = {
	.name          = "help",
    .usage         = "[id] help",
    .help_info     = "\tShow help info about cmd system.",
	.cmd_id        = 0x0001,
};

int register_help_cmd(void)
{
    return registerCmd(&g_tHelpCmd);
}


int cmd_register_enter(void)
{
    int ret = 0;
    ret += register_help_cmd();
    ret += register_reboot_cmd();
    ret += register_network_cmd();
    ret += register_media_cmd();
    ret += register_system_cmd();
    return ret;
}