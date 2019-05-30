#include "common.h"

static T_CircleQueue cmd_circle_queue;


static int init_circle_queue(T_CircleQueue *pCQ)
{
	pCQ = (T_CircleQueue *)malloc(sizeof(T_CircleQueue));
	if (pCQ == NULL)
		return -1;
	else
	{
		pCQ->front = 0;
		pCQ->tail = 0;
		return 0;
	}
}
 
static int is_queue_empty(T_CircleQueue *pCQ)
{
	if (pCQ->front == pCQ->tail)
		return -1;
	else
		return 0;
}
 
static int is_queue_full(T_CircleQueue *pCQ)
{
	if ((pCQ->tail + 1) % QUEUESIZE == pCQ->front)
		return -1;
	else
		return 0;
}
 
static int push_element(T_CircleQueue *pCQ, DataType dData)
{
	if (is_queue_full(pCQ))
		return -1;
 
	pCQ->tail = (pCQ->tail + 1) % QUEUESIZE;
	pCQ->data[pCQ->tail] = dData;
	return 0;
}
 
static int pop_element(T_CircleQueue *pCQ, DataType *pData)
{
	if (is_queue_empty(pCQ))
		return -1;
 
	pCQ->front = (pCQ->front + 1) % QUEUESIZE;
	*pData = pCQ->data[pCQ->front];
	return 0;
}
 
static int get_head_element(T_CircleQueue *pCQ, DataType *pData)
{
	if (is_queue_empty(pCQ))
		return -1;
 
	*pData = pCQ->data[(pCQ->front + 1) % QUEUESIZE];
	return 0;
}

int init_cmd_queue(void)
{
    return init_circle_queue(&cmd_circle_queue);
}

int push_cmd_queue(st_cmd_frame cmd)
{
    return push_element(&cmd_circle_queue, cmd);
}

int pop_cmd_queue(st_cmd_frame *cmd)
{
    return pop_element(&cmd_circle_queue, cmd);
}

int get_cmd_head(st_cmd_frame *cmd)
{
    return get_head_element(&cmd_circle_queue, cmd);
}
