#include "common.h"
#include <sys/reboot.h>

int reboot_system(unsigned char *pucBufHead, u16_t len)
{
    logger_info("Get reboot cmd. Restart the system Now !\n");
    sync(); 
    return reboot(RB_AUTOBOOT);
}

int text_reboot_system(char **pucBufHead, u16_t len)
{
    logger_info("Get reboot cmd. Restart the system Now !\n");
    sync(); 
    return reboot(RB_AUTOBOOT);
}




static T_CmdList g_tRebootCmd = {
	.name          = "reboot",
    .usage         = "[id] reboot",
    .help_info     = "\tWill reboot the system with no response.",
	.cmd_id        = 0x0009,
	.do_cmd        = reboot_system,
    .do_text_cmd   = text_reboot_system,
};

int register_reboot_cmd(void)
{
    return registerCmd(&g_tRebootCmd);
}
