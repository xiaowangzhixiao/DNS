//
// Created by wangzhi on 17-5-2.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ipcache.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

unsigned int SDBMHash(const void *key)
{
	register unsigned int hash = 0;
	const char *str = key;
	unsigned int ch;
	while (ch = (size_t)*str++)
	{
		hash = 65599 * hash + ch;
		//hash = (size_t)ch + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}
unsigned int ipHash(const void *key)
{
	return (unsigned int) *(uint32_t *)key;
}

int cmpHost(const void *x, const void *y){
	return strcmp(x,y);
}

int cmpIp(const void *x, const void *y){
	return (*(uint32_t *)x)-(*(uint32_t *)y);
}

void freeKey(const void *key){
	free((void *)key);
}

IpCache IpCache_init()
{
	IpCache ipCache;
	ipCache = HashTable_create(100,cmpHost,SDBMHash,freeKey);
	return ipCache;
}


//从文件中读取ip和域名的映射
void IpCache_read(IpCache ipCache, char filename[])
{
	FILE * readfile = fopen(filename, "r");
	char hostbuff[256], *host;
	char ipbuff[256];
	uint32_t *ip = NULL;
	if (readfile == NULL)
	{
		fprintf(stderr, "Can't open file %s\n", filename);
		exit(-1);
	}

	fscanf(readfile, " %s", hostbuff);
	fscanf(readfile, " %s", ipbuff);
	while (!feof(readfile))
	{
		host = strdup(hostbuff);
		ip = (uint32_t *)malloc(sizeof(uint32_t));
		if (ip == NULL)
		{
			fprintf(stderr,"malloc error %s",strerror(errno));
			exit(-1);
		}
		if(inet_pton(AF_INET,ipbuff,ip) < 0)
		{
			fprintf(stderr,"convert: %s",strerror(errno));
			exit(-1);
		}

		HashSet tmpSet = HashTable_get(ipCache,host);
		if (tmpSet == NULL)
		{
			tmpSet = HashSet_create(5,cmpIp,ipHash);
			HashTable_insert(ipCache,host,tmpSet);
		}
		else
		{
			free(host);
			HashSet_insert(tmpSet,ip);
		}
		fscanf(readfile, " %s", hostbuff);
		fscanf(readfile, " %s", ipbuff);
	}

}

uint32_t **IpCache_search(IpCache ipCache, char *host)
{
	uint32_t **ipArray;
	HashSet tmpSet = HashTable_get(ipCache,host);
	if(tmpSet == NULL)
	{
		return NULL;
	}
	else
	{
		ipArray = (uint32_t **) HashSet_toArray(tmpSet, NULL);
		return ipArray;
	}
}

void IpCache_insert(IpCache ipCache, char *host, uint32_t ip)
{
	HashSet tmpSet = HashTable_get(ipCache,host);
	if (tmpSet == NULL)
	{
		tmpSet = HashSet_create(5,cmpIp,ipHash);
		HashTable_insert(ipCache,host,tmpSet);
	}
	else
	{
		free(host);
		uint32_t *ip_ptr = (uint32_t*)malloc(sizeof(uint32_t));
		*ip_ptr = ip;
		HashSet_insert(tmpSet,ip_ptr);
	}
}



void freeIp(const void *key, void **value, void *c1)
{
	free(*value);
}

void freeSet(const void *key, void **value, void *c1)
{
	HashSet tmpSet = *value;
	HashSet_map(tmpSet,freeIp,c1);
	HashSet_destory(&tmpSet);
}

void IpCache_destory(IpCache ipCache)
{
	HashTable_map(ipCache,freeSet,NULL);
	HashTable_destory(&ipCache);
}
