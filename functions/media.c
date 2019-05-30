#include "common.h"
#include <stdlib.h>

int do_preview_on(unsigned char *pucBufHead, u16_t len)
{
    system("/etc/init.d/S75camera stop");
    return system("/etc/init.d/rtsp_server start");
}

int text_do_preview_on(char **pucBufHead, u16_t len)
{
    system("/etc/init.d/S75camera stop");
    return system("/etc/init.d/rtsp_server start");
}


int do_preview_off(unsigned char *pucBufHead, u16_t len)
{
    system("/etc/init.d/rtsp_server stop");
    return system("/etc/init.d/S75camera start");
}

int text_do_preview_off(char **pucBufHead, u16_t len)
{
    system("/etc/init.d/rtsp_server stop");
    return system("/etc/init.d/S75camera start");
}




static T_CmdList g_tPreviewOnCmd = {
	.name          = "preview_on",
    .usage         = "[id] preview_on",
    .help_info     = "\tPreview the video via RTSP Server and Stop record video.",
	.cmd_id        = 0x000c,
	.do_cmd        = do_preview_on,
    .do_text_cmd   = text_do_preview_on,
};

static T_CmdList g_tPreviewOffCmd = {
	.name          = "preview_off",
    .usage         = "[id] preview_off",
    .help_info     = "Stop to preview the video and Start to record video.",
	.cmd_id        = 0x000d,
	.do_cmd        = do_preview_off,
    .do_text_cmd   = text_do_preview_off,
};


int register_media_cmd(void)
{
    int ret = 0;
    ret += registerCmd(&g_tPreviewOnCmd);
    ret += registerCmd(&g_tPreviewOffCmd);
    return ret;
}
