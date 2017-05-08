//
// Created by wangzhi on 17-5-2.
//http://blog.csdn.net/djinglan/article/details/8812934 字符串的经典哈希函数

#include <stdint.h>
#include "hash.h"

#ifndef DNS_IPCACHE_H
#define DNS_IPCACHE_H

typedef HashTable IpCache;

IpCache IpCache_init();

void IpCache_read(IpCache ipCache, char filename[]);

uint16_t IpCache_search(IpCache ipCache, char host[]);

void IpCache_insert(IpCache ipCache, char host[], __int16_t ip);

void IpCache_destory(IpCache ipCache);

#endif //DNS_IPCACHE_H