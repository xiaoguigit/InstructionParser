#ifndef __QUEUE_H__
#define __QUEUE_H__

#define QUEUESIZE 100 //定义队列的大小
typedef st_cmd_frame DataType; //定义队列元素类型

typedef struct
{
	DataType data[QUEUESIZE];
	int front; //指向队头的索引，这个所指的空间不存放元素
	int tail; //指向队尾的索引，存放最后一个元素
}T_CircleQueue;

#endif /* __QUEUE_H__ */