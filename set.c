//
// Created by wangzhi on 17-4-29.
//

#include <wchar.h>
#include <assert.h>
#include <malloc.h>
#include "set.h"

HashSet HashSet_create(int hint, int (*cmp)(const void *, const void *), unsigned int (*hash)(const void *))
{
	return HashTable_create(hint,cmp,hash,NULL);
}

void HashSet_destory(HashSet *hashSet)
{
	HashTable_destory(hashSet);
}

int HashSet_length(HashSet hashSet)
{
	return hashSet->length;
}

void *HashSet_insert(HashSet hashSet, void *member)
{
	return HashTable_insert(hashSet,member,member);
}

void *HashSet_remove(HashSet hashSet, void *member)
{
	return HashTable_remove(hashSet,member);
}

void **HashSet_toArray(HashSet hashSet, void *end)
{
	int i, j = 0;
	void **array;
	HashNode *p;
	assert(hashSet);
	array = malloc((hashSet->length + 1)*sizeof (*array));
	for (i = 0; i < hashSet->size; i++)
		for (p = hashSet->bucket[i]; p; p = p->next) {
			array[j++] = p->value;
		}
	array[j] = end;
	return array;
}

void HashSet_map(HashSet hashSet, void (*apply)(const void *, void **, void *), void *c1)
{
	HashTable_map(hashSet,apply,c1);
}


