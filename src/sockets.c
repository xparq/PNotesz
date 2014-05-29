// Copyright (C) 2011 Andrey Gruber (aka lamer)

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

#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>
#include <math.h>
#include <winsock2.h>
#include <richedit.h>

#include "shared.h"
#include "sockets.h"
#include "note.h"
#include "stringconstants.h"
#include "memorynotes.h"
#include "numericconstants.h"
#include "enums.h"
#include "encryption.h"
#include "contacts.h"

#include <stdlib.h>

//commstrl.h macros
#define MAKEIPADDRESS(b1,b2,b3,b4)  ((LPARAM)(((DWORD)(b1)<<24)+((DWORD)(b2)<<16)+((DWORD)(b3)<<8)+((DWORD)(b4))))
#define FIRST_IPADDRESS(x)  ((x>>24)&0xFF)
#define SECOND_IPADDRESS(x) ((x>>16)&0xFF)
#define THIRD_IPADDRESS(x)  ((x>>8)&0xFF)
#define FOURTH_IPADDRESS(x) (x&0xFF)

#define CHUNK_SIZE 				512

DWORD WINAPI ClientFunc(LPVOID lpvData);
static BOOL ProcessData(ULONG bytesReceived, LPBYTE data);

void GetHostProperties(char * lpName){
	DWORD			nSize = 256;
	char			szName[256];
	HOSTENT 		*host;

	lpName[0] = '\0';

	GetComputerName(szName, &nSize);
	strcpy(lpName, szName);
	host = gethostbyname(szName);
	if(host){
		strcpy(lpName, szName);
	}
}

DWORD WINAPI SocketClientFunc(LPVOID lpParam){
	LPBYTE			pData = NULL, pTemp = NULL;
	ULONG			cbSize = 0, cbTotal = 0;
	char 			buff[1024], server_address[256];
	int				nSize;

	pData = GetDataToSend(g_wsTempID, &cbSize);
	if(!pData){
		PostMessageW(g_wsCaller, WSPM_ERROR, E_POINTER, 0);
		return -1;
	}
	pTemp = pData;
	cbTotal = cbSize;
	strcpy(server_address, g_wsAddress);
	
	//step 1- create socket
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(my_sock < 0)
	{
		PostMessageW(g_wsCaller, WSPM_ERROR, WSAGetLastError(), 0);
		free(pData);
		return -1;
	}

	//step 2 - estabilish connection
	//fill sockaddr_in structure – server address and port
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(g_wsDefPort);
	HOSTENT *hst;
	//convert IP-address from symbolic to network format
	if(inet_addr(server_address) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(server_address);
	else
	{
		//try to get IP-address by server domain name
		if((hst = gethostbyname(server_address)))
			// hst->h_addr_list contains not array of addresses, but array of pointers to addresses
			((unsigned long *)&dest_addr.sin_addr)[0] =	((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			PostMessageW(g_wsCaller, WSPM_ERROR, WSAEFAULT, 0);
			closesocket(my_sock);
			free(pData);
			return -1;
		}
	}

	//we've got an address of server - try to estabilish a connection
	if(connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		PostMessageW(g_wsCaller, WSPM_ERROR, WSAGetLastError(), 0);
		free(pData);
		closesocket(my_sock);
		return -1;
	}

	//receive and send in cycle
	while((nSize = recv(my_sock, buff, sizeof(buff)-1, 0)) != SOCKET_ERROR){
		buff[nSize] = 0;	//add null symbol to reply
		if(!strcmp(buff, REPLY_WELCOME)){
			//if just connected - start to send data
			if(cbTotal >= CHUNK_SIZE){
				send(my_sock, (char *)pTemp, CHUNK_SIZE, 0);
				cbTotal -= CHUNK_SIZE;
				pTemp += CHUNK_SIZE;
			}
			else{
				//less than CHUNK_SIZE data available - send remained data
				send(my_sock, (char *)pTemp, cbTotal, 0);
				cbTotal = 0;
			}
		}
		else if(!strcmp(buff, REPLY_OK)){
			//packet received successfully
			PostMessageW(g_wsCaller, WSPM_PROGRESS, floor(((double)(cbSize - cbTotal) / (double)cbSize) * 100.0), 0);
			if(cbTotal == 0){
				//all data sent - send EOF message
				send(my_sock, MSG_EOF, strlen(MSG_EOF), 0);
			}
			else if(cbTotal >= CHUNK_SIZE){
				//more than CHUNK_SIZE data available - send CHUNK_SIZE
				send(my_sock, (char *)pTemp, CHUNK_SIZE, 0);
				cbTotal -= CHUNK_SIZE;
				pTemp += CHUNK_SIZE;
			}
			else{
				//less than CHUNK_SIZE data available - send remained data
				send(my_sock, (char *)pTemp, cbTotal, 0);
				cbTotal = 0;
			}
		}
		else if(!strcmp(buff, REPLY_DONE)){
			//all transfer done
			PostMessageW(g_wsCaller, WSPM_SEND_FINISHED, 0, 0);
			closesocket(my_sock);
			free(pData);
			return 0;
		}
	}
	closesocket(my_sock);
	free(pData);
	return -1;
}

int SendData(LPBYTE pData, int cbSize){
	LPBYTE			pTemp = pData;
	ULONG			cbTotal = cbSize;
	char 			buff[1024], server_address[256];
	int				nSize;


	if(!pData || !cbSize){
		return -1;
	}

	strcpy(server_address, g_wsAddress);
	
	//step 1- create socket
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(my_sock < 0)
	{
		PostMessageW(g_wsCaller, WSPM_ERROR, WSAGetLastError(), 0);
		free(pData);
		return -1;
	}

	//step 2 - estabilish connection
	//fill sockaddr_in structure – server address and port
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(g_wsDefPort);
	HOSTENT *hst;
	//convert IP-address from symbolic to network format
	if(inet_addr(server_address) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(server_address);
	else
	{
		//try to get IP-address by server domain name
		if((hst = gethostbyname(server_address)))
			// hst->h_addr_list contains not array of addresses, but array of pointers to addresses
			((unsigned long *)&dest_addr.sin_addr)[0] =	((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			PostMessageW(g_wsCaller, WSPM_ERROR, WSAEFAULT, 0);
			closesocket(my_sock);
			free(pData);
			return -1;
		}
	}
	//we've got an address of server - try to estabilish a connection
	if(connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		PostMessageW(g_wsCaller, WSPM_ERROR, WSAGetLastError(), 0);
		free(pData);
		closesocket(my_sock);
		return -1;
	}
	//receive and send in cycle
	while((nSize = recv(my_sock, buff, sizeof(buff)-1, 0)) != SOCKET_ERROR){
		buff[nSize] = 0;	//add null symbol to reply
		if(!strcmp(buff, REPLY_WELCOME)){
			//if just connected - start to send data
			if(cbTotal >= CHUNK_SIZE){
				send(my_sock, (char *)pTemp, CHUNK_SIZE, 0);
				cbTotal -= CHUNK_SIZE;
				pTemp += CHUNK_SIZE;
			}
			else{
				//less than CHUNK_SIZE data available - send remained data
				send(my_sock, (char *)pTemp, cbTotal, 0);
				cbTotal = 0;
			}
		}
		else if(!strcmp(buff, REPLY_OK)){
			//packet received successfully
			PostMessageW(g_wsCaller, WSPM_PROGRESS, floor(((double)(cbSize - cbTotal) / (double)cbSize) * 100.0), 0);
			if(cbTotal == 0){
				//all data sent - send EOF message
				send(my_sock, MSG_EOF, strlen(MSG_EOF), 0);
			}
			else if(cbTotal >= CHUNK_SIZE){
				//more than CHUNK_SIZE data available - send CHUNK_SIZE
				send(my_sock, (char *)pTemp, CHUNK_SIZE, 0);
				cbTotal -= CHUNK_SIZE;
				pTemp += CHUNK_SIZE;
			}
			else{
				//less than CHUNK_SIZE data available - send remained data
				send(my_sock, (char *)pTemp, cbTotal, 0);
				cbTotal = 0;
			}
		}
		else if(!strcmp(buff, REPLY_DONE)){
			//all transfer done
			PostMessageW(g_wsCaller, WSPM_SEND_FINISHED, 0, 0);
			closesocket(my_sock);
			free(pData);
			return 0;
		}
	}
	closesocket(my_sock);
	free(pData);
	return -1;
}

BOOL InitializeWinsock(void){
	//initialize Winsock
	int		iResult;
	char	buff[1024];

	if((iResult = WSAStartup(0x0202,(WSADATA *)buff)))
	{
		//error
		PostMessageW(g_hMain, WSPM_ERROR, iResult, 0);
		return FALSE;
	}
	g_wsReadMutex = CreateMutex(NULL, FALSE, READ_MUTEX);
	if(!g_wsReadMutex){
		//error
		PostMessageW(g_hMain, WSPM_ERROR, GetLastError(), 0);
		return FALSE;
	}
	g_wsStopServer = FALSE;
	return TRUE;
}

void DeInitializeWinsock(void){
	g_wsStopServer = TRUE;
	WSACleanup();
	CloseHandle(g_wsReadMutex);
}

DWORD WINAPI SocketServerFunc(LPVOID lpParam){
	HWND					hCaller;

	g_wsServerUp = TRUE;
	hCaller = (HWND)lpParam;

	//step 1 - create socket
	SOCKET mysocket;
	//AF_INET - internet
	//SOCK_STREAM - stream socket (with connection)
	//0 - TCP protocol (default)
	if((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		//error
		PostMessageW(hCaller, WSPM_ERROR, WSAGetLastError(), 0);
		g_wsServerUp = FALSE;
		return -1;
	}

	//step 2 - binding socket to local address
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(g_wsDefPort); 	//do not forget about network order
	local_addr.sin_addr.s_addr = 0; 			//server accept connections to all its IP-addresses

	//binding
	if (bind(mysocket,(sockaddr *)&local_addr, sizeof(local_addr)))
	{
		//error
		PostMessageW(hCaller, WSPM_ERROR, WSAGetLastError(), 0);
		closesocket(mysocket); //close socket
		g_wsServerUp = FALSE;
		return -1;
	}

	//step 3 - waiting for connections
	//queue size – 0x100
	if(listen(mysocket, 0x100))
	{
		//error
		PostMessageW(hCaller, WSPM_ERROR, WSAGetLastError(), 0);
		closesocket(mysocket);
		g_wsServerUp = FALSE;
		return -1;
	}

	//step 4 - fetch message from queue
	SOCKET client_socket; 						//client socket
	sockaddr_in client_addr; 					//client address (filled by system)
	int client_addr_size = sizeof(client_addr);	//'accept' function requires size of structure

	//cycle fetching connection requests from queue
	while((client_socket = accept(mysocket, (sockaddr *) &client_addr, &client_addr_size))){
		if(g_wsStopServer){
			g_wsServerUp = FALSE;
			return -1;
		}
		DWORD thID;
		HANDLE	hClient = CreateThread(NULL, 0, ClientFunc, &client_socket, 0, &thID);
		if(hClient)
			CloseHandle(hClient);
		ReleaseMutex(g_wsReadMutex);
	}

	g_wsServerUp = FALSE;
	return 0;
}

DWORD WINAPI ClientFunc(LPVOID lpvData){
	SOCKET						my_sock = ((SOCKET *)lpvData)[0];
	int 						bytes_recv = 0;
	LPBYTE						lpBuffer = NULL, lpCurrent = NULL, lpTemp;
	ULONG						cbReceived = 0;

	lpTemp = calloc(1, CHUNK_SIZE);
	if(!lpTemp){
		return -1;
	}
	//send Welcome to client
	send(my_sock, REPLY_WELCOME, strlen(REPLY_WELCOME), 0);
	while(TRUE){
		ZeroMemory(lpTemp, CHUNK_SIZE);
		bytes_recv = recv(my_sock, (char *)lpTemp, CHUNK_SIZE ,0);
		if(!bytes_recv || bytes_recv == SOCKET_ERROR){
			//client disconnected - exit loop
			free(lpTemp);
			break;
		}
		if(strcmp((char *)lpTemp, MSG_EOF)){
			cbReceived += bytes_recv;
			if(lpBuffer == NULL)
				lpBuffer = malloc(cbReceived);
			else
				lpBuffer = realloc(lpBuffer, cbReceived);

			if(!lpBuffer){
				PostMessageW(g_hMain, WSPM_ERROR, WSA_NOT_ENOUGH_MEMORY, 0);
				free(lpTemp);
				return -1;
			}
			lpCurrent = lpBuffer + (cbReceived - bytes_recv);
			memcpy(lpCurrent, lpTemp, bytes_recv);
			send(my_sock, REPLY_OK, strlen(REPLY_OK), 0);
		}
		else{
			send(my_sock, REPLY_DONE, strlen(REPLY_DONE), 0);
		}
	}
	//save received data
	while(g_wsPause)
		;
	g_wsPause = TRUE;
	if(ProcessData(cbReceived, lpBuffer)){
		SendMessageW(g_hMain, WSPM_RECEIVE_FINISHED, 0, 0);
	}
	else{
		PostMessageW(g_hMain, WSPM_DATA_SAVING_ERROR, 0, 0);
	}

	//free memory
	free(lpBuffer);
	//close socket
	closesocket(my_sock);
	//end thread
	return 0;
}

LPBYTE GetDataToSend(wchar_t * lpID, ULONG * pSize){
	LPBYTE					pData = NULL, pTemp = NULL;
	PMEMNOTE				pNote;
	HANDLE					hFile = INVALID_HANDLE_VALUE;
	wchar_t					szPath[MAX_PATH], szTempPath[MAX_PATH + 128], szHash[256];
	DWORD					size, dwRead;	
	BOOL					fDeleteTemp = FALSE;

	pNote = MemNoteById(lpID);
	if(!pNote)
		return NULL;

	//load RTF file contents
	if(pNote->pFlags->fromDB && pNote->pFlags->saved){
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, lpID);
		wcscat(szPath, NOTE_EXTENTION);
		if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
			GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szHash, 256, g_NotePaths.INIFile);
			GetNoteTempFileName(szTempPath);
			if(CryptDecryptFile(szPath, szTempPath, szHash)){
				hFile = CreateFileW(szTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			}
		}
		else{
			hFile = CreateFileW(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		if(hFile != INVALID_HANDLE_VALUE){
			size = GetFileSize(hFile, NULL);
			if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
				fDeleteTemp = TRUE;
			}
		}
		else{
			//error reading file
			return NULL;
		}
	}
	else{
		EDITSTREAM			esm;
		HWND				hEdit;

		hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		GetNoteTempFileName(szTempPath);
		hFile = CreateFileW(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			esm.dwCookie = (DWORD)hFile;
			esm.dwError = 0;
			esm.pfnCallback = SharedOutStreamCallback;
			RichEdit_StreamOut(hEdit, SF_RTF | SF_UNICODE, &esm);
			CloseHandle(hFile);
			hFile = CreateFileW(szTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE){
				size = GetFileSize(hFile, NULL);
			}
			else{
				//error opening file
				DeleteFileW(szTempPath);
				return NULL;
			}
			fDeleteTemp = TRUE;
		}
		else{
			//error creating file
			return NULL;
		}
	}
	*pSize = size + 
		256 +
		sizeof(NOTE_APPEARANCE) + 
		sizeof(NOTE_DATA) + 
		sizeof(SCHEDULE_TYPE) + 
		sizeof(ADDITIONAL_NAPP) + 
		sizeof(SYSTEMTIME) + 
		sizeof(SYSTEMTIME) + 
		sizeof(SYSTEMTIME);

	pData = calloc(1, *pSize);
	pTemp = pData;
	//add sender name
	strcpy((char *)pTemp, g_wsLocalHost);	
	pTemp += 256;
	//add note appearance
	memcpy(pTemp, pNote->pAppearance, sizeof(NOTE_APPEARANCE));
	pTemp += sizeof(NOTE_APPEARANCE);
	//add note data
	memcpy(pTemp, pNote->pData, sizeof(NOTE_DATA));
	pTemp += sizeof(NOTE_DATA);
	//add note schedule
	memcpy(pTemp, pNote->pSchedule, sizeof(SCHEDULE_TYPE));
	pTemp += sizeof(SCHEDULE_TYPE);
	//add additional note appearance
	memcpy(pTemp, pNote->pAddNApp, sizeof(ADDITIONAL_NAPP));
	pTemp += sizeof(ADDITIONAL_NAPP);
	//add creation time
	memcpy(pTemp, pNote->pCreated, sizeof(SYSTEMTIME));
	pTemp += sizeof(SYSTEMTIME);
	//add deletion time
	if(pNote->pDeleted->wDay != 0)
		memcpy(pTemp, pNote->pDeleted, sizeof(SYSTEMTIME));
	pTemp += sizeof(SYSTEMTIME);
	//add real deletion time
	if(pNote->pRealDeleted->wDay != 0)
		memcpy(pTemp, pNote->pRealDeleted, sizeof(SYSTEMTIME));
	pTemp += sizeof(SYSTEMTIME);
	//add file content
	ReadFile(hFile, pTemp, size, &dwRead, NULL);
	CloseHandle(hFile);
	if(fDeleteTemp)
		DeleteFileW(szTempPath);


	return pData;
}

static BOOL ProcessData(ULONG bytesReceived, LPBYTE data){
	wchar_t				szID[128], szName[128], szPath[MAX_PATH], szTempPath[MAX_PATH + 128], szHash[256], *pName;
	LPBYTE				pTemp = data;
	DWORD				dwWritten, bytesToWrite = bytesReceived;
	HANDLE 				hFile;
	NOTE_DATA			nd = {0};
	SRSTATUS			srs = {0};
	LPSYSTEMTIME		lpst = NULL;
	RECT				rcD;
	int					w, h;
	char				senderName[256];

	FormatNewNoteID(szID);
	//get sender name
	strcpy(senderName, (char *)pTemp);
	MultiByteToWideChar(CP_ACP, 0, senderName, -1, szName, 256);
	//increase pointer
	pTemp += 256;
	// memcpy(&sd, pTemp, sizeof(SOCKET_SENDER_DATA));
	pName = PContactsNameByHost(g_PContacts, szName);
	if(pName)
		wcscpy(srs.recFrom, pName);
	else
		wcscpy(srs.recFrom, szName);
	GetLocalTime(&srs.lastRec);
	WritePrivateProfileStructW(szID, S_SEND_RECEIVE_STATUS, &srs, sizeof(SRSTATUS), g_NotePaths.DataFile);
	WritePrivateProfileStructW(szID, S_APPEARANCE, pTemp, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	pTemp += sizeof(NOTE_APPEARANCE);
	bytesToWrite -= sizeof(NOTE_APPEARANCE);
	memcpy(&nd, pTemp, sizeof(NOTE_DATA));
	nd.visible = FALSE;
	nd.idGroup = GROUP_INCOMING;
	//set received note initially centered
	GetWindowRect(GetDesktopWindow(), &rcD);
	w = nd.rcp.right - nd.rcp.left;
	h = nd.rcp.bottom - nd.rcp.top;
	nd.rcp.left = ((rcD.right - rcD.left) - w) / 2;
	nd.rcp.top = ((rcD.bottom - rcD.top) - h) / 2;
	nd.rcp.right = nd.rcp.left + w;
	nd.rcp.bottom = nd.rcp.top + h;
	//undock possible docked note
	nd.dockData = MAKELONG(DOCK_NONE, -777);
	WritePrivateProfileStructW(szID, S_DATA, &nd, sizeof(NOTE_DATA), g_NotePaths.DataFile);
	pTemp += sizeof(NOTE_DATA);
	bytesToWrite -= sizeof(NOTE_DATA);
	WritePrivateProfileStructW(szID, S_SCHEDULE, pTemp, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
	pTemp += sizeof(SCHEDULE_TYPE);
	bytesToWrite -= sizeof(SCHEDULE_TYPE);
	WritePrivateProfileStructW(szID, S_ADD_APPEARANE, pTemp, sizeof(ADDITIONAL_NAPP), g_NotePaths.DataFile);
	pTemp += sizeof(ADDITIONAL_NAPP);
	bytesToWrite -= sizeof(ADDITIONAL_NAPP);
	WritePrivateProfileStructW(szID, S_NOTE_CREATION, pTemp, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
	pTemp += sizeof(SYSTEMTIME);
	bytesToWrite -= sizeof(SYSTEMTIME);
	lpst = (LPSYSTEMTIME)pTemp;
	if(lpst->wDay != 0)
		WritePrivateProfileStructW(szID, IK_DELETION_TIME, pTemp, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
	pTemp += sizeof(SYSTEMTIME);
	bytesToWrite -= sizeof(SYSTEMTIME);
	lpst = (LPSYSTEMTIME)pTemp;
	if(lpst->wDay != 0)
		WritePrivateProfileStructW(szID, IK_REAL_DELETION_TIME, pTemp, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
	pTemp += sizeof(SYSTEMTIME);
	bytesToWrite -= sizeof(SYSTEMTIME);
	//prepare note file path
	wcscpy(szPath, g_NotePaths.DataDir);
	wcscat(szPath, szID);
	wcscat(szPath, NOTE_EXTENTION);
	if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
		GetNoteTempFileName(szTempPath);
		hFile = CreateFileW(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else{
		hFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}
	if(hFile != INVALID_HANDLE_VALUE){
		WriteFile(hFile, pTemp, bytesToWrite, &dwWritten, NULL);
		CloseHandle(hFile);
		if(dwWritten == bytesToWrite){
			if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
				GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szHash, 256, g_NotePaths.INIFile);
				if(CryptEncryptFile(szTempPath, szPath, szHash)){
					DeleteFileW(szTempPath);
					wcscpy(g_wsID, szID);
					return TRUE;
				}
			}
			else{
				wcscpy(g_wsID, szID);
				return TRUE;
			}
		}
	}
	return FALSE;
}
