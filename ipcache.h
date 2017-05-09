//
// Created by wangzhi on 17-5-2.
//http://blog.csdn.net/djinglan/article/details/8812934 字符串的经典哈希函数

#include <stdint.h>
#include "hash.h"
#include "set.h"

#ifndef DNS_IPCACHE_H
#define DNS_IPCACHE_H

typedef HashTable IpCache;

IpCache IpCache_init();

//从文件中读取ip和域名的映射
void IpCache_read(IpCache ipCache, char filename[]);

/*
 * @note 查找域名对应的ip地址
 *
 * @param IpCache ipCache  ip缓存表
 *        char *host       域名
 *
 * @return 返回ip地址指针组成的数组，以NULL结尾，如查询不到，则返回NULL
 *
 */
uint32_t **IpCache_search(IpCache ipCache, char *host);

void IpCache_insert(IpCache ipCache, char *host, uint32_t ip);

void IpCache_destory(IpCache ipCache);

#endif //DNS_IPCACHE_H
