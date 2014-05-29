#ifndef __CONTGROUPS_H__
#define __CONTGROUPS_H__

#include <windows.h>

#include "structures.h"

LPPCONTGROUP PContGroupsAdd(LPPCONTGROUP * this, LPPCONTGROUP lpgr, BOOL fNewId);
void PContGroupsFree(LPPCONTGROUP * this);
LPPCONTGROUP PContGroupsCopy(LPPCONTGROUP * tDest, LPPCONTGROUP tSrc);
int PContGroupsNewId(LPPCONTGROUP this);
BOOL PContGroupsContains(LPPCONTGROUP this, wchar_t * lpName);
int PContGroupsCount(LPPCONTGROUP this);
BOOL PContGroupsDifference(LPPCONTGROUP pc1, LPPCONTGROUP pc2);
LPPCONTGROUP PContGroupsRemove(LPPCONTGROUP * this, wchar_t * lpName);
LPPCONTGROUP PContGroupsItem(LPPCONTGROUP this, wchar_t * lpName);
LPPCONTGROUP PContGroupsItemById(LPPCONTGROUP this, int id);

LPPCONTGROUP		g_TempContGroups;
PCONTGROUP			g_ContGroup;

#endif	// CONTGROUPS_H__
