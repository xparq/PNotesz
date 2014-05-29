#ifndef __REGISTRYCLEAN_H__
#define __REGISTRYCLEAN_H__

#include <windows.h>
#include <shlwapi.h>

void CleanRegMUICache(const char * lpcProgName);
void CleanRegMRU(const char * lpcProgName);
void CleanRegOpenWithList(const char * lpcProgName);
void CleanRegOpenSaveMRU(const char * lpcProgName);

#endif	// REGISTRYCLEAN_H__
