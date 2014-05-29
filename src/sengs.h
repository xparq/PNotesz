#ifndef __SENGS_H__
#define __SENGS_H__

#include <windows.h>
#include <wchar.h>
#include <windowsx.h>

#include "structures.h"

void SEngsFree(LPPSENG this);
LPPSENG SEngsAdd(LPPSENG this, wchar_t * lpName, wchar_t * lpQuery);
LPPSENG SEngsAddWithCommandLine(LPPSENG this, wchar_t * lpName, wchar_t * lpQuery, wchar_t * lpCommand);
LPPSENG SEngsCopy(LPPSENG tDest, LPPSENG tSrc);
void SEngsReplace(LPPSENG this, wchar_t * lpOldName, wchar_t * lpNewName, wchar_t * lpNewQuery);
void SEngsReplaceWithCommandLine(LPPSENG this, wchar_t * lpOldName, wchar_t * lpNewName, wchar_t * lpNewQuery, wchar_t * lpNewCommand);
LPPSENG SEngsRemove(LPPSENG this, wchar_t * lpName);
BOOL SEngsContains(LPPSENG this, wchar_t * lpName);
int SEngsCount(LPPSENG this);
LPPSENG SEngByName(LPPSENG this, wchar_t * lpName);
BOOL SEngsDifference(LPPSENG lp1, LPPSENG lp2);

#endif	// SENGS_H__
