#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

typedef struct ProtocolSet {
	char *name;
    int (*check_data)(unsigned char *pucBufHead);
    void (*parse)(void* fd, unsigned char *pucBufHead);
	struct ProtocolSet *ptNext;
}T_ProtocolSet, *PT_ProtocolSet;


PT_ProtocolSet do_protocol_handle(unsigned char *pucBufHead);
int registerProtocol(PT_ProtocolSet ptProtocol);


#endif