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

#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#include <wchar.h>
#include <commdlg.h>

#include "global.h"
#include "shared.h"
#include "stringconstants.h"
#include "sync.h"
#include "sockerrors.h"
#include "sockets.h"
#include "progress.h"

#define	TIMER_SYNC_ID		1111

#define	PROGRESS_VARS(hwnd)		(LPPROGRESS_VARS)GetWindowLongPtrW((hwnd), (GWLP_USERDATA))

typedef struct _PROGRESS_VARS {
	PROGRESS_TYPE	type;
	HWND			hCaller;
	BOOL			syncTimer;
	int				progIconIndex;
	HICON			progressIcons[6];
	LPBOOL			pStopParameter;
	wchar_t			szProgress[256];
}PROGRESS_VARS, *LPPROGRESS_VARS;

static BOOL Progress_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Progress_OnClose(HWND hwnd);
static void Progress_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void CALLBACK ProgressTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void ClearProgress(HWND hwnd);

BOOL CALLBACK Progress_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Progress_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Progress_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, Progress_OnClose);
		case FTPM_FINISHED:
			ClearProgress(hwnd);
			EndDialog(hwnd, IDOK);
			return TRUE;
		case FTPM_NOTHING:
			ClearProgress(hwnd);
			EndDialog(hwnd, IDYES);
			return TRUE;
		case FTPM_INETERROR:{
			MessageBoxW(hwnd, gs_ErrDesc, PROG_NAME, MB_OK | MB_ICONERROR);
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			return TRUE;
		}
		case WSPM_ERROR:{
			wchar_t			szErrDesc[1024], szMessage[1024];
			
			if(g_wsPData){
				free(g_wsPData);
				g_wsPData = NULL;
			}
			GetSockErrorDescription(wParam, szErrDesc);
			swprintf(szMessage, 1024, L"Error: %d\n", wParam);
			wcscat(szMessage, szErrDesc);
			MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK | MB_ICONERROR);
			ClearProgress(hwnd);
			EndDialog(hwnd, IDCANCEL);
			return TRUE;
		}
		case WSPM_PROGRESS:{
			LPPROGRESS_VARS		lpVars = PROGRESS_VARS(hwnd);
			wchar_t				szProgress[256], szTemp[16];
			wcscpy(szProgress, lpVars->szProgress);
			swprintf(szTemp, 16, L" %d%%", wParam);
			wcscat(szProgress, szTemp);
			SetDlgItemTextW(hwnd, IDC_ST_DOWNLOAD_PERC, szProgress);
			return TRUE;
		}
		case WSPM_SEND_FINISHED:{
			LPPROGRESS_VARS		lpVars = PROGRESS_VARS(hwnd);
			wchar_t				szProgress[256];
			wcscpy(szProgress, lpVars->szProgress);
			wcscat(szProgress, L" 100%");
			SetDlgItemTextW(hwnd, IDC_ST_DOWNLOAD_PERC, szProgress);
			// if(g_wsPData){
				// free(g_wsPData);
				// g_wsPData = NULL;
			// }
			ClearProgress(hwnd);
			EndDialog(hwnd, IDOK);
			return TRUE;
		}
		default: return FALSE;
	}
}

static BOOL Progress_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HANDLE				hThread = NULL;
	DWORD				dwID;
	PROGRESS_STRUCT		ps = {0};
	wchar_t 			szBuffer[128];
	LPPROGRESS_VARS		lpVars = NULL;
	HMODULE				hIcons;

	lpVars = calloc(1, sizeof(PROGRESS_VARS));
	if(lpVars){
		memcpy(&ps, (LPPROGRESS_STRUCT)lParam, sizeof(PROGRESS_STRUCT));
		GetPrivateProfileStringW(S_OPTIONS, ps.szCaptionKey, ps.szCaptionDef, szBuffer, 256, ps.szLangFile);
		SetWindowTextW(hwnd, szBuffer);
		GetPrivateProfileStringW(S_MESSAGES, ps.szProgressKey, ps.szProgressDef, lpVars->szProgress, 256, ps.szLangFile);
		SetDlgItemTextW(hwnd, IDC_ST_DOWNLOAD_PERC, lpVars->szProgress);
		if(!ps.fShowCancel){
			ShowWindow(GetDlgItem(hwnd, IDCANCEL), SW_HIDE);
		}
		else{
			SetDlgCtlText(hwnd, IDCANCEL, ps.szLangFile, DS_CANCEL);
		}
		lpVars->hCaller = ps.hCaller;
		lpVars->type = ps.type;
		if(ps.type == PTYPE_SOCKET){
			//get caller
			g_wsCaller = hwnd;
		}
		hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(hIcons){
			for(int i = IDR_ICO_PROGRESS_1, j = 0; i <= IDR_ICO_PROGRESS_6; i++, j++){
				lpVars->progressIcons[j] = LoadIconW(hIcons, MAKEINTRESOURCEW(i));
			}
			FreeLibrary(hIcons);
		}
		lpVars->pStopParameter = ps.pStopParameter;
		lpVars->syncTimer = (BOOL)SetTimer(hwnd, TIMER_SYNC_ID, 200, ProgressTimerProc);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)lpVars);
		
		if(ps.type == PTYPE_SYNC){
			hThread = CreateThread(NULL, 0, ps.lpStartAddress, hwnd, 0, &dwID);
		}
		else if(ps.type == PTYPE_SOCKET){
			hThread = CreateThread(NULL, 0, ps.lpStartAddress, NULL, 0, &dwID);
		}
		if(hThread){
			CloseHandle(hThread);
		}
	}
	return FALSE;
}

static void Progress_OnClose(HWND hwnd)
{
	ClearProgress(hwnd);
	EndDialog(hwnd, IDCANCEL);
}

static void Progress_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	LPPROGRESS_VARS		lpVars = NULL;

	lpVars = PROGRESS_VARS(hwnd);

	switch(id){
		case IDCANCEL:
			if(lpVars){
				if(lpVars->pStopParameter){
					*lpVars->pStopParameter = TRUE;
				}
			}
			ClearProgress(hwnd);
			EndDialog(hwnd, IDCANCEL);
			break;
	}
}

static void CALLBACK ProgressTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	LPPROGRESS_VARS		lpVars = NULL;

	lpVars = PROGRESS_VARS(hwnd);
	if(lpVars){
		if(lpVars->progIconIndex == 5)
			lpVars->progIconIndex = 0;
		SendDlgItemMessageW(hwnd, IDC_ST_PROGRESS, STM_SETICON, (WPARAM)lpVars->progressIcons[lpVars->progIconIndex++], 0);
	}
}

static void ClearProgress(HWND hwnd){
	LPPROGRESS_VARS		lpVars = NULL;

	lpVars = PROGRESS_VARS(hwnd);
	if(lpVars){
		for(int i = IDR_ICO_PROGRESS_1; i <= IDR_ICO_PROGRESS_6; i++){
			if(lpVars->progressIcons[i]){
				DestroyIcon(lpVars->progressIcons[i]);
				lpVars->progressIcons[i] = NULL;
			}
		}
		if(lpVars->syncTimer){
			KillTimer(hwnd, TIMER_SYNC_ID);
			lpVars->syncTimer = FALSE;
		}
		free(lpVars);
	}
}
