#ifndef __TEXT_PROTOCOL_H__
#define __TEXT_PROTOCOL_H__


int text_check_data(unsigned char *pucBufHead);
void text_parse(void *fd, unsigned char *pucBufHead);
int text_protocolRegister(void);


#endif  /* __TEXT_PROTOCOL_H__ */