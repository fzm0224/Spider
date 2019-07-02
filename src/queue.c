#include "queue.h"

//初始化队列
queue_t * queue_init()
{
	queue_t * que = (queue_t *)malloc(sizeof(queue_t));
	if (que == NULL)
	{
		perror("error: In queue_init(), malloc error, que == NULL!");
		exit(-1);
	}

	//申请头节点的空间
	que->front = (node_t *)malloc(sizeof(node_t));
	if (que->front == NULL)
	{
		perror("error: In queue_init(), malloc error, que->front == NULL!");
		exit(-1);
	}

	que->front->next = NULL;
	que->tail = que->front;

	return que;
}

//向队列添加节点
void queue_push(queue_t * const que, node_t * const node)
{
	if (que == NULL || node == NULL)
	{
		perror("error: In queue_push(), parameter error, que == NULL || node == NULL!");
		exit(-1);
	}

	que->tail->next = node;
	node->next = NULL;

	que->tail = node;
	return ;
}

//从队列删除节点
void queue_pop(queue_t * const que)
{
	if (que == NULL)
	{
		perror("error: In queue_pop(), parameter error, que == NULL!");
		exit(-1);
	}

	if (queue_is_empty(que) == 1)
	{
		printf("queue is empty!\n");
		return ;
	}

	//剩下最后一个节点
	if (que->front->next == que->tail)
	{
		que->tail = que->front;
	}

	node_t * node = que->front->next;
	que->front->next = node->next;

	free(node);
	return ;
}

//判断是否队空
int queue_is_empty(queue_t * const que)
{
	if (que == NULL)
	{
		perror("error: In queue_is_empty(), parameter error, que == NULL!");
		exit(-1);
	}
	if (que->tail == que->front)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//取出队首节点
node_t * queue_front(queue_t * const que)
{
	if (que == NULL)
	{
		perror("error: In queue_front(), parameter error, que == NULL!");
		exit(-1);
	}
	return que->front->next;
}

//取出队尾节点
node_t * queue_tail(queue_t * const que)
{
	if (que == NULL)
	{
		perror("error: In queue_tail(), parameter error, que == NULL!");
		exit(-1);
	}
	return que->tail;
}

//销毁队列
void queue_destroy(queue_t * que)
{
	if (que == NULL)
	{
		perror("error: In queue_destroy(), parameter error, que == NULL!");
		exit(-1);
	}
	
	while (!queue_is_empty(que))
	{
		queue_pop(que);
	}

	free(que->front);
	que->front = NULL;
	que->tail = NULL;
	free(que);
	que = NULL;
	return ;
}

//判断队列中是否存在重复url[]
int queue_is_contain_same(queue_t * const que, char * const pic_url)
{
	if (que == NULL || pic_url == NULL)
	{
		perror("error: In queue_is_contain_same(), parameter error, que == NULL || pic_url == NULL!");
		exit(-1);
	}

	node_t * node = NULL;
	int len_pic = strlen(pic_url);
	int len_tmp = 0;

	if (!queue_is_empty(que))
	{
		node = queue_front(que);
	}

	while (node != NULL)
	{
		len_tmp = strlen(node->url.url);
		if (strcmp(node->url.url, pic_url) == 0)
		{
			return 1;
		}
		else if (len_pic > len_tmp)
		{
			if (((len_pic - len_tmp) == 1) && (strstr(pic_url, node->url.url) != NULL) && (pic_url[len_pic - 1] == '/'))
			{
				return 1;
			}
		}
		else if (len_pic < len_tmp)
		{
			if (((len_tmp - len_pic) == 1) && (strstr(node->url.url, pic_url) != NULL) && (node->url.url[len_tmp - 1] == '/'))
			{
				return 1;
			}
		}
		else {}

		node = node->next;
	}

	return 0;
}