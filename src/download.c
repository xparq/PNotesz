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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winbase.h>
#include <wchar.h>
#include <tchar.h>
#include <wininet.h>
#include "download.h"
#include "ineterror.h"

DWORD WINAPI DownloadFileFunc(LPVOID lpParam){
	HINTERNET 		hInternet, hRemoteFile;		//internet handles
	HANDLE			hLocalFile;					//local file handle
	wchar_t			szLengthBuffer[128];		//buffer to get the size of remote file
	BYTE			bytes[4096];				//buffer to read from remote file
	DWORD 			bytesRead = 4096;			//number of bytes read
	DWORD 			bytesWritten;				//number of bytes written
	DWORD 			sizeReceived = 0;			//number of bytes received
	DWORD 			sizeTotal;					//total number of bytes to receive (size of remote file)
	DWORD 			sizeLength = 128;			//length of buffer receives the size of remote file
	BOOL			result = FALSE;				//result of read operation
	HWND			hwnd = (HWND)lpParam;		//window handle for posting messages

	//get handle to the current Internet session
	hInternet = InternetOpenW(L"InetURL/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hInternet){
		//get error information
		GetInternetErrorDescriptionW(gd_ErrDesc, 0);
		//post error information
		PostMessageW(hwnd, DNLM_INETERROR, 0, 0);
		//exit thread
		return 1;
	}
	//open remote resource
	hRemoteFile = InternetOpenUrlW(hInternet, gd_URL, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if(!hRemoteFile){
		//get error information
		GetInternetErrorDescriptionW(gd_ErrDesc, 0);
		//close handle
		InternetCloseHandle(hInternet);
		//post error information
		PostMessageW(hwnd, DNLM_INETERROR, 0, 0);
		//exit thread
		return 1;
	}
	//get total size of remote file
	HttpQueryInfoW(hRemoteFile, HTTP_QUERY_CONTENT_LENGTH, szLengthBuffer, &sizeLength, 0);
	sizeTotal = _wtol(szLengthBuffer);
	//create local file
	hLocalFile = CreateFileW(gd_LocalFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hLocalFile != INVALID_HANDLE_VALUE){
		do{
			//read remote file
			result = InternetReadFile(hRemoteFile, bytes, 4096, &bytesRead);
			if(!result){
				//get error information
				GetInternetErrorDescriptionW(gd_ErrDesc, 0);
				//post error information
				PostMessageW(hwnd, DNLM_INETERROR, 0, 0);
				break;
			}
			//write to local file
			WriteFile(hLocalFile, bytes, bytesRead, &bytesWritten, NULL);
			//increase number of bytes downloaded
			sizeReceived += bytesRead;
			//post progress message with total size as wParam and downloaded size as lParam
			PostMessageW(hwnd, DNLM_PROGRESS, sizeTotal, sizeReceived);
		}while(result && bytesRead > 0);
		//close local file
		CloseHandle(hLocalFile);
		if(!result){
			//in case of error delete local file
			DeleteFileW(gd_LocalFile);
			//exit thread
			return 1;
		}
		//post finish message
		PostMessageW(hwnd, DNLM_FINISHED, 0, 0);
	}
	//close handles
	InternetCloseHandle(hRemoteFile);
	InternetCloseHandle(hInternet);
	//exit thread
	return 0;
}

