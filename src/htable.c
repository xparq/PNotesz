#include <wchar.h>
#include <stdlib.h>

#include "htable.h"

#define	MULTIPLIER			31

static unsigned int _hash(wchar_t * s);

PHENTRY addEntry(PHENTRY * pArray, wchar_t * key, wchar_t * value){
	unsigned int	hash = _hash(key);
	PHENTRY			pe, pn;
	
	pn = getEntry(pArray, key);
	if(pn){
		return pn;
	}

	pe = malloc(sizeof(HENTRY));
	pe->key = _wcsdup(key);
	pe->value = _wcsdup(value);
	pe->next = NULL;
	if(pArray[hash] != NULL){
		pe->next = pArray[hash];
	}
	pArray[hash] = pe;

	return pe;
}

void freeTable(PHENTRY * pArray){
	PHENTRY			p1, p2;

	for(int i = 0; i < HASH_SIZE; i++){
		p1 = pArray[i];
		while(p1){
			p2 = p1->next;
			free(p1->key);
			free(p1->value);
			free(p1);
			p1 = p2;
		}
	}
}

PHENTRY getEntry(PHENTRY * pArray, wchar_t * key){
	PHENTRY			pe = pArray[_hash(key)];

	while(pe){
		if(_wcsicmp(pe->key, key) == 0)
			return pe;
		pe = pe->next;
	}
	return NULL;
}

static unsigned int _hash(wchar_t * s){
	unsigned int hash = 0;
	while(*s){
		hash = *s + hash * MULTIPLIER;
		s++;
	}
	hash %= HASH_SIZE;
	return hash;
}

