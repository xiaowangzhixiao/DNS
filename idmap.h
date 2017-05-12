//
// Created by wangzhi on 17-5-10.
//

#include <stdint.h>
#include <netdb.h>
#include "hash.h"
#include <time.h>

#ifndef DNS_IDMAP_H
#define DNS_IDMAP_H

typedef HashTable IdMap;
#define AGE 30

typedef struct {
	struct sockaddr_in clientAddr;
	uint16_t id;
	time_t requireTime;
}IpId;

//初始化
IdMap IdMap_init();

IpId *IdMap_search(IdMap idMap, uint16_t id);

void IdMap_insert(IdMap idMap, uint16_t id, struct sockaddr_in clientAddr,uint16_t idOld);

void IdMap_update(IdMap idMap);

IpId * IdMap_remove(IdMap idMap, uint16_t id);

void IdMap_destory(IdMap idMap);

#endif //DNS_IDMAP_H
