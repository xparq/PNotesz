// Copyright (C) 2010 Andrey Gruber (aka lamer)

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <wchar.h>

#include "contgroups.h"

LPPCONTGROUP PContGroupsAdd(LPPCONTGROUP * this, LPPCONTGROUP lpgr, BOOL fNewId){
	LPPCONTGROUP		pNew = calloc(1, sizeof(PCONTGROUP));
	LPPCONTGROUP		pNext;

	if(pNew){
		memcpy(pNew, lpgr, sizeof(PCONTGROUP));
		pNew->next = NULL;
		pNew->prev = NULL;
		if(fNewId)
			pNew->id = PContGroupsNewId(*this);
		if(!*this){
			*this = pNew;
		}
		else{
			pNext = *this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pNew;
			pNew->prev = pNext;
		}
	}
	return *this;
}

void PContGroupsFree(LPPCONTGROUP * this){
	LPPCONTGROUP	pNext, pTemp;

	pNext = *this;
	while(pNext){
		pTemp = pNext;
		pNext = pNext->next;
		free(pTemp);
	}
	*this = NULL;
}

LPPCONTGROUP PContGroupsCopy(LPPCONTGROUP * tDest, LPPCONTGROUP tSrc){
	LPPCONTGROUP	pNext;

	if(*tDest)
		PContGroupsFree(tDest);
	*tDest = NULL;
	pNext = tSrc;
	while(pNext){
		*tDest = PContGroupsAdd(tDest, pNext, FALSE);
		pNext = pNext->next;
	}
	return *tDest;
}

int PContGroupsNewId(LPPCONTGROUP this){
	int		result = -1;
	for(LPPCONTGROUP pTemp = this; pTemp; pTemp = pTemp->next)
		result = pTemp->id;
	return (++result);
}

BOOL PContGroupsContains(LPPCONTGROUP this, wchar_t * lpName){
	for(LPPCONTGROUP pTemp = this; pTemp; pTemp = pTemp->next){
		if(_wcsicmp(pTemp->name, lpName) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

int PContGroupsCount(LPPCONTGROUP this){
	int			count = 0;

	for(LPPCONTGROUP pNext = this; pNext; pNext = pNext->next)
		count++;
	return count;
}

BOOL PContGroupsDifference(LPPCONTGROUP pc1, LPPCONTGROUP pc2){
	int				c1, c2;
	LPPCONTGROUP	pN1 = pc1, pN2 = pc2;

	c1 = PContGroupsCount(pc1);
	c2 = PContGroupsCount(pc2);

	if(c1 != c2){
		return TRUE;
	}
	else{
		while(pN1){
			if(wcscmp(pN1->name, pN2->name) != 0){
				return TRUE;
			}
			pN1 = pN1->next;
			pN2 = pN2->next;
		}
	}
	return FALSE;
}

LPPCONTGROUP PContGroupsRemove(LPPCONTGROUP * this, wchar_t * lpName){
	LPPCONTGROUP	pNext = *this, pTemp;

	while(pNext){
		if(_wcsicmp(pNext->name, lpName) == 0){
			pTemp = pNext;
			if(pNext->prev == NULL){
				//very first contacts group, i.e. "this"
				if(pNext->next){
					pNext->next->prev = NULL;
					*this = pNext->next;
				}
				else{
					*this = NULL;
				}
			}
			else if(pNext->next == NULL){
				//very last contacts group
				pNext->prev->next = NULL;
			}
			else{
				pNext->prev->next = pNext->next;
				pNext->next->prev = pNext->prev;
			}
			free(pTemp);
			break;
		}
		pNext = pNext->next;
	}
	return *this;
}

LPPCONTGROUP PContGroupsItem(LPPCONTGROUP this, wchar_t * lpName){
	for(LPPCONTGROUP pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->name, lpName) == 0){
			return pNext;
		}
	}
	return NULL;
}

LPPCONTGROUP PContGroupsItemById(LPPCONTGROUP this, int id){
	for(LPPCONTGROUP pNext = this; pNext; pNext = pNext->next){
		if(pNext->id == id){
			return pNext;
		}
	}
	return NULL;
}
