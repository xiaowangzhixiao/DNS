//
// Created by wangzhi on 17-5-10.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "idmap.h"

int cmpId(const void *x, const void *y){
	return (*(uint16_t *)x)-(*(uint16_t *)y);
}

static void freeKey(const void *key){

}

unsigned int idHash(const void *key)
{
	return (unsigned int) *(uint32_t *)key;
}

IdMap IdMap_init()
{
	IdMap idMap;
	idMap = HashTable_create(300,cmpId,idHash,freeKey);
	return idMap;
}

IpId *IdMap_search(IdMap idMap, uint16_t id)
{
	IpId *result;
	result = HashTable_get(idMap,&id);
	return result;
}

void IdMap_insert(IdMap idMap, uint16_t id, struct sockaddr_in clientAddr, uint16_t idOld)
{
	uint16_t *id_ptr;
	IpId * ipId_ptr;

	id_ptr = malloc(sizeof(uint16_t));
	if (id_ptr == NULL)
	{
		fprintf(stderr,"malloc error %s",strerror(errno));
		exit(-1);
	}
	*id_ptr = id;

	ipId_ptr = malloc(sizeof(IpId));
	if (ipId_ptr == NULL)
	{
		fprintf(stderr,"malloc error %s",strerror(errno));
		exit(-1);
	}
	ipId_ptr->id = idOld;
	ipId_ptr->clientAddr = clientAddr;
	ipId_ptr->requireTime = time(NULL);

	HashTable_insert(idMap, id_ptr, ipId_ptr);
}

typedef struct{
	uint16_t **idArray;
	int length;
}IdArray;

void apply(const void *key, void **value, void *c1)
{
	time_t timeNow = time(NULL);
	uint16_t **idArray = ((IdArray *)c1)->idArray;
	IpId *ipId = *value;
	assert(ipId);
	if (timeNow - ipId->requireTime > AGE)
	{
	    idArray[((IdArray *)c1)->length++] = key;
	}
}

void IdMap_update(IdMap idMap)
{
	IdArray * idArray = malloc(sizeof(IdArray));
	idArray->length = 0;
	idArray->idArray = malloc(sizeof(uint16_t *)*HashTable_length(idMap));

	HashTable_map(idMap,apply,idArray);

	for (int i = 0; i < idArray->length; ++i)
	{
		HashTable_remove(idMap,idArray->idArray[i]);
	}

	free(idArray->idArray);
	free(idArray);
}

IpId * IdMap_remove(IdMap idMap, uint16_t id)
{
	IpId *ipId;
	ipId = HashTable_remove(idMap, &id);
	return ipId;
}


void freeIdIp(const void *key, void **value, void *c1)
{
	free(*value);
}

void IdMap_destory(IdMap idMap)
{
	HashTable_map(idMap,freeIdIp,NULL);
	HashTable_destory(&idMap);
}

