《一个爬虫项目---爬取网站上的图片》
版本：version1.0

功能：
	根据输入的网页的url，程序抓取当前网站下所有能跳转的本网站页面上的图片数据，保存到事先创建好的目录img中。

大致逻辑：
	1. 解析出用户输入url的域名，端口，ip等信息。根据这些信息去请求该页面，保存其源代码。
	2. 从该页面的源代码中抓取其他页面的url链接，并且抓取的url链接必须和用户输入的url是同一个域名（即属于同一个网站的页面），将其存入一个队列html_que中，同时保证这些url没有重复。
	3. 不断地从该队列中取出url进行请求，并从响应的html页面源代码中解析出所有图片的url，存入到队列pic_que中，同时也要保证图片的url不能重复。
	4. 逐个的从图片队列pic_que中获取图片的url，请求下载，将数据保存到已经建好的目录img中。



问题：
	在使用valgrind工具对编译好的程序进行内存检查时，发现有几个错误提示，不知道该如何处理。（这些报错信息并不影响程序运行，也不是内存泄漏提示。）
	valgrind工具进行内存检查的命令：
		valgrind --tool=memcheck --leak-check=full ./a.out
	基于Debain的Linux安装valgrind的命令：
		sudo apt-get install valgrind
	注意：
		在使用gcc进行编译的时候，要加上-g参数，以便使用valgrind时可以定位到内存泄漏发生在哪一行。


测试：
	输入的起始URL：http://www.umei.cc/p/gaoqing/cn/
	程序中间输出信息保存：./bin/spider_printf.log
	valgrind的输出的信息：./bin/spider_valgrind.log
