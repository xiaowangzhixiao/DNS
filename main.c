#include <stdio.h>
#include "DNS.h"
#include "hash.h"
#include "ipcache.h"
#include "idmap.h"
#include <stdbool.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 53

bool debug1 = false;

int main(int argc, char **argv)
{
	int socketfd;
	char *filename = NULL;
	uint32_t upIp = 0;
	uint16_t id = 1;

	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (argv[i][1] == '-')
			{
				for (int j = 0; argv[i][j] != '\0'; ++j)
				{
					if (argv[i][j] == 'd')
					{
						debug1 = true;
					}else if (argv[i][j] == 'f')
					{
					    i++;
						filename = argv[i];
						break;
					}
					if (argv[i][j] == 'i')
					{
					    i++;
						if(inet_pton(AF_INET,argv[i],&upIp) < 0)
						{
							fprintf(stderr,"convert: %s",strerror(errno));
							exit(-1);
						}
					}
				}
			}
		}
	}

	/********************各组件初始化*********************/
	IpCache ipCache = IpCache_init();
	if (filename != NULL)
	{
	    IpCache_read(ipCache,filename);
	}
	IdMap idMap = IdMap_init();
	if (upIp == 0)
	{
		if(inet_pton(AF_INET,"8.8.8.8",&upIp) < 0)
		{
			fprintf(stderr,"convert: %s",strerror(errno));
			exit(-1);
		}
	}

	/*****************UDP服务器开启***********************/
	socketfd = socket(AF_INET,SOCK_DGRAM,0);//UDP报文流
	if (socketfd < 0)
	{
		fprintf(stderr,"socket: %s\n",strerror(errno));
		exit(1);
	}

	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	memset(&addr, 0, len);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(socketfd, (const struct sockaddr *) &addr, len))//绑定
	{
		fprintf(stderr, "bind: %s\n", strerror(errno));
		exit(1);
	}

	char buff[65536];
	char addrstr[256];
	struct sockaddr_in client_addr;
	size_t client_len = sizeof(client_addr);

	struct sockaddr_in up_addr;
	up_addr.sin_addr.s_addr = upIp;
	up_addr.sin_family = AF_INET;
	up_addr.sin_port = htons(PORT);

	/************大循环反复检查是否收到报文*************/
	while (1)
	{
		IdMap_update(idMap);//删除超时的映射关系

		memset(buff,0,65536);
		memset(&client_addr,0,client_len);
		ssize_t n = recvfrom(socketfd, buff, 65536, 0, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
		if (n < 0)
		{
			fprintf(stderr, "recvfrom: %s\n", strerror(errno));
			exit(1);
		}

		printf("receive from: %s\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,addrstr,256));

		DNSHeader dnsHeader = DNS_getHead(buff);
		if (dnsHeader.QR == 1)  //提问报文
		{
			if (debug1)
			{
			    printf("receive a question\n");
			}

			char *host = DNS_getHost(buff);
			uint32_t **ipArray = IpCache_search(ipCache,host);
			if (ipArray == NULL || ipArray[0] == NULL)  //本地缓存表中未查询到
			{
				IdMap_insert(idMap,id++,client_addr,dnsHeader.id);//添加映射
				size_t size_n = DNS_changeId(buff,id);//更改id
				if(sendto(socketfd, buff, size_n, 0, (const struct sockaddr *) &up_addr, sizeof(up_addr)) <0)
				{
					fprintf(stderr, "sendto: %s\n", strerror(errno));
					exit(1);
				}
			}
			else                                        //本地缓存表中查询到
			{
				size_t size_n;
				if (*(ipArray[0]) == 0)   //非法地址
				{
					size_n = DNS_errorAnswer(buff);//返回找不到域名
				}else{
					size_n = DNS_addAnswer(buff, *(ipArray[0]));//只加一个ip地址进去
				}

				//发送回包
				if(sendto(socketfd, buff, size_n, 0, (const struct sockaddr *) &client_addr, (socklen_t) client_len) <0)
				{
					fprintf(stderr, "sendto: %s\n", strerror(errno));
					exit(1);
				}
			}
		} else                  //回答报文
		{
			if (debug1)
			{
				printf("receive a answer\n");
			}

			IpId *ipId = IdMap_search(idMap,dnsHeader.id);
			if (ipId != NULL)                               //找到映射
			{
				size_t size = DNS_changeId(buff,ipId->id);
				if(sendto(socketfd, buff, size, 0, (const struct sockaddr *) &(ipId->clientAddr), sizeof(ipId->clientAddr)) <0)
				{
					fprintf(stderr, "sendto: %s\n", strerror(errno));
					exit(1);
				}

				//加入缓存。。。后续加入的功能
			}
			else
			{
				//超时删掉了id映射
				if(debug1)
				{
					printf("can't find the id\n");
				}
			}
		}

	}

	close(socketfd);

	return 0;
}