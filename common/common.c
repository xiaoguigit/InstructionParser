#include "common.h"


/**
* @brief 查表方式获取校验值
* @param[in] *buf 待计算数据缓冲区
* @param[in] len 待计算数据长度
* @return u16_t 计算结果
*/
u16_t uhf_crc16_ccitt_tab (const u8_t *buf, u32_t len)
{
    u32_t i = 0;
    u16_t cksum = 0;
    cksum = 0;
    for (i = 0; i < len; i++) {
        cksum = crc16_tab[((cksum>>8) ^ *buf++) & 0xFF] ^ (cksum << 8);
    }
    return cksum;
}

/**
* @brief 写入配置文件，记录相关信息
* @param[in] *fileneme 配置文件完整路径名称
* @param[in] atrribute 属性名字
* @param[in] value 属性值
* @return int 返回结果
*/
int config_file(const char * fileneme, char* atrribute, char *value)
{
	char linebuffer[512] = {0};
	char buffer1[512] = {0};
	char buffer2[512] = {0};
	char clear[512] = {0};
	int line_len = 0;
	int len = 0;
	int res;

	FILE *fp = fopen(fileneme, "r+");
	if(fp == NULL)
	{
		printf("open error");
		return -1;
	}
	while(fgets(linebuffer, 512, fp))
	{
		line_len = strlen(linebuffer);
		len += line_len;
		sscanf(linebuffer, "%[^=]=%[^=]", buffer1,buffer2);
		if(!strcmp(atrribute, buffer1))
		{
			len -= strlen(linebuffer);
			res = fseek(fp, len, SEEK_SET);
			if(res < 0)
			{
				perror("fseek");
				return -1;
			}

			/* 清除原有数据 */
			fwrite(clear, line_len, 1, fp);
			res = fseek(fp, len, SEEK_SET);
			if(res < 0)
			{
				perror("fseek");
				return -1;
			}
			fprintf(fp, "%s=%s\n", buffer1, value);
			fclose(fp);
			return 0;
		}
	}
	/* 如果没有该属性 */ 
	res = fseek(fp, len, SEEK_SET);
	if(res < 0)
	{
		perror("fseek");
		return -1;
	}
	fprintf(fp, "%s=%s\n", atrribute,value);
	fclose(fp);
	return 0;
}


/**
* @brief 执行shell命令，获取控制台输出
* @param[in] *cmd 命令
* @param[in] *result 输出结果
* @return int 返回结果
*/
int shell_cmd(char* cmd, char* result)
{
    char buffer[10240];
    FILE* pipe = popen(cmd, "r");
    if (!pipe)
    return -1;
    while(!feof(pipe)) {
        if(fgets(buffer, 4096, pipe)){
            strcat(result, buffer);
        }
    }
    pclose(pipe);
    return 0;
}

