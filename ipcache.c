//
// Created by wangzhi on 17-5-2.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ipcache.h"

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
	free(key);
}

IpCache IpCache_init()
{
	IpCache ipCache;
	ipCache = HashTable_create(100,cmp,SDBMHash,freeKey);
	return ipCache;
}
