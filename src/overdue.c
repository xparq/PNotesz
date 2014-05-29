// Copyright (C) 2009 Andrey Gruber (aka lamer)

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

#include <olectl.h>

#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "note.h"
#include "splitcontainer.h"
#include "shared.h"
#include "enums.h"

static void Overdue_OnClose(HWND hwnd);
static BOOL Overdue_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Overdue_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Overdue_OnSize(HWND hwnd, UINT state, int cx, int cy);
static void PrepareOverdueList(void);
static void ApplyOverdueLanguage(void);
static void FillOverdueList(PMEMNOTE * parr);
static void SetOverdueScheduleDescription(PMEMNOTE pNote, int index);
static void UpdateOverdueItem(PMEMNOTE pNote);
static int CALLBACK OverdueStringCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static PMEMNOTE SelectedNote(void);
static void SetPreviewBackgroundColor(BOOL clear, COLORREF color);
static void ShowNotePreview(int selCount);

static HWND					m_hSplitter, m_hList, m_hEdit;
static HIMAGELIST			m_hImlSmall;
static int					m_SortColumn, m_SortType, m_SortTypeName, m_SortTypeSchedule;
static int					m_PvwColor;
static BOOL					m_PvwUseColor;

BOOL CALLBACK Overdue_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR			lpnm;
	NMLISTVIEW		* nml;
	LVITEMW			lvi;
	PMEMNOTE		pTemp;
	
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_CLOSE, Overdue_OnClose);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Overdue_OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND, Overdue_OnCommand);
		HANDLE_MSG (hwnd, WM_SIZE, Overdue_OnSize);
		case WM_NOTIFY:
	    	lpnm = (LPNMHDR)lParam;
			switch(lpnm->code){
				case LVN_ITEMCHANGED:
					//get selected item and note associated with it
					nml = (NMLISTVIEW *)lParam;
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.iItem = nml->iItem;
					lvi.mask = LVIF_PARAM;
					SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
					pTemp = (PMEMNOTE)lvi.lParam;
					if(nml->uNewState == 3 || nml->uNewState == 2){
						ShowNotePreview(1);
					}
					else{
						//if no item is selected - clear edit box
						SetPreviewBackgroundColor(TRUE, 0);
						SetWindowTextW(m_hEdit, NULL);
					}
					break;
				case LVN_COLUMNCLICK:
					//sort list view by columns
					nml = (NMLISTVIEW *)lParam;
					m_SortColumn = nml->iSubItem;
					switch(m_SortColumn){
						case 0:
							if(m_SortTypeName == LVS_SORTASCENDING)
								m_SortTypeName = LVS_SORTDESCENDING;
							else
								m_SortTypeName = LVS_SORTASCENDING;
							m_SortType = m_SortTypeName;
							break;
						case 1:
							if(m_SortTypeSchedule == LVS_SORTASCENDING)
								m_SortTypeSchedule = LVS_SORTDESCENDING;
							else
								m_SortTypeSchedule = LVS_SORTASCENDING;
							m_SortType = m_SortTypeSchedule;
							break;
					}
					SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_SortType, (LPARAM)OverdueStringCompareFunc);
					break;
				case NM_DBLCLK:
					//show adjust dialog (schedule tab) on double click on item
					if(lpnm->idFrom == IDC_LST_OVERDUE){
						pTemp = SelectedNote();
						if(pTemp){
							ADJUST_STRUCT		as;

							as.pNote = pTemp;
							as.type = 1;
							DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_ADJUST), hwnd, Adjust_DlgProc, (LPARAM)&as);
							g_hAdjust = NULL;
							ShowNotePreview(ListView_GetSelectedCount(m_hList));
						}
					}
					break;
			}
			return TRUE;
		case PNM_CTRL_UPD_LANG:
			//change UI language
			ApplyOverdueLanguage();
			return TRUE;
		case PNM_CTRL_ITEM_UPDATE:
			//update note properties
			UpdateOverdueItem((PMEMNOTE)lParam);
			return TRUE;
		default: return FALSE;
	}
}

static BOOL Overdue_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT			rc;
	int				cH;

	//save dialog handle
	g_hOverdueDialog = hwnd;
	g_hLastModal = hwnd;
	//save list view handle
	m_hList = GetDlgItem(hwnd, IDC_LST_OVERDUE);
	//allow list view full row select
	SendMessageW(m_hList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//get needed measures
	GetWindowRect(GetDlgItem(hwnd, IDOK), &rc);
	cH = rc.bottom - rc.top + 16;
	GetClientRect(hwnd, &rc);
	//create edit box
	m_hEdit = CreateWindowExW(WS_EX_STATICEDGE, RICHEDIT_CLASSW, NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_NOHIDESEL | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_EDT_OVERDUE, g_hInstance, NULL);
	//create horizontal splitter
	m_hSplitter = CreateMainPanel(SPLIT_HORZ, g_hInstance, hwnd, g_CurH, 36, 0, 0, 0, cH, -1);
	//add list view as top child of splitter
	SendMessageW(m_hSplitter, SPM_ADD_CHILD, (WPARAM)m_hList, CHILD_TOP);
	//add edit box as bottom child of splitter
	SendMessageW(m_hSplitter, SPM_ADD_CHILD, (WPARAM)m_hEdit, CHILD_BOTTOM);
	//set up list view
	PrepareOverdueList();
	//fill list view with overdue notes
	FillOverdueList((PMEMNOTE *)lParam);
	//apply program UI language
	ApplyOverdueLanguage();
	//get preview color settings
	m_PvwUseColor = (BOOL)GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_USE_COLOR, 0, g_NotePaths.INIFile);
	m_PvwColor = GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_COLOR, -1, g_NotePaths.INIFile);
	SetPreviewBackgroundColor(TRUE, 0);

	return FALSE;
}

static void Overdue_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	//resize splitter
	SendMessageW(m_hSplitter, SPM_MAIN_RESIZED, (WPARAM)hwnd, MAKELPARAM(cx, cy));
}

static void Overdue_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
		case IDOK:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
	}
}

static void Overdue_OnClose(HWND hwnd)
{
	if(m_hImlSmall){
		ImageList_Destroy(m_hImlSmall);
		m_hImlSmall = NULL;
	}
	g_hOverdueDialog = NULL;
	if(g_hLastModal == hwnd)
		EndDialog(hwnd, 0);
	else
		DestroyWindow(hwnd);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PrepareOverdueList
 Created  : Sun Jul  5 17:12:55 2009
 Modified : Sun Jul  5 17:12:55 2009

 Synopsys : Prepares overdue notes list view
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PrepareOverdueList(void){
	
	HBITMAP			hBmp;
	LVCOLUMNW		lvc;
	HMODULE			hIcons;

	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.cx = 160;
	lvc.iSubItem = 0;
	SendMessageW(m_hList, LVM_INSERTCOLUMNW, 0, (LPARAM)&lvc);
	lvc.cx = 160;
	lvc.iSubItem = 1;
	SendMessageW(m_hList, LVM_INSERTCOLUMNW, 1, (LPARAM)&lvc);
	//prepare and set list view image lists
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_CTRL_SMALL));
		m_hImlSmall = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 6, 6);
		ImageList_AddMasked(m_hImlSmall, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		FreeLibrary(hIcons);
	}
	//set list view small image list
	ListView_SetImageList(m_hList, m_hImlSmall, LVSIL_SMALL);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FillOverdueList
 Created  : Sun Jul  5 17:12:13 2009
 Modified : Sun Jul  5 17:12:13 2009

 Synopsys : Fills list view with overdue notes properties
 Input    : parr - array of overdue notes
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void FillOverdueList(PMEMNOTE * parr){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	
	for(int i = 0; i < g_OverdueCount; i++){
		lvi.lParam = (LONG)parr[i];
		lvi.iItem = i;
		lvi.pszText = parr[i]->pData->szName;
		lvi.iImage = ItemImageIndex(parr[i]);
		//insert item
		SendMessageW(m_hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
		//insert schedule description subitem
		SetOverdueScheduleDescription(parr[i], i);
		//remove schedule - may be in future versions
		//if(parr[i]->pSchedule->scType == SCH_ONCE || parr[i]->pSchedule->scType == SCH_AFTER){
			////stop timer and remove scheduling because alarm should fire only once
			//ZeroMemory(parr[i]->pSchedule, sizeof(SCHEDULE_TYPE));
			//KillTimer(NULL, parr[i]->pRTHandles->idTimer);
			//WritePrivateProfileStructW(parr[i]->pFlags->id, S_SCHEDULE, parr[i]->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
			//if(parr[i]->pData->visible){
				////redraw note window
				//RedrawWindow(parr[i]->hwnd, NULL, NULL, RDW_INVALIDATE);
				//ShowNoteMarks(parr[i]->hwnd);
			//}
			//if(g_hCPDialog){
				////update Control Panel item
				//SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)parr[i]);
			//}
		//}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetOverdueScheduleDescription
 Created  : Sun Jul  5 17:11:12 2009
 Modified : Sun Jul  5 17:11:12 2009

 Synopsys : Sets text for schedule column of list view
 Input    : pNote - note
            index - list view item index
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetOverdueScheduleDescription(PMEMNOTE pNote, int index){
	wchar_t 			szBuffer[256], szDate[128];
	LVITEMW				lvi;

	szBuffer[0] = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = index;
	lvi.iSubItem = 1;
	lvi.pszText = szBuffer;
	//get schedule description according to UI language
	switch(pNote->pSchedule->scType){
		case SCH_ED:
			GetScheduleDescription(IDC_OPT_EV_DAY, szBuffer, g_NotePaths.CurrLanguagePath, L"Every day at:");
			break;
		case SCH_ONCE:
			GetScheduleDescription(IDC_OPT_ONCE, szBuffer, g_NotePaths.CurrLanguagePath, L"Once at:");
			break;
		case SCH_REP:
		case SCH_REP + START_PROG:
		case SCH_REP + START_COMP:
			GetScheduleDescription(IDC_OPT_REPEAT, szBuffer, g_NotePaths.CurrLanguagePath, L"Repeat every:");
			break;
		case SCH_PER_WD:
			GetScheduleDescription(IDC_OPT_EVERY_WD, szBuffer, g_NotePaths.CurrLanguagePath, L"Weekly on:");
			break;
		case SCH_AFTER:
		case SCH_AFTER + START_PROG:
		case SCH_AFTER + START_COMP:
			GetScheduleDescription(IDC_OPT_AFTER, szBuffer, g_NotePaths.CurrLanguagePath, L"After:");
			break;
		case SCH_MONTHLY_EXACT:
			GetScheduleDescription(IDC_OPT_MONTH_EXACT, szBuffer, g_NotePaths.CurrLanguagePath, L"Monthly (exact)");
			break;
		case SCH_MONTHLY_RELATIVE:
			GetScheduleDescription(IDC_OPT_MONTH_RELATIVE, szBuffer, g_NotePaths.CurrLanguagePath, L"Monthly (relative)");
			break;
	}
	//parse schedule type
	ParseScheduleType(pNote->pSchedule, szDate);
	wcscat(szBuffer, L" ");
	wcscat(szBuffer, szDate);
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ApplyOverdueLanguage
 Created  : Sun Jul  5 17:09:56 2009
 Modified : Sun Jul  5 17:09:56 2009

 Synopsys : Changes dialog UI language
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void ApplyOverdueLanguage(void){
	wchar_t 		szBuffer[256], szKey[16];
	LVCOLUMNW		lvc;

	_itow(DLG_OVERDUE, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Overdue reminders", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(g_hOverdueDialog, szBuffer);
	//set list view columns text
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	SendMessageW(m_hList, LVM_GETCOLUMNW, 0, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"0", L"Note Name", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hList, LVM_SETCOLUMNW, 0, (LPARAM)&lvc);
	SendMessageW(m_hList, LVM_GETCOLUMNW, 1, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"3", L"Schedule Type", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hList, LVM_SETCOLUMNW, 1, (LPARAM)&lvc);

	SetDlgCtlText(g_hOverdueDialog, IDOK, g_NotePaths.CurrLanguagePath, L"OK");
}

/*-@@+@@------------------------------------------------------------------
 Procedure: UpdateOverdueItem
 Created  : Sun Jul  5 17:09:18 2009
 Modified : Sun Jul  5 17:09:18 2009

 Synopsys : Updates list view item for specified note
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void UpdateOverdueItem(PMEMNOTE pNote){
	int			count, index = 0;
	LVITEMW		lvi;
	BOOL		found = FALSE;

	count = ListView_GetItemCount(m_hList);
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;
	//find list view item which corresponds to note
	for(int i = 0; i < count; i++){
		lvi.iItem = i;
		index = i;
		SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if((PMEMNOTE)lvi.lParam == pNote){
			found = TRUE;
			break;
		}
	}
	if(!found){
		return;
	}
	//update the name column
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iImage = ItemImageIndex(pNote);
	lvi.pszText = pNote->pData->szName;
	lvi.iSubItem = 0;
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	//set item schedule description
	SetOverdueScheduleDescription(pNote, index);
	//ensure note visible
	SendMessageW(m_hList, LVM_ENSUREVISIBLE, index, FALSE);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SelectedNote
 Created  : Sun Jul  5 17:08:15 2009
 Modified : Sun Jul  5 17:08:15 2009

 Synopsys : Returns note associated with selected list view item
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static PMEMNOTE SelectedNote(void){
	int			index;
	LVITEMW		lvi;

	//find selected list view item - if any
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;
	index = ListView_GetNextItem(m_hList, -1, LVNI_SELECTED);
	if(index > -1){
		lvi.iItem = index;
		SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if(lvi.lParam){
			return (PMEMNOTE)lvi.lParam;
		}
	}
	return NULL;
}

static void ShowNotePreview(int selCount){
	if(selCount == 1){
		PMEMNOTE		pNote = SelectedNote();
		if(pNote->pRTHandles->hbSkin){
			COLORREF		color;
			
			OleTranslateColor(0x800000 | GetSysColor(COLOR_WINDOW), NULL, &color);
			SetPreviewBackgroundColor(FALSE, color);
		}
		else{
			SetPreviewBackgroundColor(FALSE, pNote->pAppearance->crWindow);
		}
		ReadNoteRTFFile(pNote->pFlags->id, m_hEdit, NULL);
	}
	else{
		SetPreviewBackgroundColor(TRUE, 0);
		SetWindowTextW(m_hEdit, NULL);
	}
}

static void SetPreviewBackgroundColor(BOOL clear, COLORREF color){
	if(!clear){
		//set the color
		if(!m_PvwUseColor){
			//use note's color
			RichEdit_SetBkgndColor(m_hEdit, 0, color);
		}
		else{
			//use predefined color
			if(m_PvwColor != -1)
				//if the color is really defined
				RichEdit_SetBkgndColor(m_hEdit, 0, m_PvwColor);
			else
				//if not - use system window background color
				RichEdit_SetBkgndColor(m_hEdit, 1, 0);
		}
	}
	else{
		//clear edit box
		if(!m_PvwUseColor){
			//clear with system window background color
			RichEdit_SetBkgndColor(m_hEdit, 1, 0);
		}
		else{
			//clear with predefined color
			if(m_PvwColor != -1)
				//clear with predefined color
				RichEdit_SetBkgndColor(m_hEdit, 0, m_PvwColor);
			else
				//clear with system window background color
				RichEdit_SetBkgndColor(m_hEdit, 1, 0);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: OverdueStringCompareFunc
 Created  : Fri May 25 21:55:16 2007
 Modified : Fri May 25 21:55:16 2007

 Synopsys : Callback function for string list view column sorting
 Input    : lParam1 - index of first item
            lParam2 - index of second item
            lParamSort - sort order
 Output   : A negative value if the first item should precede the second, a 
            positive value if the first item should follow the second, or 
            zero if the two items are equivalent
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int CALLBACK OverdueStringCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	wchar_t		szName1[128], szName2[128];
	LVITEMW		lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = szName1;
	lvi.iSubItem = m_SortColumn;
	SendMessageW(m_hList, LVM_GETITEMTEXTW, (WPARAM)lParam1, (LPARAM)&lvi);
	lvi.pszText = szName2;
	SendMessageW(m_hList, LVM_GETITEMTEXTW, (WPARAM)lParam2, (LPARAM)&lvi);
	if((int)lParamSort == LVS_SORTASCENDING)
		return _wcsicmp(szName1, szName2);
	else
		return _wcsicmp(szName2, szName1);
}
