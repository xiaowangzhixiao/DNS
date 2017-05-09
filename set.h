//
// Created by wangzhi on 17-4-29.
// 使用哈希表实现集合,简化的集合，只有插入和读取两种操作
// 主要为ip地址服务，由于一个域名可能对应多个ip地址，所以使用一个集合来作为多ip地址的存储结构
//

#ifndef DNS_SET_H
#define DNS_SET_H

#include "hash.h"

typedef HashTable HashSet;

HashSet HashSet_create(int hint, int (*cmp)(const void *x, const void * y),unsigned int (*hash)(const void *member));

void HashSet_destory(HashSet * hashSet);

int HashSet_length(HashSet hashSet);

/*
 * @brief 插入一个表项
 *
 * @param HashTable  hashTable  哈希表
 *        void *member       成员
 *
 * @return 若之前有过该值，则覆盖之前的值，返回之前的值，否则返回NULL
 */
void * HashSet_insert(HashSet  hashSet, void * member);

/*
 * @brief 删除某键值对
 *
 * @param HashTable  hashTable  哈希表
 *        const void *key       键
 *
 * @return 查找key对应的值，查找到，删除并返回查找到的键值对，否则返回NULL
 */
void * HashSet_remove(HashSet hashSet, void *member);

void HashSet_map(HashSet hashSet, void (*apply)(const void *key, void **value, void *c1), void *c1);

void **HashSet_toArray(HashSet hashSet, void *end);

#endif //DNS_SET_H
