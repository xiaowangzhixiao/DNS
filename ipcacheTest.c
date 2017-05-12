#include <stdio.h>
#include "ipcache.h"

int main()
{
	IpCache ipCache = IpCache_init();
	IpCache_read(ipCache,"dnsrelay.txt");
	char host[256];
	while (scanf(" %s",host) != -1)
	{
		uint32_t **ipArray = IpCache_search(ipCache,host);
		if (ipArray == NULL || ipArray[0] == NULL)
		{
		    printf("未找到\n");
		}
		else
		{
			for (int i = 0; ipArray[i] != NULL ; ++i)
			{
				printf("%u\n",*(ipArray[i]));
			}
		}
	}
	return 0;
}