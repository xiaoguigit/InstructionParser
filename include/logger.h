#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include "zlog.h"




zlog_category_t *category;
int logger_init(char *configure);
void logger_error(const char *format, ...);
void logger_info(const char *format, ...);
void logger_debug(const char *format, ...);
void logger_exit(void);



#endif /* __LOGGER_H__ */