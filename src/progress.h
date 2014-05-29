#ifndef __PROGRESS_H__
#define __PROGRESS_H__

#include "sockets.h"

typedef enum _PROGRESS_TYPE {
	PTYPE_SYNC,
	PTYPE_SOCKET
}PROGRESS_TYPE;

typedef struct _PROGRESS_STRUCT {
	PROGRESS_TYPE			type;
	HWND					hCaller;
	wchar_t					szCaptionKey[128];
	wchar_t					szCaptionDef[256];
	wchar_t					szProgressKey[128];
	wchar_t					szProgressDef[256];
	wchar_t					szLangFile[MAX_PATH];
	BOOL					fShowCancel;
	LPTHREAD_START_ROUTINE	lpStartAddress;
	LPBOOL					pStopParameter;
}PROGRESS_STRUCT, *LPPROGRESS_STRUCT;

BOOL CALLBACK Progress_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif	// PROGRESS_H__
