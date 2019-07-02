#include "spider.h"

//解析url
int spider_analytic_url(url_t * const u)
{
    //init url
    memset(u->domain, 0, DOMAIN_SIZE);
    memset(u->path, 0, PATH_SIZE);
    memset(u->name, 0, NAME_SIZE);
    memset(u->ip, 0, IP_SIZE);
    //http://aaa.com/s/s/shtml
    int start = 0;
    int i = 0, j = 0, size = 0;
    int url_len = 0, domain_len = 0;
    struct hostent *ent = NULL;
    int num = 0;    //记录重新尝试次数

    /*1.确定http版本*/
    const char * arr[] = {"http://","https://"};
    if(strncmp(u->url, arr[0], strlen(arr[0])) == 0)
    {
        //确定端口80/443
        u->type = TRUE;
        start = strlen(arr[0]);
        u->port = 80;
    }
    else if (strncmp(u->url, arr[1], strlen(arr[1])) == 0)
    {
        start = strlen(arr[1]);
        u->type = FALSE;
        u->port = 443;
    }
    else
    {
        perror("error: In spider_analytic_url(), url is not http or https!");
        return -1;
    }
    /*2.获取主机名域名*/
    j = 0;
    for(i = start; u->url[i] != '/' && u->url[i] != '\0'; i++)
    {
        u->domain[j++]=u->url[i];
    }
    /*3.获取文件名*/
    url_len = strlen(u->url);
    for(i = url_len; u->url[i] != '/'; i--, size++);
    j = 0;
    for(i = url_len - size + 1; u->url[i] != '\0'; i++)
    {
        u->name[j++] = u->url[i];
    }
    /*4.获取资源路径*/
    j = 0;
    domain_len = strlen(u->domain);
    for(i = start + domain_len; i < url_len - size; i++)
    {
        u->path[j++] = u->url[i];
    }
    /*5.获得IP*/
    //通过gethostbyname函数访问公网IP（本地host文件（www.baidu.com:192.168.1.1)).调用该函数需要网络
    while ((ent = gethostbyname(u->domain)) == NULL)
    {
        printf("call gethostbyname(%s) fail! Try again three times ...... %d\n", u->domain, ++num);
        usleep(500000);
        if (num == 3)
        {
            return 1;   //再次尝试三次失败，返回1。
        }
    }
    printf("domain: %s\n", u->domain);
    inet_ntop(AF_INET, ent->h_addr_list[0], u->ip, sizeof(u->ip));

    //printf("[1]Analytic url sucess...\n");
    //printf("url[%s]\ndomain[%s]\nip[%s]\nport[%d]\ntype[%d]\npath[%s]\nname[%s]\n",u->url,u->domain,u->ip,u->port,u->type,u->path,u->name);
    return 0;
}

//创建socket
int spider_socket_create(void)
{
	int socketfd;
    if((socketfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("error: In spider_socket_create(), call function socket() fail!");
        return -1;
    }
    //printf("[2]socket create success.....\n");
    return socketfd;
}

//连接Web服务器
int spider_connection(url_t * const u, int webfd)
{
    int num = 0;    //记录重试次数
	struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(u->port);
    inet_pton(AF_INET,u->ip, &serveraddr.sin_addr.s_addr);
    while ((connect(webfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) != 0)
    {
        //printf("[3]connect webserver success...\n");
        printf("call spider_connection() fail! Try again three times ...... %d\n", ++num);
        usleep(500000);
        if (num == 3)
        {
            return 1;   //再次尝试三次失败，返回1。
        }
    }

    return 0;  
}

//创建HTTP请求头
int spider_create_head(char * const head, url_t * const u)
{
	memset(head, 0, HEAD_SIZE);
    sprintf(head, "GET %s HTTP/1.1\r\n"\
                  "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
                  "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
                  "Host:%s\r\n"\
                  "Connection:close\r\n\r\n",u->url,u->domain);
    //printf("[4] Create request head:\n%s\n",head);
    return 0;
}

//进行HTTP请求
int spider_request(char * const head,int webfd)
{
	if((send(webfd, head, strlen(head), 0)) > 0)
    {
        //printf("[5] Send Request Head to WebServer Success...\n");
        return 0;
    }
    else
    {
        perror("error: call function spider_request() fail!");
        return -1;
    }
}

//图片请求的响应
int spider_picture_response(url_t * const u, int webfd)
{
	char buf[BUF_SIZE] = {0};
    char res[SIZE] = {0};
    memset(buf, 0, BUF_SIZE);
    memset(res, 0, SIZE);
    char * pos = NULL;
    int len = 0;
    len = recv(webfd, buf, BUF_SIZE, 0);
    if((pos = strstr(buf, "\r\n\r\n")) != NULL)
    {
        snprintf(res, pos-buf, "%s", buf);
    }
	char filepath[PATH_SIZE] = "../img/";
    char time_data[TIME_SIZE] = {0};

    //获取当前时间戳(微妙级时间)
    struct timeval nowtime;
    gettimeofday(&nowtime, NULL);
    sprintf(time_data, "%ld_%ld_", nowtime.tv_sec, nowtime.tv_usec);

    if (NULL == strcat(filepath, time_data))
    {
        perror("error: In spider_picture_response(), call function strcat() fail!");
        return -1;
    }
	if (NULL == strcat(filepath, u->name))
	{
		perror("error: In spider_picture_response(), call function strcat() fail!");
		return -1;
	}

    int fd = open(filepath, O_RDWR | O_CREAT, 0775);
	if (fd == -1)
	{
		perror("error: In spider_picture_response(), call function open() fail!");
		return -1;
	}
    write(fd, pos + 4, len - (pos - buf + 4));
    //printf("[6] Get ResHead Success...\n%s\n",res);
    memset(buf, 0, BUF_SIZE);
    while((len = read(webfd, buf, BUF_SIZE))>0)
    {
        write(fd, buf, len);
        memset(buf, 0, BUF_SIZE);
    }

	close(fd);
    close(webfd);
    return 0;
}

//html页面请求的响应
int spider_html_response(url_t * const u, int webfd, char * const ch_html)
{
	int len = 0;
	char buf[BUF_SIZE] = {0};
	memset(buf, 0, BUF_SIZE);

	while ((len = read(webfd, buf, BUF_SIZE)) > 0)
	{
		strncat(ch_html, buf, len);
		memset(buf, 0, BUF_SIZE);
	}

	return 0;
}
