#include "common.h"

int logger_init(char *configure)
{
    int ret;
    ret = zlog_init(configure);
    if(ret)
    {
        printf("init failed\n");
        return -1;
    }
    category = zlog_get_category("cmd");
    if(!category)
    {
        printf("get category fail\n");
        zlog_fini();
        return -2;
    }
    return 0;
}


void logger_error(const char *format, ...)
{
    va_list args;
    va_start(args,format);
    zlog_error(category, format, args);
    va_end(args);
}

void logger_info(const char *format, ...)
{
    va_list args;
    va_start(args,format);
    zlog_info(category, format, args);
    va_end(args);
}

void logger_debug(const char *format, ...)
{
    va_list args;
    va_start(args,format);
    zlog_debug(category, format, args);
    va_end(args);
}


void logger_exit(void)
{
    zlog_fini();
}