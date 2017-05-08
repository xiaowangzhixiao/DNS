#include <stdio.h>
#include "DNS.h"
#include "hash.h"
#include "set.h"
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT 80

int main()
{
	int socketfd;

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

	char buff[512];
	memset(buff,0,512);
	struct sockaddr_in client_addr;
	size_t client_len = sizeof(client_addr);
	memset(&client_addr,0,client_len);
	while (1)
	{
		ssize_t n = recvfrom(socketfd, buff, 512, 0, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
		if (n < 0)
		{
			fprintf(stderr, "recvfrom: %s\n", strerror(errno));
			exit(1);
		}
		buff[n] = '\0';
		printf("%s\n",buff);
		if (strcmp(buff,"stop") == 0)
		{
			printf("closed the socket\n");
			break;
		}
		printf("receive from: %s\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,buff,256));
	}

	close(socketfd);

	return 0;
}