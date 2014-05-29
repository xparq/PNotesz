// Copyright (C) 2008 Andrey Gruber (aka lamer)

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
#include <windowsx.h>
#include <wchar.h>
#include <mapi.h>

static void ReplaceExtention(char * lpSource, char * lpDest);

static ULONG (PASCAL *SendMAPIMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);

BOOL SendAsAttachment(HWND hParent, wchar_t ** lpAttachment, int count, wchar_t * lpSubject, wchar_t * lpTemp){

	HINSTANCE 		hMAPI;
	char			szSubject[MAX_PATH], szTempANSI[MAX_PATH];
	MapiFileDesc 	*fDesc, *fDescTemp;
	MapiMessage 	message;
	int				result;
	BOOL			replaced = FALSE;
	wchar_t			szTempPath[MAX_PATH - 14], szTempFileName[MAX_PATH], **pArray;
	char			**pNames, **pPaths;
	char			**pFreeNames, **pFreePaths;

	*lpTemp = '\0';

	hMAPI = LoadLibrary("MAPI32.DLL");
	if(!hMAPI)
		return FALSE;

	(FARPROC)SendMAPIMail = GetProcAddress(hMAPI, "MAPISendMail");
	if(!SendMAPIMail){
		FreeLibrary(hMAPI);
		return FALSE;
	}

	pNames = (char **)calloc(count, sizeof(char *));
	pFreeNames = pNames;
	pPaths = (char **)calloc(count, sizeof(char *));
	pFreePaths = pPaths;

	fDesc = (MapiFileDesc *)calloc(count, sizeof(MapiFileDesc));
	fDescTemp = fDesc;
	pArray = lpAttachment;
	for(int i = 0; i < count; i++){
		replaced = FALSE;
		*pNames = (calloc)(MAX_PATH + wcslen(*pArray) + 8, sizeof(char));
		*pPaths = (calloc)(MAX_PATH + wcslen(*pArray) + 8, sizeof(char));
		WideCharToMultiByte(CP_ACP, 0, *pArray, -1, *pNames, MAX_PATH, "a", &replaced);
		if(replaced){
			GetTempPathW(MAX_PATH - 14, szTempPath);
			GetTempFileNameW(szTempPath, L"tpt", 1, szTempFileName);
			WideCharToMultiByte(CP_ACP, 0, szTempFileName, -1, szTempANSI, MAX_PATH, NULL, NULL);
			ReplaceExtention(*pNames, szTempANSI);
			MultiByteToWideChar(CP_ACP, 0, szTempANSI, -1, szTempFileName, MAX_PATH);
			strcpy(*pNames, szTempANSI);
			CopyFileW(*pArray, szTempFileName, FALSE);
			wcscpy(lpTemp, szTempFileName);
		}
		strcpy(*pPaths, *pNames);
		
		fDescTemp->nPosition = -1;
		fDescTemp->lpszPathName = *pPaths;
		fDescTemp->lpszFileName = *pNames;

		pNames++;
		pPaths++;
		fDescTemp++;
		pArray++;
	}

	if(lpSubject){
		WideCharToMultiByte(CP_ACP, 0, lpSubject, -1, szSubject, MAX_PATH, NULL, NULL);
	}
	else{
		*szSubject = '\0';
	}

	ZeroMemory(&message, sizeof(message));
	message.lpszSubject = szSubject;
	message.nFileCount = count;
	message.lpFiles = fDesc;

	result = SendMAPIMail(0, (ULONG_PTR)hParent, &message, MAPI_LOGON_UI | MAPI_DIALOG, 0);

	FreeLibrary(hMAPI);
	free(fDesc);
	pNames = pFreeNames;
	for(int i = 0; i < count; i++){
		free(*pNames++);
	}
	free(pFreeNames);
	pPaths = pFreePaths;
	for(int i = 0; i < count; i++){
		free(*pPaths++);
	}
	free(pFreePaths);

	if (result != SUCCESS_SUCCESS && result != MAPI_USER_ABORT && result != MAPI_E_LOGIN_FAILURE)
		return FALSE;

	return TRUE;
}

static void ReplaceExtention(char * lpSource, char * lpDest){
	while(*lpSource++)
		;
	while(*lpDest++)
		;
	while(*(--lpSource) != '.')
		;
	while(*(--lpDest) != '.')
		;
	while((*lpDest++ = *lpSource++))
		;
}
