#ifndef __HT_PROTOCOL_H__
#define __HT_PROTOCOL_H__

#define FRAME_HEAD_H    0x48
#define FRAME_HEAD_L    0x54



int ht_check_data(unsigned char *pucBufHead);
void ht_parse(void *fd, unsigned char *pucBufHead);
int ht_protocolRegister(void);


#endif  /* __HT_PROTOCOL_H__ */