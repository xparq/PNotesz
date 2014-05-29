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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif // Windows 2000 and up

#ifndef	COBJMACROS
#define COBJMACROS
#endif

#include <windows.h>
#include <winbase.h>
#include <wchar.h>
#include <wininet.h>
#include <shlwapi.h>
#include <commdlg.h>
#include <stdio.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <dlgs.h>
#include "sync.h"
#include "ineterror.h"
#include "memorynotes.h"
#include "enums.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "shared.h"
#include "fdialogs.h"
#include "global.h"
#include "localsync.h"

#define	PROP_ACTION			L"PROP_ACTION"

static BOOL AreFilesDifferentFTP(long size1, long size2, wchar_t * lpLocal, wchar_t * lpRemote, HINTERNET hConnection);
static BOOL AreLocalFilesDifferent(long size1, long size2, wchar_t * lpFile1, wchar_t * lpFile2);
static void GetRemoteFileProperties(wchar_t * lpID, wchar_t * szTempIndexFile);
static void ExchangeLocalFilesProperties(wchar_t * id, wchar_t * lpFileFrom, wchar_t * lpFileTo);
static int GetRemoteNoteGroup(wchar_t * lpFile, wchar_t * szTempIndexFile);
static int GetLocalFileGroup(wchar_t * id, wchar_t * szTempIndexFile);
static void LocalSync_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL LocalSync_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void LocalSync_OnClose(HWND hwnd);
BOOL CALLBACK NewSync_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NewSync_OnClose(HWND hwnd);
static BOOL NewSync_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void NewSync_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static UINT APIENTRY OFNFolderHook( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
static LRESULT CALLBACK OFNFolderProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL IsLastBackslash(wchar_t * src);

static WNDPROC		m_hOFWndProc;
static wchar_t		m_FolderPath[MAX_PATH];

static BOOL IsLastBackslash(wchar_t * src){
	wchar_t 	* temp;

	temp = src;
	while(*temp++)
		;
	*temp--;
	*temp--;
	if(*temp == '\\')
		return TRUE;
	else
		return FALSE;
}

local_sync_results LocalSyncFunc(wchar_t pathNotes1[MAX_PATH], wchar_t pathId1[MAX_PATH], wchar_t pathNotes2[MAX_PATH], wchar_t pathId2[MAX_PATH]){
	HANDLE				hFile1 = NULL, hFile2 = NULL;
	wchar_t				pathTemp1[MAX_PATH], pathTemp2[MAX_PATH], id[128];
	WIN32_FIND_DATAW	fd1 = {0}, fd2 = {0};
	BOOL				result = TRUE, needReload = FALSE, isIndexFile = FALSE;
	PMEMNOTE			pNote;
	long				compTime;
	int					group;

	wcscpy(pathTemp2, pathNotes2);
	if(!IsLastBackslash(pathTemp2))
		wcscat(pathTemp2, L"\\");
	wcscat(pathTemp2, L"*.pnote");
	isIndexFile = PathFileExistsW(pathId2);

	ClearLocalSyncStatus();

	__try{
		hFile2 = FindFirstFileW(pathTemp2, &fd2);
		if(hFile2 != INVALID_HANDLE_VALUE){
			while(result){
				group = 0;
				wcscpy(pathTemp2, pathNotes2);
				if(!IsLastBackslash(pathTemp2))
					wcscat(pathTemp2, L"\\");
				wcscat(pathTemp2, fd2.cFileName);
				wcscpy(id, fd2.cFileName);
				PathRemoveExtensionW(id);

				wcscpy(pathTemp1, pathNotes1);
				if(!IsLastBackslash(pathTemp1))
					wcscat(pathTemp1, L"\\");
				wcscat(pathTemp1, fd2.cFileName);

				pNote = MemNoteById(id);
				if(pNote){
					if(pNote->pData->idGroup != GROUP_RECYCLE || (pNote->pData->idGroup == GROUP_RECYCLE && IsBitOn(g_NextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE))){
						hFile1 = FindFirstFileW(pathTemp1, &fd1);
						if(hFile1 != INVALID_HANDLE_VALUE){
							FindClose(hFile1);
							hFile1 = NULL;
							compTime = CompareFileTime(&fd1.ftLastWriteTime, &fd2.ftLastWriteTime);
							if(compTime < 0){
								if(AreLocalFilesDifferent((fd1.nFileSizeHigh * MAXDWORD) + fd1.nFileSizeLow, (fd2.nFileSizeHigh * MAXDWORD) + fd2.nFileSizeLow, pathTemp1, pathTemp2)){
									pNote->pRTHandles->syncstatusLocal = SS_COPY_TO_SERVER_LOCAL;
								}
								else{
									pNote->pRTHandles->syncstatusLocal = SS_DO_NOT_COPY_LOCAL;
								}
							}
							else if(compTime > 0){
								if(AreLocalFilesDifferent((fd1.nFileSizeHigh * MAXDWORD) + fd1.nFileSizeLow, (fd2.nFileSizeHigh * MAXDWORD) + fd2.nFileSizeLow, pathTemp1, pathTemp2)){
									if(isIndexFile)
										group = GetLocalFileGroup(id, pathId2);
									if(group != GROUP_RECYCLE || (group == GROUP_RECYCLE && IsBitOn(g_NextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE))){
										pNote->pRTHandles->syncstatusLocal = SS_COPY_FROM_SERVER_LOCALP;
										if(!CopyFileW(pathTemp2, pathTemp1, FALSE))
											return LSR_FAILED;
										if(isIndexFile)
											ExchangeLocalFilesProperties(id, pathId2, pathId1);
										needReload = TRUE;
									}
									else{
										pNote->pRTHandles->syncstatusLocal = SS_DO_NOT_COPY_LOCAL;
									}
								}
								else{
									pNote->pRTHandles->syncstatusLocal = SS_DO_NOT_COPY_LOCAL;
								}
							}
							else{
								//leave file without syncing
								pNote->pRTHandles->syncstatusLocal = SS_DO_NOT_COPY_LOCAL;
							}
						}
						else{
							if(isIndexFile)
								group = GetLocalFileGroup(id, pathId2);
							if(group != GROUP_RECYCLE || (group == GROUP_RECYCLE && IsBitOn(g_NextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE))){
								needReload = TRUE;
								pNote->pRTHandles->syncstatusLocal = SS_COPY_FROM_SERVER_LOCALP;
								if(!CopyFileW(pathTemp2, pathTemp1, FALSE))
									return LSR_FAILED;
								if(isIndexFile)
									ExchangeLocalFilesProperties(id, pathId2, pathId1);
							}
						}
					}
				}
				else{
					if(isIndexFile)
						group = GetLocalFileGroup(id, pathId2);
					if(group != GROUP_RECYCLE || (group == GROUP_RECYCLE && IsBitOn(g_NextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE))){
						needReload = TRUE;
						if(!CopyFileW(pathTemp2, pathTemp1, FALSE))
							return LSR_FAILED;
						if(isIndexFile){
							ExchangeLocalFilesProperties(id, pathId2, pathId1);
						}
					}
				}
				result = FindNextFileW(hFile2, &fd2);
			}
			FindClose(hFile2);
			hFile2 = NULL;
		}
		if(!CopyFileW(pathId1, pathId2, FALSE)){
			return LSR_FAILED;
		}
		pNote = MemoryNotes();
		while(pNote){
			if(pNote->pRTHandles->syncstatusLocal == SS_COPY_TO_SERVER_LOCAL){
				if(pNote->pData->idGroup != GROUP_RECYCLE || (pNote->pData->idGroup == GROUP_RECYCLE && IsBitOn(g_NextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE))){
					wcscpy(pathTemp1, pathNotes1);
					if(!IsLastBackslash(pathTemp1))
						wcscat(pathTemp1, L"\\");
					wcscat(pathTemp1, pNote->pFlags->id);
					wcscat(pathTemp1, L".pnote");
					wcscpy(pathTemp2, pathNotes2);
					if(!IsLastBackslash(pathTemp2))
						wcscat(pathTemp2, L"\\");
					wcscat(pathTemp2, pNote->pFlags->id);
					wcscat(pathTemp2, L".pnote");
					if(!CopyFileW(pathTemp1, pathTemp2, FALSE)){
						return LSR_FAILED;
					}
				}
			}
			pNote = pNote->next;
		}
	}
	__finally{
		if(hFile1 && hFile1 != INVALID_HANDLE_VALUE)
			FindClose(hFile1);
		if(hFile2 && hFile2 != INVALID_HANDLE_VALUE)
			FindClose(hFile2);
	}
	//post finish message
	if(needReload)
		return LSR_OK_RELOAD;
	else
		return LSR_OK_SIMPLE;
}

DWORD WINAPI FTPSyncFunc(LPVOID lpParam){
	HINTERNET 			hINet = NULL, hConnection = NULL, hInetFile = NULL;
	wchar_t				szPath[MAX_PATH], szTempPath[MAX_PATH], *lpIndexFile, szLocalPath[MAX_PATH], szFileName[128];
	DWORD				dwPathSize = MAX_PATH;
	WIN32_FIND_DATAW	wf = {0}, wfFTP = {0};
	HANDLE				hFile;
	long				compTime;
	wchar_t				szErrMessage[4096];
	DWORD 				dwErr, dwSize = 4096;
	BOOL				result = TRUE, needReload = FALSE, isIndexFile = FALSE;
	HWND				hwnd = (HWND)lpParam;		//window handle for posting messages
	PMEMNOTE			pNote = NULL;
	int					group;

	gs_StopSync = FALSE;
	ClearSyncStatus();

	hINet = InternetOpenW(L"InetURL/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hINet){
		goto _exit_error;
	}
	hConnection = InternetConnectW(hINet, g_FTPData.server, g_FTPData.port, g_FTPData.user, g_FTPData.password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if(!hConnection){
		goto _exit_error;
	}
	if(!FtpGetCurrentDirectoryW(hConnection, szPath, &dwPathSize))
		goto _exit_error;
	if(wcslen(g_FTPData.directory) > 0)
		wcscat(szPath, g_FTPData.directory);
	if(!FtpSetCurrentDirectoryW(hConnection, szPath))
		goto _exit_error;
	//construct temp path for remote index file
	GetTempPathW(MAX_PATH, szTempPath);
	GetTempFileNameW(szTempPath, L"syn", 10, szTempPath);
	//get remote index file (if any)
	lpIndexFile = PathFindFileNameW(gs_DataFilePath);
	FtpGetFileW(hConnection, lpIndexFile, szTempPath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0);
	if(PathFileExistsW(szTempPath))
		isIndexFile = TRUE;
	//start remote directory enumeration
	hInetFile = FtpFindFirstFileW(hConnection, NULL, &wfFTP, INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_RESYNCHRONIZE, 0);
	if(hInetFile){
		while(hInetFile && result){
			group = 0;
			wchar_t * temp = PathFindExtensionW(wfFTP.cFileName);
			if(temp && wcscmp(temp, L".pnote") == 0){
				//build note path
				wcscpy(szLocalPath, gs_DataDir);
				wcscat(szLocalPath, wfFTP.cFileName);
				//get id
				wchar_t		id[128];
				wcscpy(id, wfFTP.cFileName);
				PathRemoveExtensionW(id);
				//get note by id
				pNote = MemNoteById(id);
				if(pNote){
					if(pNote->pData->idGroup != GROUP_RECYCLE || (pNote->pData->idGroup == GROUP_RECYCLE && IsBitOn(g_NoteSettings.reserved1, SB1_INC_DEL_IN_SYNC))){
						//get local file
						hFile = FindFirstFileW(szLocalPath, &wf);
						if(hFile != INVALID_HANDLE_VALUE){
							FindClose(hFile);
							//compare modification times
							FILETIME		ft1 = {0};
							//FTP time is always local time, so convert time of local file to local time
							FileTimeToLocalFileTime(&wf.ftLastWriteTime, &ft1);
							compTime = CompareFileTime(&ft1, &wfFTP.ftLastWriteTime);
							if(compTime > 0){
								if(AreFilesDifferentFTP((wf.nFileSizeHigh * MAXDWORD) + wf.nFileSizeLow, (wfFTP.nFileSizeHigh * MAXDWORD) + wfFTP.nFileSizeLow, szLocalPath, wfFTP.cFileName, hConnection)){
									//local file is newer - mark it for copy to FTP server
									pNote->pRTHandles->syncstatusFTP = SS_COPY_TO_SERVER_FTP;
								}
								else{
									pNote->pRTHandles->syncstatusFTP = SS_DO_NOT_COPY_FTP;
								}
							}
							else if(compTime < 0){
								if(AreFilesDifferentFTP((wf.nFileSizeHigh * MAXDWORD) + wf.nFileSizeLow, (wfFTP.nFileSizeHigh * MAXDWORD) + wfFTP.nFileSizeLow, szLocalPath, wfFTP.cFileName, hConnection)){
									//remote file is newer - copy it to local directory
									if(isIndexFile)
										group = GetRemoteNoteGroup(wfFTP.cFileName, szTempPath);
									if(group != GROUP_RECYCLE || (group == GROUP_RECYCLE && IsBitOn(g_NoteSettings.reserved1, SB1_INC_DEL_IN_SYNC))){
										pNote->pRTHandles->syncstatusFTP = SS_COPY_FROM_SERVER_FTP;
										if(!FtpGetFileW(hConnection, wfFTP.cFileName, szLocalPath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0))
											goto _exit_error;
										if(isIndexFile)
											GetRemoteFileProperties(wfFTP.cFileName, szTempPath);
										needReload = TRUE;
									}
									else{
										pNote->pRTHandles->syncstatusFTP = SS_DO_NOT_COPY_FTP;
									}
								}
								else{
									pNote->pRTHandles->syncstatusFTP = SS_DO_NOT_COPY_FTP;
								}
							}
							else{
								//leave file without syncing
								pNote->pRTHandles->syncstatusFTP = SS_DO_NOT_COPY_FTP;
							}
						}
						else{
							//copy remote file
							if(isIndexFile)
								group = GetRemoteNoteGroup(wfFTP.cFileName, szTempPath);
							if(group != GROUP_RECYCLE || (group == GROUP_RECYCLE && IsBitOn(g_NoteSettings.reserved1, SB1_INC_DEL_IN_SYNC))){
								needReload = TRUE;
								if(!FtpGetFileW(hConnection, wfFTP.cFileName, szLocalPath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0))
									goto _exit_error;
								if(isIndexFile)
									GetRemoteFileProperties(wfFTP.cFileName, szTempPath);
							}
						}
					}
				}
				else{
					//file not found - copy remote file
					if(isIndexFile)
						group = GetRemoteNoteGroup(wfFTP.cFileName, szTempPath);
					if(group != GROUP_RECYCLE || (group == GROUP_RECYCLE && IsBitOn(g_NoteSettings.reserved1, SB1_INC_DEL_IN_SYNC))){
						needReload = TRUE;
						if(!FtpGetFileW(hConnection, wfFTP.cFileName, szLocalPath, FALSE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_BINARY, 0))
							goto _exit_error;
						if(isIndexFile)
							GetRemoteFileProperties(wfFTP.cFileName, szTempPath);
					}
				}
			}
			result = InternetFindNextFileW(hInetFile, &wfFTP);
		}
		InternetCloseHandle(hInetFile);
	}
	else{
		int		nErr = GetLastError();
		if(nErr != ERROR_NO_MORE_FILES){
			//get error information
			GetInternetErrorDescriptionW(gs_ErrDesc, nErr);
			//close internet handle
			if(hConnection)
				InternetCloseHandle(hConnection);
			if(hINet)
				InternetCloseHandle(hINet);
			//post error information
			PostMessage(hwnd, FTPM_INETERROR, 0, 0);
			//exit thread
			return 1;
		}
	}
	//copy index file to server
	if(!FtpPutFileW(hConnection, gs_DataFilePath, lpIndexFile, FTP_TRANSFER_TYPE_BINARY, 0))
		goto _exit_error;
	//remove temporary saved remote index file
	if(isIndexFile){
		DeleteFileW(szTempPath);
		isIndexFile = FALSE;
	}
	//start to copy local files to server
	pNote = MemoryNotes();
	while(pNote){
		if(pNote->pRTHandles->syncstatusFTP == SS_COPY_TO_SERVER_FTP){
			if(pNote->pData->idGroup != GROUP_RECYCLE || (pNote->pData->idGroup == GROUP_RECYCLE && IsBitOn(g_NoteSettings.reserved1, SB1_INC_DEL_IN_SYNC))){
				//build local path
				wcscpy(szLocalPath, gs_DataDir);
				wcscpy(szFileName, pNote->pFlags->id);
				wcscat(szFileName, L".pnote");
				wcscat(szLocalPath, szFileName);
				//copy file to server
				if(!FtpPutFileW(hConnection, szLocalPath, szFileName, FTP_TRANSFER_TYPE_BINARY, 0))
					goto _exit_error;
			}
		}
		pNote = pNote->next;
	}
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hINet);
	//post finish message
	if(needReload)
		PostMessageW(hwnd, FTPM_FINISHED, 0, 0);
	else
		PostMessageW(hwnd, FTPM_NOTHING, 0, 0);
	return 0;
_exit_error:
	//get error information
	GetInternetErrorDescriptionW(gs_ErrDesc, 0);
	InternetGetLastResponseInfoW(&dwErr, szErrMessage, &dwSize);
	if(dwSize > 0 && wcslen(szErrMessage) > 0)
		wcscpy(gs_ErrDesc, szErrMessage);
	//close internet handle
	if(hConnection)
		InternetCloseHandle(hConnection);
	if(hINet)
		InternetCloseHandle(hINet);
	//remove temporary saved remote index file
	if(isIndexFile){
		DeleteFileW(szTempPath);
		isIndexFile = FALSE;
	}
	//post error information
	PostMessage(hwnd, FTPM_INETERROR, 0, 0);
	//exit thread
	return 1;
}

static void ExchangeLocalFilesProperties(wchar_t * id, wchar_t * lpFileFrom, wchar_t * lpFileTo){
	wchar_t			*pKeys, *pValue, *pk;
	int				sizeKey = 4096, sizeValue = 4096;

	pKeys = calloc(sizeKey, sizeof(wchar_t));
	if(pKeys){
		while(GetPrivateProfileStringW(id, NULL, NULL, pKeys, sizeKey, lpFileFrom) == sizeKey - 2){
			sizeKey *= 2;
			free(pKeys);
			pKeys = calloc(sizeKey, sizeof(wchar_t));
		}
		pk = pKeys;
		while(*pk){
			pValue = calloc(sizeValue, sizeof(wchar_t));
			if(pValue){
				while(GetPrivateProfileStringW(id, pk, NULL, pValue, sizeValue, lpFileFrom) == sizeValue - 1){
					sizeValue *= 2;
					free(pValue);
					pValue = calloc(sizeValue, sizeof(wchar_t));
				}
				WritePrivateProfileStringW(id, pk, pValue, lpFileTo);
				free(pValue);
			}
			pk += wcslen(pk) + 1;
		}
		free(pKeys);
	}
}

static void GetRemoteFileProperties(wchar_t * lpFile, wchar_t * szTempIndexFile){
	wchar_t			*pKeys, *pValue, *pk, szID[128];
	int				sizeKey = 4096, sizeValue = 4096;

	wcscpy(szID, lpFile);
	PathRemoveExtensionW(szID);
	pKeys = calloc(sizeKey, sizeof(wchar_t));
	if(pKeys){
		while(GetPrivateProfileStringW(szID, NULL, NULL, pKeys, sizeKey, szTempIndexFile) == sizeKey - 2){
			sizeKey *= 2;
			free(pKeys);
			pKeys = calloc(sizeKey, sizeof(wchar_t));
		}
		pk = pKeys;
		while(*pk){
			pValue = calloc(sizeValue, sizeof(wchar_t));
			if(pValue){
				while(GetPrivateProfileStringW(szID, pk, NULL, pValue, sizeValue, szTempIndexFile) == sizeValue - 1){
					sizeValue *= 2;
					free(pValue);
					pValue = calloc(sizeValue, sizeof(wchar_t));
				}
				WritePrivateProfileStringW(szID, pk, pValue, gs_DataFilePath);
				free(pValue);
			}
			pk += wcslen(pk) + 1;
		}
		free(pKeys);
	}
}

static int GetLocalFileGroup(wchar_t * id, wchar_t * szTempIndexFile){
	int				group = 0;
	NOTE_DATA		nd = {0};

	if(GetPrivateProfileStructW(id, S_DATA, &nd, sizeof(nd), szTempIndexFile)){
		group = nd.idGroup;
	}
	return group;
}

static int GetRemoteNoteGroup(wchar_t * lpFile, wchar_t * szTempIndexFile){
	int				group = 0;
	wchar_t			szID[128];
	NOTE_DATA		nd = {0};

	wcscpy(szID, lpFile);
	PathRemoveExtensionW(szID);
	if(GetPrivateProfileStructW(szID, S_DATA, &nd, sizeof(nd), szTempIndexFile)){
		group = nd.idGroup;
	}
	return group;
}

static BOOL AreLocalFilesDifferent(long size1, long size2, wchar_t * lpFile1, wchar_t * lpFile2){
	BOOL			result = FALSE;
	int				c1, c2;
	FILE			*f1 = NULL, *f2 = NULL;

	if(size1 != size2)
		return TRUE;
	f1 = _wfopen(lpFile1, L"rb");
	f2 = _wfopen(lpFile2, L"rb");
	if(f1 && f2){
		c1 = getc(f1);
		c2 = getc(f2);
		while(c1 != EOF){
			if(c1 != c2){
				result = TRUE;
				break;
			}
			c1 = getc(f1);
			c2 = getc(f2);
		}
		if(f1)
			fclose(f1);
		if(f2)
			fclose(f2);
	}
	return result;
}

static BOOL AreFilesDifferentFTP(long size1, long size2, wchar_t * lpLocal, wchar_t * lpRemote, HINTERNET hConnection){
	BOOL			result = FALSE;
	char			b[1];
	int				c1;
	FILE			*f1 = NULL;
	HINTERNET		hInternetFile = NULL;
	DWORD			r = 0;

	if(size1 != size2)
		return TRUE;
	//open local file
	f1 = _wfopen(lpLocal, L"rb");
	hInternetFile = FtpOpenFileW(hConnection, lpRemote, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
	if(hInternetFile){
		//as they are the same size, check only one for EOF
		c1 = getc(f1);
		InternetReadFile(hInternetFile, b, 1, &r);
		while(c1 != EOF){
			if(c1 != *b){
				result = TRUE;
				break;
			}
			c1 = getc(f1);
			InternetReadFile(hInternetFile, b, 1, &r);
		}
		InternetCloseHandle(hInternetFile);
	}
	if(f1)
		fclose(f1);
	return result;
}

BOOL CALLBACK NewSync_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, NewSync_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, NewSync_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, NewSync_OnClose);

		default: return FALSE;
	}
}

static void NewSync_OnClose(HWND hwnd)
{
	RemovePropW(hwnd ,PROP_ACTION);
	g_hSyncFolders = NULL;
}

static BOOL NewSync_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256];
	
	g_hSyncFolders = hwnd;

	CenterWindow(hwnd, FALSE);
	
	SetPropW(hwnd, PROP_ACTION, (HANDLE)lParam);

	if(lParam == 0)
		GetPrivateProfileStringW(S_CAPTIONS, L"new_sync_target", L"New local synchronization target", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"edit_sync_target", L"Edit local synchronization target", szBuffer, 255, g_NotePaths.CurrLanguagePath);
		SetDlgItemTextW(hwnd, IDC_EDT_SYNC_COMP_NAME, g_LocalSync.name);
		SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, g_LocalSync.pathNotes);
		SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, g_LocalSync.pathId);
		SendDlgItemMessageW(hwnd, IDC_EDT_SYNC_COMP_NAME, EM_SETREADONLY, TRUE, 0);
	}
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_SYNC_LOCAL_DIR_1, g_NotePaths.CurrLanguagePath, L"Notes files location");
	SetDlgCtlText(hwnd, IDC_ST_SYNC_LOCAL_DIR_2, g_NotePaths.CurrLanguagePath, L"Index file (notes.pnid) location");
	SetDlgCtlText(hwnd, IDC_CHK_SYNC_SAME_DIR, g_NotePaths.CurrLanguagePath, L"Use notes files location");
	SetDlgCtlText(hwnd, IDC_ST_SYNC_COMP_NAME, g_NotePaths.CurrLanguagePath, L"Computer name");
	if(lParam == 1){
		if(wcslen(g_LocalSync.pathId) > 0){
			CheckDlgButton(hwnd, IDC_CHK_SYNC_SAME_DIR, BST_UNCHECKED);
			EnableWindow(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_2), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_2), TRUE);
		}
	}
	else
		CheckDlgButton(hwnd, IDC_CHK_SYNC_SAME_DIR, BST_CHECKED);
	return TRUE;
}

static void NewSync_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id){
		case IDCANCEL:
			g_hSyncFolders = NULL;
			RemovePropW(hwnd ,PROP_ACTION);
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:
			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_COMP_NAME, g_LocalSync.name, 127);
			if((int)GetPropW(hwnd, PROP_ACTION) == 0 && PLocalSyncContains(g_TempLocalSyncs, g_LocalSync.name)){
				wchar_t		szMessage[256];
				GetPrivateProfileStringW(S_MESSAGES, L"local_sync_exists", L"Local synchronization target with this name already exists", szMessage, 255, g_NotePaths.CurrLanguagePath);
				MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK | MB_ICONEXCLAMATION);
				g_LocalSync.name[0] = '\0';
				break;
			}
			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, g_LocalSync.pathNotes, MAX_PATH - 1);
			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, g_LocalSync.pathId, MAX_PATH - 1);
			g_hSyncFolders = NULL;
			RemovePropW(hwnd ,PROP_ACTION);
			EndDialog(hwnd, IDOK);
			break;
		case IDC_EDT_SYNC_LOCAL_1:
		case IDC_EDT_SYNC_COMP_NAME:
			if(codeNotify == EN_CHANGE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_1)) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SYNC_COMP_NAME)) > 0)
					EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			}
			break;
		case IDC_CHK_SYNC_SAME_DIR:{
			if(IsDlgButtonChecked(hwnd, IDC_CHK_SYNC_SAME_DIR) == BST_CHECKED){
				EnableWindow(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_2), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_2), FALSE);
				SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, NULL);
			}
			else{
				EnableWindow(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_2), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_2), TRUE);
			}
			break;
		}
		case IDC_CMD_SYNC_LOCAL_1:{
			wchar_t			szBuffer[256], szFolderPath[MAX_PATH];

			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, m_FolderPath, MAX_PATH - 1);
			GetPrivateProfileStringW(S_CAPTIONS, L"choose_folder", L"Choose folder", szBuffer, 255, g_NotePaths.CurrLanguagePath);
			if(GetLocalSyncFolder(hwnd, szFolderPath, szBuffer)){
				SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, m_FolderPath);
			}
			break;
		}
		case IDC_CMD_SYNC_LOCAL_2:{
			wchar_t			szBuffer[256], szFolderPath[MAX_PATH];

			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, m_FolderPath, MAX_PATH - 1);
			GetPrivateProfileStringW(S_CAPTIONS, L"choose_folder", L"Choose folder", szBuffer, 255, g_NotePaths.CurrLanguagePath);
			if(GetLocalSyncFolder(hwnd, szFolderPath, szBuffer)){
				SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, m_FolderPath);
			}
			break;
		}
	}
}

BOOL CALLBACK LocalSync_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, LocalSync_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, LocalSync_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, LocalSync_OnClose);

		default: return FALSE;
	}
}

static void LocalSync_OnClose(HWND hwnd)
{
	g_hSyncFolders = NULL;
}

static BOOL LocalSync_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256], szKey[16];
	
	g_hSyncFolders = hwnd;

	CenterWindow(hwnd, FALSE);
	
	_itow(DLG_LOCAL_SYNC, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Manual local synchronization", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_CMD_SYNC_LOCAL_NOW, g_NotePaths.CurrLanguagePath, L"Synchronize");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_SYNC_LOCAL_DIR_1, g_NotePaths.CurrLanguagePath, L"Notes files location");
	SetDlgCtlText(hwnd, IDC_ST_SYNC_LOCAL_DIR_2, g_NotePaths.CurrLanguagePath, L"Index file (notes.pnid) location");
	SetDlgCtlText(hwnd, IDC_CHK_SYNC_SAME_DIR, g_NotePaths.CurrLanguagePath, L"Use notes files location");
	CheckDlgButton(hwnd, IDC_CHK_SYNC_SAME_DIR, BST_CHECKED);
	return TRUE;
}

static void LocalSync_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id){
		case IDCANCEL:
			g_hSyncFolders = NULL;
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDC_CMD_SYNC_LOCAL_NOW:{
			wchar_t		pathNotes2[MAX_PATH], pathId2[MAX_PATH];
			
			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, pathNotes2, MAX_PATH - 1);
			if(IsDlgButtonChecked(hwnd, IDC_CHK_SYNC_SAME_DIR) == BST_UNCHECKED){
				GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, pathId2, MAX_PATH - 1);
				_wcstrm(pathId2);
				if(wcslen(pathId2) == 0){
					wcscpy(pathId2, pathNotes2);
				}
			}
			else
				wcscpy(pathId2, pathNotes2);

			if(wcsstr(pathId2, L"notes.pnid") == NULL)
				wcscat(pathId2, L"\\notes.pnid");
			local_sync_results		syncResult = LocalSyncFunc(g_NotePaths.DataDir, g_NotePaths.DataFile, pathNotes2, pathId2);
			if(syncResult != LSR_FAILED){
				wchar_t		szMessage[256];

				GetPrivateProfileStringW(S_MESSAGES, L"sync_success", L"Synchronization completed successfully", szMessage, 256, g_NotePaths.CurrLanguagePath);
				MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK | MB_ICONINFORMATION);
				if(syncResult == LSR_OK_RELOAD)
					PostMessageW(g_hMain, PNM_NOTES_RELOAD, FALSE, 0);
			}
			g_hSyncFolders = NULL;
			EndDialog(hwnd, IDOK);
			break;
		}
		case IDC_EDT_SYNC_LOCAL_1:
			if(codeNotify == EN_CHANGE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_1)) > 0)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_NOW), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_NOW), FALSE);
			}
			break;
		case IDC_CHK_SYNC_SAME_DIR:{
			if(IsDlgButtonChecked(hwnd, IDC_CHK_SYNC_SAME_DIR) == BST_CHECKED){
				EnableWindow(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_2), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_2), FALSE);
				SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, NULL);
			}
			else{
				EnableWindow(GetDlgItem(hwnd, IDC_EDT_SYNC_LOCAL_2), TRUE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_SYNC_LOCAL_2), TRUE);
			}
			break;
		}
		case IDC_CMD_SYNC_LOCAL_1:{
			wchar_t			szBuffer[256], szFolderPath[MAX_PATH];

			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, m_FolderPath, MAX_PATH - 1);
			GetPrivateProfileStringW(S_CAPTIONS, L"choose_folder", L"Choose folder", szBuffer, 255, g_NotePaths.CurrLanguagePath);
			if(GetLocalSyncFolder(hwnd, szFolderPath, szBuffer)){
				SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_1, m_FolderPath);
			}
			break;
		}
		case IDC_CMD_SYNC_LOCAL_2:{
			wchar_t			szBuffer[256], szFolderPath[MAX_PATH];

			GetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, m_FolderPath, MAX_PATH - 1);
			GetPrivateProfileStringW(S_CAPTIONS, L"choose_folder", L"Choose folder", szBuffer, 255, g_NotePaths.CurrLanguagePath);
			if(GetLocalSyncFolder(hwnd, szFolderPath, szBuffer)){
				SetDlgItemTextW(hwnd, IDC_EDT_SYNC_LOCAL_2, m_FolderPath);
			}
			break;
		}
	}
}

static UINT APIENTRY OFNFolderHook(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG) {
		HWND hWnd = GetParent(hDlg);
		SendMessageW(hWnd, CDM_HIDECONTROL, stc2, 0); // hide "Save as type"
		SendMessageW(hWnd, CDM_HIDECONTROL, cmb1, 0); // hide the filter combo box
		SendMessageW(hWnd, CDM_SETCONTROLTEXT, IDOK, (LPARAM)L"Select");
		SendMessageW(hWnd, CDM_SETCONTROLTEXT, stc3, (LPARAM)L"Folder Name:");
		m_hOFWndProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)OFNFolderProc);

		return 1;
	}

	if (uMsg == WM_NOTIFY && ((NMHDR*)lParam)->code == CDN_FILEOK){
		// reject all files when the OK button is pressed
		// this will stop the dialog from closing
		SetWindowLongPtrW(hDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}

	return 0;
}

BOOL GetLocalSyncFolder(HWND hwndParent, wchar_t * szFolderPath, const wchar_t * szTitle)
{
	OPENFILENAMEW 	of = {0};
	wchar_t			szPath[MAX_PATH];

	szPath[0] = '\0';
	of.lStructSize = sizeof(of);
	of.hwndOwner = hwndParent;
	// weird filter to exclude all files and just keep the folders
	of.lpstrFilter = L"Folders\0qqqqqqqqqqqqqqq.qqqqqqqqq\0\0";
	of.nFilterIndex = 1;
	of.lpstrInitialDir = m_FolderPath; 
	of.lpstrFile = szPath;
	of.lpstrTitle = szTitle;
	of.nMaxFile = MAX_PATH;
	of.Flags = OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING;//|OFN_NOCHANGEDIR|OFN_ENABLESIZING;
	of.lpstrDefExt = L"";
	of.lpfnHook = OFNFolderHook;

	BOOL res = (GetOpenFileNameW(&of) != FALSE);
	
	return res;
}

static LRESULT CALLBACK OFNFolderProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	// if the OK button is pressed
	if(uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDOK){
		BOOL 	fValid = FALSE;
		HWND 	hList = GetDlgItem(GetDlgItem(hWnd, lst2), 1);
		int 	index = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
		if (GetWindowTextLength(GetDlgItem(hWnd, cmb13)) > 0) {
			// the file name box is not empty
			// use the default processing, which will open the folder with that name
			CallWindowProc(m_hOFWndProc, hWnd, uMsg, wParam, lParam);
			// then clear the text
			SetDlgItemTextW(hWnd, cmb13, L"");
			return TRUE;
		}
		else if (index >= 0) {
			// if a folder is selected in the list view, its user data is a PIDL
			// get the full folder name as described here: http://msdn.microsoft.com/msdnmag/issues/03/09/CQA/
			LVITEM item = {LVIF_PARAM,index,0};
			ListView_GetItem(hList,&item);

			int len=SendMessageW(hWnd, CDM_GETFOLDERIDLIST, 0, 0);
			if (len > 0) {
				LPCITEMIDLIST 	pidlFolder=(LPCITEMIDLIST)CoTaskMemAlloc(len);
				SendMessageW(hWnd, CDM_GETFOLDERIDLIST, len, (LPARAM)pidlFolder);

				STRRET str = {STRRET_WSTR};

				IShellFolder 	*pDesktop, *pFolder;
				SHGetDesktopFolder(&pDesktop);
				
				if (SUCCEEDED(IShellFolder_BindToObject(pDesktop, pidlFolder, NULL, &IID_IShellFolder, (void**)&pFolder))){
					if(FAILED(IShellFolder_GetDisplayNameOf(pFolder, (LPITEMIDLIST)item.lParam, SHGDN_FORPARSING,&str)))
						str.pOleStr = NULL;
					IShellFolder_Release(pFolder);
					IShellFolder_Release(pDesktop);
				}
				else {
					if (FAILED(IShellFolder_GetDisplayNameOf(pDesktop, (LPITEMIDLIST)item.lParam, SHGDN_FORPARSING, &str)))
						str.pOleStr  =NULL;
					IShellFolder_Release(pDesktop);
				}

				if (str.pOleStr) {
					DWORD 	attrib = GetFileAttributesW(str.pOleStr);
					if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY)){
						wcsncpy(m_FolderPath,str.pOleStr, MAX_PATH);
						m_FolderPath[MAX_PATH - 1] = 0;
						fValid = TRUE;
					}
					CoTaskMemFree(str.pOleStr);
				}
				CoTaskMemFree((LPVOID)pidlFolder);
			}
		}
		else {
			// no item is selected, use the current folder
			wchar_t 	path[MAX_PATH];
			SendMessageW(hWnd, CDM_GETFOLDERPATH, MAX_PATH, (LPARAM)path);
			DWORD 	attrib = GetFileAttributesW(path);
			if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY)){
				wcscpy(m_FolderPath,path);
				fValid = TRUE;
			}
		}
		if(fValid){
			EndDialog(hWnd,IDOK);
			return TRUE;
		}
	}

	return CallWindowProc(m_hOFWndProc, hWnd, uMsg, wParam, lParam);
}
