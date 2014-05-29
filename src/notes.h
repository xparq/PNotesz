#ifndef __NOTES_H__
#define __NOTES_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

// #ifndef _WIN32_IE
// #define _WIN32_IE 0x0500
// #endif

#ifndef TOOL_BAR_CUSTOM_DRAW
#define	TOOL_BAR_CUSTOM_DRAW
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commdlg.h>
#include <Mmsystem.h>
#include <richedit.h>

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

#ifndef false
#define false  0
#endif

#ifndef true
#define true  1
#endif

#endif
