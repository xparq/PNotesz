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

#include <windows.h>
#include <wchar.h>
#include "tags.h"

LPPTAG TagsAdd(LPPTAG this, wchar_t * lpText, wchar_t * lpOldText){
	LPPTAG	pTag = calloc(1, sizeof(PTAG));
	LPPTAG	pNext;
	if(pTag){
		wcscpy(pTag->text, lpText);
		if(lpOldText)
			wcscpy(pTag->oldText, lpOldText);
		if(!this){
			this = pTag;
		}
		else{
			pNext = this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pTag;
			pTag->prev = pNext;
		}
	}
	return this;
}

void TagsClear(LPPTAG this){
	for(LPPTAG pNext = this; pNext; pNext = pNext->next){
		*pNext->text = '\0';
		*pNext->oldText = '\0';
	}
}

void TagsFree(LPPTAG this){
	LPPTAG	pNext, pTemp;

	pNext = this;
	while(pNext){
		pTemp = pNext;
		pNext = pNext->next;
		free(pTemp);
	}
}

LPPTAG TagsCopy(LPPTAG tDest, LPPTAG tSrc){
	if(tDest)
		TagsFree(tDest);
	tDest = NULL;
	for(LPPTAG pNext = tSrc; pNext; pNext = pNext->next){
		tDest = TagsAdd(tDest, pNext->text, pNext->oldText);
	}
	return tDest;
}

int TagsCompare(LPPTAG this, LPPTAG that){
	LPPTAG	p1 = this, p2 = that;

	if((p1 && !p2) || (!p1 && p2))
		return 1;
	while(p1 && p2){
		if(_wcsicmp(p1->text, p2->text) != 0)
			return 1;
		p1 = p1->next;
		p2 = p2->next;
	}
	if((p1 && !p2) || (!p1 && p2))
		return 1;
	return 0;
}

BOOL TagsContains(LPPTAG this, wchar_t * lpText){
	for(LPPTAG pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->text, lpText) == 0)
			return TRUE;
	}
	return FALSE;
}

BOOL TagsContainsOld(LPPTAG this, wchar_t * lpText){
	for(LPPTAG pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->oldText, lpText) == 0)
			return TRUE;
	}
	return FALSE;
}

void TagsReplace(LPPTAG this, wchar_t * lpNew, wchar_t * lpOld){
	for(LPPTAG pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->text, lpOld) == 0){
			wcscpy(pNext->text, lpNew);
			return;
		}
	}
}

void TagsGetOld(LPPTAG this, wchar_t * lpText, wchar_t * lpOld){
	for(LPPTAG pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->text, lpText) == 0){
			wcscpy(lpOld, pNext->oldText);
			return;
		}
	}
}

LPPTAG TagsRemove(LPPTAG this, wchar_t * lpText){
	LPPTAG	pNext = this, pTemp;

	while(pNext){
		if(_wcsicmp(pNext->text, lpText) == 0){
			pTemp = pNext;
			if(pNext->prev == NULL){
				//very first tag, i.e. "this"
				if(pNext->next){
					pNext->next->prev = NULL;
					this = pNext->next;
				}
				else{
					this = NULL;
				}
			}
			else if(pNext->next == NULL){
				//very last tag
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
	return this;
}

int TagsCount(LPPTAG this){
	int		count = 0;
	for(LPPTAG pNext = this; pNext; pNext = pNext->next){
		count++;
	}
	return count;
}
