#include "regex_url.h"

int regex_draw_pic_url(queue_t * const que, char * const ch_html)
{
	if (que == NULL || ch_html == NULL)
	{
		perror("error: In regex_draw_pic_url(), parameter error, que == NULL || ch_html == NULL!");
		exit(-1);
	}

	int status = 0, i = 0, j = 0;
    int flag = REG_EXTENDED;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    const char *pattern = "(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]";

    regcomp(&reg, pattern, flag);
    char *tmp = ch_html;
    char pic_url[URL_SIZE] = {0};
    node_t * node = NULL;
    while (1)
    {
        status = regexec(&reg, tmp, nmatch, pmatch, 0);
        if(status == REG_NOMATCH)
        {
            //printf("no match\n");
            break;
        }
        else if(status == 0)
        {
            //printf("match success\n");
            memset(pic_url, 0, URL_SIZE);
            for(i = pmatch[0].rm_so, j = 0; i < pmatch[0].rm_eo; i++, j++)
            {
                pic_url[j] = tmp[i];
            }
            if (strstr(pic_url, ".jpg") != NULL || strstr(pic_url, ".png") != NULL || strstr(pic_url, ".gif") != NULL)
            {
                if (queue_is_contain_same(que, pic_url) == 0)
                {
                    node = (node_t *)malloc(sizeof(node_t));
                    strncpy(node->url.url, pic_url, strlen(pic_url));
                    node->next = NULL;
                    queue_push(que, node);

                    printf("%s\n", pic_url);
                }
            }
        }
        tmp = tmp + pmatch[0].rm_eo + 1;
    }
    regfree(&reg);

	return 0;
}

//从页面中提取其他页面的url
int regex_draw_html_url(queue_t * const que, char * const ch_html, const char * const web_domain)
{
    if (que == NULL || ch_html == NULL || web_domain == NULL)
    {
        perror("error: In regex_draw_html_url(), parameter error, que == NULL || ch_html == NULL || web_domain == NULL!");
        exit(-1);
    }

    int status = 0, i = 0, j = 0;
    int flag = REG_EXTENDED;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    const char *pattern = "(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]";

    regcomp(&reg, pattern, flag);
    char *tmp = ch_html;
    char html_url[URL_SIZE] = {0};
    node_t * node = NULL;
    while (1)
    {
        status = regexec(&reg, tmp, nmatch, pmatch, 0);
        if(status == REG_NOMATCH)
        {
            //printf("no match\n");
            break;
        }
        else if(status == 0)
        {
            //printf("match success\n");
            memset(html_url, 0, URL_SIZE);
            for(i = pmatch[0].rm_so, j = 0; i < pmatch[0].rm_eo; i++, j++)
            {
                html_url[j] = tmp[i];
            }
            if (strstr(html_url, web_domain) != NULL && strstr(html_url, ".js") == NULL)    //包含网站域名并且不是xxx.js
            {
                if (queue_is_contain_same(que, html_url) == 0)
                {
                    node = (node_t *)malloc(sizeof(node_t));
                    strncpy(node->url.url, html_url, strlen(html_url));
                    node->next = NULL;
                    queue_push(que, node);

                    printf("--- %s ---\n", html_url);
                }
            }
        }
        tmp = tmp + pmatch[0].rm_eo + 1;
    }
    regfree(&reg);

    return 0;
}
