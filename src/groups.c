// Copyright (C) 2009 Andrey Gruber (aka lamer)

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

#include "notes.h"
#include "stringconstants.h"
#include "shared.h"
#include "groups.h"
#include "numericconstants.h"

static int				m_GroupCount = 0; 

LPPNGROUP PNGroupsAdd(LPPNGROUP * this, LPPNGROUP pg){
	LPPNGROUP		pNew = calloc(1, sizeof(PNGROUP));
	LPPNGROUP		pNext;

	if(pNew){
		memcpy(pNew, pg, sizeof(PNGROUP));
		pNew->next = NULL;
		if(!*this){
			*this = pNew;
		}
		else{
			pNext = *this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pNew;
		}
	}
	return *this;
}

void PNGroupsFree(LPPNGROUP * this){
	LPPNGROUP	pTemp = *this, pNext = NULL;
	while(pTemp){
		pNext = pTemp->next;
		free(pTemp);
		pTemp = pNext;
	}
	*this = NULL;
}

void PNGroupsTotalFree(LPPNGROUP * this){
	LPPNGROUP	pTemp = *this, pNext = NULL;
	while(pTemp){
		pNext = pTemp->next;
		if(pTemp->hotKeyShow.fsModifiers != 0 || pTemp->hotKeyShow.vk != 0)
			UnregisterHotKey(g_hMain, pTemp->hotKeyShow.id);
		if(pTemp->hotKeyHide.fsModifiers != 0 || pTemp->hotKeyHide.vk != 0)
			UnregisterHotKey(g_hMain, pTemp->hotKeyHide.id);
		free(pTemp);
		pTemp = pNext;
	}
	*this = NULL;
}

LPPNGROUP PNGroupsCopy(LPPNGROUP * tDest, LPPNGROUP tSrc){
	LPPNGROUP	pNext;

	if(*tDest)
		PNGroupsFree(tDest);
	*tDest = NULL;
	pNext = tSrc;
	while(pNext){
		*tDest = PNGroupsAdd(tDest, pNext);
		pNext = pNext->next;
	}
	return *tDest;
}

int PNGroupsCount(LPPNGROUP this){
	int			count = 0;
	for(LPPNGROUP pg = this; pg; pg = pg->next)
		count++;
	return count;
}

void PNGroupsDelete(LPPNGROUP this, int id){
	LPPNGROUP	pTemp, pNext;
	LPPNGROUP	pPrev = NULL;
	wchar_t		szKey[24];

	pTemp = this;
	while(pTemp){
		pNext = pTemp->next;
		if(pTemp->id == id){
			if(pTemp == g_PGroups && pNext == NULL){
				//there is only one group
				free(pTemp);
				g_PGroups = NULL;
			}
			else if(pTemp == g_PGroups && pNext != NULL){
				//delete the very first group
				free(pTemp);
				g_PGroups = pNext;
			}
			else if(pPrev != NULL){
				//delete group in the middle of list or very last group
				free(pTemp);
				pPrev->next = pNext; //in case of last group it will be NULL
			}
			//delete group from INI file
			_itow(id, szKey, 10);
			WritePrivateProfileStructW(S_GROUPS, szKey, NULL, sizeof(PNGROUP), g_NotePaths.INIFile);
			m_GroupCount--;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->next;
	}
}

int PNGroupsNewId(LPPNGROUP this){
	int			id = 0;

	for(LPPNGROUP pTemp = this; pTemp; pTemp = pTemp->next){
		if(pTemp->id > id)
			id = pTemp->id;
	}
	id++;
	return id;
}

LPPNGROUP PNGroupsGroupById(LPPNGROUP this, int id){
	for(LPPNGROUP pTemp = this; pTemp; pTemp = pTemp->next){
		if(pTemp->id == id)
			return pTemp;
	}
	return NULL;
}

