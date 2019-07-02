#ifndef _SPIDER_H_
#define _SPIDER_H_
//#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#include "all_define.h"


/*URL类型*/
typedef  struct
{
    char url[URL_SIZE];
    char domain[DOMAIN_SIZE];
    char path[PATH_SIZE];
    char name[NAME_SIZE];
    char ip[IP_SIZE];
    int port;
    int type;   //如果为TRUE是http，FALSE为https
}url_t;

//解析url
int spider_analytic_url(url_t * const u);
//创建socket
int spider_socket_create(void);
//连接Web服务器
int spider_connection(url_t * const u, int webfd);
//创建HTTP请求头
int spider_create_head(char * const head, url_t * const u);
//进行HTTP请求
int spider_request(char * const head, int webfd);
//图片请求的响应
int spider_picture_response(url_t * const u, int webfd);
//html页面请求的响应
int spider_html_response(url_t * const u, int webfd, char * const ch_html);

#endif
