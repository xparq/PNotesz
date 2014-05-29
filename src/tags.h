#ifndef __TAGS_H__
#define __TAGS_H__

#include <windows.h>
#include <wchar.h>
#include <windowsx.h>

#include "structures.h"

LPPTAG TagsAdd(LPPTAG this, wchar_t * lpText, wchar_t * lpOldText);
void TagsClear(LPPTAG this);
void TagsFree(LPPTAG this);
LPPTAG TagsCopy(LPPTAG tDest, LPPTAG tSrc);
int TagsCompare(LPPTAG this, LPPTAG that);
BOOL TagsContains(LPPTAG this, wchar_t * lpText);
BOOL TagsContainsOld(LPPTAG this, wchar_t * lpText);
int TagsCount(LPPTAG this);
LPPTAG TagsRemove(LPPTAG this, wchar_t * lpText);
void TagsReplace(LPPTAG this, wchar_t * lpNew, wchar_t * lpOld);
void TagsGetOld(LPPTAG this, wchar_t * lpText, wchar_t * lpOld);

#endif	// TAGS_H__
