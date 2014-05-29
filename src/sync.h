#ifndef __SYNC_H__
#define __SYNC_H__

#include "enums.h"

#define	FTPM_BASE			WM_USER + 10180
#define	FTPM_PROGRESS		FTPM_BASE + 1
#define	FTPM_FINISHED		FTPM_BASE + 2
#define	FTPM_INETERROR		FTPM_BASE + 3
#define	FTPM_CANCELLED		FTPM_BASE + 4
#define	FTPM_NOTHING		FTPM_BASE + 5

typedef struct _FTP_DATA {
	wchar_t				server[MAX_PATH];
	wchar_t				directory[MAX_PATH];
	wchar_t				user[128];
	wchar_t				password[128];
	int					port;
} FTP_DATA, *PFTP_DATA;

DWORD WINAPI FTPSyncFunc(LPVOID lpParam);
local_sync_results LocalSyncFunc(wchar_t pathNotes1[MAX_PATH], wchar_t pathId1[MAX_PATH], wchar_t pathNotes2[MAX_PATH], wchar_t pathId2[MAX_PATH]);
BOOL CALLBACK LocalSync_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK NewSync_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL GetLocalSyncFolder(HWND hwndParent, wchar_t * szFolderPath, const wchar_t * szTitle);

wchar_t					gs_ErrDesc[260], gs_DataDir[MAX_PATH], gs_DataFilePath[MAX_PATH];
FTP_DATA				g_FTPData;
BOOL					gs_StopSync;

#endif	// SYNC_H__
