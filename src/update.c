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

#define WIN32_LEAN_AND_MEAN

// #ifndef _WIN32_WINNT
// #define _WIN32_WINNT 0x0501
// #endif

#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>
#include <wininet.h>
#include <ctype.h>
#include "update.h"
#include "ineterror.h"

DWORD WINAPI CheckForUpdateFunc(LPVOID lpParam){
	HINTERNET 		hINet, hConnection, hData;
	char 			szBuffer[12], szAddress[512], szFile[256], szVersion[12];
	DWORD 			dwRead;
	HWND			hwnd;

	hwnd = (HWND)lpParam;

	SendMessage(hwnd, UPDM_GETSTRINGS, (WPARAM)szAddress, (LPARAM)szFile);
	SendMessage(hwnd, UPDM_GETVERSION, (WPARAM)szVersion, 0);

	hINet = InternetOpen("InetURL/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hINet){
		GetInternetErrorDescription(gu_ErrDesc, 0);
		PostMessage(hwnd, UPDM_INETERROR, 0, 0);
		return 1;
	}
	hConnection = InternetConnect(hINet, szAddress, 80, " ", " ", INTERNET_SERVICE_HTTP, 0, 0);
	if(!hConnection){
		GetInternetErrorDescription(gu_ErrDesc, 0);
		InternetCloseHandle(hINet);
		PostMessage(hwnd, UPDM_INETERROR, 0, 0);
		return 1;
	}
	hData = HttpOpenRequest(hConnection, "GET", szFile, NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);
	if(!hData){
		GetInternetErrorDescription(gu_ErrDesc, 0);
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hINet);
		PostMessage(hwnd, UPDM_INETERROR, 0, 0);
		return 1;
	}
	HttpSendRequest(hData, NULL, 0, NULL, 0);
	if(InternetReadFile(hData, szBuffer, 12, &dwRead)){
		if(dwRead > 0){
			char	*p1, *p2;
			szBuffer[dwRead] = 0;
			p1 = strchr(szBuffer, '.');
			p2 = strrchr(szBuffer, '.');
			if(p1 && p2 && p1 < p2 && p1 > szBuffer){
				if(isdigit(szBuffer[0]) && isdigit(szBuffer[dwRead - 1])){
					if(strcmp(szVersion, szBuffer) < 0){
						PostMessage(hwnd, UPDM_UPDATEFOUND, (WPARAM)szBuffer, 0);
					}
					else{
						PostMessage(hwnd, UPDM_SAMEVERSION, 0, 0);
					}
				}
				else{
					strcpy(gu_ErrDesc, "Error reading file or file does not match.");
					PostMessage(hwnd, UPDM_INETERROR, 0, 0);
				}
			}
			else{
				strcpy(gu_ErrDesc, "Error reading file or file does not match.");
				PostMessage(hwnd, UPDM_INETERROR, 0, 0);
			}
		}
		else{
			GetInternetErrorDescription(gu_ErrDesc, 0);
			PostMessage(hwnd, UPDM_INETERROR, 0, 0);
		}
	}
	else{
		GetInternetErrorDescription(gu_ErrDesc, 0);
		PostMessage(hwnd, UPDM_INETERROR, 0, 0);
	}
	InternetCloseHandle(hData);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hINet);
	return 0;
}
