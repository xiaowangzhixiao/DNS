#include <stdio.h>
#include "DNS.h"
#include "hash.h"
#include "ipcache.h"
#include "idmap.h"
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 53

int debug = 0;

int main(int argc, char **argv)
{
	int socketfd;
	char *filename = NULL;
	uint32_t upIp = 0;
	uint16_t id = 1;

	printf("argc:%d\n",argc);
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			printf("argv[%d]:%s\n",i,argv[i]);
			if (argv[i][0] == '-')
			{
				for (int j = 0; argv[i][j] != '\0'; ++j)
				{
					if (argv[i][j] == 'd')
					{
						debug = debug==2?2:debug+1;
						printf("debug:%d\n",debug);
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
	DNS dns;
	dns.host = NULL;
	dns.buff = NULL;
	dns.size_n = 0;

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
		if (debug == 2)
		{
		    printf("waiting!\n");
		}
		IdMap_update(idMap);//删除超时的映射关系
		if (debug == 2)
		{
			printf("deleted idMap! length:%d\n",idMap->length);
		}

		memset(buff,0,65536);
		memset(&client_addr,0,client_len);
		dns.buff = buff;
		ssize_t size_n = recvfrom(socketfd, buff, 65536, 0, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
		if (size_n < 0)
		{
			fprintf(stderr, "recvfrom: %s\n", strerror(errno));
			exit(1);
		}

		printf("receive from: %s\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,addrstr,256));

		dns.size_n = (size_t) size_n;
		if (debug == 2)
		{
		    printf("size_n:%zi\n",size_n);
		}
		dns= DNS_getHead(dns);
		if (debug == 2)
		{
		    printf("id:%d\n",dns.dnsHeader.id);
			printf("QR:%d\n",dns.dnsHeader.QR);
			printf("OPCODE:%d\n",dns.dnsHeader.OPCODE);
			printf("AA:%d\n",dns.dnsHeader.AA);
			printf("TC:%d\n",dns.dnsHeader.TC);
			printf("RD:%d\n",dns.dnsHeader.RD);
			printf("RA:%d\n",dns.dnsHeader.RA);
			printf("RCODE:%d\n",dns.dnsHeader.RCODE);
			printf("QDCOUNT:%d\n",dns.dnsHeader.QDCOUNT);
			printf("ANCOUNT:%d\n",dns.dnsHeader.ANCOUNT);
			printf("NSCOUNT:%d\n",dns.dnsHeader.NSCOUNT);
			printf("ARCOUNT:%d\n",dns.dnsHeader.ARCOUNT);
		}
		if (dns.dnsHeader.QR == 0 && dns.dnsHeader.OPCODE == 0)  //提问报文
		{
			if (debug >= 1)
			{
			    printf("receive a question\n");
			}

			dns = DNS_getHost(dns);
			if (debug >= 1)
			{
			    printf("query host: %s\n", dns.host);
			}

			if (debug == 2)
			{
			    printf("QTYPE:%d QCLASS:%d\n",dns.QTYPE,dns.QCLASS);
			}

			if (dns.QTYPE == 1 && dns.QCLASS == 1)//其他问题类型不查询
			{
				uint32_t **ipArray = IpCache_search(ipCache,dns.host);
				if (ipArray == NULL || ipArray[0] == NULL)  //本地缓存表中未查询到
				{
					IdMap_insert(idMap,id,client_addr,dns.dnsHeader.id);//添加映射
					dns = DNS_changeId(dns,id);//更改id
					if (debug >= 1)
					{
						printf("sendto server id: %d\n",dns.dnsHeader.id);
					}
					id++;
					if(sendto(socketfd, buff, dns.size_n, 0, (const struct sockaddr *) &up_addr, sizeof(up_addr)) < 0)
					{
						fprintf(stderr, "sendto: %s\n", strerror(errno));
						exit(1);
					}
					printf("send to: %s\n",inet_ntop(AF_INET,&up_addr.sin_addr.s_addr,addrstr,256));
				}
				else                                        //本地缓存表中查询到
				{
					if (*(ipArray[0]) == 0)   //非法地址
					{
						if (debug > 1)
						{
						    printf("blocked site:%s\n", dns.host);
						}
						dns = DNS_errorAnswer(dns);//返回找不到域名
					}else{
						if (debug > 1)
						{
							printf("found ip:%u\n", *(ipArray[0]));
						}
						dns = DNS_addAnswer(dns, *(ipArray[0]));//只加一个ip地址进去
						if (debug == 2)
						{
							dns= DNS_getHead(dns);
							printf("id:%d\n",dns.dnsHeader.id);
							printf("QR:%d\n",dns.dnsHeader.QR);
							printf("OPCODE:%d\n",dns.dnsHeader.OPCODE);
							printf("AA:%d\n",dns.dnsHeader.AA);
							printf("TC:%d\n",dns.dnsHeader.TC);
							printf("RD:%d\n",dns.dnsHeader.RD);
							printf("RA:%d\n",dns.dnsHeader.RA);
							printf("RCODE:%d\n",dns.dnsHeader.RCODE);
							printf("QDCOUNT:%d\n",dns.dnsHeader.QDCOUNT);
							printf("ANCOUNT:%d\n",dns.dnsHeader.ANCOUNT);
							printf("NSCOUNT:%d\n",dns.dnsHeader.NSCOUNT);
							printf("ARCOUNT:%d\n",dns.dnsHeader.ARCOUNT);

							printf("size_n:%zi\n",dns.size_n);

						}
					}

					//发送回包
					if(sendto(socketfd, buff, dns.size_n, 0, (const struct sockaddr *) &client_addr, (socklen_t) client_len) < 0)
					{
						fprintf(stderr, "sendto: %s\n", strerror(errno));
						exit(1);
					}
					free(ipArray);
				}
			}

		} else if (dns.dnsHeader.QR == 1 && dns.dnsHeader.RCODE == 0)                 //回答报文
		{
			if (debug >= 1)
			{
				printf("receive a answer\n");
			}

			dns = DNS_getHost(dns);
			if (debug >= 1)
			{
				printf("answer host: %s\n",dns.host);
			}

			IpId *ipId = IdMap_search(idMap,dns.dnsHeader.id);
			if (ipId != NULL)                               //找到映射
			{
				uint16_t deleteId = dns.dnsHeader.id;
				if(debug == 2)
				{
					printf("found id:%d old id:%d\n",dns.dnsHeader.id,ipId->id);
				}
				dns = DNS_changeId(dns,ipId->id);
				if(sendto(socketfd, buff, dns.size_n, 0, (const struct sockaddr *) &(ipId->clientAddr), sizeof(ipId->clientAddr)) < 0)
				{
					fprintf(stderr, "sendto: %s\n", strerror(errno));
					exit(1);
				}

				ipId = IdMap_remove(idMap,deleteId);
				if (ipId!=NULL)
				{
					free(ipId);
				}
				//加入缓存。。。后续加入的功能
			}
			else
			{
				//超时删掉了id映射
				if(debug>=1)
				{
					printf("!!!can't find the id!!!\n");
				}
			}
		}
		DNS_clear(&dns);
	}

	close(socketfd);

	return 0;
}