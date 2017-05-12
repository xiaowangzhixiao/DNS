#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <malloc.h>
#include "idmap.h"

int main()
{
	IdMap idMap = IdMap_init();
	struct sockaddr_in sockaddrIn;
	IpId *ipId;
	int error = 0;

	for (int i = 0; i < 500; ++i)
	{
		memset(&sockaddrIn,0,sizeof(sockaddrIn));
		sockaddrIn.sin_port = (in_port_t) (i + 100);
		sockaddrIn.sin_family = AF_INET;
		sockaddrIn.sin_addr.s_addr = (in_addr_t) (i + 500);
		IdMap_insert(idMap, (uint16_t) i, sockaddrIn, (uint16_t) (i + 2000));
	}

	for (int i = 0; i < 500; ++i)
	{
		ipId = IdMap_search(idMap, (uint16_t) i);
		if (ipId != NULL)
		{
			printf("found id:%d old id:%d\n",i,ipId->id);
			IdMap_remove(idMap, (uint16_t) i);
			free(ipId);
		}
		else{
			printf("找不到\n");
			error++;
		}
	}

	printf("错误%d个\n",error);
	printf("现在表长度%d\n",idMap->length);

	return 0;
}