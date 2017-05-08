//
// Created by wangzhi on 17-5-2.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ipcache.h"
#include <stdlib.h>

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

int cmp(const void *x, const void *y){
	return strcmp(x,y);
}

void freeKey(const void *key){
	free((void *)key);
}

IpCache IpCache_init()
{
	IpCache ipCache;
	ipCache = HashTable_create(100,cmp,SDBMHash,freeKey);
	return ipCache;
}


//从文件中读取ip和域名的映射
void IpCache_read(IpCache ipCache, char filename[])
{
	FILE * readfile = fopen(filename, "r");
	char hostbuff[256], *host;
	char ipbuff[256];
	uint32_t ip;
	if (readfile == NULL)
	{
		fprintf(stderr, "Can't open file %s\n", filename);
		exit(-1);
	}

	fscanf(readfile, " %s", hostbuff);
	fscanf(readfile, " %s", ipbuff);

	host = strdup(hostbuff);
	ip = inet_

}

uint32_t IpCache_search(IpCache ipCache, char host[]);

void IpCache_insert(IpCache ipCache, char host[], uint32_t ip);

void IpCache_destory(IpCache ipCache);
