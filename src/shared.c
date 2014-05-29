// Copyright (C) 2007 Andrey Gruber (aka lamer)

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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <math.h>

#include "shared.h"
#include "darrow.h"
#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "transparentbitmap.h"
#include "gradients.h"
#include "skinlessdrawing.h"
#include "memorynotes.h"
#include "docking.h"
#include "encryption.h"
#include "oleinterface.h"
#include "enums.h"
#include "sengs.h"
#include "contacts.h"
#include "note.h"
#include "sockets.h"
#include "progress.h"
#include "login.h"
#include "groups.h"
#include "contgroups.h"

/** skin files procedures ********************************************************/
typedef int (*GETSKINPROC)(HWND, UINT);
typedef int (*GETCTLRECTPROC)(UINT, LPRECT);

/** Prototypes ********************************************************/
static HBITMAP GetBackBitmap(P_NOTE_RTHANDLES pH, LPCRECT lpRc);
static BOOL GetSkinInfo(HINSTANCE hLib, wchar_t * szBuffer);
static BOOL GetSkinVertTbr(HINSTANCE hLib);
static void InnerReadRTFFile(HWND hEdit, wchar_t * lpPath);
static HBITMAP DuplicateCross(HWND hwnd, HBITMAP hbmSrc);
static PROGRESS_STRUCT PrepareSendSocketsStruct(HWND hwnd, LPARAM lParam);
static int GetNoteCaptionFontHeight(HFONT hFont, wchar_t * lpCaption);
static int GetNoteCaptionFontHeightForDocking(LPLOGFONTW lplf, wchar_t * lpCaption);
static BOOL CALLBACK EnableThreadWndProc(HWND hwnd, LPARAM lParam);

/** Module variables ********************************************************/
static int				m_IDSizeMenu = IDI_SIZE_START;
static int				m_FSize;

/** Global functions ********************************************************/

void ChangeNoteLookByGroup(PMEMNOTE pNote, int newGroup){
	
	LPPNGROUP	pgNew = PNGroupsGroupById(g_PGroups, newGroup);

	if((pNote->pAppearance->nPrivate & F_B_COLOR) == F_B_COLOR){
		pNote->pAppearance->nPrivate &= ~F_B_COLOR;
	}
	pNote->pAppearance->crWindow = pgNew->crWindow;
	if(pNote->pData->visible){
		SendMessageW(pNote->hwnd, PNM_NEW_BGCOLOR, 0, 0);
	}
	
	if((pNote->pAppearance->nPrivate & F_C_COLOR) == F_C_COLOR){
		pNote->pAppearance->nPrivate &= ~F_C_COLOR;
	}
	pNote->pAppearance->crCaption = pgNew->crCaption;
	if(pNote->pData->visible){
		SendMessageW(pNote->hwnd, PNM_NEW_CAPTION_COLOR, 0, 0);
	}

	if((pNote->pAppearance->nPrivate & F_RTFONT) != F_RTFONT){
		pNote->pAppearance->crFont = pgNew->crFont;
		if(pNote->pData->visible){
			SendMessageW(pNote->hwnd, PNM_CHANGE_FONT_COLOR, (WPARAM)pNote->pAppearance->crFont, 0);
			RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
			ShowNoteMarks(pNote->hwnd);
		}
	}
}

void OpenPage(HWND hwnd, wchar_t * lpAddress){
	if(wcslen(g_DefBrowser) == 0)
		ShellExecuteW(hwnd, L"open", lpAddress, NULL, NULL, SW_SHOWDEFAULT);
	else{
		wchar_t		szCommandLine[2048];
		wcscpy(szCommandLine, L"\"");
		wcscat(szCommandLine, lpAddress);
		wcscat(szCommandLine, L"\"");
		if((int)ShellExecuteW(hwnd, NULL, g_DefBrowser, szCommandLine, NULL, SW_SHOWDEFAULT) <= 32){
			ShellExecuteW(hwnd, L"open", lpAddress, NULL, NULL, SW_SHOWDEFAULT);
		}
	}	
}

void InitSpeller(void){
	PHENTRY pe;

	pe = getEntry(g_DictFiles, g_Spell.spellCulture);
	if(pe){
		char 			szAff[MAX_PATH], szDict[MAX_PATH];
		wchar_t			*p, *ptr, *tmp;

		tmp = _wcsdup(pe->value);
		p = wcstok(tmp, L"|", &ptr);
		WideCharToMultiByte(CP_ACP, 0, p, -1, szAff, MAX_PATH, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, ptr, -1, szDict, MAX_PATH, NULL, NULL);
		free(tmp);
		CreateSpeller(szAff, szDict, g_hMain, g_Spell.color);
	}
}

int CALLBACK SentAtCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	FILETIME		ft1, ft2;
	LPSRSTATUS		pD1, pD2;

	pD1 = ((PMEMNOTE)lParam1)->pSRStatus;
	pD2 = ((PMEMNOTE)lParam2)->pSRStatus;
	SystemTimeToFileTime(&pD1->lastSent, &ft1);
	SystemTimeToFileTime(&pD2->lastSent, &ft2);
	if((int)lParamSort == LVS_SORTASCENDING)
		return CompareFileTime(&ft1, &ft2);
	else
		return CompareFileTime(&ft2, &ft1);
}

int CALLBACK DateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	FILETIME		ft1, ft2;
	P_NOTE_DATA		pD1, pD2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;
	SystemTimeToFileTime(&pD1->stChanged, &ft1);
	SystemTimeToFileTime(&pD2->stChanged, &ft2);
	if((int)lParamSort == LVS_SORTASCENDING)
		return CompareFileTime(&ft1, &ft2);
	else
		return CompareFileTime(&ft2, &ft1);
}

int CALLBACK DeletedDateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	FILETIME		ft1, ft2;
	PMEMNOTE		p1, p2;

	p1 = (PMEMNOTE)lParam1;
	p2 = (PMEMNOTE)lParam2;
	SystemTimeToFileTime(p1->pRealDeleted, &ft1);
	SystemTimeToFileTime(p2->pRealDeleted, &ft2);
	if((int)lParamSort == LVS_SORTASCENDING)
		return CompareFileTime(&ft1, &ft2);
	else
		return CompareFileTime(&ft2, &ft1);
}

int CALLBACK CreationDateCompareFunct(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	FILETIME		ft1, ft2;
	PMEMNOTE		p1, p2;

	p1 = (PMEMNOTE)lParam1;
	p2 = (PMEMNOTE)lParam2;
	SystemTimeToFileTime(p1->pCreated, &ft1);
	SystemTimeToFileTime(p2->pCreated, &ft2);
	if((int)lParamSort == LVS_SORTASCENDING)
		return CompareFileTime(&ft1, &ft2);
	else
		return CompareFileTime(&ft2, &ft1);
}

int CALLBACK ReceivedAtCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	FILETIME		ft1, ft2;
	LPSRSTATUS		pD1, pD2;

	pD1 = ((PMEMNOTE)lParam1)->pSRStatus;
	pD2 = ((PMEMNOTE)lParam2)->pSRStatus;
	SystemTimeToFileTime(&pD1->lastRec, &ft1);
	SystemTimeToFileTime(&pD2->lastRec, &ft2);
	if((int)lParamSort == LVS_SORTASCENDING)
		return CompareFileTime(&ft1, &ft2);
	else
		return CompareFileTime(&ft2, &ft1);
}

BOOL IsPasswordSet(void){
	wchar_t			szBuffer[256];

	GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szBuffer, 256, g_NotePaths.INIFile);
	if(_wcsicmp(szBuffer, L"N/A") == 0)
		return false;
	else
		return true;
}

BOOL IsAnyHiddenInGroup(int group){
	PMEMNOTE	pNote = MemoryNotes();
	
	while(pNote){
		if(pNote->pData->idGroup == group && !pNote->pData->visible)
			return TRUE;
		pNote = pNote->next;
	}
	return FALSE;
}

BOOL IsDiaryLocked(void){
	wchar_t		szTemp[256];
	GetPrivateProfileStringW(S_DIARY, IK_DIARY_LOCK, NULL, szTemp, 256, g_NotePaths.INIFile);
	if(*szTemp)
		return TRUE;
	return FALSE;
}

BOOL IsGroupLocked(int group){
	if(group != GROUP_DIARY){
		LPPNGROUP		pg = PNGroupsGroupById(g_PGroups, group);
		if(pg){
			if(wcslen(pg->szLock))
				return TRUE;
		}
		return FALSE;
	}
	else{
		return IsDiaryLocked();
	}
}

BOOL UnlockGroup(int group){
	//check for possible password
	if(group != GROUP_DIARY){
		LPPNGROUP		pg = PNGroupsGroupById(g_PGroups, group);

		if(pg){
			if(wcslen(pg->szLock)){
				if(ShowLoginDialog(g_hInstance, NULL, DLG_LOGIN_MAIN, NULL, LDT_GROUP, pg) != IDOK)
					return FALSE;
			}
		}
	}
	else{
		if(IsDiaryLocked()){
			if(ShowLoginDialog(g_hInstance, NULL, DLG_LOGIN_MAIN, NULL, LDT_DIARY, NULL) != IDOK)
				return FALSE;
		}
	}
	return TRUE;
}

BOOL UnlockNote(PMEMNOTE pNote){
	//check for possible password
	if(pNote->pFlags->locked){
		if(ShowLoginDialog(g_hInstance, NULL, DLG_LOGIN_MAIN, NULL, LDT_NOTE, pNote) != IDOK)
			return FALSE;
	}
	return TRUE;
}

void DisableInput(BOOL value){
	g_DisableInput = value;
	EnumThreadWindows(GetWindowThreadProcessId(g_hMain, NULL), EnableThreadWndProc, value);
}

static BOOL CALLBACK EnableThreadWndProc(HWND hwnd, LPARAM lParam){
	if(IsWindowVisible(hwnd)){
		EnableWindow(hwnd, !(BOOL)lParam);
	}
	return true;
}

BOOL IsEmptyString(wchar_t * lpString){
	int		count = 0;
	while(*lpString){
		if(*lpString != ' ' && *lpString != '\t')
			count++;
		lpString++;
	}
	if(count > 0)
		return FALSE;
	return TRUE;
}

void SaveContactsGroups(void){
	wchar_t			szId[32];

	WritePrivateProfileSectionW(S_CONT_GROUPS, NULL, g_NotePaths.INIFile);
	for(LPPCONTGROUP pTemp = g_PContGroups; pTemp; pTemp = pTemp->next){
		if(pTemp->id != 0){		//do not store (None) group
			_itow(pTemp->id, szId, 10);
			WritePrivateProfileStructW(S_CONT_GROUPS, szId, pTemp, sizeof(PCONTGROUP), g_NotePaths.INIFile);
		}
	}
}

void SaveContacts(void){
	LPPCONTACT		pTemp;

	WritePrivateProfileSectionW(S_CONTACTS, NULL, g_NotePaths.INIFile);
	PContactsSort(g_PContacts);
	pTemp = g_PContacts;
	while(pTemp){
		WritePrivateProfileStructW(S_CONTACTS, pTemp->prop.name, &pTemp->prop, sizeof(PCONTPROP), g_NotePaths.INIFile);
		pTemp = pTemp->next;
	}
}

BOOL SendNoteToContact(HWND hCaller, PMEMNOTE pNote, wchar_t * lpContact){
	LPPCONTACT				lpc = NULL;
	PROGRESS_STRUCT			ps = {0};

	lpc = PContactsItem(g_PContacts, lpContact);
	// if(!lpc){
		// return FALSE;
	// }
	ps = PrepareSendSocketsStruct(hCaller, 0);

	if(!pNote->pFlags->saved && IsBitOn(g_NextSettings.flags1, SB3_SAVE_BEFORE_SEND)){
		SaveNote(pNote);
	}

	//get address to send
	if(lpc){
		//in case of sending to contact
		if(lpc->prop.usename){
			WideCharToMultiByte(CP_ACP, 0, lpc->prop.host, -1, g_wsAddress, 256, NULL, NULL);
		}
		else{
			wchar_t		szTemp[16];
			swprintf(szTemp, 16, L"%d.%d.%d.%d", FIRST_IPADDRESS(lpc->prop.address), SECOND_IPADDRESS(lpc->prop.address), THIRD_IPADDRESS(lpc->prop.address), FOURTH_IPADDRESS(lpc->prop.address));
			WideCharToMultiByte(CP_ACP, 0, szTemp, -1, g_wsAddress, 256, NULL, NULL);
		}
	}
	else{
		//in case of replying to unknown recipient
		WideCharToMultiByte(CP_ACP, 0, lpContact, -1, g_wsAddress, 256, NULL, NULL);
	}

	wcscpy(g_wsTempID, pNote->pFlags->id);

	int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_PROGRESS), hCaller, Progress_DlgProc, (LPARAM)&ps);
	if(result == IDOK){
		//store send properties
		if(lpc)
			wcscpy(pNote->pSRStatus->sentTo, lpc->prop.name);
		else
			wcscpy(pNote->pSRStatus->sentTo, lpContact);
		GetLocalTime(&pNote->pSRStatus->lastSent);
		//update control panel
		if(g_hCPDialog)
			SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		//save note sent/received status
		WritePrivateProfileStructW(pNote->pFlags->id, S_SEND_RECEIVE_STATUS, pNote->pSRStatus, sizeof(SRSTATUS), g_NotePaths.DataFile);
		if(!IsBitOn(g_NextSettings.flags1, SB3_SEND_HIDE_BALOON)){
			if(lpc)
				SendMessageW(g_hMain, PNM_NOTE_SENT, (WPARAM)lpc->prop.name, (LPARAM)pNote->pData->szName);
			else
				SendMessageW(g_hMain, PNM_NOTE_SENT, (WPARAM)lpContact, (LPARAM)pNote->pData->szName);
		}
		if(pNote->pData->visible){
			SendMessageW(pNote->hwnd, PNM_TT_UPDATE, 0, 0);
		}
		if(pNote->pData->visible && IsBitOn(g_NextSettings.flags1, SB3_HIDE_AFTER_SEND)){
			HideNote(pNote, false);
		}
		return TRUE;
	}
	return FALSE;

	// LPBYTE pData = NULL;
	// ULONG cbSize = 0;

	// pData = GetDataToSend(pNote->pFlags->id, &cbSize);
	// if(SendData(pData, cbSize) == 0){
		// //store send properties
		// wcscpy(pNote->pSRStatus->sentTo, lpc->prop.name);
		// GetLocalTime(&pNote->pSRStatus->lastSent);
		// //update control panel
		// if(g_hCPDialog)
			// SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		// //save note sent/received status
		// WritePrivateProfileStructW(pNote->pFlags->id, S_SEND_RECEIVE_STATUS, pNote->pSRStatus, sizeof(SRSTATUS), g_NotePaths.DataFile);
		// return TRUE;
	// }
	// return FALSE;
}

DWORD SharedOutStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb){
	WriteFile((HANDLE)dwCookie, pbBuff, cb, (PULONG)pcb, NULL);
	return 0;
}

void BuildContactsMenu(HMENU hMenu, int indexContact, int indexGroup){
	HMENU					hCont, hGroup;
	int						count = 0, groupsCount = 0, cpContacts = PContactsCount(g_PContacts), id = CONTACTS_ADDITION, idGroup = CONT_GROUPS_ADDITION, j = 3;
	MITEM					mit = {0};
	LPPCONTACT				pContact = g_PContacts, pSave;
	LPPCONTGROUP			pGroups = g_PContGroups;
	P_CONT_GROUP_STRUCT		pArray = NULL;
	MENUITEMINFOW			mi = {0};

	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;

	hCont = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_SEND_INTRANET));
	count = GetMenuItemCount(hCont);
	if(count > 5){
		for(int i = count - 3; i > 2; i--){
			FreeSingleMenu(hCont, i);

			ZeroMemory(&mi, sizeof(mi));
			mi.cbSize = sizeof(mi);
			mi.fMask = MIIM_SUBMENU;
			GetMenuItemInfoW(hCont, i, TRUE, &mi);
			if(mi.hSubMenu){
				int		subcount = GetMenuItemCount(mi.hSubMenu);
				for(int k = subcount - 1; k >= 0; k--){
					FreeSingleMenu(mi.hSubMenu, k);
					DeleteMenu(mi.hSubMenu, k, MF_BYPOSITION);
				}
			}
			DeleteMenu(hCont, i, MF_BYPOSITION);
			if(mi.hSubMenu){
				DestroyMenu(mi.hSubMenu);
			}
		}
	}
	if(pContact && !IsBitOn(g_NextSettings.flags1, SB3_DONOT_SHOW_CONT_MENU)){
		InsertMenuW(hCont, 2, MF_BYPOSITION | MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
		groupsCount = PContGroupsCount(pGroups);
		if(groupsCount > 1){
			groupsCount--;
			pArray = calloc(groupsCount, sizeof(CONT_GROUP_STRUCT));
			if(pArray){
				int i = 0;
				for(LPPCONTGROUP pg = pGroups; pg; pg = pg->next){
					if(pg->id > 0){
						pArray[i].id = pg->id;
						wcscpy(pArray[i].name, pg->name);
						i++;
					}
				}
				qsort(pArray, groupsCount, sizeof(CONT_GROUP_STRUCT), ContGroupsMenusCompare);
			}
		}
		pSave = pContact;
		//insert contacts from (None) group
		while(pContact){
			if(pContact->prop.group == 0){
				mit.xPos = indexContact;
				mit.yPos = 0;
				mit.id = id++;
				wcscpy(mit.szText, pContact->prop.name);
				InsertMenuW(hCont, j, MF_BYPOSITION | MF_STRING, mit.id, mit.szText);	
				SetMenuItemProperties(&mit, hCont, j++, TRUE);
			}
			pContact = pContact->next;
		}
		//insert allother groups and their contacts
		if(pArray){
			for(int i = 0; i < groupsCount; i++){
				mit.xPos = indexGroup;
				mit.yPos = 0;
				mit.id = idGroup++;
				wcscpy(mit.szText, pArray[i].name);
				hGroup = CreatePopupMenu();
				InsertMenuW(hCont, j, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT)hGroup, mit.szText);	
				SetMenuItemProperties(&mit, hCont, j++, TRUE);
				int		k = 0;
				for(LPPCONTACT pc = pSave; pc; pc = pc->next){
					if(pc->prop.group == pArray[i].id){
						mit.id = id++;
						mit.xPos = indexContact;
						mit.yPos = 0;
						wcscpy(mit.szText, pc->prop.name);
						InsertMenuW(hGroup, k, MF_BYPOSITION | MF_STRING, mit.id, mit.szText);	
						SetMenuItemProperties(&mit, hGroup, k++, TRUE);
					}
				}
			}
			free(pArray);
		}
		// while(pContact){
			// mit.id = id++;
			// wcscpy(mit.szText, pContact->prop.name);
			// InsertMenuW(hCont, j, MF_BYPOSITION | MF_STRING, mit.id, mit.szText);	
			// SetMenuItemProperties(&mit, hCont, j++, TRUE);
			// pContact = pContact->next;
		// }
	}

	if(cpContacts == 0 || g_hSeveralContacts){
		//no contscts or only one contact or several contacts dialog is up
		EnableMenuItem(hCont, IDM_SEND_TO_SEVERAL, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		//more than one contact
		EnableMenuItem(hCont, IDM_SEND_TO_SEVERAL, MF_BYCOMMAND | MF_ENABLED);
	}
}

void BuildExternalMenu(HMENU hMenu){
	HMENU					hExt;
	int						count, id = EXTERNALS_ADDITION, j = 0;
	MITEM					mit;
	LPPSENG					pExternals, pExt, pTemp;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;

	hExt = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_RUN_PROG));
	count = GetMenuItemCount(hExt);
	for(int i = count - 1; i >= 0; i--){
		FreeSingleMenu(hExt, i);
		DeleteMenu(hExt, i, MF_BYPOSITION);
	}

	pExternals = g_PExternalls;
	count = SEngsCount(g_PExternalls);
	pExt = calloc(count, sizeof(PSENG));
	if(pExt){
		pTemp = pExt;
		while(pExternals){
			wcscpy(pTemp->name, pExternals->name);
			pTemp++;
			pExternals = pExternals->next;
		}
		pTemp = pExt;
		if(count > 1){
			qsort(pTemp, count, sizeof(PSENG), SengsCompareAsc);
		}
		for(int i = 0; i < count; i++){
			mit.id = id++;
			wcscpy(mit.szText, pTemp->name);
			AppendMenuW(hExt, MF_STRING, mit.id, mit.szText);
			SetMenuItemProperties(&mit, hExt, j++, TRUE);
			pTemp++;
		}
		free(pExt);
	}
}

void BuildShowHideByTagsMenu(HMENU hMenu){
	HMENU					hShow, hHide;
	int						count, idShow = SHOW_BY_TAG_ADDITION, idHide = HIDE_BY_TAG_ADDITION, j = 0;
	LPPTAG					pTag;
	MITEM					mitS = {0}, mitH = {0};

	mitS.xPos = -1;
	mitS.yPos = -1;
	mitS.xCheck = -1;
	mitS.yCheck = -1;
	mitH.xPos = -1;
	mitH.yPos = -1;
	mitH.xCheck = -1;
	mitH.yCheck = -1;
	pTag = g_PTagsPredefined;
	hShow = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_SHOW_BY_TAG));
	hHide = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_HIDE_BY_TAG));
	count = GetMenuItemCount(hShow);
	for(int i = count - 1; i >= 0; i--){
		FreeSingleMenu(hShow, i);
		DeleteMenu(hShow, i, MF_BYPOSITION);
		FreeSingleMenu(hHide, i);
		DeleteMenu(hHide, i, MF_BYPOSITION);
	}
	while(pTag){
		mitS.id = idShow++;
		mitH.id = idHide++;
		wcscpy(mitS.szText, pTag->text);
		wcscpy(mitH.szText, pTag->text);
		AppendMenuW(hShow, MF_STRING, mitS.id, mitS.szText);
		SetMenuItemProperties(&mitS, hShow, j, TRUE);
		AppendMenuW(hHide, MF_STRING, mitH.id, mitH.szText);
		SetMenuItemProperties(&mitH, hHide, j, TRUE);
		j++;
		pTag = pTag->next;
	}
}

HWND GetNeededArrow(int result){
	HWND hTemp = NULL;
	switch(result){
		case DA_LEFT_UP:
			if(!g_DArrows.hlu)
				g_DArrows.hlu = CreateDArrow(g_hInstance, g_hMain, DA_LEFT_UP);
			hTemp = g_DArrows.hlu;
			break;
		case DA_LEFT_DOWN:
			if(!g_DArrows.hld)
				g_DArrows.hld = CreateDArrow(g_hInstance, g_hMain, DA_LEFT_DOWN);
			hTemp = g_DArrows.hld;
			break;
		case DA_TOP_LEFT:
			if(!g_DArrows.htl)
				g_DArrows.htl = CreateDArrow(g_hInstance, g_hMain, DA_TOP_LEFT);
			hTemp = g_DArrows.htl;
			break;
		case DA_TOP_RIGHT:
			if(!g_DArrows.htr)
				g_DArrows.htr = CreateDArrow(g_hInstance, g_hMain, DA_TOP_RIGHT);
			hTemp = g_DArrows.htr;
			break;
		case DA_RIGHT_UP:
			if(!g_DArrows.hru)
				g_DArrows.hru = CreateDArrow(g_hInstance, g_hMain, DA_RIGHT_UP);
			hTemp = g_DArrows.hru;
			break;
		case DA_RIGHT_DOWN:
			if(!g_DArrows.hrd)
				g_DArrows.hrd = CreateDArrow(g_hInstance, g_hMain, DA_RIGHT_DOWN);
			hTemp = g_DArrows.hrd;
			break;
		case DA_BOTTOM_LEFT:
			if(!g_DArrows.hbl)
				g_DArrows.hbl = CreateDArrow(g_hInstance, g_hMain, DA_BOTTOM_LEFT);
			hTemp = g_DArrows.hbl;
			break;
		case DA_BOTTOM_RIGHT:
			if(!g_DArrows.hbr)
				g_DArrows.hbr = CreateDArrow(g_hInstance, g_hMain, DA_BOTTOM_RIGHT);
			hTemp = g_DArrows.hbr;
			break;
	}
	return hTemp;
}

void CheckSavedRectangle(LPRECT lpRect){
	SIZE		sz;
	int			w, h;

	sz = GetScreenMetrics();
	w = lpRect->right - lpRect->left;
	h = lpRect->bottom - lpRect->top;
	if(w > DEF_SIZE_MAX)
		lpRect->right = lpRect->left + DEF_SIZE_MAX;
	else if(w < DEF_SIZE_MIN)
		lpRect->right = lpRect->left + DEF_SIZE_MIN;
	if(h > DEF_SIZE_MAX)
		lpRect->bottom = lpRect->top + DEF_SIZE_MAX;
	else if(h < DEF_SIZE_MIN)
		lpRect->bottom = lpRect->top + DEF_SIZE_MIN;

	if(lpRect->left < 0){
		lpRect->left = 0;
		lpRect->right = w;
	}
	else if(lpRect->left > sz.cx){
		lpRect->left = sz.cx - w;
		lpRect->right = lpRect->left + w;
	}
	if(lpRect->top < 0){
		lpRect->top = 0;
		lpRect->bottom = h;
	}
	else if(lpRect->top > sz.cy){
		lpRect->top = sz.cy - h;
		lpRect->bottom = lpRect->top + h;
	}
}

SIZE GetScreenMetrics(void){
	SIZE		sz;
	RECT		rcDesktop;

	if(GetSystemMetrics(SM_CMONITORS) == 1){
		GetClientRect(GetDesktopWindow(), &rcDesktop);
		sz.cx = rcDesktop.right - rcDesktop.left;
		sz.cy = rcDesktop.bottom - rcDesktop.top;
	}
	else{
		sz.cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		sz.cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	return sz;
}

BOOL AreDatesEqual(LPSYSTEMTIME lpst1, LPSYSTEMTIME lpst2){
	if(lpst1->wYear == lpst2->wYear && lpst1->wMonth == lpst2->wMonth && lpst1->wDay == lpst2->wDay)
		return TRUE;
	return FALSE;
}

void BuildDiaryMenu(HMENU hParent){
	HMENU					hDiary;
	int						count, pages = 0, id = 0;
	MITEM					mit = {0};
	PMEMNOTE				pNote;
	SYSTEMTIME				st;
	P_DIARY_MENU_STRUCT		pPages = NULL, pTemp = NULL;

	hDiary = GetSubMenu(hParent, GetMenuPosition(hParent, IDM_DIARY));
	count = GetMenuItemCount(hDiary);
	for(int i = count - 1; i > 0; i--){
		FreeSingleMenu(hDiary, i);
		DeleteMenu(hDiary, i, MF_BYPOSITION);
	}
	if(IsBitOn(g_NoteSettings.reserved1, SB1_NO_DIARY_PAGES))
		return;

	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;

	GetLocalTime(&st);

	pNote = MemoryNotes();
	while(pNote){
		if(pNote->pData->idGroup == GROUP_DIARY || pNote->pData->idPrevGroup == GROUP_DIARY){
			if(!AreDatesEqual(&st, pNote->pCreated)){
				if(pages == 0){
					pPages = calloc(1, sizeof(DIARY_MENU_STRUCT));
				}
				else{
					pPages = realloc(pPages, sizeof(DIARY_MENU_STRUCT) * (pages + 1));
				}
				memcpy(&pPages[pages].tCreated, pNote->pCreated, sizeof(SYSTEMTIME));
				wcscpy(pPages[pages].pName, pNote->pData->szName);
				wcscpy(pPages[pages].pReserved, pNote->pFlags->id);
				pages++;
			}
		}
		pNote = pNote->next;
	}
	if(pages > 0){
		AppendMenuW(hDiary, MF_SEPARATOR | MF_OWNERDRAW, 0, 0);
		pTemp = pPages;
		if(!IsBitOn(g_NoteSettings.reserved1, SB1_DIARY_SORT_ASC))
			qsort(pTemp, pages, sizeof(DIARY_MENU_STRUCT), DiaryMenusCompareDesc);
		else
			qsort(pTemp, pages, sizeof(DIARY_MENU_STRUCT), DiaryMenusCompareAsc);
		if(pages > GetSmallValue(g_SmallValues, SMS_DIARY_PAGES))
			pages = GetSmallValue(g_SmallValues, SMS_DIARY_PAGES);
		for(int i = 0; i < pages; i++){
			mit.id = ++id + DIARY_ADDITION;
			wcscpy(mit.szText, pTemp->pName);
			wcscpy(mit.szReserved, pTemp->pReserved);
			AppendMenuW(hDiary, MF_STRING, mit.id, mit.szText);
			SetMenuItemProperties(&mit, hDiary, i + 2, TRUE);
			pTemp++;
		}
		free(pPages);
	}
}

void ApplyHotDialogLanguage(HWND hwnd){
	wchar_t 	szBuffer[256], szKey[12];

	_itow(IDC_GRP_HOTKEYS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Hot keys", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_CHNGE_HK, g_NotePaths.CurrLanguagePath, L"Type the keys to be used as hot keys for selected command");
}

void HKeysAlreadyRegistered(wchar_t * lpKey){
	wchar_t			szMessage[256];

	szMessage[0] = '\0';
	wcscpy(szMessage, g_Strings.HK1);
	wcscat(szMessage, L" ");
	wcscat(szMessage, lpKey);
	wcscat(szMessage, L" ");
	wcscat(szMessage, g_Strings.HK2);
	MessageBoxW(g_hMain, szMessage, L"Re-register hot key", MB_OK | MB_ICONEXCLAMATION);
}

BOOL RestrictedHotKey(wchar_t * szKey){
	if(_wcsicmp(szKey, L"Ctrl+S") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+C") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+X") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+V") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+O") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+P") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+F") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+A") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+Z") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+Y") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"F3") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+G") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+B") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+E") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+U") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+I") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+R") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+L") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+K") == 0)
		return TRUE;
	return FALSE;
}

void EncryptAll(wchar_t * lpHash){
	PMEMNOTE			pNote = MemoryNotes();
	wchar_t				szPath[MAX_PATH], szTempPath[MAX_PATH + 128];
	WIN32_FIND_DATAW	fd;
	BOOL				result = TRUE;
	HANDLE				handle = INVALID_HANDLE_VALUE;

	GetNoteTempFileName(szTempPath);
	while(pNote){
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, pNote->pFlags->id);
		wcscat(szPath, NOTE_EXTENTION);
		if(CryptEncryptFile(szPath, szTempPath, lpHash)){
			MoveFileExW(szTempPath, szPath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		}
		pNote = pNote->next;
	}
	//encrypt backup files
	wcscpy(szPath, g_NotePaths.BackupDir);
	wcscat(szPath, L"*_*");
	wcscat(szPath, BACK_NOTE_EXTENTION);
	handle = FindFirstFileW(szPath, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szPath, g_NotePaths.BackupDir);
			wcscat(szPath, fd.cFileName);
			if(CryptEncryptFile(szPath, szTempPath, lpHash)){
				MoveFileExW(szTempPath, szPath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
			}
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
}

void DecryptAll(wchar_t * lpHash){
	PMEMNOTE			pNote = MemoryNotes();
	wchar_t				szPath[MAX_PATH], szTempPath[MAX_PATH + 128];
	WIN32_FIND_DATAW	fd;
	BOOL				result = TRUE;
	HANDLE				handle = INVALID_HANDLE_VALUE;

	GetNoteTempFileName(szTempPath);
	while(pNote){
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, pNote->pFlags->id);
		wcscat(szPath, NOTE_EXTENTION);
		if(CryptDecryptFile(szPath, szTempPath, lpHash)){
			MoveFileExW(szTempPath, szPath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		}
		pNote = pNote->next;
	}
	//decrypt backup files
	wcscpy(szPath, g_NotePaths.BackupDir);
	wcscat(szPath, L"*_*");
	wcscat(szPath, BACK_NOTE_EXTENTION);
	handle = FindFirstFileW(szPath, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szPath, g_NotePaths.BackupDir);
			wcscat(szPath, fd.cFileName);
			if(CryptDecryptFile(szPath, szTempPath, lpHash)){
				MoveFileExW(szTempPath, szPath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
			}
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawComboItem
 Created  : Thu Aug  9 17:41:23 2007
 Modified : Thu Aug  9 17:41:23 2007

 Synopsys : Draws item in owner-drawn combo box. By unknown reason combo 
            box cannot display properly Unicode strings on some computers, 
            if the language is not specified as "language fo non-Unicode 
            programs". All questions to microsoft.com :)
 Input    : lpd - pointer to DRAWITEMSTRUCT struct
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void DrawComboItem(const DRAWITEMSTRUCT * lpd){

	wchar_t			szBuffer[SendMessageW(lpd->hwndItem, CB_GETLBTEXTLEN, lpd->itemID, 0)];
	RECT			rc;

	CopyRect(&rc, &lpd->rcItem);
	// set back mode to transparent
	SetBkMode(lpd->hDC, TRANSPARENT);
	if((lpd->itemState & ODS_SELECTED) == ODS_SELECTED){
		SetTextColor(lpd->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		FillRect(lpd->hDC, &lpd->rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
		if((lpd->itemState & ODS_FOCUS) == ODS_FOCUS){
			DrawFocusRect(lpd->hDC, &lpd->rcItem);
		}
	}
	else{
		if(((lpd->itemState & ODS_DISABLED) == ODS_DISABLED) || ((lpd->itemState & ODS_GRAYED) == ODS_GRAYED)){
			SetTextColor(lpd->hDC, GetSysColor(COLOR_GRAYTEXT));
		}
		else{
			SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
		}
		FillRect(lpd->hDC, &lpd->rcItem, GetSysColorBrush(COLOR_WINDOW));
	}
	SendMessageW(lpd->hwndItem, CB_GETLBTEXT, lpd->itemID, (LPARAM)szBuffer);
	OffsetRect(&rc, 4, 0);
	DrawTextExW(lpd->hDC, szBuffer, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL);
}

BOOL IsDateEmpty(LPSYSTEMTIME lpd){
	if(lpd->wDay == 0 && lpd->wDayOfWeek == 0 && lpd->wHour == 0 && lpd->wMilliseconds == 0 && lpd->wMinute == 0 && lpd->wMonth == 0 && lpd->wSecond == 0 && lpd->wYear == 0)
		return TRUE;
	else
		return FALSE;
}

long CompareSystemTime(const SYSTEMTIME d1, const SYSTEMTIME d2){
	FILETIME				ft1, ft2;

	SystemTimeToFileTime(&d1, &ft1);
    SystemTimeToFileTime(&d2, &ft2);
	return CompareFileTime(&ft1, &ft2);
}

void GetSystemStartTime(void){
	// FILETIME				f1, f2, f3, lf;
	// SYSTEMTIME				t1, t2, t3;

	// GetSystemTimes(&f1, &f2, &f3);
	// FileTimeToLocalFileTime(&f1, &lf);
	// FileTimeToSystemTime(&lf, &t1);
	// FileTimeToLocalFileTime(&f2, &lf);
	// FileTimeToSystemTime(&lf, &t2);
	// FileTimeToLocalFileTime(&f3, &lf);
	// FileTimeToSystemTime(&lf, &t3);
	// ZeroMemory(&t3, sizeof(SYSTEMTIME));
	// FileTimeToSystemTime(&f1, &t1);
	// FileTimeToSystemTime(&f2, &t2);
	// FileTimeToSystemTime(&f3, &t3);
}

SYSTEMTIME DateSubtract(int days){
	SYSTEMTIME		st = {0};

	GetLocalTime(&st);
	if(st.wDay > days){
		st.wDay -= days;
	}
	else{
		switch(st.wMonth){
			case 1:		//January
				st.wDay = 31 - (days - st.wDay);
				st.wMonth = 12;
				st.wYear -= 1;
				break;
			case 2:		//February
			case 4:		//April
			case 6:		//June
			case 8:		//August
			case 9:		//September
			case 11:		//November
				st.wDay = 31 - (days - st.wDay);
				st.wMonth -= 1;
				break;
			case 5:		//May
			case 7:		//July
			case 10:		//October
			case 12:		//December
				st.wDay = 30 - (days - st.wDay);
				st.wMonth -= 1;
				break;
			case 3:		//March
				if(IsLeapYear(st.wYear))
					st.wDay = 29 - (days - st.wDay);
				else
					st.wDay = 28 - (days - st.wDay);
				st.wMonth -= 1;
				break;
		}
	}
	return st;
}

int DateDiff(const SYSTEMTIME d1, const SYSTEMTIME d2, ddiff diffType){

	TIMEUNION 				ft1;
    TIMEUNION 				ft2;
	unsigned long long		delta;
	// int						multiplier = 1;
	int 					result;

    SystemTimeToFileTime(&d1, &ft1.fileTime);
    SystemTimeToFileTime(&d2, &ft2.fileTime);

	if(CompareFileTime(&ft1.fileTime, &ft2.fileTime) < 0){
    	delta = ft2.ul.QuadPart - ft1.ul.QuadPart;
		// multiplier = -1;
	}
	else{
		delta = ft1.ul.QuadPart - ft2.ul.QuadPart;
		// multiplier = 1;
	}
	// delta = ft1.ul.QuadPart - ft2.ul.QuadPart;

	switch(diffType){
		case SECOND:
			result =  delta / 10000000;
			break;
		case MINUTE:
			result = delta / 10000000 / 60;
			break;
		case HOUR:
			result = delta / 10000000 / 60 / 60;
			break;
		case DAY:
			result = delta / 10000000 / 60 / 60 / 24;
			break;
		case WEEK:
			result = delta / 10000000 / 60 / 60 / 24 / 7;
			break;
		case MONTH:
			result = delta / 10000000 / 60 / 60 / 24 / 30;
			break;
		case YEAR:
			result = delta / 10000000 / 60 / 60 / 24 / 365;
			break;
		default:
			result = 0;
			break;
	}
	return result;
}

short DockIndex(int dockData){
	return HIWORD(dockData);
}

int DockType(int dockData){
	return LOWORD(dockData);
}

void SaveNewDockData(HWND hwnd, int dockData){
	P_NOTE_FLAGS		pF = NoteFlags(hwnd);
	NOTE_DATA			data;

	if(pF->fromDB){
		if(GetPrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			data.dockData = dockData;
			WritePrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
		else{
			//version 3.5
			if(GetPrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA) - sizeof(RECT) - sizeof(BOOL), g_NotePaths.DataFile)){
				data.dockData = dockData;
				WritePrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			}
		}
	}
}

void SetDockIndex(int * dockData, short index){
	short		tl = LOWORD(*dockData);

	*dockData = MAKELONG(tl, index);
}

HWND MoveDockWindow(HWND hwnd, int dockType, short index){
	RECT			rcDesktop, rcTray;
	int				cx, cy, sx = 0, sy = 0, mcount;
	int				tLeft = 0, tTop = 0;
	BITMAP			bm;
	HWND			hTray = NULL;

	hTray = FindWindow("Shell_TrayWnd", NULL);
	GetWindowRect(hTray, &rcTray);

	mcount = GetSystemMetrics(SM_CMONITORS);
	if(mcount == 1){
		GetClientRect(GetDesktopWindow(), &rcDesktop);
		sx = rcDesktop.right - rcDesktop.left;
		sy = rcDesktop.bottom - rcDesktop.top;
	}
	else{
		sx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		sy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	
	//top
	if(rcTray.left == 0 && rcTray.top == 0 && rcTray.bottom - rcTray.top < rcTray.right - rcTray.left){
		// sy -= (rcTray.bottom - rcTray.top);
		tTop = rcTray.bottom - rcTray.top;
	}
	//bottom
	else if(rcTray.left == 0 && rcTray.bottom == sy && rcTray.bottom - rcTray.top < rcTray.right - rcTray.left){
		sy -= (rcTray.bottom - rcTray.top);
	}
	//left
	else if(rcTray.left == 0 && rcTray.top == 0 && rcTray.bottom == sy){
		// sx -= (rcTray.right - rcTray.left);
		tLeft = rcTray.right - rcTray.left;
	}
	//right
	else if(rcTray.right == sx && rcTray.top == 0 && rcTray.bottom == sy){
		sx -= (rcTray.right - rcTray.left);
	}
	if(!g_RTHandles.hbSkin){
		if(g_DockSettings.fCustSize){
			cx = g_DockSettings.custSize.cx;
			cy = g_DockSettings.custSize.cy;
		}
		else{
			cx = g_RTHandles.szDef.cx;
			cy = g_RTHandles.szDef.cy;
		}
	}
	else{
		if(g_DockSettings.fCustSkin){
			GetObject(g_DRTHandles.hbSkin, sizeof(bm), &bm);
		}
		else{
			GetObject(g_RTHandles.hbSkin, sizeof(bm), &bm);
		}
		cx = bm.bmWidth;
		cy = bm.bmHeight;
	}

	switch(dockType){
	case DOCK_LEFT:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, tLeft, sy - index * (cy + g_DockSettings.dist) - cy, cx, cy, TRUE);
		else
			MoveWindow(hwnd, tLeft, index * (cy + g_DockSettings.dist), cx, cy, TRUE);
		break;
	case DOCK_TOP:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, sx - index * (cx + g_DockSettings.dist) - cx, tTop, cx, cy, TRUE);
		else
			MoveWindow(hwnd, index * (cx + g_DockSettings.dist), tTop, cx, cy, TRUE);
		break;
	case DOCK_RIGHT:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, sx - (cx + g_DockSettings.dist), sy - index * (cy + g_DockSettings.dist) - cy, cx, cy, TRUE);
		else
			MoveWindow(hwnd, sx - (cx + g_DockSettings.dist), index * (cy + g_DockSettings.dist), cx, cy, TRUE);
		break;
	case DOCK_BOTTOM:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, sx - index * (cx + g_DockSettings.dist) - cx, sy - (cy + g_DockSettings.dist), cx, cy, TRUE);
		else
			MoveWindow(hwnd, index * (cx + g_DockSettings.dist), sy - (cy + g_DockSettings.dist), cx, cy, TRUE);
		break;
	}

	return hTray;
}

void BitOff(int * data, int position){
	*data &= ~ (1 << position);
}

void BitOn(int * data, int position){
	*data |= (1 << position);
}

BOOL IsBitOn(int data, int position){
	if((data & (1 << position)) == (1 << position))
		return TRUE;
	return FALSE;
}

int GetBit(int data, int position){
	return data & (1 << position);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ConstructDateTimeString
 Created  : Fri May 25 22:46:50 2007
 Modified : Fri May 25 22:46:50 2007

 Synopsys : Constructs date-time string
 Input    : lpSt - LPSYSTEMTIME pointer
            lpBuffer - string to return
            bWithMilliseconds - whether milliseconds should be included
 Output   : Formatted date-time string
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ConstructDateTimeString(const LPSYSTEMTIME lpSt, wchar_t * lpBuffer){
	wchar_t			szTime[128], szDTimePart[128];
	wchar_t			szDFormat[128], szTFormat[128], szTemp[128];
	int				result;
	SYSTEMTIME		st = {0}, *lst;

	if(lpSt == NULL){
		GetLocalTime(&st);
		lst = &st;
	}
	else{
		lst = lpSt;
	}
	szDTimePart[0] = '\0';
	result = ParseDateFormat(g_DTFormats.DateFormat, szDFormat, szTFormat);

	GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, lst, szDFormat, lpBuffer, 128);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, lst, szTFormat, szDTimePart, 128);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, lst, g_DTFormats.TimeFormat, szTime, 128);
	if(result == 1)
		wcscat(lpBuffer, szDTimePart);
	else{
		wcscpy(szTemp, lpBuffer);
		wcscpy(lpBuffer, szDTimePart);
		wcscat(lpBuffer, szTemp);
	}

	// wchar_t			szTime[32], szDFormat[128], szTFormat[128], szTemp[128];
	// wchar_t			szMsec[4];
	// int				result = ParseDateFormat(g_DTFormats.DateFormat, szDFormat, szTFormat);

	// GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, lpSt, szDFormat, lpBuffer, 64);
	// GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, lpSt, szTFormat, szTime, 32);
	
	// if(result == 1){
		// wcscat(lpBuffer, L" ");
		// wcscat(lpBuffer, szTime);
		// if(bWithMilliseconds){
			// wsprintfW(szMsec, L"%u", lpSt->wMilliseconds);
			// wcscat(lpBuffer, L".");
			// wcscat(lpBuffer, szMsec);
		// }
	// }
	// else{
		// wcscpy(szTemp, lpBuffer);
		// wcscpy(lpBuffer, szTime);
		// if(bWithMilliseconds){
			// wsprintfW(szMsec, L"%u", lpSt->wMilliseconds);
			// wcscat(lpBuffer, L".");
			// wcscat(lpBuffer, szMsec);
		// }
		// wcscat(lpBuffer, L" ");
		// wcscat(lpBuffer, szTemp);
	// }
}

void GetFormattedDateTime(wchar_t * lpDate){
	SYSTEMTIME		st;
	wchar_t			szTime[128], szDTimePart[128];
	wchar_t			szDFormat[128], szTFormat[128], szTemp[128];
	int				result;

	szDTimePart[0] = '\0';
	result = ParseDateFormat(g_DTFormats.DateFormat, szDFormat, szTFormat);
		
	ZeroMemory(&st, sizeof(st));
	GetLocalTime(&st);
	GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szDFormat, lpDate, 128);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szTFormat, szDTimePart, 128);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, g_DTFormats.TimeFormat, szTime, 128);
	if(result == 1)
		wcscat(lpDate, szDTimePart);
	else{
		wcscpy(szTemp, lpDate);
		wcscpy(lpDate, szDTimePart);
		wcscat(lpDate, szTemp);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetDlgCtlText
 Created  : Sat May 19 12:57:18 2007
 Modified : Sat May 19 12:57:18 2007

 Synopsys : Sets dialog item text accordingly to language
 Input    : Dialog handle, dialog item id, language file, default text
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SetDlgCtlText(HWND hwnd, int id, const wchar_t * lpFile, const wchar_t * lpDefault){

	wchar_t 		szBuffer[512], szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, lpDefault, szBuffer, 511, lpFile);
	SetDlgItemTextW(hwnd, id, szBuffer);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetFroupText
 Created  : Sat May 19 12:57:18 2007
 Modified : Sat May 19 12:57:18 2007

 Synopsys : Sets group text accordingly to language
 Input    : Dialog handle, group id, text id, language file, default text
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SetGroupText(HWND hwnd, int idGroup, int id, const wchar_t * lpFile, const wchar_t * lpDefault){

	wchar_t 		szBuffer[256], szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(S_LEFT_PANEL, szId, lpDefault, szBuffer, 256, lpFile);
	SetDlgItemTextW(hwnd, idGroup, szBuffer);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMenuText
 Created  : Sat May 19 12:55:45 2007
 Modified : Sat May 19 12:55:45 2007

 Synopsys : Sets menu item text from language file
 Input    : Menu handle, menu item id, language file section, language file 
            name, buffer for menu text
 Output   : Buffer with menu item text
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void SetMenuText(int id, const wchar_t * lcpSection, const wchar_t * lcpFile, wchar_t * lpDefault, wchar_t * lpBuffer){

	wchar_t 		szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(lcpSection, szId, lpDefault, lpBuffer, 256, lcpFile);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMenuTextWithAccelerator
 Created  : Fri May 25 22:40:17 2007
 Modified : Fri May 25 22:41:14 2007

 Synopsys : Sets menu item text from language file with accelerator
 Input    : Menu handle, menu item id, language file section, language file 
            name, buffer for menu text, accelerator text
 Output   : Buffer with menu item text
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void SetMenuTextWithAccelerator(int id, const wchar_t * lpSection, const wchar_t * lpFile, wchar_t * lpDefault, wchar_t * lpBuffer, const wchar_t * lpAcc){

	wchar_t 		szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(lpSection, szId, lpDefault, lpBuffer, 256, lpFile);
	wcscat(lpBuffer, L"\t");
	wcscat(lpBuffer, lpAcc);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FileExists
 Created  : Mon May 14 12:33:55 2007
 Modified : Mon May 14 12:33:55 2007

 Synopsys : Checks file existance by directory and file name
 Input    : Directory, file name
 Output   : TRUE if exists, otherwise FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL FileExists(wchar_t * lpDir, wchar_t * lpFile){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	wcscpy(filespec, lpDir);
	wcscat(filespec, lpFile);
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		FindClose(handle);
		return TRUE;
	}
	return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FillSkins
 Created  : Sat May 19 12:54:14 2007
 Modified : Sat May 19 12:54:14 2007

 Synopsys : Fills skins list box
 Input    : Options dialog handle, current skin name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void FillSkins(HWND hwnd, const wchar_t * lpSkinCurrent, BOOL fAllSkins, BOOL fShowNoSkin){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	wchar_t				szSkin[256];
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL 				result = TRUE, skinSelected = FALSE;
	LRESULT 			added;

	if(fShowNoSkin)
		SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_ADDSTRING, 0, (LPARAM)DS_NO_SKIN);
	if(fAllSkins){
		wcscpy(filespec, g_NotePaths.SkinDir);
		wcscat(filespec, L"*.skn");
		handle = FindFirstFileW(filespec, &fd);
		if(handle != INVALID_HANDLE_VALUE){
			while(result){
				wcscpy(szSkin, fd.cFileName);
				szSkin[wcslen(szSkin) - 4] = '\0';
				added = SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_ADDSTRING, 0, (LPARAM)szSkin);
				if(wcscmp(fd.cFileName, lpSkinCurrent) == 0){
					SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, added, 0);
					skinSelected = TRUE;
				}
				result = FindNextFileW(handle, &fd);
			}
			FindClose(handle);
		}
	}
	if(!skinSelected && fShowNoSkin)
		SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, 0, 0);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DoesAnySkinExist
 Created  : Sat May 19 12:52:28 2007
 Modified : Sat May 19 12:52:28 2007

 Synopsys : Checks whether any skin file exists
 Input    : Buffer for skin file name
 Output   : TRUE if any skin file exists, otherwise - FALSE.
            Skin file name in buffer.
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

BOOL DoesAnySkinExist(wchar_t * lpSkin){
	
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	wcscpy(filespec, g_NotePaths.SkinDir);
	wcscat(filespec, L"*.skn");
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		wcscpy(lpSkin, fd.cFileName);
		FindClose(handle);
		return TRUE;
	}
	else
		return FALSE;
}

void ClearRTHandles(P_NOTE_RTHANDLES pH){
	if(pH->hFCaption)
		DeleteFont(pH->hFCaption);
	if(pH->hFont)
		DeleteFont(pH->hFont);
	if(pH->hbBack)
		DeleteBitmap(pH->hbBack);
	if(pH->hbCommand)
		DeleteBitmap(pH->hbCommand);
	if(pH->hbDelHide)
		DeleteBitmap(pH->hbDelHide);
	if(pH->hbSkin)
		DeleteBitmap(pH->hbSkin);
	if(pH->hbSys)
		DeleteBitmap(pH->hbSys);
	pH->hFCaption = pH->hFont = NULL;
	pH->hbBack = pH->hbCommand = pH->hbDelHide = pH->hbSkin = pH->hbSys = NULL;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawSkinPreview
 Created  : Sat May 19 12:50:51 2007
 Modified : Sat May 19 12:50:51 2007

 Synopsys : Draws skin on skin preview window
 Input    : Preview window handle, skin bitmap, DRAWITEMSTRUCT from 
            WM_DRAWITEM message
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void DrawSkinPreview(HWND hwnd, P_NOTE_RTHANDLES pH, P_NOTE_APPEARANCE pA, COLORREF crMask, const DRAWITEMSTRUCT * lpDI, BOOL drawSimpleIfNoSkin){

	int				state;
	RECT 			rc, rcText, rcPaint;
	HDC				hdcComp;
	BITMAP			bm;
	HBITMAP			hBm;
	int 			w, h;

	if(pH->hbSkin){
		state = SaveDC(lpDI->hDC);
		SetBkMode(lpDI->hDC, TRANSPARENT);
		hdcComp = CreateCompatibleDC(lpDI->hDC);
		GetClientRect(hwnd, &rc);
		if(GetObject(pH->hbSkin, sizeof(bm), &bm)){
			w = rc.right - rc.left;// - 2;
			h = rc.bottom - rc.top;// - 2;
			hBm = CreateCompatibleBitmap(lpDI->hDC, bm.bmWidth, bm.bmHeight);
			SelectBitmap(hdcComp, hBm);
			SetRect(&rcPaint, 0, 0, bm.bmWidth, bm.bmHeight);
			FillRect(hdcComp, &rcPaint, GetSysColorBrush(COLOR_BTNFACE));
			DrawTransparentBitmap(pH->hbSkin, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, 0, 0, crMask);
			SetStretchBltMode(lpDI->hDC, HALFTONE);
			StretchBlt(lpDI->hDC, 0, 0, w, h, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			DeleteBitmap(hBm);
			CopyRect(&rcText, &pH->rcEdit);

			rcText.left *= (double)w / bm.bmWidth;
			rcText.top *= (double)h / bm.bmHeight;
			rcText.right *= (double)w / bm.bmWidth;
			rcText.bottom *= (double)h / bm.bmHeight;
			SelectFont(lpDI->hDC, pH->hFont);
			SetTextColor(lpDI->hDC, pA->crFont);
			if(wcslen(pH->szSkinInfo) > 0)
				DrawTextW(lpDI->hDC, pH->szSkinInfo, -1, &rcText, DT_LEFT | DT_WORDBREAK);
			else
				DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
		}
		DeleteDC(hdcComp);
		RestoreDC(lpDI->hDC, state);
	}
	else{
		if(drawSimpleIfNoSkin)
			DrawSimpleNotePreview(hwnd, lpDI, pA, pH);
		// SendMessageW(GetParent(hwnd), PNM_DRAW_SN_PVW, (WPARAM)hwnd, (LPARAM)lpDI);
	}
}

static PROGRESS_STRUCT PrepareSendSocketsStruct(HWND hwnd, LPARAM lParam){
	PROGRESS_STRUCT		ps = {0};

	wcscpy(ps.szLangFile, g_NotePaths.CurrLanguagePath);

	ps.type = PTYPE_SOCKET;
	ps.hCaller = hwnd;
	wcscpy(ps.szCaptionKey, L"1043");
	wcscpy(ps.szCaptionDef, L"Sending note(s)");
	wcscpy(ps.szProgressKey, L"sending");
	wcscpy(ps.szProgressDef, L"Sending in progress...");
	ps.fShowCancel = FALSE;
	ps.lpStartAddress = SocketClientFunc;
	return ps;
}

static HBITMAP DuplicateCross(HWND hwnd, HBITMAP hbmSrc){
	HDC				hdc, hdcComp;
	BITMAP			bm;
	HBITMAP			hBm, hbmOld;
	RECT			rc;
	HBRUSH			hbr;

	hdc = GetDC(hwnd);
	hdcComp = CreateCompatibleDC(hdc);
	GetObject(hbmSrc, sizeof(bm), &bm);
	hBm = CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);
	hbmOld = SelectBitmap(hdcComp, hBm);
	SetRect(&rc, 0, 0, bm.bmWidth, bm.bmHeight);
	hbr = CreateSolidBrush(CLR_MASK);
	FillRect(hdcComp, &rc, hbr);
	DrawTransparentBitmap(hbmSrc, hdcComp, 0, 0, bm.bmWidth / 2, bm.bmHeight, bm.bmWidth / 2, 0, CLR_MASK);
	DrawTransparentBitmap(hbmSrc, hdcComp, bm.bmWidth / 2, 0, bm.bmWidth / 2, bm.bmHeight, bm.bmWidth / 2, 0, CLR_MASK);
	SelectBitmap(hdcComp, hbmOld);
	DeleteBrush(hbr);
	DeleteDC(hdcComp);
	ReleaseDC(hwnd, hdc);
	DeleteBitmap(hbmSrc);
	return hBm;
}

void DrawSkinPreviewDock(HWND hwnd, P_NOTE_RTHANDLES pH, P_PNDOCK pA, COLORREF crMask, COLORREF crFont, const DRAWITEMSTRUCT * lpDI){

	int				state;
	RECT 			rc, rcText, rcPaint;
	HDC				hdcComp;
	BITMAP			bm;
	HBITMAP			hBm;
	int 			w, h;

	state = SaveDC(lpDI->hDC);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	hdcComp = CreateCompatibleDC(lpDI->hDC);
	GetClientRect(hwnd, &rc);
	if(GetObject(pH->hbSkin, sizeof(bm), &bm)){
		w = rc.right - rc.left;
		h = rc.bottom - rc.top;
		hBm = CreateCompatibleBitmap(lpDI->hDC, bm.bmWidth, bm.bmHeight);
		SelectBitmap(hdcComp, hBm);
		SetRect(&rcPaint ,0 , 0, bm.bmWidth, bm.bmHeight);
		FillRect(hdcComp, &rcPaint, GetSysColorBrush(COLOR_BTNFACE));
		DrawTransparentBitmap(pH->hbSkin, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, 0, 0, crMask);
		SetStretchBltMode(lpDI->hDC, HALFTONE);
		StretchBlt(lpDI->hDC, 0, 0, w, h, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		DeleteBitmap(hBm);
		CopyRect(&rcText, &pH->rcEdit);
		rcText.left *= (double)w / bm.bmWidth;
		rcText.top *= (double)h / bm.bmHeight;
		rcText.right *= (double)w / bm.bmWidth;
		rcText.bottom *= (double)h / bm.bmHeight;
		SelectFont(lpDI->hDC, pH->hFont);
		SetTextColor(lpDI->hDC, crFont);
		if(wcslen(pH->szSkinInfo) > 0)
			DrawTextW(lpDI->hDC, pH->szSkinInfo, -1, &rcText, DT_LEFT | DT_WORDBREAK);
		else
			DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
	}
	else{
		FillRect(lpDI->hDC, &rc, GetSysColorBrush(COLOR_BTNFACE));
	}
	DeleteDC(hdcComp);
	RestoreDC(lpDI->hDC, state);
}

void ReadSimpleRTFFile(HWND hEdit, wchar_t * lpID){
	wchar_t				szPath[MAX_PATH], szTempPath[MAX_PATH + 128], szHash[256];
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	EDITSTREAM			esm;

	wcscpy(szPath, g_NotePaths.DataDir);
	wcscat(szPath, lpID);
	wcscat(szPath, NOTE_EXTENTION);

	//load RTF file contents
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
		esm.dwCookie = (DWORD)hFile;
		esm.dwError = 0;
		esm.pfnCallback = InStreamCallback;
		RichEdit_StreamIn(hEdit, SF_RTF | SF_UNICODE, &esm);
		CloseHandle(hFile);
		if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
			DeleteFileW(szTempPath);
		}
	}		
}

static void InnerReadRTFFile(HWND hEdit, wchar_t * lpPath){
	wchar_t				szTempPath[MAX_PATH + 128], szHash[256];
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	EDITSTREAM			esm;

	//load RTF file contents
	if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
		GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szHash, 256, g_NotePaths.INIFile);
		GetNoteTempFileName(szTempPath);
		if(CryptDecryptFile(lpPath, szTempPath, szHash)){
			hFile = CreateFileW(szTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}
	}
	else{
		hFile = CreateFileW(lpPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if(hFile != INVALID_HANDLE_VALUE){
		esm.dwCookie = (DWORD)hFile;
		esm.dwError = 0;
		esm.pfnCallback = InStreamCallback;
		CReCallback * pCallback = new_CReCallback();
		SendMessageW(hEdit, EM_SETOLECALLBACK, 0, (LPARAM)pCallback);
		RichEdit_StreamIn(hEdit, SF_RTF | SF_UNICODE, &esm);
		if(pCallback)
			delete_CReCallback(pCallback);
		CloseHandle(hFile);
		if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
			DeleteFileW(szTempPath);
		}
		// SendMessageW(hEdit, EM_SETOLECALLBACK, 0, (LPARAM)NULL);
	}		
}

static BOOL GetSkinVertTbr(HINSTANCE hLib){
	HRSRC	hRes;
	HGLOBAL	hGlob;
	LPVOID	hLock;
	int		result;

	hRes = FindResource(hLib, MAKEINTRESOURCE(IDS_VERTICAL_TBR), SKN_VERT_TBR);
	if(hRes){
		hGlob = LoadResource(hLib, hRes);
		if(hGlob){
			hLock = LockResource(hGlob);
			if(hLock){
				memcpy(&result, hLock, sizeof(int));
				if(result == 0)
					return FALSE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

static BOOL GetSkinInfo(HINSTANCE hLib, wchar_t * szBuffer){
	HRSRC	hRes;
	HGLOBAL	hGlob;
	LPVOID	hLock;

	hRes = FindResourceW(hLib, MAKEINTRESOURCEW(IDS_SKIN_INFO), SKN_INFO);
	if(hRes){
		hGlob = LoadResource(hLib, hRes);
		if(hGlob){
			hLock = LockResource(hGlob);
			if(hLock){
				wcscpy(szBuffer, (const wchar_t *)hLock);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetSkinProperties
 Created  : Fri May 25 13:12:42 2007
 Modified : Fri May 25 13:12:42 2007

 Synopsys : Loads all needed skin's properties
 Input    : Window handle, pointer to NOTE_RTHANDLES structure, skin file 
            name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void GetSkinProperties(HWND hWnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkinFile, BOOL bDeletePrevious){
	
	HINSTANCE			hLib;
	GETSKINPROC			hSProc;
	GETCTLRECTPROC		hRProc;
	wchar_t				szPath[MAX_PATH];
	HRSRC				rRes;
	HGLOBAL				rMask;
	RECT				rcTemp;

	wcscpy(szPath, g_NotePaths.SkinDir);
	wcscat(szPath, lpSkinFile);
	//load skin library
	hLib = LoadLibraryW(szPath);
	if(hLib != NULL){
		//get possible skin info
		*pH->szSkinInfo = '\0';
		GetSkinInfo(hLib, pH->szSkinInfo);
		//get possible vertical toolbar
		pH->vertTbr = GetSkinVertTbr(hLib);
		// LoadStringW(hLib, IDS_SKIN_INFO, pH->szSkinInfo, 256);
		//get proc address for bitmap loading procedure
		hSProc = (GETSKINPROC)GetProcAddress(hLib, "GetPNGSkin");
		if(NULL != hSProc){
			//load skin bitmap
			if(pH->hbSkin && bDeletePrevious)
				DeleteBitmap(pH->hbSkin);
			pH->hbSkin = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_BODY);
			//load sysmenu bitmap
			if(pH->hbSys && bDeletePrevious)
				DeleteBitmap(pH->hbSys);
			pH->hbSys = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_SYS);
			//load delhide bitmap
			if(pH->hbDelHide && bDeletePrevious)
				DeleteBitmap(pH->hbDelHide);
			if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_DELETE) && IsBitOn(g_NoteSettings.reserved1, SB1_CROSS_INST_TRNGL))
				pH->hbDelHide = DuplicateCross(hWnd, (HBITMAP)(hSProc)(hWnd, IDS_SKIN_DEL_HIDE));
			else
				pH->hbDelHide = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_DEL_HIDE);
			//load commands bitmap
			if(pH->hbCommand && bDeletePrevious)
				DeleteBitmap(pH->hbCommand);
			pH->hbCommand = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_COMMANDS);
		}
		//get proc address for rectangles loading procedure
		hRProc = (GETCTLRECTPROC)GetProcAddress(hLib, "GetRectangle");
		if(NULL != hRProc){
			//load needed rectangles
			(hRProc)(IDS_EDIT_RECT, &pH->rcEdit);
			(hRProc)(IDS_SCH_RECT, &pH->rcSchedule);
			(hRProc)(IDS_NOT_SAVED_RECT, &pH->rcNotSaved);
			(hRProc)(IDS_PROTECTED_RECT, &pH->rcProtected);
			(hRProc)(IDS_PRIORITY_RECT, &pH->rcPriority);
			(hRProc)(IDS_COMPLETED_RECT, &pH->rcCompleted);
			(hRProc)(IDS_PASSWORD_RECT, &pH->rcPassword);
			(hRProc)(IDS_PIN_RECT, &pH->rcPin);
			(hRProc)(IDS_TT_RECT, &pH->rcTooltip);
			(hRProc)(IDS_COMMANDS_RECT, &pH->rcCommands);
			(hRProc)(IDS_DH_RECT, &pH->rcDH);
			SetRectEmpty(&rcTemp);
			(hRProc)(IDS_MARKS_COUNT, &rcTemp);
			if(rcTemp.left == 0)
				//old skins
				pH->marks = 2;
			else
				pH->marks = rcTemp.left;

			if(NULL != hSProc){
				//get back bitmap for rich edit box transparency emulation
				if(pH->hbBack && bDeletePrevious)
					DeleteBitmap(pH->hbBack);
				GetBackBitmap(pH, &pH->rcEdit);
			}
		}
		rRes = FindResource(hLib, MAKEINTRESOURCE(IDS_MASK_COLOR), SKN_MASK);
		if(!rRes)
			pH->crMask = CLR_MASK;
		else{
			rMask = LoadResource(hLib, rRes);
			if(rMask){
				memcpy(&pH->crMask, rMask, sizeof(COLORREF));
			}
		}
		//free library
		FreeLibrary(hLib);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ChangeSkinPreview
 Created  : Sat May 19 12:49:15 2007
 Modified : Sat May 19 12:49:15 2007

 Synopsys : Changes bitmap for skin preview and redraws preview window
 Input    : Options dialog handle, id of listbox with skin names, skin 
            preview window id, runtime handles, skin name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ChangeSkinPreview(HWND hwnd, int idList, int idPvw, P_NOTE_RTHANDLES pH, wchar_t * lpTempSkin, BOOL fDeletePrev){
	//get skin bitmap and show it in preview window

	wchar_t szFile[256];
	HWND	hList;

	hList = GetDlgItem(hwnd, idList);
	SendMessageW(hList, LB_GETTEXT, SendMessageW(hList, LB_GETCURSEL, 0, 0), (LPARAM)szFile);
	wcscat(szFile, L".skn");
	wcscpy(lpTempSkin, szFile);
	pH->hbSkin = GetSkinById(hwnd, IDS_SKIN_BODY, lpTempSkin);
	GetSkinProperties(hwnd, pH, lpTempSkin, fDeletePrev);
	RedrawWindow(GetDlgItem(hwnd, idPvw), NULL, NULL, RDW_INVALIDATE);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: IsFontsEqual
 Created  : Sat May 19 12:46:09 2007
 Modified : Sat May 19 12:46:09 2007

 Synopsys : Checks whether two LOGFONTW structures are equal
 Input    : Two LOGFONTW structures
 Output   : TRUE if all members of structures are equal, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

BOOL IsFontsEqual(const LOGFONTW * lf1, const LOGFONTW * lf2){
	
	if(lf1->lfHeight != lf2->lfHeight || lf1->lfWidth != lf2->lfWidth 
		|| lf1->lfEscapement != lf2->lfEscapement || lf1->lfOrientation != lf2->lfOrientation
		|| lf1->lfWeight != lf2->lfWeight || lf1->lfItalic != lf2->lfItalic
		|| lf1->lfUnderline != lf2->lfUnderline || lf1->lfStrikeOut != lf2->lfStrikeOut
		|| lf1->lfCharSet != lf2->lfCharSet || lf1->lfOutPrecision != lf2->lfOutPrecision
		|| lf1->lfClipPrecision != lf2->lfClipPrecision || lf1->lfQuality != lf2->lfQuality
		|| lf1->lfPitchAndFamily != lf2->lfPitchAndFamily || wcscmp(lf1->lfFaceName, lf2->lfFaceName) != 0)

		return FALSE;

	return TRUE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ShowPopUp
 Created  : Sat May 19 12:44:33 2007
 Modified : Sat May 19 12:44:33 2007

 Synopsys : Shows popup menu
 Input    : Window handle, popup menu handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ShowPopUp(HWND hwnd, HMENU hMenu){
	POINT pt;

	GetCursorPos(&pt);
	//set window foreground for further PostMessageW call 
	SetForegroundWindow(hwnd);
	//show popup menu
	TrackPopupMenu(hMenu, TPM_RIGHTALIGN, pt.x, pt.y, 0, hwnd, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(hwnd, WM_NULL, 0, 0);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMIText
 Created  : Sat May 19 12:43:38 2007
 Modified : Sat May 19 12:43:38 2007

 Synopsys : Sets text of specified MItem array element
 Input    : MItem structure array, size of array, item id, text to set
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void SetMIText(PMITEM lpMI, const int size, int id, const wchar_t * lpText){

	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			wcscpy(lpMI->szText, lpText);
			break;
		}
		lpMI++;
	}
}

void SetMIImageIndex(PMITEM lpMI, const int size, int id, const int idImg){

	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			lpMI->xPos = idImg;
			break;
		}
		lpMI++;
	}
}

int GetMIImageIndex(PMITEM lpMI, const int size, int id){
	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			return lpMI->xPos;
		}
		lpMI++;
	}
	return -1;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetPNotesMessage
 Created  : Sat May 19 12:41:52 2007
 Modified : Sat May 19 12:41:52 2007

 Synopsys : Gets message from language file
 Input    : Language file directory, language file name, message key, 
            default string, buffer for message
 Output   : Message
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void GetPNotesMessage(const wchar_t * lpKey, const wchar_t * lpDefault, wchar_t * lpMessage){
	GetPrivateProfileStringW(L"messages", lpKey, lpDefault, lpMessage, 256, g_NotePaths.CurrLanguagePath);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CenterWindow
 Created  : Fri May 25 22:42:43 2007
 Modified : Fri May 25 22:42:43 2007

 Synopsys : Centers window on screen
 Input    : hwnd - window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void CenterWindow(HWND hwnd, BOOL fSetDefSize){
	RECT		rcD, rcW;

	GetWindowRect(GetDesktopWindow(), &rcD);
	GetWindowRect(hwnd, &rcW);
	if(!fSetDefSize){
		MoveWindow(hwnd, ((rcD.right - rcD.left) - (rcW.right - rcW.left)) / 2, \
		((rcD.bottom - rcD.top) - (rcW.bottom - rcW.top)) / 2, \
		rcW.right - rcW.left, rcW.bottom - rcW.top, TRUE);
	}
	else{
		MoveWindow(hwnd, ((rcD.right - rcD.left) - g_RTHandles.szDef.cx) / 2, \
		((rcD.bottom - rcD.top) - g_RTHandles.szDef.cy) / 2, \
		g_RTHandles.szDef.cx, g_RTHandles.szDef.cy, TRUE);
	}
}

void RestorePlacement(HWND hwnd, BOOL fSetDefSize){
	RECT		rcW;

	GetWindowRect(hwnd, &rcW);
	if(!fSetDefSize){
		MoveWindow(hwnd, rcW.left, rcW.top, rcW.right - rcW.left, rcW.bottom - rcW.top, TRUE);
	}
	else{
		MoveWindow(hwnd, rcW.left, rcW.top, g_RTHandles.szDef.cx, g_RTHandles.szDef.cy, TRUE);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetTooltip
 Created  : Mon May 28 11:26:49 2007
 Modified : Mon May 28 11:26:49 2007

 Synopsys : Returns tooltip text based on specified menu item text
 Input    : lpTip - string to return
            pItems - pointer to array of MItem
            size - size of array
            id - menu item id
 Output   : Tooltip text
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void GetTooltip(wchar_t * lpTip, PMITEM pItems, int size, UINT id){
	//erase string
	*lpTip = '\0';
	for(int i = 0; i < size; i++, pItems++){
		if(pItems->id == id){
			wcscpy(lpTip, pItems->szText);
			break;
		}
	}
}

/** Private functions ********************************************************/

/*-@@+@@------------------------------------------------------------------
 Procedure: GetBackBitmap
 Created  : Tue Jan  8 11:59:10 2008
 Modified : Tue Jan  8 11:59:10 2008

 Synopsys : Gets bitmap for simulation of transparency
 Input    : pH - note run-time handles
            lpRc - bitmap rectangle
 Output   : Bitmap which will be drawn behind of rich edit
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static HBITMAP GetBackBitmap(P_NOTE_RTHANDLES pH, LPCRECT lpRc){
	HDC			hdc, hdcTemp, hdcBack;
	HBITMAP		hBmpTemp, hBOld, hBOldTemp;

	//create new DC to work with
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	//create compatible DCs
	hdcTemp = CreateCompatibleDC(hdc);
	hdcBack = CreateCompatibleDC(hdc);
	//create bitmap for brush
	hBmpTemp = CreateCompatibleBitmap(hdc, lpRc->right - lpRc->left, lpRc->bottom - lpRc->top);
	//select skin bitmap
	hBOld = SelectBitmap(hdcBack, pH->hbSkin);
	//select brush bitmap
	hBOldTemp = SelectBitmap(hdcTemp, pH->hbBack);
	//copy needed portion of skin bitmap into brush bitmap
	BitBlt(hdcTemp, 0, 0, lpRc->right - lpRc->left, lpRc->bottom - lpRc->top, hdcBack, lpRc->left, lpRc->top, SRCCOPY);
	//clean up
	SelectBitmap(hdcBack, hBOld);
	DeleteDC(hdcBack);
	SelectBitmap(hdcTemp, hBOldTemp);
	DeleteDC(hdcTemp);
	DeleteDC(hdc);
	//return created bitmap
	return hBmpTemp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetSkinById
 Created  : Tue May  8 16:37:08 2007
 Modified : Thu May 10 22:47:56 2007

 Synopsys : Loads specified PNG resource from SKN file
 Input    : Window handle, resource identificator, path to SKN file
 Output   : Skin bitmap handle
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

HBITMAP GetSkinById(HWND hWnd, UINT id, const wchar_t* szFile){

	HINSTANCE		hLib;
	GETSKINPROC		hProc;
	HBITMAP			hBmp = NULL;
	wchar_t			szPath[MAX_PATH];

	wcscpy(szPath, g_NotePaths.SkinDir);
	wcscat(szPath, szFile);
	hLib = LoadLibraryW(szPath);
	if(hLib != NULL){
		hProc = (GETSKINPROC)GetProcAddress(hLib, "GetPNGSkin");
		if(NULL != hProc)
			hBmp = (HBITMAP)(hProc)(hWnd, id);
		FreeLibrary(hLib);
	}
	return hBmp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetSubPathW
 Created  : Mon May 14 12:38:38 2007
 Modified : Mon May 14 12:38:38 2007

 Synopsys : Constructs path to the file, depending on executable path
 Input    : Pointer to result path, file name
 Output   : Result path sent as input
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void GetSubPathW(wchar_t * lpResult, const wchar_t * lpFile){
	GetModuleFileNameW(NULL, lpResult, MAX_PATH);
	PathRemoveFileSpecW(lpResult);
	wcscat(lpResult, lpFile);
}

void SetProperWeekday(int dow, wchar_t * lpType){

	for(int i = 1; i <= 7; i++){
		if(IsBitOn(dow, i)){
			wcscat(lpType, g_Weekdays[i - 1]);
			wcscat(lpType, L", ");
		}
	}
}

int ParseDateFormat(wchar_t * lpFormat, wchar_t * lpDFormat, wchar_t * lpTFormat){

	wchar_t			* pos;
	int				result = 1;

	lpTFormat[0] = '\0';
	wcscpy(lpDFormat, lpFormat);
	pos = _wcsichr(lpDFormat, L'H');
	if(pos){
		if(*pos == *lpFormat){
			result = 0;
			//the time part is at the beginning of string
			wcscpy(lpTFormat, lpFormat);
			pos = wcschr(lpTFormat, ' ');
			if(pos){
				wcscpy(lpDFormat, pos);
				*pos = '\0';
			}
		}
		else{
			wcscpy(lpTFormat, pos);
			*pos = '\0';
		}
	}
	return result;
}

void GetScheduleDescription(int id, wchar_t * lpBuffer, const wchar_t * lpFile, const wchar_t * lpDefault){
	wchar_t 		szId[12];

	_itow(id, szId, 10);
	//get description for schedule type
	GetPrivateProfileStringW(S_OPTIONS, szId, lpDefault, lpBuffer, 256, lpFile);
}

void ParseScheduleType(P_SCHEDULE_TYPE pS, wchar_t * lpType){
	SYSTEMTIME		st;
	wchar_t			szDDatePart[128], szDTimePart[128], szTemp[256], szStart[256], szKey[16];
	wchar_t			szDFormat[128], szTFormat[128];
	wchar_t			szY[128], szMN[128], szW[128], szD[128], szH[128], szM[128], szS[128];
	int				result = ParseDateFormat(g_DTFormats.DateFormat, szDFormat, szTFormat);

	lpType[0] = '\0';
	ZeroMemory(&st, sizeof(st));
	st = pS->scDate;
	switch(pS->scType){
		case SCH_ED:
			GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szTFormat, lpType, 128);
			break;
		case SCH_ONCE:
			GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szDFormat, lpType, 128);
			GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szTFormat, szDTimePart, 128);
			if(result == 1)
				wcscat(lpType, szDTimePart);
			else{
				wcscpy(szTemp, lpType);
				wcscpy(lpType, szDTimePart);
				wcscat(lpType, szTemp);
			}
			break;
		case SCH_REP:
		case SCH_REP + START_PROG:
		case SCH_REP + START_COMP:
			if(st.wYear > 0)
				wsprintfW(szY, L"%d %s ", st.wYear, g_RepParts[0]);
			else
				*szY = '\0';
			if(st.wMonth > 0)
				wsprintfW(szMN, L"%d %s ", st.wMonth, g_RepParts[1]);
			else
				*szMN = '\0';
			if(st.wDayOfWeek > 0)
				wsprintfW(szW, L"%d %s ", st.wDayOfWeek, g_RepParts[2]);
			else
				*szW = '\0';
			if(st.wDay > 0)
				wsprintfW(szD, L"%d %s ", st.wDay, g_RepParts[3]);
			else
				*szD = '\0';
			if(st.wHour > 0)
				wsprintfW(szH, L"%d %s ", st.wHour, g_RepParts[4]);
			else
				*szH = '\0';
			if(st.wMinute > 0)
				wsprintfW(szM, L"%d %s ", st.wMinute, g_RepParts[5]);
			else
				*szM = '\0';
			if(st.wSecond > 0)
				wsprintfW(szS, L"%d %s", st.wSecond, g_RepParts[6]);
			else
				*szS = '\0';
			wsprintfW(lpType, L"%s%s%s%s%s%s%s", szY, szMN, szW, szD, szH, szM, szS);
			_itow(IDC_ST_STARTING, szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, L"Starting from:", szStart, 128, g_NotePaths.CurrLanguagePath);
			if(pS->scType == SCH_REP){
				GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &pS->scStart, szDFormat, szDDatePart, 128);
				GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &pS->scStart, szTFormat, szDTimePart, 128);
				if(result == 1)
					wsprintfW(szTemp, L" %s %s %s", szStart, szDDatePart, szDTimePart);
				else
					wsprintfW(szTemp, L" %s %s %s", szStart, szDTimePart, szDDatePart);
			}
			else if(pS->scType == SCH_REP + START_PROG){
				_itow(IDC_OPT_PROG_START, szKey, 10);
				GetPrivateProfileStringW(S_OPTIONS, szKey, L"Program start", szDDatePart, 128, g_NotePaths.CurrLanguagePath);
				wsprintfW(szTemp, L" %s %s", szStart, szDDatePart);
			}
			else if(pS->scType == SCH_REP + START_COMP){
				_itow(IDC_OPT_COMP_START, szKey, 10);
				GetPrivateProfileStringW(S_OPTIONS, szKey, L"System start", szDDatePart, 128, g_NotePaths.CurrLanguagePath);
				wsprintfW(szTemp, L" %s %s", szStart, szDDatePart);
			}
			wcscat(lpType, szTemp);
			break;
		case SCH_PER_WD:
			SetProperWeekday(st.wDayOfWeek, lpType);
			if(lpType){
				lpType[wcslen(lpType) - 2] = '\0';
			}
			GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, L"' - 'HH:mm", szDTimePart, 128);
			wcscat(lpType, szDTimePart);
			break;
		case SCH_AFTER:
		case SCH_AFTER + START_PROG:
		case SCH_AFTER + START_COMP:
			if(st.wYear > 0)
				wsprintfW(szY, L"%d %s ", st.wYear, g_RepParts[0]);
			else
				*szY = '\0';
			if(st.wMonth > 0)
				wsprintfW(szMN, L"%d %s ", st.wMonth, g_RepParts[1]);
			else
				*szMN = '\0';
			if(st.wDayOfWeek > 0)
				wsprintfW(szW, L"%d %s ", st.wDayOfWeek, g_RepParts[2]);
			else
				*szW = '\0';
			if(st.wDay > 0)
				wsprintfW(szD, L"%d %s ", st.wDay, g_RepParts[3]);
			else
				*szD = '\0';
			if(st.wHour > 0)
				wsprintfW(szH, L"%d %s ", st.wHour, g_RepParts[4]);
			else
				*szH = '\0';
			if(st.wMinute > 0)
				wsprintfW(szM, L"%d %s ", st.wMinute, g_RepParts[5]);
			else
				*szM = '\0';
			if(st.wSecond > 0)
				wsprintfW(szS, L"%d %s", st.wSecond, g_RepParts[6]);
			else
				*szS = '\0';
			wsprintfW(lpType, L"%s%s%s%s%s%s%s", szY, szMN, szW, szD, szH, szM, szS);
			_itow(IDC_ST_STARTING, szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, L"Starting from:", szStart, 128, g_NotePaths.CurrLanguagePath);
			if(pS->scType == SCH_AFTER){
				GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &pS->scStart, szDFormat, szDDatePart, 128);
				GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &pS->scStart, szTFormat, szDTimePart, 128);
				if(result == 1)
					wsprintfW(szTemp, L" %s %s %s", szStart, szDDatePart, szDTimePart);
				else
					wsprintfW(szTemp, L" %s %s %s", szStart, szDTimePart, szDDatePart);
			}
			else if(pS->scType == SCH_AFTER + START_PROG){
				_itow(IDC_OPT_PROG_START, szKey, 10);
				GetPrivateProfileStringW(S_OPTIONS, szKey, L"Program start", szDDatePart, 128, g_NotePaths.CurrLanguagePath);
				wsprintfW(szTemp, L" %s %s", szStart, szDDatePart);
			}
			else if(pS->scType == SCH_AFTER + START_COMP){
				_itow(IDC_OPT_COMP_START, szKey, 10);
				GetPrivateProfileStringW(S_OPTIONS, szKey, L"System start", szDDatePart, 128, g_NotePaths.CurrLanguagePath);
				wsprintfW(szTemp, L" %s %s", szStart, szDDatePart);
			}
			wcscat(lpType, szTemp);
			break;
		case SCH_MONTHLY_EXACT:
			wcscat(lpType, L" - ");
			_itow(st.wDay, szTemp, 10);
			wcscat(lpType, szTemp);
			wcscat(lpType, L", ");
			GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szTFormat, szDTimePart, 128);
			wcscat(lpType, szDTimePart);
			break;
		case SCH_MONTHLY_RELATIVE:
			_itow(pS->scDate.wDayOfWeek, szKey, 10);
			GetPrivateProfileStringW(S_WEEKDAYS_FULL, szKey, NULL, szTemp, 128, g_NotePaths.CurrLanguagePath);
			wcscat(lpType, L" - ");
			wcscat(lpType, szTemp);
			_itow(pS->scDate.wMilliseconds, szKey, 10);
			GetPrivateProfileStringW(S_ORDINALS, szKey, NULL, szTemp, 128, g_NotePaths.CurrLanguagePath);
			wcscat(lpType, L", ");
			wcscat(lpType, szTemp);
			wcscat(lpType, L", ");
			GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szTFormat, szDTimePart, 128);
			wcscat(lpType, szDTimePart);
			break;
	}
}

BOOL IsLeapYear(int year){
	if(((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
		return TRUE;
	return FALSE;
}

short OrdinalDayOfWeek(int dof){
	int		dw;

	switch(dof){
		case 6:
			dw = 0;
			break;
		default:
			dw = dof + 1;
			break;
	}
	return RealDayOfWeek(dw);
}

short RealDayOfWeek(int dof){
	wchar_t			szBuffer[128];

	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:		//Monday
			switch(dof){
				case 0:
					return 6;
				default:
					return dof - 1;
			}
		case 1:		//Tuesday
			switch(dof){
				case 0:
					return 5;
				case 1:
					return 6;
				default:
					return dof - 1;
			}
		case 2:		//Wednesday
			switch(dof){
				case 0:
					return 4;
				case 1:
					return 5;
				case 2:
					return 6;
				default:
					return dof - 1;
			}
		case 3:		//Thursday
			switch(dof){
				case 0:
					return 3;
				case 1:
					return 4;
				case 2:
					return 5;
				case 3:
					return 6;
				default:
					return dof - 1;
			}
		case 4:		//Friday
			switch(dof){
				case 5:
					return 0;
				case 6:
					return 1;
				default:
					return dof + 2;
			}
		case 5:		//Saturday
			switch(dof){
				case 6:
					return 0;
				default:
					return dof + 1;
			}
		default:		//6 = Sunday
			return dof;
	}
}

void FrameSkinlessNote(HDC hdc, LPRECT lprc, COLORREF clr){
	HBRUSH		hbr;
	RECT		rc;

	CopyRect(&rc, lprc);
	// rc.right -= 20;
	// rc.bottom -= 20;
	clr = ColorAdjustLuma(clr, -95, FALSE);
	hbr = CreateSolidBrush(clr);
	FrameRect(hdc, &rc, hbr);
	DeleteBrush(hbr);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FillSkinlessNoteCaption
 Created  : Tue Jan  8 11:55:52 2008
 Modified : Tue Jan  8 11:55:52 2008

 Synopsys : Fills skinless note caption or toolbar with gradient
 Input    : hdc - device context
            lprc - drawing rectangle
            clr - note background color
            lprGrip - sizing grip rectangle (optional)
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void FillSkinlessNoteCaption(HDC hdc, LPRECT lprc, COLORREF clr, LPRECT lprGrip){
	COLORREF			clr1, clr2;
	HPEN				hPen, hOldPen;

	clr1 = clr2 = clr;
	clr1 = ColorAdjustLuma(clr1, 95, FALSE);
	clr2 = ColorAdjustLuma(clr2, -95, FALSE);

	Fill2ColorsRectangle(hdc, lprc, clr1, clr2, GRADIENT_FILL_RECT_V);

	if(lprGrip != NULL){
		//if toolbar is drawn - dra separator line
		hPen = CreatePen(PS_SOLID, 1, clr2);
		hOldPen = SelectPen(hdc, hPen);
		MoveToEx(hdc, lprGrip->right - lprGrip->left, lprGrip->top, NULL);
		LineTo(hdc, lprGrip->left - 1, lprGrip->top);
		SelectPen(hdc, hOldPen);
		DeletePen(hPen);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawSimpleNotePreview
 Created  : Tue Jan  8 11:52:37 2008
 Modified : Tue Jan  8 11:52:37 2008

 Synopsys : Draws preview of skinless note
 Input    : hwnd - handle of preview window
            lpDI - pointer to DRAWITEMSTRUCT structure
            lprWindow - pointer to note's body drawing rectangle
            lprCaption - pointe to note's caption drawing rectangle
            crWindow - note background color
            crFontCaption - caption font color
            hFontCaption - caption font
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void DrawSimpleNotePreview(HWND hwnd, const DRAWITEMSTRUCT * lpDI, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH){

	RECT		rc, rcText;
	RECT		rcWindow, rcCaption;
	int			height, fh;
	HFONT		hOldFont, hOldNoteFont;
	COLORREF	crOldColor, crOldNoteTextColor;
	HBRUSH		hBrush;
	wchar_t		szCaption[64];

	GetPrivateProfileStringW(S_PREVIEW, L"caption", L"Caption", szCaption, 64, g_NotePaths.CurrLanguagePath);
	fh = GetNoteCaptionFontHeight(pH->hFCaption, szCaption);
	hBrush = CreateSolidBrush(pA->crWindow);
	height = GetSystemMetrics(SM_CYSMCAPTION);
	if(fh > (height - 2))
		height = fh + 2;
	GetClientRect(hwnd, &rc);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	CopyRect(&rcWindow, &rc);
	SetRect(&rcCaption, rcWindow.left, rcWindow.top, rcWindow.right, height);
	CopyRect(&rcText, &rcWindow);
	rcText.top = rcText.top + (rcCaption.bottom - rcCaption.top) + 1;
	rcWindow.top += height;
	FillRect(lpDI->hDC, &rcWindow, hBrush);
	FillSkinlessNoteCaption(lpDI->hDC, &rcCaption, pA->crWindow, NULL);
	crOldNoteTextColor = SetTextColor(lpDI->hDC, pA->crFont);
	hOldNoteFont = SelectFont(lpDI->hDC, pH->hFont);
	InflateRect(&rcText, -2, -2);
	DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
	SelectFont(lpDI->hDC, hOldNoteFont);
	SetTextColor(lpDI->hDC, crOldNoteTextColor);
	crOldColor = SetTextColor(lpDI->hDC,pA->crCaption);
	hOldFont = SelectFont(lpDI->hDC, pH->hFCaption);
	DrawTextW(lpDI->hDC, szCaption, -1, &rcCaption, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	SelectFont(lpDI->hDC, hOldFont);
	SetTextColor(lpDI->hDC, crOldColor);
	DeleteBrush(hBrush);
	//frame note
	FrameSkinlessNote(lpDI->hDC, &rcWindow, pA->crWindow);
}

void DrawSimpleNotePreviewDock(HWND hwnd, const DRAWITEMSTRUCT * lpDI, P_PNDOCK pA, COLORREF crFont, P_NOTE_RTHANDLES pH){

	RECT		rc, rcText, rWindow, rCaption;
	int			height, fh;
	HFONT		hOldFont, hOldNoteFont;
	COLORREF	crOldColor, crOldNoteTextColor;
	HBRUSH		hBrush;
	wchar_t		szCaption[64];

	GetPrivateProfileStringW(S_PREVIEW, L"caption", L"Caption", szCaption, 64, g_NotePaths.CurrLanguagePath);
	fh = GetNoteCaptionFontHeight(pH->hFCaption, szCaption);
	hBrush = CreateSolidBrush(pA->crWindow);
	height = GetSystemMetrics(SM_CYSMCAPTION);
	if(fh > (height - 2))
		height = fh + 2;
	GetClientRect(hwnd, &rc);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	FillRect(lpDI->hDC, &rc, GetSysColorBrush(COLOR_BTNFACE));
	CopyRect(&rWindow, &rc);
	SetRect(&rCaption, rWindow.left, rWindow.top, rWindow.right, height);
	CopyRect(&rcText, &rWindow);
	rcText.top = rcText.top + (rCaption.bottom - rCaption.top) + 1;
	rWindow.top += height;
	FillRect(lpDI->hDC, &rWindow, hBrush);
	FillSkinlessNoteCaption(lpDI->hDC, &rCaption, pA->crWindow, NULL);
	crOldNoteTextColor = SetTextColor(lpDI->hDC, crFont);
	hOldNoteFont = SelectFont(lpDI->hDC, pH->hFont);
	InflateRect(&rcText, -2, -2);
	DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
	SelectFont(lpDI->hDC, hOldNoteFont);
	SetTextColor(lpDI->hDC, crOldNoteTextColor);
	crOldColor = SetTextColor(lpDI->hDC,pA->crCaption);
	hOldFont = SelectFont(lpDI->hDC, pH->hFCaption);
	DrawTextW(lpDI->hDC, szCaption, -1, &rCaption, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	SelectFont(lpDI->hDC, hOldFont);
	SetTextColor(lpDI->hDC, crOldColor);
	DeleteBrush(hBrush);
	//frame note
	FrameSkinlessNote(lpDI->hDC, &rWindow, pA->crWindow);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ChangeSkinlessColor
 Created  : Tue Jan  8 11:42:34 2008
 Modified : Tue Jan  8 11:42:34 2008

 Synopsys : Changes skinless note background color
 Input    : hwnd - note window handle
            ptA - pointer to note appearance structure
 Output   : TRUE on success, FALSE otherwise
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL ChangeSkinlessColor(HWND hwnd, LPCOLORREF lpcrWindow, int id){
	CHOOSECOLORW		cc;
	COLORREF			custcc[16];
	wchar_t 			szBuffer[256];

	GetPrivateProfileStringW(L"captions", L"color", L"Choose color", szBuffer, 256, g_NotePaths.CurrLanguagePath);

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	for(int i = 0; i < NELEMS(custcc); i++)
		custcc[i] = 0xffffff;
	cc.lpCustColors = custcc;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;
	cc.hwndOwner = hwnd;
	cc.rgbResult = *lpcrWindow;
	cc.lCustData = (int)szBuffer;
	cc.lpfnHook = CCHookProc;
	if(ChooseColorW(&cc)){
		if(*lpcrWindow != cc.rgbResult){
			*lpcrWindow = (cc.rgbResult == 0) ? DEF_TEXT_COLOR : (cc.rgbResult == CLR_MASK ? CR_IN_MASK : cc.rgbResult);
			RedrawWindow(GetDlgItem(hwnd, id), NULL, NULL, RDW_INVALIDATE);
			return TRUE;
		}
	}
	return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ChangeSkinlessCaptionFont
 Created  : Tue Jan  8 11:28:20 2008
 Modified : Tue Jan  8 11:29:32 2008

 Synopsys : Changes font of skinless note caption
 Input    : hwnd - note window handle
            ptA - pointer to note appearance structure
            ptH - pointer to note run-time handles
 Output   : changed part - font or/and font color
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
int ChangeSkinlessCaptionFont(HWND hwnd, LPLOGFONTW plfCaption, LPCOLORREF lpcrCaption, P_NOTE_RTHANDLES ptH, int id, BOOL fDeleteOld){
	CHOOSEFONTW			cf;
	int					ret = 0;
	LOGFONTW			lfw;
	wchar_t 			szBuffer[256];

	GetPrivateProfileStringW(L"captions", L"font", L"Choose font", szBuffer, 256, g_NotePaths.CurrLanguagePath);

	memcpy(&lfw, plfCaption, sizeof(lfw));
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lfw;
	cf.rgbColors = *lpcrCaption;
	cf.lpfnHook = CFHookProc;
	cf.lCustData = (int)szBuffer;
	if(ChooseFontW(&cf)){
		if(!IsFontsEqual(plfCaption, cf.lpLogFont)){
			memcpy(plfCaption, &lfw, sizeof(lfw));
			if(fDeleteOld && ptH->hFCaption != NULL)
				DeleteFont(ptH->hFCaption);
			ptH->hFCaption = CreateFontIndirectW(cf.lpLogFont);
			ret |= F_C_FONT;
		}
		if(*lpcrCaption != cf.rgbColors){
			ret |= F_C_COLOR;
			*lpcrCaption = (cf.rgbColors == 0) ? DEF_TEXT_COLOR : (cf.rgbColors == CLR_MASK ? CR_IN_MASK : cf.rgbColors);
		}
		RedrawWindow(GetDlgItem(hwnd, id), NULL, NULL, RDW_INVALIDATE);
	}
	return ret;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SendNoteByEmail
 Created  : Tue Jan  8 11:26:26 2008
 Modified : Tue Jan  8 11:26:26 2008

 Synopsys : Sends active note by default e-mail client
 Input    : pNote - memory note handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SendNoteByEmail(PMEMNOTE pNote){
	wchar_t				szMail[1024 * 20], szSubject[256], *body = NULL, *temp = NULL;
	GETTEXTLENGTHEX		gtx;
	TEXTRANGEW			trg;
	long				size;
	HWND				hEdit = NULL;

	if(pNote->pData){
		if(pNote->pData->visible){
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		}
		else{
			hEdit = GetUnvisibleNoteEdit(pNote);
		}

		if(hEdit){
			wcscpy(szSubject, MAIL_SUBJECT);
			wcscpy(szMail, L"mailto:?subject=");
			wcscat(szSubject, pNote->pData->szName);
			wcscat(szSubject, L"'");
			wcscat(szMail, szSubject);
			gtx.flags = GTL_USECRLF;
			gtx.codepage = 1200;
			size = SendMessageW(hEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtx, 0);
			trg.chrg.cpMin = 0;
			trg.chrg.cpMax = size + 1;
			trg.lpstrText = calloc(1, (size + 1) * sizeof(wchar_t));
			SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&trg);
			wcscat(szMail, L"&body=");
			//replace characters
			body = _wcssrpc(trg.lpstrText, L"\n", L"%0D%0A");
			if(body){
				temp = calloc(wcslen(body) + 1, sizeof(wchar_t));
				if(temp){
					wcscpy(temp, body);
					free(body);
					body = _wcssrpc(temp, L"\r", L"%0D%0A");
					free(temp);
					temp = NULL;
				}
			}
			if(body){
				temp = calloc(wcslen(body) + 1, sizeof(wchar_t));
				if(temp){
					wcscpy(temp, body);
					free(body);
					body = _wcssrpc(temp, L"\"", L"%22");
					free(temp);
					temp = NULL;
				}
			}
			if(body){
				temp = calloc(wcslen(body) + 1, sizeof(wchar_t));
				if(temp){
					wcscpy(temp, body);
					free(body);
					body = _wcssrpc(temp, L"&", L"%26");
					free(temp);
					temp = NULL;
				}
			}
			if(body)
				wcscat(szMail, body);
			else
				wcscat(szMail, trg.lpstrText);

			free(trg.lpstrText);
			if(body)
				free(body);

			if(!pNote->pData->visible)
				SendMessageW(hEdit, WM_DESTROY, 0, 0);
			ShellExecuteW(pNote->hwnd, L"open", szMail, NULL, NULL, SW_SHOWDEFAULT);
		}
	}
}

static SIZE GetUnvisibleEditSize(PMEMNOTE pNote){
	HWND				hwnd = NULL, hEdit, hStatic, hGrip, hToolbar, hSysToolbar, *phMarks, *phTemp, hParent = g_hNotesParent;
	int					style = WS_POPUP, exStyle = WS_EX_LAYERED | WS_EX_ACCEPTFILES;
	BITMAP				bm;
	P_NCMD_BAR			pC, pDH;
	SIZE				sz;
	RECT				rc, rcEdit;
	BOOL				useSkin = FALSE;
	NOTE_REL_POSITION	nrp;
	int					left, top, width, height, w, h;

	SetRectEmpty(&rc);
	SetRectEmpty(&rcEdit);
	ZeroMemory(&nrp, sizeof(nrp));

	if(pNote->pRTHandles->hbSkin){
		useSkin = TRUE;
		//get skin dimensions
		GetObject(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		//create note window with skin dimensions
		hwnd = CreateWindowExW(exStyle, NWC_SKINNABLE_CLASS, g_NotePaths.ProgName, style, 0, 0, bm.bmWidth, bm.bmHeight, hParent, NULL, g_hInstance, NULL);
	}
	else{
		//create skinless note
		hwnd = CreateWindowExW(exStyle, NWC_SKINNABLE_CLASS, g_NotePaths.ProgName, style, 0, 0, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy, hParent, NULL, g_hInstance, NULL);
	}

	SetWindowLongPtrW(hwnd, OFFSET_MNOTE, (LONG_PTR)pNote);
	//create "toolbar buttons"
	hToolbar = CreateWindowExW(0, NWC_TOOLBAR_CLASS, NULL, WS_CHILD, 0, 0, 240, 240, hwnd, NULL, g_hInstance, NULL);
	SetPropW(hwnd, PH_COMMANDS, hToolbar);
	pC = calloc(CMD_TBR_BUTTONS, sizeof(NCMD_BAR));
	if(pC){
		SetWindowLongPtrW(hwnd, OFFSET_COMMANDS, (LONG_PTR)pC);
		for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
			pC->hCmd = CreateWindowExW(0, NWC_TBR_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 24, 24, hToolbar, (HMENU)(5000 + i), g_hInstance, NULL);
			pC++;
		}
	}
	//create delete/hide toolbar and buttons
	hSysToolbar = CreateWindowExW(0, NWC_TOOLBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 240, 240, hwnd, NULL, g_hInstance, NULL);
	SetPropW(hwnd, PH_DELHIDE, hSysToolbar);
	pDH = calloc(2, sizeof(NCMD_BAR));
	if(pDH){
		SetWindowLongPtrW(hwnd, OFFSET_DH, (LONG_PTR)pDH);
		for(int i = 0; i < CMD_SYS_BUTTONS; i++){
			pDH->hCmd = CreateWindowExW(0, NWC_TBR_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 24, 24, hSysToolbar, (HMENU)(IDM_HIDE + i), g_hInstance, NULL);
			pDH++;
		}
	}
	//create marks buttons array
	if(!useSkin)
		pNote->pRTHandles->marks = MARKS_COUNT;
	phMarks = calloc(MARKS_COUNT, sizeof(HWND));
	if(phMarks){
		phTemp = phMarks;
		SetWindowLongPtrW(hwnd, OFFSET_SYS, (LONG_PTR)phMarks);
		if(!useSkin)
			w = h = SMALL_RECT;
		else
			w = h = 24;
		for(int i = 0; i < MARKS_COUNT; i++){
			*phTemp = CreateWindowExW(0, NWC_NOTE_MARK_CLASS, NULL, WS_CHILD, 0, 0, w, h, hwnd, NULL, g_hInstance, NULL);
			SetWindowLongPtr(*phTemp, GWLP_USERDATA, (LONG_PTR)i);
			phTemp++;
		}
	}

	if(!useSkin){
		//create grip window for skinless note
		hGrip = CreateWindowExW(0, NWC_GRIP_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, g_hInstance, NULL);
		SetPropW(hwnd, PH_GRIP, hGrip);
		if(g_NoteSettings.showScrollbar)
			hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, g_hInstance, NULL);
		else
			hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, g_hInstance, NULL);
	}
	else{
		//create static window for rich edit transparency simulation
		hStatic = CreateWindowExW(0, L"Static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_REALSIZEIMAGE, 0, 0, 10, 10, hwnd, NULL, g_hInstance, NULL);
		SetPropW(hwnd, PH_STATIC, hStatic);
		if(g_NoteSettings.showScrollbar)
			hEdit = CreateWindowExW(WS_EX_TRANSPARENT, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, g_hInstance, NULL);
		else
			hEdit = CreateWindowExW(WS_EX_TRANSPARENT, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, g_hInstance, NULL);
	}
	// ApplyTabStops(hEdit, FALSE);
	SetPropW(hwnd, PH_EDIT, hEdit);

	//set margins for skinless control
	if(!useSkin){
		short	marginSize = GetSmallValue(g_SmallValues, SMS_MARGIN_SIZE);
		SendMessageW(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(marginSize, marginSize));
	}

	GetPrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), g_NotePaths.DataFile);

	rc = pNote->pData->rcp;
	
	//prepare needed handles
	if(useSkin && (pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN){
		//change window size if it has a skin different from global settings
		GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, FALSE);
		//get skin dimensions
		GetObject(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
	}

	//mesure rectangles for skinless note
	if(!useSkin){
		CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
		CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
		CreateSimpleMarksBitmap(hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
		MeasureSkinlessNoteRectangles(pNote, rc.right - rc.left, rc.bottom - rc.top, pNote->pData->szName);
	}
		
	//prepare edit box rectangle
	CopyRect(&rcEdit, &pNote->pRTHandles->rcEdit);
	
	//position window
	if(useSkin){
		width = bm.bmWidth;
		height = bm.bmHeight;
	}
	else{
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
	}
	if(IsBitOn(g_NoteSettings.reserved1, SB1_RELPOSITION)){
		sz = GetScreenMetrics();
		double		lf = nrp.left * (double)sz.cx;
		double		tp = nrp.top * (double)sz.cy;
		left = floor(lf);// - width;
		top = floor(tp);// - height;
		//prevent leakage of notes
		if(left + width > sz.cx)
			left = sz.cx - width - 1;
		else if(left < 0)
			left = 1;
			if(top + height > sz.cy)
			top = sz.cy - height - 1;
		else if(top < 0)
			top = 1;
	}
	else{
		left = rc.left;
		top = rc.top;
	}
	MoveWindow(hwnd, left, top, width, height, TRUE);

	if(!useSkin){
		sz.cx = (pNote->pRTHandles->rcCaption.right - pNote->pRTHandles->rcCaption.left - 2);
		sz.cy = pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1;
	}
	else{
		//position and size edit box
		sz.cx = rcEdit.right - rcEdit.left;
		sz.cy = rcEdit.bottom - rcEdit.top;
	}
	SendMessageW(hwnd, WM_CLOSE, 0, 0);
	return sz;
}

static int GetNoteCaptionFontHeight(HFONT hFont, wchar_t * lpCaption){
	SIZE				sz = {0};
	HDC					hdcScreen, hdcTemp;
	HFONT				hOldFont;

	//get caption font height
	hdcScreen = GetDC(NULL);
	hdcTemp = CreateCompatibleDC(hdcScreen);
	hOldFont = SelectFont(hdcTemp, hFont);
	GetTextExtentPoint32W(hdcTemp, lpCaption, wcslen(lpCaption), &sz);
	SelectFont(hdcTemp, hOldFont);
	DeleteDC(hdcTemp);
	ReleaseDC(NULL, hdcScreen);
	return sz.cy;
}

void MeasureSkinlessNoteRectangles(PMEMNOTE pNote, int cx, int cy, wchar_t * lpCaption){
	int					width, height, top, bottom, fh;
	BITMAP				bm;
	P_NOTE_RTHANDLES 	pH = pNote->pRTHandles;

	if(DockType(pNote->pData->dockData) == DOCK_NONE || !g_DockSettings.fCustCaption)
		fh = GetNoteCaptionFontHeight(pH->hFCaption, lpCaption);
	else{
		HFONT	hFont = CreateFontIndirectW(&g_DockSettings.lfCaption);
		fh = GetNoteCaptionFontHeight(hFont, lpCaption);
		DeleteFont(hFont);
	}
	//get width of grip rectangle
	width = GetSystemMetrics(SM_CXVSCROLL);
	//get caption height
	height = GetSystemMetrics(SM_CYSMCAPTION);
	if(fh > (height - 2))
		height = fh + 2;
	//set all needed rectangles
	SetRect(&pH->rcGrip, cx - width, cy - width, cx, cy);
	if(pH->hbDelHide){
		SetRect(&pH->rcDH, cx - SMALL_RECT * 2 - 2, 2, cx - 2, SMALL_RECT + 2);
	}
	//cy - SMALL_RECT - 2
	top = cy - width + (width - SMALL_RECT) / 2;
	bottom = top + SMALL_RECT;
	SetRect(&pH->rcCaption, 0, 0, cx, height);
	SetRect(&pH->rcEdit, 1, pH->rcCaption.bottom + 1, cx - 2, pH->rcGrip.top - 1);
	SetRect(&pH->rcTextCaption, 2, 0, pH->rcDH.left, height);
	SetRect(&pH->rcSchedule, 2, top, SMALL_RECT + 2, bottom);
	SetRect(&pH->rcNotSaved, pH->rcSchedule.right + 2, top, pH->rcSchedule.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcProtected, pH->rcNotSaved.right + 2, top, pH->rcNotSaved.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcPriority, pH->rcProtected.right + 2, top, pH->rcProtected.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcCompleted, pH->rcPriority.right + 2, top, pH->rcPriority.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcPassword, pH->rcCompleted.right + 2, top, pH->rcCompleted.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcPin, pH->rcPassword.right + 2, top, pH->rcPassword.right + SMALL_RECT + 2, bottom);
	CopyRect(&pH->rcTooltip, &pH->rcTextCaption);
	if(pH->hbCommand){
		int diff;
		GetObject(pH->hbCommand, sizeof(bm), &bm);
		diff = ((pH->rcGrip.bottom - pH->rcGrip.top) - SMALL_RECT) / 2;
		SetRect(&pH->rcCommands, pH->rcPin.right + 24 + 8, pH->rcGrip.top + diff, pH->rcPin.right + 24 + 8 + bm.bmWidth, pH->rcGrip.top + diff + SMALL_RECT);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetUnvisibleNoteEdit
 Created  : Wed Jun 10 10:14:16 2009
 Modified : Wed Jun 10 10:14:16 2009

 Synopsys : Returns handle to Rich Edit control with loaded RTF content of 
            unvisible note
 Input    : lpID - note ID
 Output   : Handle to Rich edit control with loaded RTF content of 
            unvisible note. It's up to developer to destroy this handle 
            after use.
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
HWND GetUnvisibleNoteEdit(PMEMNOTE pNote){
	HWND	hEdit = NULL;
	SIZE	sz;

	sz = GetUnvisibleEditSize(pNote);
	if(g_NoteSettings.showScrollbar)
		hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, sz.cx, sz.cy, g_hMain, NULL, g_hInstance, NULL);
	else
		hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, sz.cx, sz.cy, g_hMain, NULL, g_hInstance, NULL);

	// hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, WS_CHILD | ES_MULTILINE, 0, 0, 0, 0, g_hMain, NULL, g_hInstance, NULL);
	if(hEdit){
		ReadNoteRTFFile(pNote->pFlags->id, hEdit, NULL);
	}
	return hEdit;
}

void ReadRestoreRTFFile(HWND hEdit, wchar_t * lpPath){
	//load RTF file contents
	InnerReadRTFFile(hEdit, lpPath);
}

void ReadNoteRTFFile(wchar_t * lpID, HWND hEdit, wchar_t * lpCopyFrom){
	wchar_t				szPath[MAX_PATH];

	wcscpy(szPath, g_NotePaths.DataDir);
	wcscat(szPath, lpID);
	wcscat(szPath, NOTE_EXTENTION);
	if(lpCopyFrom){
		CopyFileW(lpCopyFrom, szPath, FALSE);
	}
	//load RTF file contents
	InnerReadRTFFile(hEdit, szPath);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: NotesDirExists
 Created  : Sun Aug 12 15:43:40 2007
 Modified : Sun Aug 12 15:43:40 2007

 Synopsys : Checks whether one of program global paths exists
 Input    : lpDir - global path
 Output   : TRUE if succeeded, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL NotesDirExists(wchar_t * lpDir){
	wchar_t				filespec[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	wcscpy(filespec, lpDir);
	//remove trailing backslash
	filespec[wcslen(filespec) - 1] = '\0';
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return FALSE;
	else{
		FindClose(handle);
		return TRUE;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: NotesDirCreate
 Created  : Sun Aug 12 15:42:23 2007
 Modified : Sun Aug 12 15:42:23 2007

 Synopsys : Creates directory, specified in one of global paths
 Input    : lpDir - global path
 Output   : TRUE if succeeded, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL NotesDirCreate(wchar_t * lpDir){
	wchar_t				filespec[MAX_PATH];

	wcscpy(filespec, lpDir);
	//remove trailing backslash
	filespec[wcslen(filespec) - 1] = '\0';
	return CreateDirectoryW(filespec, NULL);
}

UINT APIENTRY CCHookProc(HWND hdlg,	UINT uiMsg,	WPARAM wParam, LPARAM lParam){
	if(uiMsg == WM_INITDIALOG){
		CHOOSECOLOR *lcc = (CHOOSECOLOR *)lParam;
		SetWindowTextW(hdlg, (LPCWSTR)lcc->lCustData);
	}
	return FALSE;
}

UINT APIENTRY CFHookProc(HWND hdlg,	UINT uiMsg,	WPARAM wParam, LPARAM lParam){
	if(uiMsg == WM_INITDIALOG){
		CHOOSEFONT *lcc = (CHOOSEFONT *)lParam;
		SetWindowTextW(hdlg, (LPCWSTR)lcc->lCustData);
	}
	return FALSE;
}

void CreateColorMenu(HMENU hMenu, int * idMenu, const int idStart, wchar_t * lpCaption){

	int 		count = GetMenuItemCount(hMenu);
	int			pos = 0;
	MITEM		mit;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;
	FreeMenus(hMenu);
	for(int i = count - 1; i >= 0; i--){
		DeleteMenu(hMenu, i, MF_BYPOSITION);
	}
	mit.id = idStart;
	if(*lpCaption != '\0'){
		wcscpy(mit.szText, lpCaption);
		AppendMenuW(hMenu, MF_STRING, mit.id++, lpCaption);
		SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	}
	mit.type = MT_COLORITEM;
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"0");
	wcscpy(mit.szText, L"0");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8388608");
	wcscpy(mit.szText, L"8388608");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"32768");
	wcscpy(mit.szText, L"32768");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8421376");
	wcscpy(mit.szText, L"8421376");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"128");
	wcscpy(mit.szText, L"128");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8388736");
	wcscpy(mit.szText, L"8388736");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"32896");
	wcscpy(mit.szText, L"32896");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"12632256");
	wcscpy(mit.szText, L"12632256");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8421504");
	wcscpy(mit.szText, L"8421504");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16711680");
	wcscpy(mit.szText, L"16711680");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"65280");
	wcscpy(mit.szText, L"65280");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16776960");
	wcscpy(mit.szText, L"16776960");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"255");
	wcscpy(mit.szText, L"255");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16711935");
	wcscpy(mit.szText, L"16711935");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"65535");
	wcscpy(mit.szText, L"65535");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16777215");
	wcscpy(mit.szText, L"16777215");
	SetMenuItemProperties(&mit, hMenu, pos++, TRUE);
	*idMenu = --mit.id;
}

int GetMaxSizeMenuID(void){
	return m_IDSizeMenu;
}

void RemoveSizeMenu(void){
	if(g_hSizePopUp){
		DestroyMenu(g_hSizePopUp);
		g_hSizePopUp = NULL;
	}
}

void CreateSizeMenu(HMENU * pMenu, BOOL fCreate){
	wchar_t		szSize[8];
	int			size, type;

	type = MF_STRING;
	if(fCreate){
		*pMenu = CreatePopupMenu();
	}
	else{
		int count = GetMenuItemCount(*pMenu);
		for(int i = 0; i < count; i++)
			RemoveMenu(*pMenu, i, MF_BYPOSITION);
		// type = MF_OWNERDRAW;
	}
	size = 8;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 9;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 10;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 11;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 12;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 14;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 16;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 18;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 20;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 22;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 24;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 26;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 28;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 36;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 48;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 72;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
}

void SetCFSize(int size){
	m_FSize = size;
}

int GetCFSize(void){
	return m_FSize;
}

void ConstructSaveMessage(wchar_t * lpNoteName, wchar_t * lpBuffer){

	wchar_t		szName[128];

	wcscpy(szName, L"\n<");
	wcscat(szName, lpNoteName);
	wcscat(szName, L">");
	wcscpy(lpBuffer, g_Strings.SNote);
	wcscat(lpBuffer, szName);
	wcscat(lpBuffer, L"\n");
	wcscat(lpBuffer, g_Strings.SQuestion);
}

void LoadSearchHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	for(int i = 0; i < MAX_SEARCH_COUNT; i++){
		_itow(i, szKey, 10);
		if(GetPrivateProfileStringW(S_SEARCH_HIST, szKey, NULL, szBuffer, 256, g_NotePaths.INIFile)){
			if(wcslen(szBuffer)){
				SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_ADDSTRING, 0, (LPARAM)szBuffer);
			}
		}
	}
}

void LoadReplaceHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	for(int i = 0; i < MAX_SEARCH_COUNT; i++){
		_itow(i, szKey, 10);
		if(GetPrivateProfileStringW(S_REPLACE_HIST, szKey, NULL, szBuffer, 256, g_NotePaths.INIFile)){
			if(wcslen(szBuffer)){
				SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
			}
		}
	}
}

int SharedReplaceProc(HWND hEdit){
	TEXTRANGEW		tr;
	int				result;
	
	result = SharedFindProc(hEdit);
	if(result != -1){
		SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&tr.chrg);
		SendMessageW(hEdit, EM_REPLACESEL, TRUE, (LPARAM)g_ReplaceString);
	}

	return result;
}

int SharedFindProc(HWND hEdit){
	TEXTRANGEW		tr;
	FINDTEXTEXW		ft;
	int				result;

	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&tr.chrg);

	if((g_Flags & FR_DOWN) == FR_DOWN){
		ft.chrg.cpMin = tr.chrg.cpMax;
		ft.chrg.cpMax = -1;
	}
	else{
		ft.chrg.cpMin = tr.chrg.cpMin;
		ft.chrg.cpMax = -1;
	}
	ft.lpstrText = g_SearchString;
	result = SendMessageW(hEdit, EM_FINDTEXTEXW, g_Flags, (LPARAM)&ft);
	if(result != -1){
		SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
	}
	return result;
}

void SaveSearchHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETCOUNT, 0, 0) > 0){
		_itow(MAX_SEARCH_COUNT, szBuffer, 10);
		WritePrivateProfileStringW(S_SEARCH_HIST, L"count", szBuffer, g_NotePaths.INIFile);
		for(int i = 0; i < MAX_SEARCH_COUNT; i++){
			*szBuffer = '\0';
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETLBTEXT, i, (LPARAM)szBuffer) != CB_ERR){
				if(wcslen(szBuffer)){
					_itow(i, szKey, 10);
					WritePrivateProfileStringW(S_SEARCH_HIST, szKey, szBuffer, g_NotePaths.INIFile);
				}
			}
		}
	}
}

void SaveReplaceHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETCOUNT, 0, 0) > 0){
		_itow(MAX_SEARCH_COUNT, szBuffer, 10);
		WritePrivateProfileStringW(S_REPLACE_HIST, L"count", szBuffer, g_NotePaths.INIFile);
		for(int i = 0; i < MAX_SEARCH_COUNT; i++){
			*szBuffer = '\0';
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETLBTEXT, i, (LPARAM)szBuffer) != CB_ERR){
				if(wcslen(szBuffer)){
					_itow(i, szKey, 10);
					WritePrivateProfileStringW(S_REPLACE_HIST, szKey, szBuffer, g_NotePaths.INIFile);
				}
			}
		}
	}
}

int GetMenuPosition(HMENU hMenu, int id){
	int				count = GetMenuItemCount(hMenu);
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID;
	for(int i = 0; i < count; i++){
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if(mi.wID == id){
			return i;
		}
	}
	return -1;
}

void PrepareDockMenu(PMEMNOTE pNote, HMENU hMenu, int index){
	HMENU				hDock;
	int					count;

	hDock = GetSubMenu(hMenu, index);
	count = GetMenuItemCount(hDock);
	for(int i = 0; i < count; i++){
		EnableMenuItem(hDock, i, MF_BYPOSITION | MF_ENABLED);
	}
	if(pNote){
		switch(DockType(pNote->pData->dockData)){
		case DOCK_NONE:
			EnableMenuItem(hDock, IDM_DOCK_NONE, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_LEFT:
			EnableMenuItem(hDock, IDM_DOCK_LEFT, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_TOP:
			EnableMenuItem(hDock, IDM_DOCK_TOP, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_RIGHT:
			EnableMenuItem(hDock, IDM_DOCK_RIGHT, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_BOTTOM:
			EnableMenuItem(hDock, IDM_DOCK_BOTTOM, MF_BYCOMMAND | MF_GRAYED);
			break;
		}
	}
}

void ApplyDockColors(PMEMNOTE pNote){
	NOTE_APPEARANCE		nap;

	if(g_DockSettings.fCustColor)
		RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, g_DockSettings.crWindow);
	else
		RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
	if(g_DockSettings.fCustCaption)
		nap.crCaption = g_DockSettings.crCaption;
	else
		nap.crCaption = pNote->pAppearance->crCaption;
	if(g_DockSettings.fCustColor)
		nap.crWindow = g_DockSettings.crWindow;
	else
		nap.crWindow = pNote->pAppearance->crWindow;
	CreateSkinlessToolbarBitmap(pNote->hwnd, &nap, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
	CreateSkinlessSysBitmap(pNote->hwnd, &nap, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
}


/*-@@+@@------------------------------------------------------------------
 Procedure: GetHotKeyByID
 Created  : Sun Aug 12 16:47:38 2007
 Modified : Sun Aug 12 16:47:38 2007

 Synopsys : Returns pointer to hot key for specified array by id
 Input    : id - hot key id
            phk - pointer to hot keys array
 Output   : Pointer to the hot key if found, otherwise - NULL
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
P_HK_TYPE GetHotKeyByID(int id, P_HK_TYPE phk, int count){
	for(int i = 0; i < count; i++){
		if(phk->id == id)
			return phk;
		phk++;
	}
	return NULL;
}

BYTE GetSmallValue(SMALLVALUES sv, int position){
	switch(position){
		case SMS_BACKUP_COUNT:
			return sv.value1;
		case SMS_TAB_WIDTH:
			return sv.value2;
		case SMS_BULLET_INDENT:
			return sv.value3;
		case SMS_MARGIN_SIZE:
			return sv.value4;
		case SMS_DIARY_PAGES:
			return sv.value5;
		case SMS_DEF_SAVE_CHARACTERS:
			return sv.value6;
		case SMS_CONTENT_LENGTH:
			return sv.value7;
		case SMS_DEF_NAMING:
			return sv.value8;
		default:
			return 0;
	}
}

void SetSmallValue(PSMALLVALUES psv, int position, BYTE value){
	switch(position){
		case SMS_BACKUP_COUNT:
			psv->value1 = value;
			break;
		case SMS_TAB_WIDTH:
			psv->value2 = value;
			break;
		case SMS_BULLET_INDENT:
			psv->value3 = value;
			break;
		case SMS_MARGIN_SIZE:
			psv->value4 = value;
			break;
		case SMS_DIARY_PAGES:
			psv->value5 = value;
			break;
		case SMS_DEF_SAVE_CHARACTERS:
			psv->value6 = value;
			break;
		case SMS_CONTENT_LENGTH:
			psv->value7 = value;
			break;
		case SMS_DEF_NAMING:
			psv->value8 = value;
			break;
		default:
			break;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PreviewSound
 Created  : Sun Aug 12 15:37:56 2007
 Modified : Sun Aug 12 15:37:56 2007

 Synopsys : Plays selected sound
 Input    : lpSound - sound file short name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void PreviewSound(wchar_t * lpSound){
	wchar_t			szPath[MAX_PATH];

	if(wcscmp(lpSound, DS_DEF_SOUND) == 0)
		PlaySoundW(L"Notify", 0, SND_ALIAS | SND_ASYNC);
	else{
		wcscpy(szPath, g_NotePaths.SoundDir);
		wcscat(szPath, lpSound);
		PlaySoundW(szPath, 0, SND_FILENAME | SND_ASYNC);
	}
}

void PrepareSingleMenuWithAccelerator(PMITEM pMenus, int size, int id, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection, P_HK_TYPE lpKeys, int keysCount){
	MENUITEMINFOW	mi = {0};
	wchar_t			szText[128], szDefault[128];
	PMITEM			pmi;
	P_HK_TYPE		pHK;
	wchar_t			szBuffer[128];

	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE;
	mi.wID = id;
	GetMenuItemInfoW(hMenu, id, FALSE, &mi);
	GetMIText(pMenus, size, mi.wID, szDefault);
	pHK = GetHotKeyByID(mi.wID, lpKeys, keysCount);
	if(pHK){
		if(pHK->fsModifiers || pHK->vk)
			SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, pHK->szKey);
		else{
			*szBuffer = '\0';
			GetMIReserved(pMenus, size, mi.wID, szBuffer);
			if(*szBuffer)
				SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
			else
				SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
		}
	}		
	else{
		*szBuffer = '\0';
		GetMIReserved(pMenus, size, mi.wID, szBuffer);
		if(*szBuffer)
			SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
		else
			SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
	}
	SetMIText(pMenus, size, mi.wID, szText);
	pmi = GetMItem(pMenus, size, mi.wID);
	if(pmi){
		SetMenuItemProperties(pmi, hMenu, GetMenuPosition(hMenu, id), TRUE);
	}
	if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
		mi.fType |= MFT_OWNERDRAW;
		SetMenuItemInfoW(hMenu, id, FALSE, &mi);
	}
}

void PrepareMenuRecursiveWithAccelerators(PMITEM pMenus, int size, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection, P_HK_TYPE lpKeys, int keysCount){
	int				count;
	MENUITEMINFOW	mi;
	wchar_t			szText[128], szDefault[128];
	PMITEM			pmi;
	P_HK_TYPE		pHK;
	wchar_t			szBuffer[128];

	count = GetMenuItemCount(hMenu);
	for(int i = 0; i < count; i++){
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE;
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if((mi.fType & MFT_SEPARATOR) != MFT_SEPARATOR){
			if(mi.hSubMenu){
				PrepareMenuRecursiveWithAccelerators(pMenus, size, mi.hSubMenu, lpLangFile, lpSection, lpKeys, keysCount);
			}
			GetMIText(pMenus, size, mi.wID, szDefault);
			pHK = GetHotKeyByID(mi.wID, lpKeys, keysCount);
			if(pHK){
				if(pHK->fsModifiers || pHK->vk)
					SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, pHK->szKey);
				else{
					*szBuffer = '\0';
					GetMIReserved(pMenus, size, mi.wID, szBuffer);
					if(*szBuffer)
						SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
					else
						SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
				}
			}		
			else{
				*szBuffer = '\0';
				GetMIReserved(pMenus, size, mi.wID, szBuffer);
				if(*szBuffer)
					SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
				else
					SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
			}

			if(mi.wID == IDM_SYNC_NOW){
				if(wcsstr(szText, L"FTP") == NULL)
					wcscat(szText, L" (FTP)");
			}

			SetMIText(pMenus, size, mi.wID, szText);
			pmi = GetMItem(pMenus, size, mi.wID);
			if(pmi){
				SetMenuItemProperties(pmi, hMenu, i, TRUE);
			}
		}
		if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
			mi.fType |= MFT_OWNERDRAW;
			SetMenuItemInfoW(hMenu, i, TRUE, &mi);
		}
	}
}

void SetMenuTextByAnotherID(PMITEM pMenus, int size, HMENU hMenu, int idMenu, int idFrom, wchar_t * lpLangFile, wchar_t * lpSection){
	wchar_t			szText[128], szDefault[128], szBuffer[128];
	PMITEM			pmi;

	GetMIText(pMenus, size, idMenu, szDefault);
	*szBuffer = '\0';
	GetMIReserved(pMenus, size, idMenu, szBuffer);
	if(*szBuffer)
		SetMenuTextWithAccelerator(idFrom, lpSection, lpLangFile, szDefault, szText, szBuffer);
	else
		SetMenuText(idFrom, lpSection, lpLangFile, szDefault, szText);
	SetMIText(pMenus, size, idMenu, szText);
	pmi = GetMItem(pMenus, size, idMenu);
	if(pmi){
			SetMenuItemProperties(pmi, hMenu, idMenu, FALSE);
	}
}

void PrepareMenuRecursive(PMITEM pMenus, int size, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection){
	int				count;
	MENUITEMINFOW	mi;
	wchar_t			szText[128], szDefault[128], szBuffer[128];
	PMITEM			pmi;

	count = GetMenuItemCount(hMenu);
	for(int i = 0; i < count; i++){
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE;
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if((mi.fType & MFT_SEPARATOR) != MFT_SEPARATOR){
			if(mi.hSubMenu){
				PrepareMenuRecursive(pMenus, size, mi.hSubMenu, lpLangFile, lpSection);
			}
			GetMIText(pMenus, size, mi.wID, szDefault);
			*szBuffer = '\0';
			//do not get reserved for bullets menu
			switch(mi.wID){
				case IDM_SIMPLE_BULLETS:
				case IDM_NUMBERS_PERIOD:
				case IDM_NUMBERS_PARENS:
				case IDM_LC_PERIOD:
				case IDM_LC_PARENS:
				case IDM_UC_PERIOD:
				case IDM_UC_PARENS:
				case IDM_ROMAN_LC:
				case IDM_ROMAN_UC:
					break;
				default:
					GetMIReserved(pMenus, size, mi.wID, szBuffer);
					break;
			}
			if(*szBuffer)
				SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
			else
				SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);

			if(mi.wID == IDM_SYNC_NOW){
				if(wcsstr(szText, L"FTP") == NULL)
					wcscat(szText, L" (FTP)");
			}

			SetMIText(pMenus, size, mi.wID, szText);
			pmi = GetMItem(pMenus, size, mi.wID);
			if(pmi){
				switch(pmi->id){
					case IDM_SIMPLE_BULLETS:
						_ltow(MAKELONG(0, PFN_BULLET), pmi->szReserved, 10);
						break;
					case IDM_NUMBERS_PERIOD:
						_ltow(MAKELONG(PFNS_PERIOD, PFN_ARABIC), pmi->szReserved, 10);
						break;
					case IDM_NUMBERS_PARENS:
						_ltow(MAKELONG(PFNS_PAREN, PFN_ARABIC), pmi->szReserved, 10);
						break;
					case IDM_LC_PERIOD:
						_ltow(MAKELONG(PFNS_PERIOD, PFN_LCLETTER), pmi->szReserved, 10);
						break;
					case IDM_LC_PARENS:
						_ltow(MAKELONG(PFNS_PAREN, PFN_LCLETTER), pmi->szReserved, 10);
						break;
					case IDM_UC_PERIOD:
						_ltow(MAKELONG(PFNS_PERIOD, PFN_UCLETTER), pmi->szReserved, 10);
						break;
					case IDM_UC_PARENS:
						_ltow(MAKELONG(PFNS_PAREN, PFN_UCLETTER), pmi->szReserved, 10);
						break;
					case IDM_ROMAN_LC:
						_ltow(MAKELONG(PFNS_PERIOD, PFN_LCROMAN), pmi->szReserved, 10);
						break;
					case IDM_ROMAN_UC:
						_ltow(MAKELONG(PFNS_PERIOD, PFN_UCROMAN), pmi->szReserved, 10);
						break;
				}
				SetMenuItemProperties(pmi, hMenu, i, TRUE);
			}
		}
		if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
			mi.fType |= MFT_OWNERDRAW;
			SetMenuItemInfoW(hMenu, i, TRUE, &mi);
		}
	}
}

PMITEM GetMItem(PMITEM	lpMI, const int size, int id){
	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			return lpMI;
		}
		lpMI++;
	}
	return NULL;
}

void GetMIReserved(PMITEM lpMI, const int size, int id, wchar_t * lpRes){
	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			wcscpy(lpRes, lpMI->szReserved);
			break;
		}
		lpMI++;
	}
}

void GetMIText(PMITEM lpMI, const int size, int id, wchar_t * lpText){

	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			wcscpy(lpText, lpMI->szText);
			break;
		}
		lpMI++;
	}
}

int ItemImageIndex(PMEMNOTE pNote){
	if(pNote->pData->visible){
		if(pNote->pFlags->fromDB){
			//loaded note
			if(pNote->pFlags->saved){
				//unchanged or saved note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 0;
				}
				else{
					//scheduled note
					return 2;
				}
			}
			else{
				//changed note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 1;
				}
				else{
					//scheduled note
					return 3;
				}
			}
		}
		else{
			//new note
			if(pNote->pFlags->saved){
				//unchanged or saved note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 4;
				}
				else{
					//scheduled note
					return 6;
				}
			}
			else{
				//changed note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 5;
				}
				else{
					//scheduled note
					return 7;
				}
			}
		}
	}
	else{
		//hidden note
		if(pNote->pData->idGroup == GROUP_RECYCLE){
			//deleted note
			return 16;
		}
		else{
			if(pNote->pFlags->fromDB){
				//loaded note
				if(pNote->pFlags->saved){
					//unchanged or saved note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 8;
					}
					else{
						//scheduled note
						return 10;
					}
				}
				else{
					//changed note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 9;
					}
					else{
						//scheduled note
						return 11;
					}
				}
			}
			else{
				//new note
				if(pNote->pFlags->saved){
					//unchanged or saved note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 12;
					}
					else{
						//scheduled note
						return 14;
					}
				}
				else{
					//changed note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 13;
					}
					else{
						//scheduled note
						return 15;
					}
				}
			}
		}
	}
}

int DaysInMonth(SYSTEMTIME st){
	switch(st.wMonth){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
		default:		//februar
			if(IsLeapYear(st.wYear))
				return 29;
			else
				return 28;
	}
}

ordinal_days DayOfWeekOrdinal(SYSTEMTIME st){
	if(st.wDay <= 7)
		return first;
	else if(st.wDay <= 14)
		return second;
	else if(st.wDay <= 21)
		return third;
	else if(st.wDay <= 28){
		if((DaysInMonth(st) - st.wDay) >= 7)
			return fourth;
		else
			return last;
	}
	else
		return last;

}

void AddVoicesToControl(HWND hControl, UINT uAddMsg, UINT uSetDataMsg, UINT uSetCurrSelMsg, wchar_t * lpDefVoice){
	int				index = 0;
	BOOL			fFound = false;

	for(int i = 0; i < g_VoicesCount; i++){
		index = SendMessageW(hControl, uAddMsg, 0, (LPARAM)g_PVoices[i].name);
		if(index != LB_ERR){
			SendMessageW(hControl, uSetDataMsg, index, (LPARAM)g_PVoices[i].token);
			if(wcscmp(g_PVoices[i].name, lpDefVoice) == 0){
				SendMessageW(hControl, uSetCurrSelMsg, index, 0);
				fFound = true;
			}
		}
	}
	if(!fFound){
		SendMessageW(hControl, uSetCurrSelMsg, 0, 0);
	}
}

int GetHotkeyID(P_HK_TYPE hotKeys, int count, int mod, int key){
	for(int i = 0; i < count; i++){
		if(hotKeys[i].fsModifiers == mod && hotKeys[i].vk == key){
			return hotKeys[i].id;
		}
	}
	return -1;
}

void SaveGroup(LPPNGROUP ppg){
	wchar_t		szKey[24];

	_itow(ppg->id, szKey, 10);
	WritePrivateProfileStructW(S_GROUPS, szKey, ppg, sizeof(PNGROUP), g_NotePaths.INIFile);
}

void GetNoteTempFileName(wchar_t * lpPath){
	wchar_t			szTempPath[MAX_PATH - 14];

	GetTempPathW(MAX_PATH - 14, szTempPath);
	GetTempFileNameW(szTempPath, L"pnf", 1, lpPath);
}

BOOL IsTextInClipboard(void){
	if(!IsClipboardFormatAvailable(CF_TEXT))
		if(!IsClipboardFormatAvailable(CF_UNICODETEXT))
			if(!IsClipboardFormatAvailable(CF_OEMTEXT))
				return FALSE;
	return TRUE;
}

DWORD InStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb){
	ReadFile((HANDLE)dwCookie, pbBuff, cb, (PULONG)pcb, NULL);
	return 0;
}

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam){
	if(IS_INTRESOURCE(lpszName)){
		HBITMAP		hBmp = LoadBitmap(hModule, lpszName);
		ImageList_AddMasked((HIMAGELIST)lParam, hBmp, RGB(255, 0, 255));
		DeleteBitmap(hBmp);
		DeleteObject((void *)RGB(255, 0, 255));
	}
	return TRUE;
}

BOOL IsVisualStyleEnabled(void){
	HANDLE				hLib;
	FARPROC				hProc1, hProc2;
	BOOL				result;

	if(g_WinVer < WV_XP || !IsDLLVersionForStyle())
		return FALSE;
	hLib = LoadLibrary("UxTheme.dll");
	if(!hLib)
		return FALSE;
	hProc1 = GetProcAddress(hLib, "IsThemeActive");
	hProc2 = GetProcAddress(hLib, "IsAppThemed");
	if(hProc1){
		__asm{
			call hProc1
			mov result, eax
		}
	}
	if(hProc2){
		__asm{
			call hProc2
			or result, eax
		}
	}
	FreeLibrary(hLib);
	return result;
}

BOOL IsDLLVersionForStyle(void){
	DLLVERSIONINFO		dvi;
	HANDLE				hLib;
	FARPROC				hProc;
	BOOL				bReturn = FALSE;

	dvi.cbSize = sizeof(dvi);
	hLib = LoadLibrary("comctl32.dll");
	if(hLib){
		hProc = GetProcAddress(hLib, "DllGetVersion");
		if(hProc){
			__asm{
				lea eax, dvi
				push eax
				call hProc
			}
			if(dvi.dwMajorVersion > 5)
				bReturn = TRUE;
			else
				bReturn = FALSE;
		}
		else{
			bReturn = FALSE;
		}
		FreeLibrary(hLib);
	}
	return bReturn;
}

void RemoveDoubleSpace(wchar_t * s){
	wchar_t		*p, *t;
	
	while(TRUE){
		p = _wcsistr(s, L"  ");
		if(!p)
			break;
		t = p;
		t++;
		while(*t){
			*p++ = *t++;
		}
		*p = '\0';
	}
}

void RemoveDoubleComma(wchar_t * s){
	wchar_t		*p, *t;
	
	while(TRUE){
		p = _wcsistr(s, L",,");
		if(!p)
			break;
		t = p;
		t++;
		while(*t){
			*p++ = *t++;
		}
		*p = '\0';
	}
}

void _wcsltrm(wchar_t * s){
	wchar_t		*p = s, *t = s;

	if(!p)
		return;
	while(*p == ' ' || *p == '\t')
		p++;
	do{
		*t++ = *p++;
	}while(*p);
	*t = '\0';
}

void _wcsrtrm(wchar_t * s){
	wchar_t		*p = s;

	if(!*p)
		return;
	while(*p)
		p++;
	p--;
	while(*p == ' ' || *p == '\t')
		*p-- = '\0';
}

void _wcstrm(wchar_t * s){
	_wcsltrm(s);
	_wcsrtrm(s);
}

void _wcsrpc(wchar_t * src, wchar_t c1, wchar_t c2){
	wchar_t		*p = src;

	while(*p){
		if(*p == c1)
			*p = c2;
		p++;
	}
}

wchar_t * _wcssrpc(wchar_t * src, const wchar_t * s1, const wchar_t * s2){
	wchar_t 	*ret, *temp = src;
	int			i, count = 0;
	size_t		lenOld = wcslen(s1);
	size_t		lenNew = wcslen(s2);

	for(i = 0; src[i]; i++){
		if(wcsstr(&src[i], s1) == &src[i]){
			count++;
			i += lenOld - 1;
		}
	}

	ret = calloc(i + 1 + count * (lenNew - lenOld), sizeof(wchar_t));

	if(!ret){
		return NULL;
	}

	i = 0;
	while (*temp) {
		if (wcsstr(temp, s1) == temp) {
			wcscpy(&ret[i], s2);
			i += lenNew;
			temp += lenOld;
		} 
		else{
			ret[i++] = *temp++;
		}
	}
	// ret[i] = '\0';

	return ret;
}

int __cdecl SengsCompareAsc(const void *p1, const void *p2){
	LPPSENG					ps1, ps2;

	ps1 = (LPPSENG)p1;
	ps2 = (LPPSENG)p2;

	return _wcsicmp(ps1->name, ps2->name);
}

int __cdecl GroupMenusCompare(const void *p1, const void *p2){
	P_GROUP_MENU_STRUCT		pg1, pg2;

	pg1 = (P_GROUP_MENU_STRUCT)p1;
	pg2 = (P_GROUP_MENU_STRUCT)p2;

	return _wcsicmp(pg1->pName, pg2->pName);
}

int __cdecl FavMenusCompare(const void *p1, const void *p2){
	P_FAV_MENU_STRUCT		pg1, pg2;

	pg1 = (P_FAV_MENU_STRUCT)p1;
	pg2 = (P_FAV_MENU_STRUCT)p2;

	return _wcsicmp(pg1->pName, pg2->pName);
}

int __cdecl ContGroupsMenusCompare(const void *p1, const void *p2){
	P_CONT_GROUP_STRUCT		pg1, pg2;

	pg1 = (P_CONT_GROUP_STRUCT)p1;
	pg2 = (P_CONT_GROUP_STRUCT)p2;

	return _wcsicmp(pg1->name, pg2->name);
}

int __cdecl DiaryMenusCompareAsc(const void *pp1, const void *pp2){
	P_DIARY_MENU_STRUCT		p1, p2;
	FILETIME				f1, f2;

	p1 = (P_DIARY_MENU_STRUCT)pp1;
	p2 = (P_DIARY_MENU_STRUCT)pp2;
	SystemTimeToFileTime(&p1->tCreated, &f1);
	SystemTimeToFileTime(&p2->tCreated, &f2);
	return CompareFileTime(&f1, &f2);
}

int __cdecl DiaryMenusCompareDesc(const void *pp1, const void *pp2){
	P_DIARY_MENU_STRUCT		p1, p2;
	FILETIME				f1, f2;

	p1 = (P_DIARY_MENU_STRUCT)pp1;
	p2 = (P_DIARY_MENU_STRUCT)pp2;
	SystemTimeToFileTime(&p1->tCreated, &f1);
	SystemTimeToFileTime(&p2->tCreated, &f2);
	return (CompareFileTime(&f1, &f2) * -1);
}
