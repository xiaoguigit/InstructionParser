#include "common.h"
#include <regex.h>





int StringToInteger(char *p)
{
    int value = 0;
    while (*p != '\0')
    {
        if ((*p >= '0') && (*p <= '9'))
        {
            value = value * 10 + *p - '0';
        }
        p++;
 
    }
    return value;
}



// 文本协议格式：
// [id] cmd $1 $2 ...
// id 与 cmd 直接必须存在空格符
int text_check_data(unsigned char *pucBufHead)
{
    char *p_str = NULL;
    int status;
    int i;
    int cflags = REG_EXTENDED;
    regmatch_t pmatch[1];
    const size_t nmatch =1 ;
    regex_t reg;
    const char * pattern="^\[\[1-9]+]$";
    char tmp_str[128];
    strncpy(tmp_str, (char*)pucBufHead, strlen((char *)pucBufHead));
    p_str = strtok(tmp_str, " ");  // 拆分字符串
    if(p_str){
        regcomp(&reg,pattern,cflags);
        status=regexec(&reg,p_str,nmatch,pmatch,0);
        if(status == REG_NOMATCH){
            return -1;
        }else if(status ==0)
        {
            return 0;
        }
    }

    return -1;
}


void text_parse(void *fd, unsigned char *pucBufHead)
{
    char *p_str = NULL;
    int i = 0;
    st_cmd_frame cmd;
    int ret = 0;
    p_str = strtok((char *)pucBufHead, " ");  // 拆分字符串
    while (p_str != NULL)
    {
        printf("%d: %s\n", i, p_str);
        if(i == 0){
            cmd.frame_id = StringToInteger(p_str);
        }
        if(i == 1){
            strcpy((char *)cmd.cmd_name, (char *)p_str);
        }else{
            strcpy((char *)cmd.cmd_data[i-2], (char *)p_str);
        }
        i++;
        p_str = strtok(NULL, " ");
    }

    cmd.fd = (*(int *)fd);
    cmd.cmd_len = i;
    
    ret = push_cmd_queue(cmd);
    if(ret){
        printf("PushElement error\n");
    }
}


static T_ProtocolSet g_tTEXTProtocol = {
    .name          = "TEXT",
    .check_data    = text_check_data,
    .parse         = text_parse,
};

int text_protocolRegister(void)
{
    printf("text_protocolRegister\n");
    return registerProtocol(&g_tTEXTProtocol);
}