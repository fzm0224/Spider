#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spider.h"

typedef struct node_t
{
	url_t url;
	struct node_t * next;
}node_t;

typedef struct
{
	node_t * front;
	node_t * tail;
}queue_t;

//初始化队列
queue_t * queue_init();
//向队列添加节点
void queue_push(queue_t * const que, node_t * const node);
//从队列删除节点
void queue_pop(queue_t * const que);
//判断是否队空
int queue_is_empty(queue_t * const que);
//取出队首节点
node_t * queue_front(queue_t * const que);
//取出队尾节点
node_t * queue_tail(queue_t * const que);
//销毁队列
void queue_destroy(queue_t * const que);

//判断队列中是否存在重复url[]
int queue_is_contain_same(queue_t * const que, char * const pic_url);

#endif
