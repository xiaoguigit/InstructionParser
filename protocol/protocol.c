#include "common.h"


static PT_ProtocolSet ptProtocolSupportedHead = NULL;




PT_ProtocolSet do_protocol_handle(unsigned char *pucBufHead)
{
    PT_ProtocolSet ptTmp = ptProtocolSupportedHead;

	while (ptTmp)
	{	
		if (ptTmp->check_data(pucBufHead) == 0)
			return ptTmp;
		else
			ptTmp = ptTmp->ptNext;
	}
	return NULL;
}


int registerProtocol(PT_ProtocolSet ptProtocol)
{
	PT_ProtocolSet ptTmp;

	if (!ptProtocolSupportedHead)
	{
		ptProtocolSupportedHead   = ptProtocol;
		ptProtocol->ptNext = NULL;
	}
	else
	{
		ptTmp = ptProtocolSupportedHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	      = ptProtocol;
		ptProtocol->ptNext = NULL;
	}

	return 0;
}