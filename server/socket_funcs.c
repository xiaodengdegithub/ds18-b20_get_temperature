/*********************************************************************************
 *      Copyright:  (C) 2022 Deng Yonghao<dengyonghao2001@163.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_funcs.c
 *    Description:  This file socket_funcs.c
 *                 
 *        Version:  1.0.0(2022年11月20日)
 *         Author:  Deng Yonghao <dengyonghao2001@163.com>
 *      ChangeLog:  1, Release initial version on "2022年11月20日 21时54分56秒"
 *                 
 ********************************************************************************/

#include "socket_funcs.h"

int socket_server_init(char *listen_ip, int listen_port)
{
	struct sockaddr_in servaddr;
	int rv = 0;
	int on = 1;
	int listenfd;
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		 dbg_printf("Use socket() to create a TCP socket failure: %s\n", strerror(errno));
		return -1;
	}

	/* Set socket port reuseable, fix 'Address already in use' bug when socket server restart */
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(listen_port);
	if( !listen_ip ) /* Listen all the local IP address */
	{
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else /* listen the specified IP address */
	{
		if (inet_pton(AF_INET, listen_ip, &servaddr.sin_addr) <= 0)
		{
			 dbg_printf("inet_pton() set listen IP address failure.\n");
			rv = -2;
			goto CleanUp;
		}
	}
	if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		 dbg_printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
		rv = -3;
		goto CleanUp;
	}
	if(listen(listenfd, 64) < 0)
	{
		 dbg_printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
		rv = -4;
		goto CleanUp;
	}
CleanUp:
	if(rv<0)
		close(listenfd);
	else
		rv = listenfd;
	return rv;
}

/* Set open file description count to max */
void set_socket_rlimit(void)
{
    struct rlimit limit = {0};
    getrlimit(RLIMIT_NOFILE, &limit );
    limit.rlim_cur = limit.rlim_max;
    setrlimit(RLIMIT_NOFILE, &limit );
     dbg_printf("set socket open fd max count to %ld\n", limit.rlim_max);
}


