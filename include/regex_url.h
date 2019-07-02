#ifndef _REGEX_URL_H_
#define _REGEX_URL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>

#include "queue.h"
#include "spider.h"
#include "all_define.h"

//从页面中提取图片的url
int regex_draw_pic_url(queue_t * const que, char * const ch_html);
//从页面中提取其他页面的url
int regex_draw_html_url(queue_t * const que, char * const ch_html, const char * const web_domain);

#endif