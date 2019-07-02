#include "queue.h"
#include "spider.h"
#include "regex_url.h"
#include "all_define.h"


//测试队列函数。
void test_queue()
{
	queue_t * que = queue_init();

	node_t * node1 = (node_t *)malloc(sizeof(node_t));
	node_t * node2 = (node_t *)malloc(sizeof(node_t));
	node_t * node3 = (node_t *)malloc(sizeof(node_t));
	node_t * node4 = (node_t *)malloc(sizeof(node_t));

	strcpy(node1->url.url, "aaaaa");
	node1->next = node2;
	strcpy(node2->url.url, "bbbbb");
	node2->next = node3;
	strcpy(node3->url.url, "ccccc");
	node3->next = node4;
	strcpy(node4->url.url, "ddddd");
	node4->next = NULL;

	que->front->next = node1;
	que->tail = node4;

	node_t * tmp = que->front->next;
	while (tmp != NULL)
	{
		printf("%s\n", tmp->url.url);
		tmp = tmp->next;
	}

	printf("-=-=-=-=-=-=-=-=-=-=-\n");

	node_t * newnode = (node_t *)malloc(sizeof(node_t));
	strcpy(newnode->url.url, "xxxxxx");
	newnode->next = NULL;
	queue_push(que, newnode);

	tmp = que->front->next;
	while (tmp != NULL)
	{
		printf("%s\n", tmp->url.url);
		tmp = tmp->next;
	}

	printf("-=-=-=-=-=-=-=-=-=-=-\n");

	queue_pop(que);

	tmp = que->front->next;
	while (tmp != NULL)
	{
		printf("%s\n", tmp->url.url);
		tmp = tmp->next;
	}

	printf("-=-=-=-=-=-=-=-=-=-=-\n");
	node_t * na = queue_front(que);
	printf("头：%s\n", na->url.url);

	node_t * nb = queue_tail(que);
	printf("尾：%s\n", nb->url.url);

	while (!queue_is_empty(que))
	{
		printf("-=-=-=-=-=-=-=-=-=-=-\n");
		tmp = que->front->next;
		while (tmp != NULL)
		{
			printf("%s\n", tmp->url.url);
			tmp = tmp->next;
		}
		queue_pop(que);
	}

	queue_destroy(que);
}

//从队列中下载所有的图片
int download_pic(queue_t * const que)
{
	if (que == NULL)
	{
		perror("error: In download_pic(), parameter error, que == NULL!");
		exit(-1);
	}

	node_t * node = NULL;
	int webfd = 0;
	char head[HEAD_SIZE] = {0};
	int res = 0;	//用于接收返回值
	while (!queue_is_empty(que))
    {
    	memset(head, 0, HEAD_SIZE);
    	node = NULL;
    	webfd = 0;

    	node = queue_front(que);

    	//url解析
		res = spider_analytic_url(&node->url);
		if (res == 1)	//域名解析尝试三次失败，返回1。
		{
			queue_pop(que);
			close(webfd);
			continue;
		}
		//创建socket
	    webfd = spider_socket_create();
	    //建立与web服务器的链接
	    res = spider_connection(&node->url, webfd);
	    if (res == 1)	//建立连接尝试三次失败，返回1。
	    {
	    	queue_pop(que);
	    	close(webfd);
	    	continue;
	    }
	    //构造请求头
	    spider_create_head(head, &node->url);
	    //发送请求
	    spider_request(head, webfd);
	    //获取图片请求得到响应
	    spider_picture_response(&node->url, webfd);

	    printf("Download %s is Sucess!\n", node->url.url);

    	queue_pop(que);
		close(webfd);
    }
    return 0;
}

int main(void)
{
	// test_queue();
	
	int res = 0;	//用于接收返回值
	url_t url;
	char head[HEAD_SIZE] = {0};
	char web_domain[DOMAIN_SIZE] = {0};
	char *ch_html = (char *)malloc(MAX_HTML_FILE_SIZE);
	memset(ch_html, 0, MAX_HTML_FILE_SIZE);

	queue_t * pic_que = queue_init();
	queue_t * html_que = queue_init();
	printf("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n");
	printf("-*-              欢迎使用爬虫系统                   -*-\n");
	printf("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n");
	printf("输入所要爬取图片的网页的URL：");
	scanf("%s", url.url);
	// strcpy(url.url, "http://www.umei.cc/p/gaoqing/cn/");

	//提取出抓取网站的网址
	sscanf(url.url, "%*[^:]://%[^/]", web_domain);
	
	//url解析
	res = spider_analytic_url(&url);
	if (res == 1)	//main函数中三次域名解析失败
	{
		perror("error: In main(), call spider_analytic_url() fail! Please check input url is it right!");
		return -1;
	}
	//创建socket
    int webfd = spider_socket_create();
    //建立与web服务器的链接
    res = spider_connection(&url, webfd);
    if (res == 1)	//main函数中三次建立连接失败
    {
    	perror("error: In main(), call spider_connection() fail! Please check input url is it accessible!");
    	return -1;
    }
    //构造请求头
    spider_create_head(head, &url);
    //发送请求
    spider_request(head, webfd);
    //获取html页面请求的响应
    spider_html_response(&url, webfd, ch_html);
    //通信结束，断开连接
    close(webfd);

    //从html页面中提取页面url放入队列
    regex_draw_html_url(html_que, ch_html, web_domain);

    //提取html_que队列中所有页面中的全部图片url
    node_t * node = NULL;
    while (!queue_is_empty(html_que))
    {
    	memset(ch_html, 0, MAX_HTML_FILE_SIZE);
    	memset(head, 0, HEAD_SIZE);

    	node = queue_front(html_que);
    	//printf("%s\n", &node->url);

    	//url解析
		spider_analytic_url(&node->url);
		//创建socket
	    webfd = spider_socket_create();
	    //建立与web服务器的链接
	    spider_connection(&node->url, webfd);
	    //构造请求头
    	spider_create_head(head, &node->url);
	    //发送请求
	    spider_request(head, webfd);
	    //获取html页面请求的响应
   		spider_html_response(&node->url, webfd, ch_html);
   		//通信结束，断开连接
    	close(webfd);

    	//从html页面中提取图片url放入队列
    	regex_draw_pic_url(pic_que, ch_html);

    	queue_pop(html_que);
    	//printf("===== over =====\n");
    }

    //销毁队列html_que
    queue_destroy(html_que);
	free(ch_html);

    // printf("测试结束！\n");
    // exit(1);

    //从队列中下载所有的图片
    download_pic(pic_que);
    //销毁队列pic_que
    queue_destroy(pic_que);

    printf("========== Download All Picture Is Over ==========\n");

	return 0;
}
