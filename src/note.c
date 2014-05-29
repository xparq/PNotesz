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

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "shared.h"
#include "memorynotes.h"
#include "docking.h"
#include "bell.h"
#include "combodialog.h"
#include "note.h"
#include "transparentbitmap.h"
#include "skin.h"
#include "groups.h"
#include "skinlessdrawing.h"
#include "toolbar.h"
#include "find.h"
#include "send.h"
#include "print.h"
#include "encryption.h"
#include "gcolors.h"
#include "gcpicker.h"
#include "smilies.h"
#include "dataobject.h"
#include "oleinterface.h"
#include "picture.h"
#include "diary.h"
#include "enums.h"
#include "sengs.h"
#include "hotkeys.h"
#include "fdialogs.h"
#include "linksdialog.h"
#include "plinks.h"
#include "contacts.h"
#include "hosts.h"
#include "sockerrors.h"
#include "sockets.h"
#include "multicontacts.h"
#include "login.h"
#include "multigroups.h"
#include "contgroups.h"

#define	EDIT_CALLBACK			L"EDIT_CALLBACK"
#define	PN_PROP					L"PN_PROP"

#define	CLOSE_TIMER_ID			201
#define	STOP_LOOP_TIMER_ID		202

#define HANDLE_WM_MOUSELEAVE(hwnd,wParam,lParam,fn) ((fn)(hwnd),0)

/** Toolbar buttons ********************************************************/
#if defined(_WIN64)
static TBBUTTON				m_SoundButtonsF[] = {
							{0,IDC_CMD_PVW_CUST_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0}};
static TBBUTTON				m_SoundButtonsV[] = {
							{0,IDC_CMD_PVW_CUST_VOICE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0}};
#else
static TBBUTTON				m_SoundButtonsF[] = {
							{0,IDC_CMD_PVW_CUST_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}};
static TBBUTTON				m_SoundButtonsV[] = {
							{0,IDC_CMD_PVW_CUST_VOICE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}};
#endif

typedef struct {
	HWND	hAppearance;
	HWND	hSchedule;
}NDLG_WNDS;

typedef struct {
	HWND	hOnce;
	HWND	hEveryDay;
	HWND	hRepeat;
	HWND	hWeekly;
	HWND	hAfter;
	HWND	hMonthExact;
	HWND	hMonthRel;
}DNS_WNDS;

/** Enums ********************************************************/
typedef enum  
{
	CODE_TEXT, 
	CODE_UNICODE, 
	CODE_UNICODE_BE, 
	CODE_UTF8
}code_types;

typedef enum 
{
	MARK_SCHEDULE, 
	MARK_CHANGED, 
	MARK_PROTECTED, 
	MARK_PRIORITY, 
	MARK_COMPLETED,
	MARK_PASSWORD,
	MARK_PIN
} note_marks;

typedef enum 
{
	NDF, 
	CC_TEXT, 
	CC_HIGHLIGHT
} cmd_colors;

typedef enum 
{
	FD_BOLD, 
	FD_ITALIC, 
	FD_UNDERLINE, 
	FD_STRIKETHROUGH
} font_decoration;

/** Prototypes ********************************************************/
static void DoAlarm(PMEMNOTE pNote);
static void SpeakNote(PMEMNOTE pNote, void * pToken);
static int CheckDockPositionForAlarm(HWND hwnd);
static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData);
static LRESULT CALLBACK Note_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Note_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Note_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Note_OnKillFocus(HWND hwnd, HWND hwndNewFocus);
static void Note_OnRButtonUp(HWND hwnd, int x, int y, UINT flags);
static void Note_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Note_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Note_OnSetFocus(HWND hwnd, HWND hwndOldFocus);
static void Note_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Note_OnClose(HWND hwnd);
static BOOL Note_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void Note_OnPaint(HWND hwnd);
static void Note_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void Note_OnSize(HWND hwnd, UINT state, int cx, int cy);
static BOOL Note_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos);
static HBRUSH Note_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static void Note_OnDropFiles(HWND hwnd, HDROP hdrop);
static void Note_OnLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Note_OnMouseLeave(HWND hwnd);
static void Note_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void OnResizeNote(HWND hwnd, LPRECT lpRc);
static LRESULT CALLBACK Edit_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Edit_OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
// static BOOL IsCharrangeLink(HWND hEdit);
static void Adjust_OnClose(HWND hwnd);
static BOOL Adjust_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Adjust_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void AddTabs(HWND hwnd);
static BOOL IsAfterMatch(P_SCHEDULE_TYPE pS, LPSYSTEMTIME lpst);
static void FillScheduleTypes(HWND hwnd, wchar_t * lpFile);
static void FillDaysOfWeek(wchar_t * lpLang);
static void FillStopLoop(HWND hCombo);
static BOOL ScheduleChanged(PMEMNOTE pNote);
static void SetSizes(HWND hwnd, int cx, int cy);
static void SaveAs_OnDestroy(HWND hwnd);
static BOOL SaveAs_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void SaveAs_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void SaveAs_OnClose(HWND hwnd);
static void SetTooltip(HWND hOwner, HWND hTooltip, P_NOTE_DATA pD, P_NOTE_RTHANDLES pH, LPSRSTATUS pSRStatus, BOOL bUpdate);
static void ColorChanged(HWND hNote, int id);
static void SetNoteCaption(HWND hwnd);
static LRESULT CALLBACK Grip_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Grip_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void PaintGrip(HWND hwnd, HDC hdcPS, COLORREF crWindow);
static void PaintSkinnableWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH);
static void PaintSimpleWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH);
static void RestoreStandardView(HWND hNote);
static void ClearOnAdjustClose(PMEMNOTE pNote);
static void SetREFontColorCommon(HWND hEdit, COLORREF crText);
static void SetREHighlight(HWND hNote, COLORREF crColor);
static void SetREFontFace(HWND hNote, P_FONT_TYPE pft);
static void SetREFontSize(HWND hNote, int size);
static void SetREFontColor(HWND hNote, COLORREF crColor);
static void SetREFontDecoration(HWND hNote, font_decoration fd);
static void SetREAlignment(HWND hNote, int alignment);
static void SetREBulletsAndNumbering(HWND hNote, int id);
static void RemoveAllBulletsNumbering(HWND hNote);
static DWORD OutStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
static LRESULT CALLBACK Command_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Command_OnPaint(HWND hwnd);
static void Command_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void Tbr_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Tbr_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Tbr_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Tbr_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void SetNoteCommandsBar(HWND hwnd, P_NOTE_RTHANDLES pH);
static void SetCommandsTooltips(HWND hwnd);
static void RecreateCommandTooltip(HWND hwnd);
static LRESULT CALLBACK Tbr_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void DeleteAutoSavedFile(wchar_t * id);
static void SetCurrentFontFace(HWND hEdit);
static int GetCurrentFontSize(HWND hEdit);
static BOOL ApplyNoteChanges(HWND hwnd);
static BOOL CALLBACK NApp_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NApp_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void NApp_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL NApp_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void NApp_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static BOOL CALLBACK NSched_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NSched_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL NSched_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void InitAppearancePart(HWND hwnd);
static void InitSchedulePart(HWND hwnd);
static void AddGroupsMenuItems(HWND hwnd, HMENU hMenu);
static void FreeNoteMenus(HMENU hMenu);
static void RemoveBell(HWND hwnd);
static void rseed(int seed);
static int rnd(int base);
static COLORREF MakeRandomColor(void);
static BOOL CALLBACK Dns_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Dns_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Dns_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void SetDockType(int * dockData, dock_type dockType);
static void DockNote(HWND hwnd, dock_type dockType, P_NOTE_DATA pD);
static void UndockNote(PMEMNOTE pNote);
static void WriteNotePlacement(HWND hwnd, BOOL fSave);
static void SaveNewBColor(PMEMNOTE pNote, COLORREF crWindow);
static void SaveNewCFont(PMEMNOTE pNote, LPLOGFONTW lplf);
static void SaveNewCColor(PMEMNOTE pNote, COLORREF crCaption);
static int ArrowRequired(HWND hwnd, dock_type dockType, int dockData);
static void ApplyNewSkin(HWND hNote, P_NOTE_RTHANDLES pH);
static void GetPesterPart(LPSYSTEMTIME lps);
static void SetPesterPart(LPSYSTEMTIME lps);
static void ShowHideNoteToolbar(HWND hNote);
static void ShiftNotes(int pos);
static void SetWeekdayText(int id, int day, const wchar_t * lpFile);
static short SelectedWeekdays(void);
static void PasteContentsOfFile(HWND hwnd);
static void PasteLinkToFile(HWND hwnd);
static LRESULT CALLBACK Marks_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Marks_OnPaint(HWND hwnd);
static void Marks_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void AddWeekdayToCombo(int day, const wchar_t * lpFile);
static void AddOrdinalToCombo(int id, const wchar_t * lpFile);
static void FillFullDaysOfWeek(wchar_t * lpLang);
static void ManageBackupFiles(wchar_t * lpNoteID, wchar_t * lpNoteFile);
static void RestoreFromBackup(PMEMNOTE pNote);
static void CALLBACK PreventCommandTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void CheckBulletsMenuItems(HWND hEdit);
static void LoadDuplicatedNoteProperties(PMEMNOTE pNote, P_NOTE_RTHANDLES pHandles, PMEMNOTE pSrc);
static void CALLBACK CloseTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void CALLBACK StopAlertTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void SetTransValue(HWND hwnd, int value);
static void InsertPicture(HWND hNote, wchar_t * lpFile);
static int ChoosePictureFile(HWND hNote, wchar_t * lpFile);
static HBRUSH PatternBrushFromSkin(HWND hNote, SIZE sz);
static void Tags_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Tags_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void AddTags(HWND hwnd);
static void LoadTags(PMEMNOTE pNote);
static void ReadTagsFromString(PMEMNOTE pNote, wchar_t * pText);
static void ExchangeREContents(HWND hEditSrc, HWND hEditDest);
static void BuildSEngsMenu(void);
static void AddGroupsToCombo(HWND hCombo);
static void InsertDateTime(HWND hwnd);
static void * GetNoteVoiceToken(P_SCHEDULE_TYPE pS);
static BOOL IsNoteHotkeyAvailable(PMEMNOTE pNote, HWND hEdit, int command);
static void SaveLinks(PMEMNOTE pNote);
static void LoadLinks(PMEMNOTE pNote);
static void BuildLinksMenu(PMEMNOTE pNote);
static void RemoveNoteFromLinks(PMEMNOTE pNote);
static BOOL DelHideInvisible(void);
static BOOL CALLBACK WinList_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void WinList_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL WinList_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
static void SetFlippedMenu(UINT id, UINT idCaption, int image, PMITEM pMenus, HMENU hMenu, wchar_t * lpLangFile, P_HK_TYPE lpKeys, int keysCount, wchar_t * lpDefault);
static void CALLBACK PinTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void DestroySuggestionsMenu(void);
static HMENU CreateSuggestionsMenu(PSUGGESTION ps);
// static void ApplyInterLink(HWND hwnd);
// static void ExecuteInternalLink(HWND hwnd, CHARRANGE * lpchr);
// static void ReApplayInterLinks(HWND hEdit);

/** Module variables ********************************************************/
static BOOL				m_fFullDrag;
static NOTE_RTHANDLES	m_TNHandles;
static NOTE_APPEARANCE	m_TNAppearance;
static ADDITIONAL_NAPP	m_TAddAppearance;
static int				m_DataRes1;
static BOOL				m_JustCreated = TRUE;
static cmd_colors		m_CurrColorsChoice = NDF;
static BOOL				m_Reload = FALSE;
static NDLG_WNDS		m_Dlgs;
static DNS_WNDS			m_Dns;
static BOOL				m_DblClickSize = FALSE, m_InDblClick = FALSE;
static HMENU			m_hTempMenu = NULL, m_hMenuTColor, m_hMenuTHighlight, m_hMenuSuggestions;
static int 				rnd_seed = 12345678;
static wchar_t			m_DropFile[MAX_PATH * 2];
static UINT				m_PreventCmdTimer;
static BOOL				m_InCopy = FALSE;
static wchar_t			m_TransCaption[128];
static int				m_CurrType;
static BYTE				m_BOMUnicode[] = {0xff, 0xfe};
static BYTE				m_BOMBE[] = {0xfe, 0xff};
static BYTE				m_BOMUTF8[] = {0xef, 0xbb, 0xbf};

/** Popup menus ********************************************************/
static MITEM			m_NMenus[] = {{IDM_SAVE, 30, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save", L"Ctrl+S"}, 
									{IDM_SAVE_AS, 31, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save As", L""}, 
									{IDM_SAVE_AS_TEXT, 32, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save As Text File", L""}, 
									{IDM_RESTORE_BACKUP, 33, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore From Backup", L""}, 
									{IDM_DUPLICATE_NOTE, 34, 0, -1, -1, MT_REGULARITEM, FALSE, L"Duplicate Note", L""},
									{IDM_PRINT_NOTE, 35, 0, -1, -1, MT_REGULARITEM, FALSE, L"Print", L"Ctrl+P"}, 
									{IDM_ADJUST_APPEARANCE, 36, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Appearance", L""}, 
									{IDM_ADJUST_SCHEDULE, 37, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Schedule", L""}, 
									{IDM_HIDE, 38, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide", L""}, 
									{IDM_DEL, 39, 0, -1, -1, MT_REGULARITEM, FALSE, L"Delete", L""},
									{IDM_DOCK, 14, 0, -1, -1, MT_REGULARITEM, FALSE, L"Dock", L""},
									{IDM_DOCK_NONE, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
									{IDM_DOCK_LEFT, 15, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
									{IDM_DOCK_TOP, 16, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
									{IDM_DOCK_RIGHT, 17, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
									{IDM_DOCK_BOTTOM, 18, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
									{IDM_EMAIL, 40, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send", L""},
									{IDM_ATTACHMENT, 41, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send As Attachment", L""},
									{IDM_SEND_INTRANET, 42, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send Via Network", L""},
									{IDM_REPLY, 43, 0, -1, -1, MT_REGULARITEM, FALSE, L"Reply", L""},
									{IDM_ADD_CONTACT, 62, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add Contact", L""},
									{IDM_ADD_CONT_GROUP, 63, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add Group", L""},
									{IDM_SEND_TO_SEVERAL, 64, 0, -1, -1, MT_REGULARITEM, FALSE, L"Select Contact", L""},
									{IDM_SELECT_CONT_GROUP, 65, 0, -1, -1, MT_REGULARITEM, FALSE, L"Select Group", L""},
									{IDM_TAGS, 44, 0, -1, -1, MT_REGULARITEM, FALSE, L"Tags", L""},
									{IDM_NOTE_GROUPS, 45, 0, -1, -1, MT_REGULARITEM, FALSE, L"Groups", L""},
									{IDM_LINKS, 46, 0, -1, -1, MT_REGULARITEM, FALSE, L"Linked Notes", L""},
									{IDM_MANAGE_LINKS, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Manage Linked Notes", L""},
									{IDM_ADD_TO_FAVORITES, 47, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add To Favorites", L""},
									{IDM_ON_TOP, -1, 0, 49, 0, MT_REGULARITEM, FALSE, L"On Top", L""},
									{IDM_HIGH_PRIORITY, -1, 0, 49, 0, MT_REGULARITEM, FALSE, L"Toggle High Priority", L""}, 
									{IDM_TOGGLE_PROTECTION, -1, 0, 49, 0, MT_REGULARITEM, FALSE, L"Toggle Protection Mode", L""},
									{IDM_LOCK_NOTE, 53, 0, -1, -1, MT_REGULARITEM, FALSE, L"Set Note Password", L""},
									{IDM_MARK_AS_COMPLETED, -1, 0, 49, 0, MT_REGULARITEM, FALSE, L"Mark As Completed", L""},
									{IDM_ROLL_UNROLL, 50, 0, -1, -1, MT_REGULARITEM, FALSE, L"Roll/Unroll", L""}, 
									{IDM_ADD_PIN, 57, 0, -1, -1, MT_REGULARITEM, FALSE, L"Pin To Window", L""}};
static MITEM			m_EMenus[] = {{IDM_UNDO, 0, 0, -1, -1, MT_REGULARITEM, FALSE, L"Undo", L"Ctrl+Z"}, 
									{IDM_REDO, 1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Redo", L"Ctrl+Y"},
									{IDM_CUT, 2, 0, -1, -1, MT_REGULARITEM, FALSE, L"Cut", L"Ctrl+X"},
									{IDM_COPY, 3, 0, -1, -1, MT_REGULARITEM, FALSE, L"Copy", L"Ctrl+C"},
									{IDM_PASTE, 4, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste", L"Ctrl+V"},
									{IDM_COPY_AS_TEXT, 3, 0, -1, -1, MT_REGULARITEM, FALSE, L"Copy As Plain Text", L""},
									{IDM_PASTE_AS_TEXT, 4, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste As Plain Text", L""},
									{IDM_FORMAT_NOTE_TEXT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Format", L""},								
									{IDM_FORMAT_FONT_FAMILY, 10, 0, -1, -1, MT_REGULARITEM, FALSE, L"Font", L""},
									{IDM_FORMAT_FONT_SIZE, 11, 0, -1, -1, MT_REGULARITEM, FALSE, L"Font Size", L""},
									{IDM_FORMAT_FONT_COLOR, 12, 0, -1, -1, MT_REGULARITEM, FALSE, L"Font Color", L""},
									{IDM_FORMAT_FONT_BOLD, 13, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bold", L"Ctrl+B"},
									{IDM_FORMAT_FONT_ITALIC, 14, 0, -1, -1, MT_REGULARITEM, FALSE, L"Italic", L"Ctrl+I"},
									{IDM_FORMAT_FONT_UNDER, 15, 0, -1, -1, MT_REGULARITEM, FALSE, L"Underline", L"Ctrl+U"},
									{IDM_FORMAT_FONT_STRIKE, 16, 0, -1, -1, MT_REGULARITEM, FALSE, L"Strikethrough", L"Ctrl+K"},
									{IDM_FORMAT_FONT_HILIT, 17, 0, -1, -1, MT_REGULARITEM, FALSE, L"Highlight", L""},
									{IDM_FORMAT_ALIGN_LEFT, 18, 0, -1, -1, MT_REGULARITEM, FALSE, L"Align Left", L"Ctrl+L"},
									{IDM_FORMAT_ALIGN_CENTER, 19, 0, -1, -1, MT_REGULARITEM, FALSE, L"Center", L"Ctrl+E"},
									{IDM_FORMAT_ALIGN_RIGHT, 20, 0, -1, -1, MT_REGULARITEM, FALSE, L"Align Right", L"Ctrl+R"},
									{IDM_FORMAT_BULLETS, 21, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bullets", L""},
									{IDM_INSERT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Insert", L""}, 
									{IDM_INSERT_PICTURE, 100, 0, -1, -1, MT_REGULARITEM, FALSE, L"Insert picture", L""}, 
									{IDM_INSERT_SMILIE, 101, 0, -1, -1, MT_REGULARITEM, FALSE, L"Insert smiley", L""}, 
									{IDM_INSERT_DATETIME, 107, 0, -1, -1, MT_REGULARITEM, FALSE, L"Insert date/time", L""},
									{IDM_SPELL_CHECK, 22, 0, -1, -1, MT_REGULARITEM, FALSE, L"Spell Checking", L""},
									{IDM_SPELL_NOW, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Check Now", L""},
									{IDM_SPELL_AUTO, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"Check Automatically", L""},
									{IDM_SPELL_DOWNLOAD, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Download Dictionary", L""},
									{IDM_SEARCH_NOTE, 5, 0, -1, -1, MT_REGULARITEM, FALSE, L"Find", L"Ctrl+F"},
									{IDM_SEARCH_NOTE_NEXT, 6, 0, -1, -1, MT_REGULARITEM, FALSE, L"Find Next", L"F3"},
									{IDM_REPLACE_NOTE, 7, 0, -1, -1, MT_REGULARITEM, FALSE, L"Replace", L"Ctrl+H"},
									{IDM_SEARCH_WEB, 8, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search With:", L""},
									{IDM_SELECT_ALL, 9, 0, -1, -1, MT_REGULARITEM, FALSE, L"Select All", L"Ctrl+A"},
									{IDM_NO_BULLETS, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"No Bullets/Numbering", L""},
									{IDM_SIMPLE_BULLETS, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"Simple bullets", L""},
									{IDM_NUMBERS_PERIOD, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"1. 2. 3.", L""},
									{IDM_NUMBERS_PARENS, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"1) 2) 3)", L""},
									{IDM_LC_PERIOD, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"a. b. c.", L""},
									{IDM_LC_PARENS, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"a) b) c)", L""},
									{IDM_UC_PERIOD, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"A. B. C.", L""},
									{IDM_UC_PARENS, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"A) B) C)", L""},
									{IDM_ROMAN_LC, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"i. ii. iii.", L""},
									{IDM_ROMAN_UC, -1, 0, 23, 0, MT_REGULARITEM, FALSE, L"I. II. III.", L""}};
static MITEM			m_DMenus[] = {{IDM_DROP_SHORTCUT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste File As Link", L""}, 
									{IDM_DROP_CONTENT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste Contents Of File", L""}};

BOOL RegisterNoteClass(HINSTANCE hInstance){

	WNDCLASSEXW		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcx.lpfnWndProc = (WNDPROC)Note_WndProc;
	wcx.hInstance = hInstance;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.cbWndExtra = 16;
	wcx.lpszClassName = NWC_SKINNABLE_CLASS;
	//register skinnable class
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register grip class
	wcx.cbWndExtra = 0;
	wcx.lpfnWndProc = (WNDPROC)Grip_WndProc;
	wcx.lpszClassName = NWC_GRIP_CLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_SIZENWSE);
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register command button class
	wcx.lpfnWndProc = (WNDPROC)Command_WndProc;
	wcx.lpszClassName = NWC_TBR_BUTTON_CLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register toolbar class
	wcx.lpfnWndProc = (WNDPROC)Tbr_WndProc;
	wcx.lpszClassName = NWC_TOOLBAR_CLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register marks class
	wcx.lpfnWndProc = (WNDPROC)Marks_WndProc;
	wcx.lpszClassName = NWC_NOTE_MARK_CLASS;
	if(!RegisterClassExW(&wcx))
		return FALSE;
	return TRUE;
}

void LoadNewDiaryNoteProperties(PMEMNOTE pNote, wchar_t ** formats){
	BOOL				useSkin = FALSE;
	wchar_t				szID[28];
	COLORREF			textColor;

	pNote->pData->idGroup = GROUP_DIARY;
	//set note initially not saved
	pNote->pFlags->maskChanged |= F_TEXT;
	pNote->pFlags->saved = FALSE;
	//get creation time
	GetLocalTime(pNote->pCreated);
	if(g_RTHandles.hbSkin)
		useSkin = TRUE;
	if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS))
		memcpy(pNote->pRTHandles, &g_DiaryRTHandles, sizeof(NOTE_RTHANDLES));
	else
		memcpy(pNote->pRTHandles, &g_RTHandles, sizeof(NOTE_RTHANDLES));
	//set mask color for skinless note in order to prevent becoming selected text (black) transparent
	if(!useSkin)
		pNote->pRTHandles->crMask = CLR_MASK;
	//copy common appearance
	if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS))
		memcpy(pNote->pAppearance, &g_DiaryAppearance, sizeof(NOTE_APPEARANCE));
	else
		memcpy(pNote->pAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));

	//set dock index to -777
	SetDockIndex(&pNote->pData->dockData, -777);
	FormatNewNoteID(szID);
	//set note id
	wcscpy(pNote->pFlags->id, szID);
	//set note's name
	GetDiaryDateString(g_NoteSettings.reserved1, pNote->pCreated, formats, g_DiaryFormatIndex, pNote->pData->szName);

	//check for random color setting
	if(IsBitOn(g_NoteSettings.reserved1, SB1_RANDCOLOR)){		
		pNote->pAppearance->crWindow = MakeRandomColor();
		if(IsBitOn(g_NoteSettings.reserved1, SB1_INVERT_TEXT)){	
			textColor = ~pNote->pAppearance->crWindow;
			textColor <<= 8;
			textColor >>= 8;
			pNote->pAppearance->crFont = textColor;
			pNote->pAppearance->nPrivate |= F_RTFONT;
			pNote->pFlags->maskChanged |= F_RTFONT;
		}
		pNote->pAppearance->nPrivate |= F_B_COLOR;
		pNote->pFlags->maskChanged |= F_B_COLOR;
	}
	//new note is always on-top
	if(g_NoteSettings.newOnTop){
		pNote->pData->onTop = TRUE;
		pNote->pData->prevOnTop = TRUE;
	}
	//set visibility
	//new note is always visible
	pNote->pData->visible = TRUE;
}

void LoadNoteProperties(PMEMNOTE pNote, P_NOTE_RTHANDLES pHandles, wchar_t * lpID, BOOL loadNew){
	BOOL				res, useSkin = FALSE, fOld = FALSE, fSetPrivate, fSaveSchedule = FALSE;
	wchar_t				szID[28], szBuffer[256];
	RECT				rc;
	NOTE_APPEARANCE		tmpA;
	COLORREF			textColor;
	PPIN				pin = {0};
	// LPPNGROUP			ppg;


	SetRectEmpty(&rc);

	if(pHandles->hbSkin)
		useSkin = TRUE;
	memcpy(pNote->pRTHandles, pHandles, sizeof(NOTE_RTHANDLES));
	//set mask color for skinless note in order to prevent becoming selected text (black) transparent
	if(!useSkin)
		pNote->pRTHandles->crMask = CLR_MASK;
	//set note initially saved
	pNote->pFlags->saved = TRUE;
	//copy common appearance
	memcpy(pNote->pAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));
	if(lpID != NULL && !loadNew){				//in case of loading existing note from program DB
		pNote->pFlags->fromDB = TRUE;
		//get note data
		//get locked status
		GetPrivateProfileStringW(lpID, IK_LOCK, NULL, szBuffer, 256, g_NotePaths.DataFile);
		if(wcslen(szBuffer) > 0)
			pNote->pFlags->locked = true;
		else
			pNote->pFlags->locked = false;
		//get note favorite status
		GetPrivateProfileStringW(S_FAVORITES, lpID, N_NOTE_IN_FAVORITES, szBuffer, 128, g_NotePaths.INIFile);
		if(wcscmp(szBuffer, N_NOTE_IN_FAVORITES) != 0){
			pNote->pRTHandles->favorite = GROUP_FAVORITES;
		}
		//get note send/receive status
		GetPrivateProfileStructW(lpID, S_SEND_RECEIVE_STATUS, pNote->pSRStatus, sizeof(SRSTATUS), g_NotePaths.DataFile);
		//get note creation time
		GetPrivateProfileStructW(lpID, S_NOTE_CREATION, pNote->pCreated, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
		//for version 4.0
		if(!GetPrivateProfileStructW(lpID, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			//for version 3.5
			GetPrivateProfileStructW(lpID, S_POSITION, &rc, sizeof(rc), g_NotePaths.DataFile);
			pNote->pData->rcp = rc;
			//set note initally visible
			pNote->pData->visible = TRUE;
			if(!GetPrivateProfileStructW(lpID, S_DATA, pNote->pData, sizeof(NOTE_DATA) - sizeof(RECT) - sizeof(BOOL) - sizeof(BOOL) - sizeof(BOOL) - 2 * sizeof(int), g_NotePaths.DataFile)){
				wcscpy(pNote->pData->szName, lpID);
				GetLocalTime(&pNote->pData->stChanged);
			}
			//save note new data
			WritePrivateProfileStructW(lpID, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
		if(pNote->pData->idGroup == GROUP_DIARY && IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
			memcpy(pNote->pAppearance, &g_DiaryAppearance, sizeof(NOTE_APPEARANCE));
			memcpy(pNote->pRTHandles, &g_DiaryRTHandles, sizeof(NOTE_RTHANDLES));
		}
		//save prevOnTop
		pNote->pData->prevOnTop = pNote->pData->onTop;
		//get scheduling (if any)
		//version 9.0
		if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile)){
		//version 6.5
			if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(WORD), g_NotePaths.DataFile)){
				//version 6.0
				if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(int), g_NotePaths.DataFile)){
					//version 5.0
					if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(wchar_t) * 128 - sizeof(int), g_NotePaths.DataFile)){
						//version 4.7
						if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(SYSTEMTIME) - sizeof(wchar_t) * 128 - sizeof(int), g_NotePaths.DataFile)){
							//check for version previous to 4.5
							GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(SYSTEMTIME) * 2 - sizeof(SYSTEMTIME) - sizeof(wchar_t) * 128 - sizeof(int), g_NotePaths.DataFile);
						}
					}
				}
			}
		}
		//check for possible schedule from versions prior to 5.5 in order to set start time
		if(pNote->pSchedule->scType == SCH_REP){
			if(pNote->pSchedule->scStart.wYear == 0 && pNote->pSchedule->scStart.wMonth == 0 && pNote->pSchedule->scStart.wDay == 0 
			&& pNote->pSchedule->scStart.wHour == 0 && pNote->pSchedule->scStart.wMinute == 0){
				GetLocalTime(&pNote->pSchedule->scStart);
				fSaveSchedule = TRUE;
			}
		}
		//check other schedules
		else if(pNote->pSchedule->scType - SCH_REP == START_PROG){
			// memcpy(&pNote->pSchedule->scStart, &g_StartTimeProg, sizeof(SYSTEMTIME));
			pNote->pSchedule->scStart = g_StartTimeProg;
			// memcpy(&pNote->pSchedule->scLastRun, &g_StartTimeProg, sizeof(SYSTEMTIME));
			pNote->pSchedule->scLastRun = g_StartTimeProg;
		}
		else if(pNote->pSchedule->scType - SCH_REP == START_COMP){
			// memcpy(&pNote->pSchedule->scStart, &g_StartTimeSystem, sizeof(SYSTEMTIME));
			pNote->pSchedule->scStart = g_StartTimeSystem;
			// memcpy(&pNote->pSchedule->scLastRun, &g_StartTimeSystem, sizeof(SYSTEMTIME));
			pNote->pSchedule->scLastRun = g_StartTimeSystem;
		}
		else if(pNote->pSchedule->scType - SCH_AFTER == START_PROG){
			// memcpy(&pNote->pSchedule->scStart, &g_StartTimeProg, sizeof(SYSTEMTIME));
			pNote->pSchedule->scStart = g_StartTimeProg;
		}
		else if(pNote->pSchedule->scType - SCH_AFTER == START_COMP){
			// memcpy(&pNote->pSchedule->scStart, &g_StartTimeSystem, sizeof(SYSTEMTIME));
			pNote->pSchedule->scStart = g_StartTimeSystem;
		}
		//check schedule
		if(pNote->pSchedule->scType != 0 && pNote->pData->idGroup != GROUP_RECYCLE){
			pNote->isAlarm = TRUE;
		}
		//reset changed flag
		pNote->pFlags->maskChanged = 0;
		//get individual appearance
		ZeroMemory(&tmpA, sizeof(tmpA));
		res = GetPrivateProfileStructW(lpID, S_APPEARANCE, &tmpA, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
		if(!res){
			//try to get structure from previous version
			res = GetPrivateProfileStructW(lpID, S_APPEARANCE, &tmpA, 
				sizeof(NOTE_APPEARANCE) - sizeof(LOGFONTW) - sizeof(COLORREF) -
				sizeof(COLORREF) - sizeof(int), g_NotePaths.DataFile);
			fOld = TRUE;
		}
		if(res){
			//check skin
			if(*tmpA.szSkin){
				fSetPrivate = FALSE;
				if((tmpA.nPrivate & F_SKIN) == F_SKIN){
					fSetPrivate = TRUE;
				}
				else{
					if(fOld){
						if(wcscmp(tmpA.szSkin, pNote->pAppearance->szSkin) != 0){
							fSetPrivate = TRUE;
						}
					}
				}
				if(fSetPrivate){
					//save private skin
					wcscpy(pNote->pAppearance->szSkin, tmpA.szSkin);
					pNote->pAppearance->nPrivate |= F_SKIN;
				}
			}
			//check note's font
			if(tmpA.fFontSet){
				pNote->pAppearance->fFontSet = TRUE;
				pNote->pAppearance->nPrivate |= F_RTFONT;
			}
			//check skinless note caption's font
			if(*tmpA.lfCaption.lfFaceName && (tmpA.nPrivate & F_C_FONT) == F_C_FONT){
					//save private caption font
					// memcpy(&pNote->pAppearance->lfCaption, &tmpA.lfCaption, sizeof(tmpA.lfCaption));
					pNote->pAppearance->lfCaption = tmpA.lfCaption;
					pNote->pRTHandles->hFCaption = CreateFontIndirectW(&pNote->pAppearance->lfCaption);
					if(!fOld)
						pNote->pAppearance->nPrivate |= F_C_FONT;
			}
			//check skinless note caption's font color
			if(tmpA.crCaption && (tmpA.nPrivate & F_C_COLOR) == F_C_COLOR){
				//save private caption font color
				pNote->pAppearance->crCaption = tmpA.crCaption;
				if(!fOld)
					pNote->pAppearance->nPrivate |= F_C_COLOR;
			}
			//check skinless note background color
			if(tmpA.crWindow && (tmpA.nPrivate & F_B_COLOR) == F_B_COLOR){
				//save private background color
				pNote->pAppearance->crWindow = tmpA.crWindow;
				if(!fOld)
					pNote->pAppearance->nPrivate |= F_B_COLOR;
			}
		}
		//set note id
		wcscpy(pNote->pFlags->id, lpID);
		//check schedule
		if(pNote->pSchedule->scType != 0 && pNote->pData->idGroup != GROUP_RECYCLE){
			if(pNote->pSchedule->scType == SCH_REP || pNote->pSchedule->scType - SCH_REP == START_COMP 
			|| pNote->pSchedule->scType - SCH_REP == START_PROG){	//set repeat count according to timer interval
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
			}
			pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
		}
		//docking
		if(DockType(pNote->pData->dockData) == DOCK_NONE || pNote->pData->idGroup == GROUP_RECYCLE){
			//set dock index to -777 for notes from previous version
			if(DockIndex(pNote->pData->dockData) != -777)
				SetDockIndex(&pNote->pData->dockData, -777);
		}
		if(fSaveSchedule){
			//save changed schedule (from versions prior to 5.5)
			WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
		}
		//additional note appearance
		GetPrivateProfileStructW(lpID, S_ADD_APPEARANE, pNote->pAddNApp, sizeof(ADDITIONAL_NAPP), g_NotePaths.DataFile);
		//note's tags
		LoadTags(pNote);
		//load note deletion time
		GetPrivateProfileStructW(lpID, IK_DELETION_TIME, pNote->pDeleted, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
		if(pNote->pData->idGroup == GROUP_RECYCLE && pNote->pDeleted->wDay == 0){
			ZeroMemory(pNote->pDeleted, sizeof(SYSTEMTIME));
			GetLocalTime(pNote->pDeleted);
			WritePrivateProfileStructW(lpID, IK_DELETION_TIME, pNote->pDeleted, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
		}
		//load note real deletion time
		GetPrivateProfileStructW(lpID, IK_REAL_DELETION_TIME, pNote->pRealDeleted, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
		if(pNote->pData->idGroup == GROUP_RECYCLE && pNote->pRealDeleted->wDay == 0){
			ZeroMemory(pNote->pRealDeleted, sizeof(SYSTEMTIME));
			GetLocalTime(pNote->pRealDeleted);
			WritePrivateProfileStructW(lpID, IK_REAL_DELETION_TIME, pNote->pRealDeleted, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
		}
		//load possible links
		LoadLinks(pNote);
		//load possible pin data
		if(GetPrivateProfileStructW(lpID, IK_PIN, &pin, sizeof(PPIN), g_NotePaths.DataFile)){
			pNote->pPin = calloc(1, sizeof(PPIN));
			memcpy(pNote->pPin, &pin, sizeof(PPIN));
			pNote->pData->visible = FALSE;
			pNote->pRTHandles->pinTimer = SetTimer(NULL, 0, PIN_TIMER_INTERVAL, PinTimerProc);
		}
	}
	else if(lpID != NULL && loadNew){		//load note which is not from program DB
		//store note's id
		wcscpy(pNote->pFlags->id, lpID);
		//store temporary note's name
		wcscpy(pNote->pData->szName, lpID);
		//set dock index to -777
		SetDockIndex(&pNote->pData->dockData, -777);
		//check for random color setting
		if(IsBitOn(g_NoteSettings.reserved1, SB1_RANDCOLOR)){		
			pNote->pAppearance->crWindow = MakeRandomColor();
			pNote->pAppearance->nPrivate |= F_B_COLOR;
			pNote->pFlags->maskChanged |= F_B_COLOR;
		}
	}
	else{			//new note
		//get creation time
		GetLocalTime(pNote->pCreated);
		//set dock index to -777
		SetDockIndex(&pNote->pData->dockData, -777);
		FormatNewNoteID(szID);
		//set note id
		wcscpy(pNote->pFlags->id, szID);
		//set note's name
		// FormatNewNoteName(szID);
		// wcscpy(pNote->pData->szName, szID);
		wcscpy(pNote->pData->szName, g_NewNoteName);
		// //get note's group
		// ppg = PNGroupsGroupById(g_PGroups, pNote->pData->idGroup);
		//check for random color setting
		// if(!ppg->crWindow){
			if(IsBitOn(g_NoteSettings.reserved1, SB1_RANDCOLOR)){		
				pNote->pAppearance->crWindow = MakeRandomColor();
				if(IsBitOn(g_NoteSettings.reserved1, SB1_INVERT_TEXT)){	
					textColor = ~pNote->pAppearance->crWindow;
					textColor <<= 8;
					textColor >>= 8;
					pNote->pAppearance->crFont = textColor;
					pNote->pAppearance->crCaption = textColor;
					pNote->pAppearance->nPrivate |= (F_RTFONT | F_C_COLOR);
					pNote->pFlags->maskChanged |= (F_RTFONT | F_C_COLOR);
				}
				pNote->pAppearance->nPrivate |= F_B_COLOR;
				pNote->pFlags->maskChanged |= F_B_COLOR;
			}
		// }
		//new note is always on-top
		if(g_NoteSettings.newOnTop){
			pNote->pData->onTop = TRUE;
			pNote->pData->prevOnTop = TRUE;
		}
	}
	//set visibility
	if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
		if(lpID == NULL || loadNew){
			//new note is always visible
			pNote->pData->visible = TRUE;
		}
	}
	else{
		pNote->pData->visible = FALSE;
	}
}

HWND CreateNote(PMEMNOTE pNote, HINSTANCE hInstance, BOOL loadNew, wchar_t * lpFile){

	HWND				hwnd = NULL, hEdit, hStatic = NULL, hGrip = NULL, hToolbar, hSysToolbar, *phMarks, *phTemp, hParent = g_hNotesParent;
	int					style = WS_POPUP, exStyle = WS_EX_LAYERED | WS_EX_ACCEPTFILES;
	BITMAP				bm;
	P_NCMD_BAR			pC, pDH;
	SIZE				sz;
	RECT				rc, rcEdit;
	BOOL				useSkin = FALSE;
	NOTE_REL_POSITION	nrp;
	int					left = 0, top = 0, width = 0, height = 0, w, h;
	LPPNGROUP			ppg;

	//get note's group
	ppg = PNGroupsGroupById(g_PGroups, pNote->pData->idGroup);
	if(ppg && (pNote->pAppearance->nPrivate & F_B_COLOR) != F_B_COLOR){
		//prepare note's background color
		pNote->pAppearance->crWindow = ppg->crWindow;
	}
	if(ppg && (pNote->pAppearance->nPrivate & F_C_COLOR) != F_C_COLOR){
		//prepare note's caption color
		pNote->pAppearance->crCaption = ppg->crCaption;
	}
	if(ppg && (pNote->pAppearance->nPrivate & F_RTFONT) != F_RTFONT){
		//prepare note's text color
		pNote->pAppearance->crFont = ppg->crFont;
	}
	if(ppg && (pNote->pAppearance->nPrivate & F_SKIN) != F_SKIN){
		//prepare note's skin
		wcscpy(pNote->pAppearance->szSkin, ppg->szSkin);
	}

	SetRectEmpty(&rc);
	SetRectEmpty(&rcEdit);
	ZeroMemory(&nrp, sizeof(nrp));

	if(IsBitOn(g_NoteSettings.reserved1, SB1_NO_ALT_TAB)){
		exStyle |= WS_EX_TOOLWINDOW;
		hParent = NULL;
	}

	if(pNote->pRTHandles->hbSkin){
		useSkin = TRUE;
		//get skin dimensions
		GetObject(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		//create note window with skin dimensions
		hwnd = CreateWindowExW(exStyle, NWC_SKINNABLE_CLASS, g_NotePaths.ProgName, style, 0, 0, bm.bmWidth, bm.bmHeight, hParent, NULL, hInstance, NULL);
	}
	else{
		//create skinless note
		hwnd = CreateWindowExW(exStyle, NWC_SKINNABLE_CLASS, g_NotePaths.ProgName, style, 0, 0, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy, hParent, NULL, hInstance, NULL);
	}
	if(hwnd == NULL){
		return NULL;
	}
	m_JustCreated = TRUE;
	pNote->hwnd = hwnd;
	SetWindowLongPtrW(hwnd, OFFSET_MNOTE, (LONG_PTR)pNote);
	//create "toolbar buttons"
	hToolbar = CreateWindowExW(0, NWC_TOOLBAR_CLASS, NULL, WS_CHILD, 0, 0, 240, 240, hwnd, NULL, hInstance, NULL);
	SetPropW(hwnd, PH_COMMANDS, hToolbar);
	pC = calloc(CMD_TBR_BUTTONS, sizeof(NCMD_BAR));
	if(pC){
		SetWindowLongPtrW(hwnd, OFFSET_COMMANDS, (LONG_PTR)pC);
		for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
			pC->hCmd = CreateWindowExW(0, NWC_TBR_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 24, 24, hToolbar, (HMENU)(5000 + i), hInstance, NULL);
			pC++;
		}
	}
		
	//create delete/hide toolbar and buttons
	hSysToolbar = CreateWindowExW(0, NWC_TOOLBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 240, 240, hwnd, NULL, hInstance, NULL);
	SetPropW(hwnd, PH_DELHIDE, hSysToolbar);
	pDH = calloc(2, sizeof(NCMD_BAR));
	if(pDH){
		SetWindowLongPtrW(hwnd, OFFSET_DH, (LONG_PTR)pDH);
		for(int i = 0; i < CMD_SYS_BUTTONS; i++){
			pDH->hCmd = CreateWindowExW(0, NWC_TBR_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 24, 24, hSysToolbar, (HMENU)(IDM_HIDE + i), hInstance, NULL);
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
			*phTemp = CreateWindowExW(0, NWC_NOTE_MARK_CLASS, NULL, WS_CHILD, 0, 0, w, h, hwnd, NULL, hInstance, NULL);
			SetWindowLongPtr(*phTemp, GWLP_USERDATA, (LONG_PTR)i);
			phTemp++;
		}
	}

	if(!useSkin){
		//create grip window for skinless note
		hGrip = CreateWindowExW(0, NWC_GRIP_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hInstance, NULL);
		SetPropW(hwnd, PH_GRIP, hGrip);
		if(g_NoteSettings.showScrollbar)
			hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
		else
			hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
	}
	else{
		//create static window for rich edit transparency simulation
		hStatic = CreateWindowExW(0, L"Static", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_REALSIZEIMAGE, 0, 0, 10, 10, hwnd, NULL, hInstance, NULL);
		SetPropW(hwnd, PH_STATIC, hStatic);
		if(g_NoteSettings.showScrollbar)
			hEdit = CreateWindowExW(WS_EX_TRANSPARENT, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
		else
			hEdit = CreateWindowExW(WS_EX_TRANSPARENT, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
	}
	// ApplyTabStops(hEdit, FALSE);
	SetPropW(hwnd, PH_EDIT, hEdit);
	RichEdit_AutoURLDetect(hEdit, TRUE);
	
	//set margins for skinless control
	if(!useSkin){
		short	marginSize = GetSmallValue(g_SmallValues, SMS_MARGIN_SIZE);
		SendMessageW(hEdit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(marginSize, marginSize));
	}
	//create tooltip
	pNote->pRTHandles->hTooltip = CreateWindowExW(WS_EX_TOPMOST, L"Tooltips_class32", NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	

	if(pNote->pFlags->fromDB){				//in case of loading existing note from program DB
		//load RTF file contents
		ReadNoteRTFFile(pNote->pFlags->id, hEdit, NULL);

		GetPrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), g_NotePaths.DataFile);

		rc = pNote->pData->rcp;
		
		//prepare needed handles
		if(useSkin && (pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN){
			//change window size if it has a skin different from global settings
			GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, FALSE);
			//get skin dimensions
			GetObject(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		}

		//check note's font
		if(!pNote->pAppearance->fFontSet){
			//set richtext font to common
			SetREFontCommon(hEdit, &g_Appearance.lfFont);
			if(ppg && (pNote->pAppearance->nPrivate & F_RTFONT) != F_RTFONT){
				SetREFontColorCommon(hEdit, pNote->pAppearance->crFont);
			}
			else{
				SetREFontColorCommon(hEdit, g_Appearance.crFont);
			}
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

		//check for OnTop
		if(pNote->pData->onTop){
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		
		//set tooltip
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, pNote->pSRStatus, FALSE);
		//docking
		if(DockType(pNote->pData->dockData) != DOCK_NONE && pNote->pData->idGroup != GROUP_RECYCLE){
			AddDockItem(DHeader(DockType(pNote->pData->dockData)), hwnd, DockIndex(pNote->pData->dockData));
		}		
	}
	else if(!pNote->pFlags->fromDB && loadNew){		//load note which is not from program DB
		//load RTF file contents
		ReadNoteRTFFile(pNote->pFlags->id, hEdit, lpFile);
		//prepare edit box rectangle
		CopyRect(&rcEdit, &pNote->pRTHandles->rcEdit);
		//measure skinless note rectangles
		if(!useSkin){
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			CreateSimpleMarksBitmap(hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
			MeasureSkinlessNoteRectangles(pNote, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy, pNote->pData->szName);
		}
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, pNote->pSRStatus, FALSE);
		pNote->pFlags->saved = FALSE;
		SaveNote(pNote);
	}
	else{			//new note
		//prepare def width and height
		width = pNote->pRTHandles->szDef.cx;
		height = pNote->pRTHandles->szDef.cy;
		//prepare edit box rectangle
		CopyRect(&rcEdit, &pNote->pRTHandles->rcEdit);
		//measure skinless note rectangles
		if(!useSkin){
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			CreateSimpleMarksBitmap(hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
			MeasureSkinlessNoteRectangles(pNote, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy, pNote->pData->szName);
		}
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, pNote->pSRStatus, FALSE);
		//set richtext font to common
		SetREFontCommon(hEdit, &pNote->pAppearance->lfFont);
		SetREFontColorCommon(hEdit, pNote->pAppearance->crFont);
		if(pNote->pData->onTop){
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	if(!useSkin){
		//set richtext backcolor
		RichEdit_SetBkgndColor(hEdit, 0, pNote->pAppearance->crWindow);
		//position and size edit box
		MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, (pNote->pRTHandles->rcCaption.right - pNote->pRTHandles->rcCaption.left - 2), pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
		//position and size grip box
		MoveWindow(hGrip, pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.top, pNote->pRTHandles->rcGrip.right - pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.bottom - pNote->pRTHandles->rcGrip.top, TRUE);
	}
	else{
		//position and size static window
		MoveWindow(hStatic, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, TRUE);
		SendMessageW(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)pNote->pRTHandles->hbBack);
		//position and size edit box
		MoveWindow(hEdit, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, TRUE);
	}

	if(useSkin){
		SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
	}
	else{
		SetLayeredWindowAttributes(hwnd, CLR_MASK, 255, LWA_COLORKEY);
		SetRect(&pNote->pRTHandles->rcSize, left, top, left + width, top + height);
	}

	SetNoteCommandsBar(hwnd, pNote->pRTHandles);

	//initialize rich edit properties
	RichEdit_EmptyUndoBuffer(hEdit);
	RichEdit_SetEventMask(hEdit, ENM_CHANGE | ENM_DROPFILES | ENM_LINK | ENM_KEYEVENTS | ENM_MOUSEEVENTS);
	//remove possible protection mode from previous versions
	CHARFORMAT2		chr;
	ZeroMemory(&chr, sizeof(chr));
	chr.cbSize = sizeof(chr);
	chr.dwMask = CFM_PROTECTED;
	//don't set chr.dwEffects = CFE_PROTECTED, this will remove possible protection
	SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&chr);
	//set protection mode
	SendMessageW(hEdit, EM_SETREADONLY, IsBitOn(pNote->pData->res1, NB_PROTECTED), 0);
	RichEdit_SetModify(hEdit, FALSE);
	SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)Edit_WndProc));

	SetCommandsTooltips(hwnd);

	if(g_NoteSettings.rollOnDblClick){
		if(!useSkin && DockType(pNote->pData->dockData) == DOCK_NONE){
			if(pNote->pData->idGroup != GROUP_RECYCLE){
				if(pNote->pData->rolled){
					if(!IsBitOn(g_NoteSettings.reserved1, SB1_FIT_TO_CAPTION)){
						m_DblClickSize = TRUE;
						MoveWindow(hwnd, left, top, width, pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top, TRUE);
						pNote->pRTHandles->rolled = TRUE;
						m_DblClickSize = FALSE;
					}
					else{
						FitToCaption(pNote, FALSE);
					}
					pNote->pFlags->trackRoll = TRUE;
				}
			}
		}
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	
	SetNoteCaption(hwnd);

	if(DockType(pNote->pData->dockData) == DOCK_NONE){ //not docked and from DB
		if(pNote->pFlags->fromDB)
			WriteNotePlacement(hwnd, TRUE);
		else
			WriteNotePlacement(hwnd, FALSE);
	}
	m_JustCreated = FALSE;
	
	if(g_WinVer == WV_VISTA_AND_MORE && IsBitOn(g_NoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK)){
		//exclude note from peek (don't hide it when cursor is over "Show Desktop" corner)
		HMODULE							hLib = NULL;
		LPFN_DwmSetWindowAttribute		pProc = NULL;

		hLib = LoadLibraryW(L"dwmapi.dll");
		if(hLib){
			pProc = (LPFN_DwmSetWindowAttribute)GetProcAddress(hLib, "DwmSetWindowAttribute");
			if(pProc){
				BOOL		fAttribute = TRUE;
				(*pProc)(hwnd, 12, &fAttribute, sizeof(BOOL));
			}
			FreeLibrary(hLib);
		}
	}

	SetForegroundWindow(hwnd);
	SetFocus(hEdit);
	ShowHideNoteToolbar(hwnd);
	ShowNoteMarks(hwnd);
	
	return hwnd;
}

// void ApplyTabStops(HWND hEdit, BOOL invalidate){
	// PARAFORMAT2			pfm;
	// int				t = GetSmallValue(g_SmallValues, SMS_TAB_WIDTH) * 4;
	// ZeroMemory(&pfm, sizeof(pfm));
	// pfm.cbSize = sizeof(pfm);
	// pfm.dwMask = PFM_TABSTOPS;
	// RichEdit_GetParaFormat(hEdit, &pfm);
	// pfm.cTabCount = MAX_TAB_STOPS;
	// for(int i = 0; i < MAX_TAB_STOPS; i++)
		// pfm.rgxTabs[i] = 120 * t * (i + 1);
	// if(invalidate){
		// SendMessageW(hEdit, EM_SETSEL, 0, -1);
		// RichEdit_SetParaFormat(hEdit, &pfm);
		// int result = SendMessageW(hEdit, EM_SETTABSTOPS, 1, (LPARAM)&t);
		// InvalidateRect(hEdit, NULL, FALSE);
		// SendMessageW(hEdit, EM_SETSEL, -1, 0);
	// }
	// else{
		// SendMessageW(hEdit, EM_SETTABSTOPS, 1, (LPARAM)&t);
	// }
	
// }

void AddColorsMenu(void){
	MENUITEMINFOW	mi;
	HMENU			hFormat, hSize;
	wchar_t 		szAutomatic[256], szNoFill[256], szKey[12];

	_itow(ID_FC_CAPTION, szKey, 10);
	GetPrivateProfileStringW(S_COMMANDS, szKey, L"Automatic", szAutomatic, 256, g_NotePaths.CurrLanguagePath);
	_itow(ID_FH_CAPTION, szKey, 10);
	GetPrivateProfileStringW(S_COMMANDS, szKey, L"No Fill", szNoFill, 256, g_NotePaths.CurrLanguagePath);
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_SUBMENU | MIIM_ID;
	GetMenuItemInfoW(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_FORMAT_NOTE_TEXT), TRUE, &mi);
	hFormat = mi.hSubMenu;
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_FONT_COLOR), TRUE, &mi);
	m_hMenuTColor = mi.hSubMenu;
	CreateColorMenu(m_hMenuTColor, &g_FormatColorID, IDI_FORMAT_COLOR_START, szAutomatic);
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_FONT_HILIT), TRUE, &mi);
	m_hMenuTHighlight = mi.hSubMenu;
	CreateColorMenu(m_hMenuTHighlight, &g_FormatHighlightID, IDI_FORMAT_HIGHLIGHT_START, szNoFill);
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_FONT_SIZE), TRUE, &mi);
	hSize = mi.hSubMenu;
	CreateSizeMenu(&hSize, FALSE);
}

void SetScheduleForPrevious(HWND hwnd, wchar_t * lpID, wchar_t * lpFile){
	PMEMNOTE			pNote = MEMNOTE(hwnd);

	if(pNote){
		//get scheduling (if any)
		//version 9.0
		if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), lpFile)){
			//version 6.5
			if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(WORD), lpFile)){
				//version 6.0
				if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(int), g_NotePaths.DataFile)){
					if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(wchar_t) * 128, lpFile)){
						//check for version previous to 4.5
						GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(SYSTEMTIME), lpFile);
					}
				}
			}	
		}
		if(pNote->pSchedule->scType != 0){
			pNote->isAlarm = TRUE;
			if(pNote->pSchedule->scType == SCH_REP || pNote->pSchedule->scType - SCH_REP == START_COMP 
			|| pNote->pSchedule->scType - SCH_REP == START_PROG){	//set repeat count according to timer interval
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
			}
			pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
		}
	}
}

static void SetREFontSize(HWND hNote, int size){
	CHARFORMAT2W		chf;
	HDC					hdc;
	HWND				hEdit;
	PMEMNOTE			pNote = MEMNOTE(hNote);

	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_SIZE;
	hdc = GetDC(hEdit);
	size = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	chf.yHeight = 20 * -(size * 72) / GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(hEdit, hdc);
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontFace(HWND hNote, P_FONT_TYPE pft){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);

	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	if(wcslen(pft->szFace)){
		ZeroMemory(&chf, sizeof(chf));
		chf.cbSize = sizeof(chf);
		chf.dwMask = CFM_FACE | CFM_CHARSET;
		wcscpy(chf.szFaceName, pft->szFace);
		chf.bCharSet = (BYTE)g_SelectedFont.nCharSet;
		RichEdit_SetCharFormat((HWND)GetPropW(hNote, PH_EDIT), SCF_SELECTION, &chf);
		pNote->pFlags->maskChanged |= F_RTFONT;
		pNote->pAppearance->fFontSet = TRUE;
	}
}

static void SetREHighlight(HWND hNote, COLORREF crColor){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;

	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	//prevent transparency
	if(crColor == pNote->pRTHandles->crMask)
		crColor--;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	if(crColor == -1){
		chf.dwMask = CFE_AUTOBACKCOLOR;
		chf.dwEffects = CFE_AUTOBACKCOLOR;
	}
	else{
		chf.dwMask = CFM_BACKCOLOR;
		chf.crBackColor = crColor;
	}
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontColor(HWND hNote, COLORREF crColor){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;
	COLORREF			crDef;


	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	//prevent transparency
	if(crColor == pNote->pRTHandles->crMask)
		crColor--;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_COLOR;
	RichEdit_GetCharFormat(hEdit, SCF_DEFAULT, &chf);
	crDef = chf.crTextColor;
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_COLOR;
	if(crColor == -1){
		if((pNote->pAppearance->nPrivate & F_RTFONT) == F_RTFONT)
			chf.crTextColor = crDef;
		else
			chf.crTextColor = g_Appearance.crFont;
	}
	else{
		chf.crTextColor = crColor;
	}
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontDecoration(HWND hNote, font_decoration fd){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;
	int					state = 0;
	BOOL				set;

	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);

	//get character format (the first selected character) and save desired format
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	switch(fd){
	case FD_BOLD:
		chf.dwMask = CFM_BOLD;
		state = CFE_BOLD;
		break;
	case FD_ITALIC:
		chf.dwMask = CFM_ITALIC;
		state = CFE_ITALIC;
		break;
	case FD_UNDERLINE:
		chf.dwMask = CFM_UNDERLINE;
		state = CFE_UNDERLINE;
		break;
	case FD_STRIKETHROUGH:
		chf.dwMask = CFM_STRIKEOUT;
		state = CFE_STRIKEOUT;
		break;
	}
	RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	
	if((chf.dwEffects & state) == state)
		//remove format
		set = FALSE;
	else
		//set format
		set = TRUE;

	//set character format
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	switch(fd){
	case FD_BOLD:
		chf.dwMask = CFM_BOLD;
		break;
	case FD_ITALIC:
		chf.dwMask = CFM_ITALIC;
		break;
	case FD_UNDERLINE:
		chf.dwMask = CFM_UNDERLINE;
		break;
	case FD_STRIKETHROUGH:
		chf.dwMask = CFM_STRIKEOUT;
		break;
	}
	if(set)
		//set format, otherwise the field stays 0 - remove format
		chf.dwEffects = state;
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);

	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREAlignment(HWND hNote, int alignment){
	PARAFORMAT2			pfm;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;

	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&pfm, sizeof(pfm));
	pfm.cbSize = sizeof(pfm);
	pfm.dwMask = PFM_ALIGNMENT;
	pfm.wAlignment = alignment;
	RichEdit_SetParaFormat(hEdit, &pfm);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void RemoveAllBulletsNumbering(HWND hNote){
	PARAFORMAT2			pfm;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit = (HWND)GetPropW(hNote, PH_EDIT);

	ZeroMemory(&pfm, sizeof(pfm));
	pfm.cbSize = sizeof(pfm);
	pfm.dwMask = PFM_NUMBERING;
	RichEdit_GetParaFormat(hEdit, &pfm);
	if(pfm.wNumbering != 0){
		ZeroMemory(&pfm, sizeof(pfm));
		pfm.cbSize = sizeof(pfm);
		pfm.dwMask = PFM_NUMBERING | PFM_NUMBERINGSTYLE | PFM_OFFSET | PFM_NUMBERINGSTART | PFM_NUMBERINGTAB;
		RichEdit_SetParaFormat(hEdit, &pfm);
		pNote->pFlags->maskChanged |= F_RTFONT;
		pNote->pAppearance->fFontSet = TRUE;
	}
}

static void CheckBulletsMenuItems(HWND hEdit){
	PARAFORMAT2			pfm;

	CheckMenuItem(g_hEditPopUp, IDM_SIMPLE_BULLETS, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_NUMBERS_PERIOD, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_NUMBERS_PARENS, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_LC_PERIOD, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_LC_PARENS, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_UC_PERIOD, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_UC_PARENS, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_ROMAN_LC, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(g_hEditPopUp, IDM_ROMAN_UC, MF_BYCOMMAND | MF_UNCHECKED);

	ZeroMemory(&pfm, sizeof(pfm));
	pfm.cbSize = sizeof(pfm);
	pfm.dwMask = PFM_NUMBERING | PFM_NUMBERINGSTYLE;
	RichEdit_GetParaFormat(hEdit, &pfm);
	if(pfm.wNumbering == PFN_BULLET){
		CheckMenuItem(g_hEditPopUp, IDM_SIMPLE_BULLETS, MF_BYCOMMAND | MF_CHECKED);
		return;
	}

	if(pfm.wNumbering == PFN_ARABIC){
		if(pfm.wNumberingStyle == PFNS_PERIOD){
			CheckMenuItem(g_hEditPopUp, IDM_NUMBERS_PERIOD, MF_BYCOMMAND | MF_CHECKED);
			return;
		}
		else if(pfm.wNumberingStyle == PFNS_PAREN){
			CheckMenuItem(g_hEditPopUp, IDM_NUMBERS_PARENS, MF_BYCOMMAND | MF_CHECKED);
			return;
		}
	}

	if(pfm.wNumbering == PFN_LCLETTER){
		if(pfm.wNumberingStyle == PFNS_PERIOD){
			CheckMenuItem(g_hEditPopUp, IDM_LC_PERIOD, MF_BYCOMMAND | MF_CHECKED);
			return;
		}
		else if(pfm.wNumberingStyle == PFNS_PAREN){
			CheckMenuItem(g_hEditPopUp, IDM_LC_PARENS, MF_BYCOMMAND | MF_CHECKED);
			return;
		}
	}

	if(pfm.wNumbering == PFN_UCLETTER){
		if(pfm.wNumberingStyle == PFNS_PERIOD){
			CheckMenuItem(g_hEditPopUp, IDM_UC_PERIOD, MF_BYCOMMAND | MF_CHECKED);
			return;
		}
		else if(pfm.wNumberingStyle == PFNS_PAREN){
			CheckMenuItem(g_hEditPopUp, IDM_UC_PARENS, MF_BYCOMMAND | MF_CHECKED);
			return;
		}
	}

	if(pfm.wNumbering == PFN_LCROMAN){
		CheckMenuItem(g_hEditPopUp, IDM_ROMAN_LC, MF_BYCOMMAND | MF_CHECKED);
		return;
	}

	if(pfm.wNumbering == PFN_UCROMAN){
		CheckMenuItem(g_hEditPopUp, IDM_ROMAN_UC, MF_BYCOMMAND | MF_CHECKED);
		return;
	}

}

static void SetREBulletsAndNumbering(HWND hNote, int id){
	MENUITEMINFOW		mi;
	PMITEM				pmi;
	PARAFORMAT2			pfm;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;
	BOOL				isSet;
	int					temp;
	short				wNumbering, wNumberingStyle;

	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		return;
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA;
	GetMenuItemInfoW(g_hBulletsMenu, id, FALSE, &mi);
	pmi = (PMITEM)mi.dwItemData;
	if(pmi){
		//get type and last sign of bullets
		temp = _wtoi(pmi->szReserved);
		wNumbering = HIWORD(temp);
		wNumberingStyle = LOWORD(temp);
		//get current bullets state
		hEdit = (HWND)GetPropW(hNote, PH_EDIT);
		ZeroMemory(&pfm, sizeof(pfm));
		pfm.cbSize = sizeof(pfm);
		pfm.dwMask = PFM_NUMBERING | PFM_NUMBERINGSTYLE;
		RichEdit_GetParaFormat(hEdit, &pfm);
		if(pfm.wNumbering == wNumbering	&& pfm.wNumberingStyle == wNumberingStyle)
			isSet = TRUE;
		else
			isSet = FALSE;
		ZeroMemory(&pfm, sizeof(pfm));
		pfm.cbSize = sizeof(pfm);
		// pfm.dwMask = PFM_NUMBERING | PFM_NUMBERINGSTYLE;
		// if(!isSet){
			// pfm.wNumbering = wNumbering;
			// pfm.wNumberingStyle = wNumberingStyle;
		// }
		if(wNumbering == PFN_BULLET){
			pfm.dwMask = PFM_NUMBERING | PFM_OFFSET | PFM_NUMBERINGTAB;
			if(!isSet){
				pfm.wNumbering = wNumbering;
				pfm.wNumberingTab = GetSmallValue(g_SmallValues, SMS_BULLET_INDENT) * 100;
				pfm.dxOffset = pfm.wNumberingTab;
			}
		}
		else{
			pfm.dwMask = PFM_NUMBERING | PFM_NUMBERINGSTYLE | PFM_OFFSET | PFM_NUMBERINGSTART | PFM_NUMBERINGTAB;
			if(!isSet){
				pfm.wNumbering = wNumbering;
				pfm.wNumberingStyle = wNumberingStyle;
				pfm.wNumberingTab = GetSmallValue(g_SmallValues, SMS_BULLET_INDENT) * 100;
				pfm.dxOffset = pfm.wNumberingTab;
				pfm.wNumberingStart = 1;
			}
		}
		RichEdit_SetParaFormat(hEdit, &pfm);
		pNote->pFlags->maskChanged |= F_RTFONT;
		pNote->pAppearance->fFontSet = TRUE;
	}
}

static void SetREFontColorCommon(HWND hEdit, COLORREF crText){
	CHARFORMAT2W		chf;
	
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_COLOR;
	chf.crTextColor = crText;
	RichEdit_SetCharFormat(hEdit, SCF_ALL, &chf);
}

void SetREFontCommon(HWND hEdit, LPLOGFONTW lf){
	CHARFORMAT2W		chf;
	HDC					hdc;

	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_BOLD | CFM_FACE | CFM_ITALIC | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_CHARSET | CFM_SIZE;
	if(lf->lfWeight >= FW_BOLD)
		chf.dwEffects |= CFE_BOLD;
	if(lf->lfItalic)
		chf.dwEffects |= CFE_ITALIC;
	if(lf->lfStrikeOut)
		chf.dwEffects |= CFE_STRIKEOUT;
	if(lf->lfUnderline)
		chf.dwEffects |= CFE_UNDERLINE;
	hdc = GetDC(hEdit);
	chf.yHeight = 20 * -(lf->lfHeight * 72) / GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(hEdit, hdc);
	chf.bCharSet = lf->lfCharSet;
	chf.bPitchAndFamily = lf->lfPitchAndFamily;
	wcscpy(chf.szFaceName, lf->lfFaceName);
	RichEdit_SetCharFormat(hEdit, SCF_ALL, &chf);
}

void SetNoteMIImageIndex(int id, int index){
	//set menu item index
	SetMIImageIndex(m_NMenus, NELEMS(m_NMenus), id, index);
}

void SetNoteMIText(int id, wchar_t * lpText){
	//set menu item text
	SetMIText(m_NMenus, NELEMS(m_NMenus), id, lpText);
}

void SetEditMIText(int id, wchar_t * lpText){
	//set menu item text
	SetMIText(m_EMenus, NELEMS(m_EMenus), id, lpText);
}

int SetRepeatCount(LPSYSTEMTIME lst){
	return ((lst->wYear * 365 * 24 * 3600 + lst->wMonth * 30 * 24 * 3600 + lst->wDayOfWeek * 7 * 24 * 3600 + lst->wDay * 24 * 3600 + lst->wHour * 3600 + lst->wMinute * 60 + lst->wSecond) * (1000 / NOTE_TIMER_INTERVAL));
}

static void ColorChanged(HWND hNote, int id){
	PMITEM			pmi;
	COLORREF		color;
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA;
	if(m_CurrColorsChoice == CC_TEXT)
		GetMenuItemInfoW(m_hMenuTColor, id, FALSE, &mi);
	else
		GetMenuItemInfoW(m_hMenuTHighlight, id, FALSE, &mi);

	if(id == IDI_FORMAT_COLOR_START || id == IDI_FORMAT_HIGHLIGHT_START){
		if(m_CurrColorsChoice == CC_TEXT)
			SetREFontColor(hNote, -1);
		else if(m_CurrColorsChoice == CC_HIGHLIGHT)
			SetREHighlight(hNote, -1);
	}
	else{
		pmi = (PMITEM)mi.dwItemData;
		if(pmi){
			color = _wtoi(pmi->szText);
			if(m_CurrColorsChoice == CC_TEXT)
				SetREFontColor(hNote, color);
			else if(m_CurrColorsChoice == CC_HIGHLIGHT)
				SetREHighlight(hNote, color);
		}
	}
	m_CurrColorsChoice = NDF;
	SetFocus(hNote);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetTooltip
 Created  : Sun May 27 15:19:09 2007
 Modified : Sun May 27 15:24:02 2007

 Synopsys : Creates or updates note tooltip
 Input    : hOwner - note window handle
            hTooltip - tooltip handle
            pD - pointer to NOTE_DATA
            pH - pointer to NOTE_RTHANDLES
            bUpdate - whether tooltip should be created or updated
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetTooltip(HWND hOwner, HWND hTooltip, P_NOTE_DATA pD, P_NOTE_RTHANDLES pH, LPSRSTATUS pSRStatus, BOOL bUpdate){
	
	wchar_t			szTooltip[256], szTemp[64], szBuffer[256], szDate[128];
	TOOLINFOW		ti;
	HDC				hdc, hdcTemp;
	HFONT			hOldFont;
	RECT			rc = {0, 0, 0, 0};
	//int				style;

	//create some help objects
	hdc = GetDC(hTooltip);
	hdcTemp = CreateCompatibleDC(hdc);
	hOldFont = SelectFont(hdcTemp, g_hMenuFont);
	
	ZeroMemory(&ti, sizeof(ti));
	ti.cbSize = sizeof(ti);
	ti.hwnd = hOwner;
	ti.uFlags = TTF_SUBCLASS;
	wcscpy(szTooltip, pD->szName);
	wcscat(szTooltip, L" - ");
	if(pD->stChanged.wDay != 0){
		//add last saved time if note has been saved
		ConstructDateTimeString(&pD->stChanged, szTemp);
		wcscat(szTooltip, szTemp);
	}
	else{
		//note has not been saved yet
		wcscat(szTooltip, L"Not saved");
	}

	//set tooltip window width
	DrawTextW(hdcTemp, szTooltip, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
	ReleaseDC(hTooltip, hdc);
	SelectFont(hdcTemp, hOldFont);
	DeleteDC(hdcTemp);
	SendMessageW(hTooltip, TTM_SETMAXTIPWIDTH, 0, rc.right - rc.left);

	//check whether note has been received from something
	if(pSRStatus->lastRec.wDay != 0){
		wcscpy(szBuffer, L" ");
		GetPrivateProfileStringW(S_COLUMNS, L"17", L"Received From", szTemp, 64, g_NotePaths.CurrLanguagePath);
		wcscat(szBuffer, szTemp);
		wcscat(szBuffer, L": ");
		wcscat(szBuffer, pSRStatus->recFrom);
		wcscat(szBuffer, L"; ");
		GetPrivateProfileStringW(S_COLUMNS, L"18", L"Received At", szTemp, 64, g_NotePaths.CurrLanguagePath);
		wcscat(szBuffer, szTemp);
		wcscat(szBuffer, L": ");
		ConstructDateTimeString(&pSRStatus->lastRec, szDate);
		wcscat(szBuffer, szDate);
		wcscat(szTooltip, szBuffer);
	}

	//check whether note has been sent to something
	if(pSRStatus->lastSent.wDay != 0){
		wcscpy(szBuffer, L" ");
		GetPrivateProfileStringW(S_COLUMNS, L"15", L"Sent To", szTemp, 64, g_NotePaths.CurrLanguagePath);
		wcscat(szBuffer, szTemp);
		wcscat(szBuffer, L": ");
		wcscat(szBuffer, pSRStatus->sentTo);
		wcscat(szBuffer, L"; ");
		GetPrivateProfileStringW(S_COLUMNS, L"16", L"Sent At", szTemp, 64, g_NotePaths.CurrLanguagePath);
		wcscat(szBuffer, szTemp);
		wcscat(szBuffer, L": ");
		ConstructDateTimeString(&pSRStatus->lastSent, szDate);
		wcscat(szBuffer, szDate);
		wcscat(szTooltip, szBuffer);
	}

	//set tooltip text
	ti.lpszText = szTooltip;	
	//set tooltip rectangle
	CopyRect(&ti.rect, &pH->rcTooltip);

	if(bUpdate){
		//update tooltip
		SendMessageW(hTooltip, TTM_NEWTOOLRECTW, 0, (LPARAM)&ti);
		SendMessageW(hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
	}
	else{
		//add tooltip
		SendMessageW(hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
	}
	
	//style = GetWindowLongPtrW(hTooltip, GWL_EXSTYLE);
	//style |= WS_EX_TOPMOST;
	//SetWindowLongPtrW(hTooltip, GWL_EXSTYLE, style);
}

static void CALLBACK StopAlertTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	PMEMNOTE		pNote = MEMNOTE(hwnd);
	if(pNote->pRTHandles->stopAlarmCounter == 0){
		KillTimer(hwnd, STOP_LOOP_TIMER_ID);
		RemoveBell(hwnd);
	}
	else{
		pNote->pRTHandles->stopAlarmCounter--;
	}
}

static void CALLBACK CloseTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	PMEMNOTE		pNote = MEMNOTE(hwnd);

	if(pNote->pRTHandles->closeCounter == 10){
		KillTimer(hwnd, CLOSE_TIMER_ID);
		HideNote(pNote, false);
		pNote->pRTHandles->closeCounter = 0;
	}
	else{
		pNote->pRTHandles->closeCounter += 2;
		SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255 - 25 * pNote->pRTHandles->closeCounter, LWA_ALPHA);
	}
}

static void CALLBACK PreventCommandTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	KillTimer(NULL, m_PreventCmdTimer);
	m_InDblClick = FALSE;
}

void CALLBACK NoteTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){

	SYSTEMTIME			st, tDate, tStart;
	PMEMNOTE			pNote;
	int					dof;

	GetLocalTime(&st);
	pNote = MemNoteByTimer(idEvent);
	if (!pNote){
		return;
	}
	
	tDate = pNote->pSchedule->scDate;
	tStart = pNote->pSchedule->scStart;
	if(pNote->pSchedule->scType == SCH_ED){
		if((st.wHour < tDate.wHour)
			|| (st.wHour == tDate.wHour && st.wMinute < tDate.wMinute)){
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
		if(tDate.wHour == st.wHour && tDate.wMinute == st.wMinute){
			if(pNote->isAlarm){
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_ONCE){
		if((st.wYear < tDate.wYear) || (st.wYear == tDate.wYear && st.wMonth < tDate.wMonth)
			|| (st.wYear == tDate.wYear && st.wMonth == tDate.wMonth && st.wDay < tDate.wDay)
			|| (st.wYear == tDate.wYear && st.wMonth == tDate.wMonth && st.wDay == tDate.wDay && st.wHour < tDate.wHour)
			|| (st.wYear == tDate.wYear && st.wMonth == tDate.wMonth && st.wDay == tDate.wDay && st.wHour == tDate.wHour && st.wMinute < tDate.wMinute)){
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
		if(tDate.wYear == st.wYear && tDate.wMonth == st.wMonth 
			&& tDate.wDay == st.wDay && tDate.wHour == st.wHour 
			&& tDate.wMinute == st.wMinute){

			if(pNote->isAlarm){
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_REP 
	|| pNote->pSchedule->scType - START_PROG == SCH_REP 
	|| pNote->pSchedule->scType - START_COMP == SCH_REP){
		if(CompareSystemTime(st, tStart) == 0){
			DoAlarm(pNote);
		}
		else if(CompareSystemTime(st, tStart) > 0){
			int diff = DateDiff(st, tStart, SECOND);
			if((diff % pNote->pRTHandles->nRepeat) == 0){
				DoAlarm(pNote);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_PER_WD){
		dof = RealDayOfWeek(st.wDayOfWeek);
		if(!IsBitOn(tDate.wDayOfWeek, dof + 1)){
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
		else{
			if(tDate.wHour == st.wHour && tDate.wMinute == st.wMinute){
				if(pNote->isAlarm){
					//stop checking and alarm
					pNote->isAlarm = FALSE;
					DoAlarm(pNote);
				}
			}
			else{
				//continue checking
				pNote->isAlarm = TRUE;
				return;
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_AFTER 
	|| pNote->pSchedule->scType - START_PROG == SCH_AFTER 
	|| pNote->pSchedule->scType - START_COMP == SCH_AFTER){
		if(pNote->isAlarm){
			if(IsAfterMatch(pNote->pSchedule, &st)){
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
			else{
				//continue checking
				pNote->isAlarm = TRUE;
				return;
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_MONTHLY_EXACT){
		if(tDate.wDay <= 28){
			if(st.wDay == tDate.wDay 
			&& st.wHour == tDate.wHour 
			&& st.wMinute == tDate.wMinute){
				if(pNote->isAlarm){
					pNote->pSchedule->scLastRun.wMonth = st.wMonth;
					//stop checking and alarm
					pNote->isAlarm = FALSE;
					DoAlarm(pNote);
				}
			}
			else{
				//continue checking
				pNote->isAlarm = TRUE;
				return;
			}
		}
		else if(tDate.wDay == 29){
			if(st.wMonth != 2 || IsLeapYear(st.wYear)){
				if(st.wDay == tDate.wDay 
				&& st.wHour == tDate.wHour 
				&& st.wMinute == tDate.wMinute){
					if(pNote->isAlarm){
						pNote->pSchedule->scLastRun.wMonth = st.wMonth;
						//stop checking and alarm
						pNote->isAlarm = FALSE;
						DoAlarm(pNote);
					}
				}
				else{
					//continue checking
					pNote->isAlarm = TRUE;
					return;
				}
			}
			else{
				if(st.wDay == 28 
				&& st.wHour == tDate.wHour 
				&& st.wMinute == tDate.wMinute){
					if(pNote->isAlarm){
						pNote->pSchedule->scLastRun.wMonth = st.wMonth;
						//stop checking and alarm
						pNote->isAlarm = FALSE;
						DoAlarm(pNote);
					}
				}
				else{
					//continue checking
					pNote->isAlarm = TRUE;
					return;
				}
			}
		}
		else if(tDate.wDay == 30){
			if(st.wMonth != 2){
				if(st.wDay == tDate.wDay 
				&& st.wHour == tDate.wHour 
				&& st.wMinute == tDate.wMinute){
					if(pNote->isAlarm){
						pNote->pSchedule->scLastRun.wMonth = st.wMonth;
						//stop checking and alarm
						pNote->isAlarm = FALSE;
						DoAlarm(pNote);
					}
				}
				else{
					//continue checking
					pNote->isAlarm = TRUE;
					return;
				}
			}
			else{
				if((IsLeapYear(st.wYear) && st.wDay == 29) || (!IsLeapYear(st.wYear) && st.wDay == 28)){
					if(st.wHour == tDate.wHour 
					&& st.wMinute == tDate.wMinute){
						//stop checking and alarm
						pNote->isAlarm = FALSE;
						DoAlarm(pNote);
					}
					else{
						//continue checking
						pNote->isAlarm = TRUE;
						return;
					}
				}
				else{
					//continue checking
					pNote->isAlarm = TRUE;
					return;
				}
			}
		}
		else if(tDate.wDay == 31){
			switch(st.wMonth){
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					if(st.wDay == tDate.wDay 
					&& st.wHour == tDate.wHour 
					&& st.wMinute == tDate.wMinute){
						if(pNote->isAlarm){
							pNote->pSchedule->scLastRun.wMonth = st.wMonth;
							//stop checking and alarm
							pNote->isAlarm = FALSE;
							DoAlarm(pNote);
						}
					}
					else{
						//continue checking
						pNote->isAlarm = TRUE;
						return;
					}
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					if(st.wDay == 30 
					&& st.wHour == tDate.wHour 
					&& st.wMinute == tDate.wMinute){
						if(pNote->isAlarm){
							pNote->pSchedule->scLastRun.wMonth = st.wMonth;
							//stop checking and alarm
							pNote->isAlarm = FALSE;
							DoAlarm(pNote);
						}
					}
					else{
						//continue checking
						pNote->isAlarm = TRUE;
						return;
					}
					break;
				case 2:
					if((IsLeapYear(st.wYear) && st.wDay == 29) || (!IsLeapYear(st.wYear) && st.wDay == 28)){
						if(st.wHour == tDate.wHour 
						&& st.wMinute == tDate.wMinute){
							//stop checking and alarm
							pNote->isAlarm = FALSE;
							DoAlarm(pNote);
						}
						else{
							//continue checking
							pNote->isAlarm = TRUE;
							return;
						}
					}
					else{
						//continue checking
						pNote->isAlarm = TRUE;
						return;
					}
					break;
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_MONTHLY_RELATIVE){
		if(pNote->isAlarm){
			dof = RealDayOfWeek(st.wDayOfWeek);
			ordinal_days	ordinal = DayOfWeekOrdinal(st);

			if(pNote->pSchedule->scLastRun.wMonth != st.wMonth 
			&& OrdinalDayOfWeek(tDate.wDayOfWeek) == dof 
			&& tDate.wMilliseconds == ordinal
			&& st.wHour == tDate.wHour 
			&& st.wMinute == tDate.wMinute){
				pNote->pSchedule->scLastRun.wMonth = st.wMonth;
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
			else{
				//continue checking
				pNote->isAlarm = TRUE;
				return;
			}
		}
		else{
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
	}
}

static BOOL IsAfterMatch(P_SCHEDULE_TYPE pS, LPSYSTEMTIME lpst){
	TIMEUNION 				ft1;
	TIMEUNION 				ft2;
	unsigned long long		res1, res2;

	//return false if starting date is in the future
	if(lpst->wYear < pS->scStart.wYear)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth < pS->scStart.wMonth)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay < pS->scStart.wDay)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay == pS->scStart.wDay && lpst->wHour < pS->scStart.wHour)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay == pS->scStart.wDay && lpst->wHour == pS->scStart.wHour && lpst->wMinute < pS->scStart.wMinute)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay == pS->scStart.wDay && lpst->wHour == pS->scStart.wHour && lpst->wMinute == pS->scStart.wMinute && lpst->wSecond < pS->scStart.wSecond)
		return FALSE;
	SystemTimeToFileTime(lpst, &ft1.fileTime);
	SystemTimeToFileTime(&pS->scStart, &ft2.fileTime);
	res1 = (ft1.ul.QuadPart - ft2.ul.QuadPart) / 10000000;
	res2 = pS->scDate.wSecond;
	res2 += pS->scDate.wMinute * 60;
	res2 += pS->scDate.wHour * 60 * 60;
	res2 += pS->scDate.wDay * 60 * 60 * 24;
	res2 += pS->scDate.wDayOfWeek * 60 * 60 * 24 * 7;
	for(int i = 0, j = pS->scStart.wMonth, y = pS->scStart.wYear; i < pS->scDate.wMonth; i++){
		switch(j){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			res2 += 31 * 60 * 60 * 24;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			res2 += 30 * 60 * 60 * 24;
			break;
		case 2:
			if(IsLeapYear(y))
				res2 += 29 * 60 * 60 * 24;
			else
				res2 += 28 * 60 * 60 * 24;
			break;
		}
		if(j < 12)
			j++;
		else{
			j = 1;
			y++;
		}
	}
	for(int i = 0, j = pS->scStart.wYear; i < pS->scDate.wYear; i++, j++){
		if(IsLeapYear(j) || IsLeapYear(j + 1))
			res2 += 366 * 60 * 60 * 24;
		else
			res2 += 365 * 60 * 60 * 24;
	}
	if(res1 == res2)
		return TRUE;
	return FALSE;
}

static void ShiftNotes(int pos){
	PDOCKITEM		pItem;
	PDOCKHEADER 	pHeader = NULL;
	int				offset = 0, dockType = 0;

	switch(pos){
		case DA_LEFT_UP:
			pHeader = DHeader(DOCK_LEFT);
			dockType = DOCK_LEFT;
			offset = 1;
			break;
		case DA_LEFT_DOWN:
			pHeader = DHeader(DOCK_LEFT);
			dockType = DOCK_LEFT;
			offset = -1;
			break;
		case DA_TOP_LEFT:
			pHeader = DHeader(DOCK_TOP);
			dockType = DOCK_TOP;
			offset = 1;
			break;
		case DA_TOP_RIGHT:
			pHeader = DHeader(DOCK_TOP);
			dockType = DOCK_TOP;
			offset = -1;
			break;
		case DA_RIGHT_UP:
			pHeader = DHeader(DOCK_RIGHT);
			dockType = DOCK_RIGHT;
			offset = 1;
			break;
		case DA_RIGHT_DOWN:
			pHeader = DHeader(DOCK_RIGHT);
			dockType = DOCK_RIGHT;
			offset = -1;
			break;
		case DA_BOTTOM_LEFT:
			pHeader = DHeader(DOCK_BOTTOM);
			dockType = DOCK_BOTTOM;
			offset = 1;
			break;
		case DA_BOTTOM_RIGHT:
			pHeader = DHeader(DOCK_BOTTOM);
			dockType = DOCK_BOTTOM;
			offset = -1;
			break;
	}
	pItem = pHeader->pNext;
	while(pItem){
		pItem->index += offset;
		MoveDockWindow(pItem->hwnd, dockType, pItem->index);
		pItem = pItem->pNext;
	}
}

static int CheckDockPositionForAlarm(HWND hwnd){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	int					result;

	result = ArrowRequired(hwnd, DockType(pNote->pData->dockData), pNote->pData->dockData);
	if(result == -1){
		return 0;
	}
	else{
		ShiftNotes(result);
		return 1;
	}
}

static void SpeakNote(PMEMNOTE pNote, void * pToken){
	HWND				hEdit;
	GETTEXTLENGTHEX		gtl = {0};
	GETTEXTEX			gt = {0};
	wchar_t 			*pText;
	long				length = 0;

	if(pNote->pData->visible){
		hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
	}
	else{
		hEdit = g_hTEditContent;
		ReadSimpleRTFFile(hEdit, pNote->pFlags->id);
	}
	gtl.flags = GTL_DEFAULT | GTL_PRECISE;
	gtl.codepage = 1200;
	length = SendMessageW(hEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0) + 1;
	
	gt.cb = length + 1;
	gt.flags = GT_DEFAULT;
	gt.codepage = 1200;

	pText = calloc(length + 1, sizeof(wchar_t));
	if(pText){
		SendMessageW(hEdit, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)pText);
		Speak(pNote->pVoice, pText, pToken, g_PVoices);
		free(pText);
	}
	if(!pNote->pData->visible){
		SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)L"");
	}
}

static void * GetNoteVoiceToken(P_SCHEDULE_TYPE pS){
	for(int i = 0; i < g_VoicesCount; i++){
		if(wcscmp(pS->szSound, g_PVoices[i].name) == 0){
			return (void *)g_PVoices[i].token;
		}
	}
	return NULL;
}

static void DoAlarm(PMEMNOTE pNote){
	wchar_t				szPath[MAX_PATH];
	RECT				rc;
	HWND				hBell;
	HWND 				hNote = NULL;
	SYSTEMTIME			st;
	BOOL				loopSound = FALSE, useText = false;

	if(DockType(pNote->pData->dockData) == DOCK_NONE){
		if(IsBitOn(g_NoteSettings.reserved1, SB1_DONOT_CENTER))
			CenterRestorePlacementNote(pNote, IDM_SHOW_CENTER, TRUE);
		else
			CenterRestorePlacementNote(pNote, IDM_SHOW_CENTER, FALSE);
	}
	else{
		if(pNote->pData->visible){
			while(CheckDockPositionForAlarm(hNote) == 1){
				;
			}
		}
	}
	hNote = pNote->hwnd;
	if(g_NoteSettings.visualAlert){
		hBell = (HWND)GetPropW(hNote, PH_BELL);
		if(hBell && IsWindow(hBell)){
			DestroyWindow(hBell);
		}
		CopyRect(&rc, &pNote->pRTHandles->rcTooltip);
		MapWindowPoints(hNote, HWND_DESKTOP, (LPPOINT)&rc, 2);
		if(!pNote->pRTHandles->hbSkin)
			OffsetRect(&rc, -16, -16);
		SetPropW(hNote, PH_BELL, (HANDLE)CreateBell(hNote, g_hInstance, rc.left, rc.top, pNote->pData->onTop));
	}

	if(g_Sound.allowSound){
		if(!IsBitOn(pNote->pSchedule->params, SP_USE_TTS)){
			if(wcscmp(pNote->pSchedule->szSound, DS_DEF_SOUND) == 0 || *pNote->pSchedule->szSound == '\0'){
				if(IsBitOn(pNote->pSchedule->params, SP_SOUND_IN_LOOP)){
					loopSound = TRUE;
					PlaySoundW(L"Notify", 0, SND_ALIAS | SND_ASYNC | SND_LOOP);
				}
				else
					PlaySoundW(L"Notify", 0, SND_ALIAS | SND_ASYNC);
			}
			else{
				wcscpy(szPath, g_NotePaths.SoundDir);
				wcscat(szPath, pNote->pSchedule->szSound);
				if(IsBitOn(pNote->pSchedule->params, SP_SOUND_IN_LOOP)){
					loopSound = TRUE;
					PlaySoundW(szPath, 0, SND_FILENAME | SND_ASYNC | SND_LOOP);
				}
				else
					PlaySoundW(szPath, 0, SND_FILENAME | SND_ASYNC);
			}
		}
		else{
			useText = true;
			if(IsBitOn(pNote->pSchedule->params, SP_SOUND_IN_LOOP)){
				SetVoiceNotification(pNote->pVoice, pNote->hwnd);
				loopSound = TRUE;
			}
			SpeakNote(pNote, GetNoteVoiceToken(pNote->pSchedule));
		}
	}

	if(pNote->pSchedule->stopLoop > 0){
		pNote->pRTHandles->stopAlarmCounter = pNote->pSchedule->stopLoop;
		SetTimer(hNote, STOP_LOOP_TIMER_ID, 1000, StopAlertTimerProc);
	}

	if(pNote->pSchedule->scType == SCH_ONCE 
	|| pNote->pSchedule->scType == SCH_AFTER){
		//stop timer and remove scheduling because alarm should fire only once
		wchar_t		szTemp[128];
		wcscpy(szTemp, pNote->pSchedule->szSound);
		ZeroMemory(pNote->pSchedule, sizeof(SCHEDULE_TYPE));
		wcscpy(pNote->pSchedule->szSound, szTemp);
		KillTimer(NULL, pNote->pRTHandles->idTimer);
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
		RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(hNote);
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
	else{
		//save last alarm time
		GetLocalTime(&st);
		// memcpy(&pNote->pSchedule->scLastRun, &st, sizeof(SYSTEMTIME));
		pNote->pSchedule->scLastRun = st;
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
	}
	BitOn(&pNote->pSchedule->params, SP_IN_ALARM);
	if(loopSound)
		BitOn(&pNote->pSchedule->params, SP_SOUND_IN_LOOP);
	if(useText)
		BitOn(&pNote->pSchedule->params, SP_USE_TTS);
}

static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData){
	if((DWORD)(VOID *)hData != 0xffffffff){
		RemovePropW(hwnd, lpszString);
	}
	return TRUE;
}

//static void FormatNewNoteName(wchar_t * lpID){
	
	//SYSTEMTIME		st;

	//GetLocalTime(&st);
	//ConstructDateTimeString(&st, lpID, TRUE);
//}

void FormatNewNoteID(wchar_t * lpID){
	SYSTEMTIME		st;
	wchar_t			buff[8];

	GetLocalTime(&st);
	_itow(st.wYear, buff, 10);
	wcscpy(lpID, buff);
	_itow(st.wMonth, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wDay, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wHour, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wMinute, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wSecond, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wMilliseconds, buff, 10);
	wcscat(lpID, buff);
}

static void ManageBackupFiles(wchar_t * lpNoteID, wchar_t * lpNoteFile){
	wchar_t				filespec[MAX_PATH], szPathOld[MAX_PATH], szTemp[MAX_PATH], szIndex[12], szPathNew[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	int					count = GetSmallValue(g_SmallValues, SMS_BACKUP_COUNT);

	handle = FindFirstFileW(lpNoteFile, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return;
	FindClose(handle);
	wcscpy(filespec, g_NotePaths.BackupDir);
	wcscat(filespec, lpNoteID);
	wcscpy(szTemp, filespec);
	if(!NotesDirExists(g_NotePaths.BackupDir))
		NotesDirCreate(g_NotePaths.BackupDir);

	wcscat(szTemp, L"_*");
	wcscat(szTemp, BACK_NOTE_EXTENTION);

	handle = FindFirstFileW(szTemp, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		FindClose(handle);
		for(int i = count; i > 0; i--){
			wcscpy(szPathOld, filespec);
			wcscpy(szPathNew, filespec);
			_itow(i, szIndex, 10);
			wcscat(szPathOld, L"_");
			wcscat(szPathOld, szIndex);
			wcscat(szPathOld, BACK_NOTE_EXTENTION);
			handle = FindFirstFileW(szPathOld, &fd);
			if(handle != INVALID_HANDLE_VALUE){
				FindClose(handle);
				if(i >= count){
					DeleteFileW(szPathOld);
				}
				else{
					_itow(i + 1, szIndex, 10);
					wcscat(szPathNew, L"_");
					wcscat(szPathNew, szIndex);
					wcscat(szPathNew, BACK_NOTE_EXTENTION);
					MoveFileExW(szPathOld, szPathNew, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
				}
			}
		}
	}
	wcscat(filespec, L"_1");
	wcscat(filespec, BACK_NOTE_EXTENTION);
	CopyFileW(lpNoteFile, filespec, FALSE);
	//update control panel
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, PNM_CTRL_BACK_UPDATE, 0, (LPARAM)lpNoteID);
}

void AutosaveNote(PMEMNOTE pNote){
	HWND				hwnd = pNote->hwnd;
	wchar_t				szPath[MAX_PATH], szDBFile[MAX_PATH], szTempPath[MAX_PATH + 128], szHash[256];
	HANDLE				hFile;
	EDITSTREAM			esm;
	RECT				rcNote;
	NOTE_REL_POSITION	nrp;
	SIZE				sz;

	if(!NotesDirExists(g_NotePaths.DataDir))
		NotesDirCreate(g_NotePaths.DataDir);
	wcscpy(szDBFile, g_NotePaths.DataDir);
	wcscat(szDBFile, AUTOSAVE_INDEX_FILE);
	if(!PathFileExistsW(szDBFile)){
		//create file with "Hidden" attribute
		hFile = CreateFileW(szDBFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return;
		CloseHandle(hFile);
	}
	
	if(pNote->pData->visible){
		if(!pNote->pFlags->fromDB){
			GetTemporarySaveName(hwnd, pNote->pData->szName, GetSmallValue(g_SmallValues, SMS_DEF_SAVE_CHARACTERS));
		}
		//save window position and size
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			GetWindowRect(hwnd, &rcNote);
			//check rectangle metrics and repair them if needed
			CheckSavedRectangle(&rcNote);
			//save current absolute position
			if(pNote->pRTHandles->rolled){
				rcNote.bottom = rcNote.top + (pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_FIT_TO_CAPTION)){
					rcNote.right = rcNote.left + (pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left);
				}
			}
			CopyRect(&pNote->pData->rcp, &rcNote);
			//save rolled status
			pNote->pData->rolled = pNote->pRTHandles->rolled;
			//save current relational position
			sz = GetScreenMetrics();
			nrp.left = (double)rcNote.left / (double)sz.cx;
			nrp.top = (double)rcNote.top / (double)sz.cy;
			nrp.width = rcNote.right - rcNote.left;
			nrp.height = rcNote.bottom - rcNote.top;
			WritePrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), szDBFile);
		}
		//save note
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, L"~");
		wcscat(szPath, pNote->pFlags->id);
		wcscat(szPath, L".no~");
		
		if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
			GetNoteTempFileName(szTempPath);
			hFile = CreateFileW(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else{
			hFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}

		if(hFile != INVALID_HANDLE_VALUE){
			esm.dwCookie = (DWORD)hFile;
			esm.dwError = 0;
			esm.pfnCallback = OutStreamCallback;
			RichEdit_StreamOut((HWND)GetPropW(hwnd, PH_EDIT), SF_RTF | SF_UNICODE, &esm);
			CloseHandle(hFile);
			if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
				GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szHash, 256, g_NotePaths.INIFile);
				if(CryptEncryptFile(szTempPath, szPath, szHash)){
					DeleteFileW(szTempPath);
				}
			}
		}
	}
	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN 
	|| (pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT 
	|| (pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR
	|| (pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT
	|| (pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR){
		if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
			pNote->pAppearance->nPrivate |= F_SKIN;
		if((pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT)
			pNote->pAppearance->nPrivate |= F_RTFONT;
		if((pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR)
			pNote->pAppearance->nPrivate |= F_C_COLOR;
		if((pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT)
			pNote->pAppearance->nPrivate |= F_C_FONT;
		if((pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR)
			pNote->pAppearance->nPrivate |= F_B_COLOR;
		WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, pNote->pAppearance, sizeof(NOTE_APPEARANCE), szDBFile);
	}
	if((pNote->pFlags->maskChanged & F_SCHEDULE) == F_SCHEDULE)
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), szDBFile);
	GetLocalTime(&pNote->pData->stChanged);
	WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, pNote->pData, sizeof(NOTE_DATA), szDBFile);
	//save additional appearance
	WritePrivateProfileStructW(pNote->pFlags->id, S_ADD_APPEARANE, pNote->pAddNApp, sizeof(ADDITIONAL_NAPP), szDBFile);
}

void SaveNoteAs(PMEMNOTE pNote, int nAsOrRename){
	// wchar_t			szBuffer[256];
	int				result = 0, oldGroup;
	// BOOL			in_favorites = FALSE;
	N2DLG			n2d;
	NOTE_DATA		data;

	// //check whether note was in favorites
	// GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	// if(wcslen(szBuffer) > 0){
		// //in such case delete its name from favorites
		// in_favorites = TRUE;
	// }
	oldGroup = pNote->pData->idGroup;
	n2d.reserved = nAsOrRename;
	n2d.pNote = pNote;
	if(pNote->pData->visible)
		result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SAVE_AS), pNote->hwnd, SaveAs_DlgProc, (LPARAM)&n2d);
	else
		result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SAVE_AS), g_hMain, SaveAs_DlgProc, (LPARAM)&n2d);
	if(result == IDOK){
		if(!SaveNote(pNote)){
			if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
				wcscpy(data.szName, pNote->pData->szName);
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			}
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
		}
		if(g_hCPDialog && (oldGroup != pNote->pData->idGroup))
			SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, oldGroup, pNote->pData->idGroup);
		if(pNote->pRTHandles->favorite == GROUP_FAVORITES){
			//if note was previously in favorites - remove it and save in favorites with new name
			WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, g_NotePaths.INIFile);
			WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, pNote->pData->szName, g_NotePaths.INIFile);
		}
	}
	//make note opaque
	if(pNote->pData->visible){
		if(g_NoteSettings.transAllow){
			if(GetFocus() == (HWND)GetPropW(pNote->hwnd, PH_EDIT)){
				SetLayeredWindowAttributes(pNote->hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
		}
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
	}
}

BOOL SaveNote(PMEMNOTE pNote){
	
	wchar_t				szPath[MAX_PATH], szTempPath[MAX_PATH + 128], szHash[256];
	HWND				hwnd;
	HANDLE				hFile;
	EDITSTREAM			esm;

	//remove duplicated flag
	BitOff(&pNote->pData->res1, NB_DUPLICATED);
	if(!pNote->pFlags->maskChanged && pNote->pFlags->saved){
		return FALSE;
	}
	hwnd = pNote->hwnd;
	if(pNote->pData->visible){
		//save window position and size
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			WriteNotePlacement(hwnd, TRUE);
		}
		//save text
		if(((pNote->pFlags->maskChanged & F_TEXT) == F_TEXT) || (!pNote->pFlags->fromDB && !pNote->pFlags->saved)){
			if(!NotesDirExists(g_NotePaths.DataDir))
				NotesDirCreate(g_NotePaths.DataDir);
			//prepare note file path
			wcscpy(szPath, g_NotePaths.DataDir);
			wcscat(szPath, pNote->pFlags->id);
			wcscat(szPath, NOTE_EXTENTION);
			//check whether backup should be done
			if(IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP)){
				if(wcslen(pNote->pFlags->idBackup) == 0)
					ManageBackupFiles(pNote->pFlags->id, szPath);
				else{
					//in this case flip contents of two files
					wchar_t		szBackup[MAX_PATH];
					wcscpy(szBackup, g_NotePaths.BackupDir);
					wcscat(szBackup, pNote->pFlags->idBackup);
					CopyFileW(szPath, szBackup, FALSE);
				}
			}
			if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
				GetNoteTempFileName(szTempPath);
				hFile = CreateFileW(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			else{
				hFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			if(hFile != INVALID_HANDLE_VALUE){
				esm.dwCookie = (DWORD)hFile;
				esm.dwError = 0;
				esm.pfnCallback = OutStreamCallback;
				RichEdit_StreamOut((HWND)GetPropW(hwnd, PH_EDIT), SF_RTF | SF_UNICODE, &esm);
				CloseHandle(hFile);
				DeleteAutoSavedFile(pNote->pFlags->id);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
					GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szHash, 256, g_NotePaths.INIFile);
					if(CryptEncryptFile(szTempPath, szPath, szHash)){
						DeleteFileW(szTempPath);
					}
				}
			}
		}
	}
	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN 
	|| (pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT 
	|| (pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR
	|| (pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT
	|| (pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR){
		if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
			pNote->pAppearance->nPrivate |= F_SKIN;
		if((pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT)
			pNote->pAppearance->nPrivate |= F_RTFONT;
		if((pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR)
			pNote->pAppearance->nPrivate |= F_C_COLOR;
		if((pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT)
			pNote->pAppearance->nPrivate |= F_C_FONT;
		if((pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR)
			pNote->pAppearance->nPrivate |= F_B_COLOR;
		WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, pNote->pAppearance, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	}
	if((pNote->pFlags->maskChanged & F_SCHEDULE) == F_SCHEDULE)
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);

	GetLocalTime(&pNote->pData->stChanged);
	WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);

	//save additional appearance
	WritePrivateProfileStructW(pNote->pFlags->id, S_ADD_APPEARANE, pNote->pAddNApp, sizeof(ADDITIONAL_NAPP), g_NotePaths.DataFile);

	//save styles
	WritePrivateProfileStructW(pNote->pFlags->id, S_NOTE_CREATION, pNote->pCreated, sizeof(SYSTEMTIME), g_NotePaths.DataFile);

	//save tags
	SaveTags(pNote);

	pNote->pFlags->saved = TRUE;
	pNote->pFlags->fromDB = TRUE;
	pNote->pFlags->idBackup[0] = '\0';

	if(pNote->pData->visible){
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(hwnd);
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, pNote->pSRStatus, TRUE);
	}
	
	if(g_hCPDialog){
		SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	}
	pNote->pFlags->maskChanged = 0;
	return TRUE;
}

BOOL CALLBACK SaveAs_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, SaveAs_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, SaveAs_OnCommand);
	HANDLE_MSG (hwnd, WM_CLOSE, SaveAs_OnClose);
	HANDLE_MSG (hwnd, WM_DESTROY, SaveAs_OnDestroy);

	default: return FALSE;
	}
}

static void SaveAs_OnDestroy(HWND hwnd)
{
	DisableInput(FALSE);
}

static BOOL SaveAs_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	PMEMNOTE		pNote;
	wchar_t 		szBuffer[256];
	P_N2DLG			pn2d;
	int				count;

	g_hLastModal = hwnd;
	DisableInput(TRUE);
	//add groups to combo
	AddGroupsToCombo(GetDlgItem(hwnd, IDC_CBO_SAVE_IN_GROUP));
	pn2d = (P_N2DLG)lParam;
	pNote = pn2d->pNote;
	//set dialog and controls text
	if(pn2d->reserved == 0)
		GetPrivateProfileStringW(S_OPTIONS, L"1002", L"Save note as...", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	else
		GetPrivateProfileStringW(S_CAPTIONS, IK_RENAME_CAPTION, L"Rename Note", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	count = SendDlgItemMessageW(hwnd, IDC_CBO_SAVE_IN_GROUP, CB_GETCOUNT, 0, 0);
	for(int i = 0; i < count; i++){
		if(SendDlgItemMessageW(hwnd, IDC_CBO_SAVE_IN_GROUP, CB_GETITEMDATA, i, 0) == pNote->pData->idGroup){
			SendDlgItemMessageW(hwnd, IDC_CBO_SAVE_IN_GROUP, CB_SETCURSEL, i, 0);
			break;
		}
	}
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_SAVE_AS, g_NotePaths.CurrLanguagePath, L"Note name");
	SetDlgCtlText(hwnd, IDC_ST_SAVE_IN_GROUP, g_NotePaths.CurrLanguagePath, L"Save in group:");
	SendDlgItemMessageW(hwnd, IDC_EDT_SAVE_AS, EM_SETLIMITTEXT, 127, 0);
	//save memory note handle
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pNote);
	if(pNote->pFlags->fromDB || pNote->pFlags->saved || IsBitOn(pNote->pData->res1, NB_DUPLICATED))
		wcscpy(szBuffer, pNote->pData->szName);
	else{
		int		count = GetTemporarySaveName(pNote->hwnd, szBuffer, GetSmallValue(g_SmallValues, SMS_DEF_SAVE_CHARACTERS));
		if(count == 0)
			wcscpy(szBuffer, pNote->pData->szName);
		else if(IsEmptyString(szBuffer))
			wcscpy(szBuffer, pNote->pData->szName);
	}
	//trim note name
	_wcstrm(szBuffer);
	SetDlgItemTextW(hwnd, IDC_EDT_SAVE_AS, szBuffer);
	return FALSE;
}

static void SaveAs_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			PMEMNOTE	pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			GetDlgItemTextW(hwnd, IDC_EDT_SAVE_AS, pNote->pData->szName, 128);
			int		index = SendDlgItemMessageW(hwnd, IDC_CBO_SAVE_IN_GROUP, CB_GETCURSEL, 0, 0);
			if(index != CB_ERR){
				int		iNew = SendDlgItemMessageW(hwnd, IDC_CBO_SAVE_IN_GROUP, CB_GETITEMDATA, index, 0);
				if(iNew != pNote->pData->idGroup){
					pNote->pData->idGroup = iNew;
					if(!pNote->pData->visible){
						pNote->pFlags->saved = false;
					}
					ChangeNoteLookByGroup(pNote, iNew);
				}
			}
			if(pNote->pData->visible){
				SetNoteCaption(pNote->hwnd);
			}
			EndDialog(hwnd, IDOK);
			break;
		}
		case IDC_EDT_SAVE_AS:
			if(codeNotify == EN_CHANGE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SAVE_AS)))
					EnableWindow(GetDlgItem(hwnd ,IDOK), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd ,IDOK), FALSE);
			}
			break;
	}
}

static void SaveAs_OnClose(HWND hwnd)
{
	EndDialog(hwnd, IDCANCEL);
}

static void RestoreFromBackup(PMEMNOTE pNote){
	wchar_t				szNoBackup[256], filespec[MAX_PATH];
	wchar_t				szFilter[256], szBuffer[256], szTitle[128];
	HANDLE				handle = INVALID_HANDLE_VALUE ;
	WIN32_FIND_DATAW	fd;

	wcscpy(filespec, g_NotePaths.BackupDir);
	wcscat(filespec, pNote->pFlags->id);
	wcscat(filespec, L"_*");
	wcscat(filespec, BACK_NOTE_EXTENTION);
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE){
		GetPrivateProfileStringW(S_MESSAGES, L"no_backup", L"There are no backup copies for current note", szNoBackup, 256, g_NotePaths.CurrLanguagePath);
		MessageBoxW(g_hMain, szNoBackup, PROG_NAME, MB_OK);
	}
	else{
		FindClose(handle);
		GetPrivateProfileStringW(S_CAPTIONS, IK_REST_FILTER, L"Backup copies of notes", szFilter, 256, g_NotePaths.CurrLanguagePath);
		wcscat(szFilter, L" (");
		wcscat(szFilter, pNote->pFlags->id);
		wcscat(szFilter, L"*.pnback)@");
		wcscat(szFilter, pNote->pFlags->id);
		wcscat(szFilter, L"*.pnback@");
		//replace '@' character with '\0'
		wchar_t		*pTemp = szFilter;
		while(*pTemp){
			if(*pTemp == '@')
				*pTemp = '\0';
			pTemp++;
		}
		GetPrivateProfileStringW(S_CAPTIONS, IK_REST_CAPTION, L"Restore note", szBuffer, 256, g_NotePaths.CurrLanguagePath);
		wcscat(szBuffer, L" (");
		wcscat(szBuffer, pNote->pData->szName);
		wcscat(szBuffer, L")");
		if(ShowOpenFileDlg(pNote->hwnd, filespec, szTitle, szFilter, szBuffer, g_NotePaths.BackupDir)){
			ReadRestoreRTFFile((HWND)GetPropW(pNote->hwnd, PH_EDIT), filespec);
			pNote->pFlags->saved = FALSE;
			wcscpy(pNote->pFlags->idBackup, szTitle);
		}
	}
}

static LRESULT CALLBACK Note_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	PMEMNOTE			pNote;

	switch(msg){
		HANDLE_MSG (hwnd, WM_CREATE, Note_OnCreate);
		HANDLE_MSG (hwnd, WM_PAINT, Note_OnPaint);
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, Note_OnMouseMove);
		HANDLE_MSG (hwnd, WM_CLOSE, Note_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Note_OnCommand);
		HANDLE_MSG (hwnd, WM_SETFOCUS, Note_OnSetFocus);
		HANDLE_MSG (hwnd, WM_LBUTTONDOWN, Note_OnLButtonDown);
		HANDLE_MSG (hwnd, WM_RBUTTONUP, Note_OnRButtonUp);
		HANDLE_MSG (hwnd, WM_KILLFOCUS, Note_OnKillFocus);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Note_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, Note_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_SIZE, Note_OnSize);
		HANDLE_MSG (hwnd, WM_DROPFILES, Note_OnDropFiles);
		HANDLE_MSG (hwnd, WM_WINDOWPOSCHANGING, Note_OnWindowPosChanging);
		HANDLE_MSG (hwnd, WM_RBUTTONDOWN, Note_OnRButtonDown);
		HANDLE_MSG (hwnd, WM_LBUTTONDBLCLK, Note_OnLButtonDblClk);
		HANDLE_MSG (hwnd, WM_MOUSELEAVE, Note_OnMouseLeave);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Note_OnInitMenuPopup);

		case WM_SYSCOMMAND:
			//hide note window instead of closing when Alt+F4 is pressed
			if(wParam == SC_CLOSE){
			// if((wParam & SC_CLOSE) == SC_CLOSE){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIDE, 0), 0);
				return FALSE;
			}
			return DefWindowProcW (hwnd, msg, wParam, lParam);
		case PNM_SOUND_ENDS:
			pNote = MEMNOTE(hwnd);
			SpeakNote(pNote, GetNoteVoiceToken(pNote->pSchedule));
			return false;
		case PNM_RESIZE:
			OnResizeNote(hwnd, (LPRECT)wParam);
			return FALSE;
		case PNM_SAVE:
			SaveNote(MEMNOTE(hwnd));
			return FALSE;
		case PNM_SAVE_ON_SHUTDOWN:{
			wchar_t szBuffer[128];
			pNote = MEMNOTE(hwnd);
			if(!IsBitOn(pNote->pData->res1, NB_DUPLICATED)){
				if(GetTemporarySaveName(hwnd, szBuffer, GetSmallValue(g_SmallValues, SMS_DEF_SAVE_CHARACTERS)) != 0)
					wcscpy(pNote->pData->szName, szBuffer);
			}
			SaveNote(MEMNOTE(hwnd));
			return FALSE;
		}
		//case PNM_AUTOSAVE:
			//AutosaveNote(hwnd);
			//return FALSE;
		case PNM_TT_UPDATE:{
			P_NOTE_RTHANDLES	pH;
			P_NOTE_DATA			pD;
			LPSRSTATUS			pS;

			if(g_RTHandles.hbSkin){
				if(g_DockSettings.fCustSkin){
					pH = &g_DRTHandles;
				}
				else{
					pH = NoteRTHandles(hwnd);
				}
			}
			else{
				pH = NoteRTHandles(hwnd);
			}
			pD = NoteData(hwnd);
			pS = NoteSRStatus(hwnd);
			SetTooltip(hwnd, pH->hTooltip, pD, pH, pS, TRUE);
			return FALSE;
		}
		case PNM_CHANGE_FONT:
			SetREFontCommon((HWND)GetPropW(hwnd, PH_EDIT), (LPLOGFONTW)wParam);
			return FALSE;
		case PNM_CHANGE_FONT_COLOR:
			SetREFontColorCommon((HWND)GetPropW(hwnd, PH_EDIT), (COLORREF)wParam);
			return FALSE;
		case PNM_NEW_LANGUAGE:
			SetCommandsTooltips(hwnd);
			return FALSE;
		case PNM_MOVE_DOCK_NOTE:{
			P_NOTE_DATA pD = NoteData(hwnd);
			if(DockType(pD->dockData) != DOCK_NONE){
				HWND hTray = MoveDockWindow(hwnd, DockType(pD->dockData), DockIndex(pD->dockData));
				if(hTray)
					SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			}
			return FALSE;
		}
		case PNM_NEW_BGCOLOR:
			pNote = MEMNOTE(hwnd);
			RichEdit_SetBkgndColor(GetPropW(hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			CreateSimpleMarksBitmap(hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);	
			ShowNoteMarks(hwnd);
			return FALSE;
		case PNM_NEW_CAPTION_COLOR:
			pNote = MEMNOTE(hwnd);
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			CreateSimpleMarksBitmap(hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);	
			ShowNoteMarks(hwnd);
			return FALSE;
		case PNM_APPLY_NEW_SKIN:
			ApplyNewSkin(hwnd, (P_NOTE_RTHANDLES)wParam);
			return FALSE;
		case WM_NOTIFY:{
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if(pnmh->code == EN_MSGFILTER){
				MSGFILTER * lpmf = (MSGFILTER *)lParam;
				if(lpmf->msg == WM_CHAR || lpmf->msg == WM_KEYDOWN){
					pNote = MEMNOTE(hwnd);
					if(pNote->pFlags->transparent){
						//draw window opaque while typing
						SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
						pNote->pFlags->transparent = FALSE;
					}
					if(lpmf->msg == WM_KEYDOWN){ 
 						//if((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000){
							//if((GetKeyState(VK_F) & 0x8000) == 0x8000)
								//if((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADD_TO_FAVORITES, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SEARCH_NOTE, 0), 0);
							//else if((GetKeyState(VK_H) & 0x8000) == 0x8000){
								//if((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIDE, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_REPLACE_NOTE, 0), 0);
							//}
							//else if((GetKeyState(VK_S) & 0x8000) == 0x8000){
								//if((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE_AS, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
							//}
							//else if((GetKeyState(VK_P) & 0x8000) == 0x8000){
								//if((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIGH_PRIORITY, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_PRINT_NOTE, 0), 0);
							//}
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_X) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADJUST, 0), 0);
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_T) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ON_TOP, 0), 0);
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_F6) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_EMAIL, 0), 0);
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_F7) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ATTACHMENT, 0), 0);	
						//}
						//else if((GetKeyState(VK_F3) & 0x8000) == 0x8000)
							//SharedFindProc((HWND)GetPropW(hwnd, PH_EDIT));
						//if((GetKeyState(VK_F3) & 0x8000) == 0x8000)
							//SharedFindProc((HWND)GetPropW(hwnd, PH_EDIT));
					}
				}
			}
			else if(pnmh->code == EN_LINK){
				ENLINK * lpel = (ENLINK *)lParam;
				if(lpel->msg == WM_LBUTTONUP){
					wchar_t 	szBuffer[256];
					TEXTRANGEW	tr;
					tr.chrg.cpMax = lpel->chrg.cpMax;
					tr.chrg.cpMin = lpel->chrg.cpMin;
					tr.lpstrText = szBuffer;
					SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_GETTEXTRANGE, 0, (LPARAM)&tr);
					
					if(_wcsistr(szBuffer, L"http:") == szBuffer || _wcsistr(szBuffer, L"ftp:") == szBuffer || _wcsistr(szBuffer, L"https:") == szBuffer || _wcsistr(szBuffer, L"gopher:") == szBuffer || _wcsistr(szBuffer, L"www.") == szBuffer){
						if(wcslen(g_DefBrowser) == 0)
							ShellExecuteW(hwnd, L"open", szBuffer, NULL, NULL, SW_SHOWDEFAULT);
						else{
							if((int)ShellExecuteW(hwnd, NULL, g_DefBrowser, szBuffer, NULL, SW_SHOWDEFAULT) <= 32){
								ShellExecuteW(hwnd, L"open", szBuffer, NULL, NULL, SW_SHOWDEFAULT);
							}
						}
					}
					else{
						if(_wcsistr(szBuffer, L"file:") == szBuffer || _wcsistr(szBuffer, L"mailto:") == szBuffer){
							ShellExecuteW(hwnd, L"open", szBuffer, NULL, NULL, SW_SHOWDEFAULT);
						}
					}
				}
			}
			return FALSE;
		}
		default: 
			return DefWindowProcW (hwnd, msg, wParam, lParam);
	}
}

// static void ExecuteInternalLink(HWND hwnd, CHARRANGE * lpchr){
	// HWND 			hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
	// CHARRANGE		chr = {0};
	// CHARFORMAT2		chf = {0};
	// PMEMNOTE		pNote = MemNoteByHwnd(hwnd);

	// SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&chr);
	// SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)lpchr);
	// chf.cbSize = sizeof(chf);
	// chf.dwMask = CFM_ANIMATION;
	// RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	// SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
	// if(pNote){
		// LPPLINK		pLink = PLinksItem(pNote->pLinks, chf.bAnimation);
		// if(pLink){
			// PMEMNOTE	pn = MemNoteById(pLink->id);
			// if(pn){
				// ShowNote(pn);
			// }
		// }
	// }
// }

static void Note_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PMEMNOTE		pNote = MemNoteByHwnd(hwnd);

	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
	if(g_NoteSettings.rollOnDblClick && !MEMNOTE(hwnd)->pRTHandles->hbSkin)
		EnableMenuItem(g_hNotePopUp, IDM_ROLL_UNROLL, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(g_hNotePopUp, IDM_ROLL_UNROLL, MF_BYCOMMAND | MF_GRAYED);

	if(!NotesDirExists(g_NotePaths.BackupDir) || !IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP))
		EnableMenuItem(g_hNotePopUp, IDM_RESTORE_BACKUP, MF_BYCOMMAND | MF_GRAYED);
	else
		EnableMenuItem(g_hNotePopUp, IDM_RESTORE_BACKUP, MF_BYCOMMAND | MF_ENABLED);

	if(g_hSmilies)
		EnableMenuItem(g_hEditPopUp, IDM_INSERT_SMILIE, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(g_hEditPopUp, IDM_INSERT_SMILIE, MF_BYCOMMAND | MF_GRAYED);

	if(g_hTags)
		EnableMenuItem(g_hNotePopUp, IDM_TAGS, MF_BYCOMMAND | MF_GRAYED);
	else
		EnableMenuItem(g_hNotePopUp, IDM_TAGS, MF_BYCOMMAND | MF_ENABLED);

	if(pNote){
		if(pNote->pData->idGroup == GROUP_DIARY){
			EnableMenuItem(g_hNotePopUp, IDM_DUPLICATE_NOTE, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(g_hNotePopUp, IDM_SAVE_AS, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(g_hNotePopUp, IDM_NOTE_GROUPS, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(g_hNotePopUp, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(g_hNotePopUp, IDM_ADD_PIN, MF_BYCOMMAND | MF_GRAYED);
		}
		else{
			EnableMenuItem(g_hNotePopUp, IDM_DUPLICATE_NOTE, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(g_hNotePopUp, IDM_NOTE_GROUPS, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(g_hNotePopUp, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			if(pNote->pFlags->fromDB)
				EnableMenuItem(g_hNotePopUp, IDM_SAVE_AS, MF_BYCOMMAND | MF_ENABLED);
			else
				EnableMenuItem(g_hNotePopUp, IDM_SAVE_AS, MF_BYCOMMAND | MF_GRAYED);
			if(g_hPinToWindow){
				EnableMenuItem(g_hNotePopUp, IDM_ADD_PIN, MF_BYCOMMAND | MF_GRAYED);
			}
			else{
				EnableMenuItem(g_hNotePopUp, IDM_ADD_PIN, MF_BYCOMMAND | MF_ENABLED);
			}
		}
	}
}

static void Note_OnMouseLeave(HWND hwnd){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	RECT				rc;
	dock_type 			dockType;

	GetCursorPos(&pt);
	GetWindowRect(hwnd, &rc);
	pNote->pFlags->mouseIn = FALSE;
	dockType = DockType(pNote->pData->dockData);
	if(dockType != DOCK_NONE){
		if(!PtInRect(&rc, pt)){
			switch(dockType){
				case DOCK_LEFT:
					if(g_DArrows.hlu)
						ShowWindow(g_DArrows.hlu, SW_HIDE);
					if(g_DArrows.hld)
						ShowWindow(g_DArrows.hld, SW_HIDE);
					break;
				case DOCK_TOP:
					if(g_DArrows.htl)
						ShowWindow(g_DArrows.htl, SW_HIDE);
					if(g_DArrows.htr)
						ShowWindow(g_DArrows.htr, SW_HIDE);
					break;
				case DOCK_RIGHT:
					if(g_DArrows.hru)
						ShowWindow(g_DArrows.hru, SW_HIDE);
					if(g_DArrows.hrd)
						ShowWindow(g_DArrows.hrd, SW_HIDE);
					break;
				case DOCK_BOTTOM:
					if(g_DArrows.hbl)
						ShowWindow(g_DArrows.hbl, SW_HIDE);
					if(g_DArrows.hbr)
						ShowWindow(g_DArrows.hbr, SW_HIDE);
					break;
				case DOCK_NONE:
					break;
			}
		}
	}
}

static void Note_OnLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	
	pt.x = x;
	pt.y = y;

	if(PtInRect(&pNote->pRTHandles->rcCaption, pt)){
		if(pNote->pRTHandles->rolled){
			UnrollNote(pNote);
		}
		else{
			m_InDblClick = TRUE;
			RollNote(pNote);
			m_PreventCmdTimer = SetTimer(NULL, 0, 1000, PreventCommandTimerProc);
		}
	}

}

static void PasteLinkToFile(HWND hwnd){
	int				len;
	wchar_t			*pFile, *pPrefix = L"<file:", *pPostfix = L">";
	
	len = wcslen(m_DropFile) + wcslen(pPrefix) + wcslen(pPostfix);
	pFile = (wchar_t *)calloc(len + 1, sizeof(wchar_t));
	if(pFile){
		wcscpy(pFile, pPrefix);
		wcscat(pFile, m_DropFile);
		wcscat(pFile, pPostfix);
		SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_REPLACESEL, TRUE, (LPARAM)pFile);
		free(pFile);
	}
}

static void PasteContentsOfFile(HWND hwnd){
	PMEMNOTE		pNote = MEMNOTE(hwnd);
	wchar_t			szMessage[256], * lpExt;
	BYTE			bytes[5];
	int				flags = 0, result = IDYES;
	ULONG			read;
	HANDLE			hFile;

	//check whether dropped file is image file
	lpExt = PathFindExtensionW(m_DropFile);
	if(_wcsicmp(lpExt, L".png") == 0 || _wcsicmp(lpExt, L".bmp") == 0 || _wcsicmp(lpExt, L".gif") == 0 
	|| _wcsicmp(lpExt, L".jpg") == 0 || _wcsicmp(lpExt, L".ico") == 0 || _wcsicmp(lpExt, L".cur") == 0 
	|| _wcsicmp(lpExt, L".emf") == 0 || _wcsicmp(lpExt, L".wmf") == 0 || _wcsicmp(lpExt, L".jpeg") == 0){
		//insert image and return
		InsertPicture(hwnd, m_DropFile);
		return;
	}
	if(!pNote->pFlags->saved){
		ConstructSaveMessage(pNote->pData->szName, szMessage);
		result = MessageBoxW(hwnd, szMessage, g_Strings.SCaption, MB_YESNOCANCEL | MB_ICONQUESTION);
		if(result == IDYES)
			SaveNote(MEMNOTE(hwnd));
		else if(result == IDCANCEL)
			return;
	}
	hFile = CreateFileW(m_DropFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		//check which file is it
		ReadFile(hFile, bytes, 5, &read, NULL);
		if(bytes[2] == 'r' && bytes[3] == 't' && bytes[4] == 'f'){
			flags = SF_RTF;
		}
		if((bytes[0] == 0x0ff && bytes[1] == 0x0fe) || (bytes[0] == 0x0fe && bytes[1] == 0x0ff)){
			//unicode or unicode BE
			flags |= SF_TEXT | SF_UNICODE;
			SetFilePointer(hFile, -3, 0, FILE_CURRENT);
		}
		else if(bytes[0] == 0x0ef && bytes[1] == 0x0bb && bytes[2] == 0x0bf){
			//utf8
			flags |= SF_TEXT;
		}
		else{
			flags |= SF_TEXT;
			SetFilePointer(hFile, -5, 0, FILE_CURRENT);
		}
		EDITSTREAM			esm;
		esm.dwCookie = (DWORD)hFile;
		esm.dwError = 0;
		esm.pfnCallback = InStreamCallback;
		RichEdit_StreamIn((HWND)GetPropW(hwnd, PH_EDIT), flags, &esm);
		CloseHandle(hFile);
	}		
}

static void Note_OnDropFiles(HWND hwnd, HDROP hdrop)
{
	int 			count = DragQueryFileW(hdrop, 0xffffffff, NULL, 0);

	if(count > 1){
		MessageBoxW(hwnd, g_Strings.TooManyFiles, PROG_NAME, MB_OK | MB_ICONERROR);
	}
	else{
		*m_DropFile = '\0';
		DragQueryFileW(hdrop, 0, m_DropFile, MAX_PATH);
		ShowPopUp(hwnd, g_hDropPopUp);
	}
}

static BOOL Note_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos)
{
	P_NOTE_RTHANDLES	pH = NoteRTHandles(hwnd);

	if(pH){
		if(!pH->hbSkin){
			if(lpwpos->cx < DEF_SIZE_MIN){
				lpwpos->cx = DEF_SIZE_MIN;
			}
			if(m_DblClickSize){
				if(lpwpos->cy < (pH->rcCaption.bottom - pH->rcCaption.top)){
					lpwpos->cy = pH->rcCaption.bottom - pH->rcCaption.top;
				}
			}
			else{
				if(!pH->rolled){
					if(lpwpos->cy < (pH->rcCaption.bottom - pH->rcCaption.top) + (pH->rcGrip.bottom - pH->rcGrip.top) * 2){
						lpwpos->cy = (pH->rcCaption.bottom - pH->rcCaption.top) + (pH->rcGrip.bottom - pH->rcGrip.top) * 2;
					}
				}
				else{
					if(lpwpos->cy < (pH->rcCaption.bottom - pH->rcCaption.top)){
						lpwpos->cy = pH->rcCaption.bottom - pH->rcCaption.top;
					}
				}
			}
		}
	}
	return FALSE;
}

static void Note_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	HWND				hEdit, hGrip;

	if(pNote){
		if(pNote->pRTHandles && pNote->pRTHandles->hbSkin){
			if(DockType(pNote->pData->dockData) != DOCK_NONE){
				memcpy(pNote->pRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
			}
		}

		if(pNote->pRTHandles && !pNote->pRTHandles->hbSkin){
			if(!m_DblClickSize){
				//save note size if the resize event was not raised from double click
				GetWindowRect(hwnd, &pNote->pRTHandles->rcSize);
				MeasureSkinlessNoteRectangles(pNote, cx, cy, pNote->pData->szName);
				hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
				hGrip = (HWND)GetPropW(hwnd, PH_GRIP);
				MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, cx - 2, pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
				MoveWindow(hGrip, pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.top, pNote->pRTHandles->rcGrip.right - pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.bottom - pNote->pRTHandles->rcGrip.top, TRUE);
				ShowNoteMarks(hwnd);
				SetNoteCommandsBar(hwnd, pNote->pRTHandles);
				pNote->pRTHandles->rolled = FALSE;
			}
			else{
				;
			}
			m_DblClickSize = FALSE;
		}
	}
}

static void Note_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_MENU){
		if(m_hTempMenu == g_hEditPopUp)
			DrawMItem(lpDrawItem, g_hBmpEditNormal, g_hBmpEditGray, CLR_MASK);
		else
			DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
	}
}

static void Note_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void Note_OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
	PMEMNOTE	pNote = MEMNOTE(hwnd);

	if(pNote){
		if(hwndNewFocus != (HWND)GetPropW(hwnd, PH_EDIT)){
			if(g_NoteSettings.transAllow){
				if(pNote->pAddNApp->transValue != 0)
					SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, pNote->pAddNApp->transValue, LWA_COLORKEY | LWA_ALPHA);
				else
					SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, g_NoteSettings.transValue, LWA_COLORKEY | LWA_ALPHA);
				pNote->pFlags->transparent = TRUE;
			}
		}
	}
}

static void Note_OnRButtonUp(HWND hwnd, int x, int y, UINT flags)
{
	PMEMNOTE		pNote = MEMNOTE(hwnd);
	HMENU			hMenu = g_hNotePopUp;
	dock_type		dockType;

	m_hTempMenu = hMenu;
	
	dockType = DockType(pNote->pData->dockData);
	if(pNote->pFlags->maskChanged && !pNote->pFlags->saved)
		EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);

	if(g_hAdjust == NULL){
		EnableMenuItem(hMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_ENABLED);
	}
	else{
		EnableMenuItem(hMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_GRAYED);
	}

	if(g_hLinksDlg == NULL)
		EnableMenuItem(hMenu, IDM_LINKS, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_LINKS, MF_BYCOMMAND | MF_GRAYED);

	if(pNote->pSRStatus->lastRec.wDay > 0 && IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK))
		EnableMenuItem(hMenu, IDM_REPLY, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_REPLY, MF_BYCOMMAND | MF_GRAYED);

	if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK))
		EnableMenuItem(hMenu, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_GRAYED);

	FreeNoteMenus(hMenu);

	PrepareDockMenu(MEMNOTE(hwnd), hMenu, GetMenuPosition(hMenu, IDM_DOCK));
	AddGroupsMenuItems(hwnd, hMenu);
	BuildLinksMenu(MemNoteByHwnd(hwnd));
	BuildContactsMenu(hMenu, 60, 61);

	if(pNote->pData->onTop || dockType != DOCK_NONE)
		CheckMenuItem(hMenu, IDM_ON_TOP, MF_CHECKED | MF_BYCOMMAND);
	else
		CheckMenuItem(hMenu, IDM_ON_TOP, MF_UNCHECKED | MF_BYCOMMAND);

	if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY)){
		CheckMenuItem(hMenu, IDM_HIGH_PRIORITY, MF_CHECKED | MF_BYCOMMAND);
	}
	else{
		CheckMenuItem(hMenu, IDM_HIGH_PRIORITY, MF_UNCHECKED | MF_BYCOMMAND);
	}
	if(IsBitOn(pNote->pData->res1, NB_PROTECTED)){
		CheckMenuItem(hMenu, IDM_TOGGLE_PROTECTION, MF_CHECKED | MF_BYCOMMAND);
	}
	else{
		CheckMenuItem(hMenu, IDM_TOGGLE_PROTECTION, MF_UNCHECKED | MF_BYCOMMAND);
	}
	if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
		CheckMenuItem(hMenu, IDM_MARK_AS_COMPLETED, MF_CHECKED | MF_BYCOMMAND);
	}
	else{
		CheckMenuItem(hMenu, IDM_MARK_AS_COMPLETED, MF_UNCHECKED | MF_BYCOMMAND);
	}
	if(dockType != DOCK_NONE){
		EnableMenuItem(hMenu, IDM_ON_TOP, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		EnableMenuItem(hMenu, IDM_ON_TOP, MF_BYCOMMAND | MF_ENABLED);
	}

	ShowPopUp(hwnd, hMenu);

	// FreeNoteMenus(hMenu);
}

static void SetDockType(int * dockData, dock_type dockType){
	short		th = HIWORD(*dockData);

	*dockData = MAKELONG(dockType, th);
}

static void UndockNote(PMEMNOTE pNote){

	dock_type			dockType;
	PDOCKITEM			pRemain;
	P_NOTE_DATA			ptd;
	HWND				hTray = NULL;
	BOOL				rolled;
	RECT				rc;

	rolled = pNote->pSavedHandles->rolled;

	dockType = DockType(pNote->pData->dockData);
	pRemain = NextDockItem(DHeader(dockType), pNote->hwnd);
	SetDockType(&pNote->pData->dockData, DOCK_NONE);
	SetDockIndex(&pNote->pData->dockData, -777);
	SaveNewDockData(pNote->hwnd, pNote->pData->dockData);
	RemoveDockItem(DHeader(dockType), pNote->hwnd);

	//restore saved run-time handles
	memcpy(pNote->pRTHandles, pNote->pSavedHandles, sizeof(NOTE_RTHANDLES));

	MoveWindow(pNote->hwnd, pNote->pData->rcp.left, pNote->pData->rcp.top, pNote->pData->rcp.right - pNote->pData->rcp.left, pNote->pData->rcp.bottom - pNote->pData->rcp.top, TRUE);

	if(!pNote->pData->prevOnTop){
		SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if(g_RTHandles.hbSkin){
		ApplyNewSkin(pNote->hwnd, pNote->pRTHandles);
	}
	else{
		// if(g_DockSettings.fCustSize){
			GetWindowRect(pNote->hwnd, &rc);
			MeasureSkinlessNoteRectangles(pNote, rc.right - rc.left, rc.bottom - rc.top, pNote->pData->szName);
		// }
		// else{
			// MeasureSkinlessNoteRectangles(pNote, pNote->pData->rcp.right - pNote->pData->rcp.left, pNote->pData->rcp.bottom - pNote->pData->rcp.top, pNote->pData->szName);
		// }
		HWND hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, (pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left) - 2, pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
		CreateSkinlessToolbarBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
		CreateSkinlessSysBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
		CreateSimpleMarksBitmap(pNote->hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
		MeasureSkinlessNoteRectangles(pNote, pNote->pData->rcp.right - pNote->pData->rcp.left, pNote->pData->rcp.bottom - pNote->pData->rcp.top, pNote->pData->szName);
		SetNoteCommandsBar(pNote->hwnd, pNote->pRTHandles);
		if(g_DockSettings.fCustColor){
			RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
		}
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
	}

	while(pRemain){
		ptd = NoteData(pRemain->hwnd);
		SetDockIndex(&ptd->dockData, pRemain->index);
		hTray = MoveDockWindow(pRemain->hwnd, dockType, pRemain->index);
		pRemain = pRemain->pNext;
	}

	if(rolled){
		pNote->pRTHandles->rolled = FALSE;
		SendMessageW(pNote->hwnd, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(pNote->pRTHandles->rcCaption.left + 1, pNote->pRTHandles->rcCaption.top + 1));
	}
	
	ShowNoteMarks(pNote->hwnd);
	if(hTray)
		SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}

static void DockNote(HWND hwnd, dock_type dockType, P_NOTE_DATA pD){
	dock_type			dtPrev;
	PDOCKITEM			pRemain = NULL, pTemp;
	HWND				hTray = NULL;
	BOOL				rolled;
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	RECT				rc;

	rolled = pNote->pRTHandles->rolled;

	dtPrev = DockType(pD->dockData);

	if(dtPrev != DOCK_NONE){
		pRemain = NextDockItem(DHeader(dtPrev), hwnd);
	}
	else{
		//save run-time handles
		memcpy(pNote->pSavedHandles, pNote->pRTHandles, sizeof(NOTE_RTHANDLES));
	}

	SetDockType(&pD->dockData, dockType);
	pTemp = AddDockItem(DHeader(dockType), hwnd, -1);
	SetDockIndex(&pD->dockData, pTemp->index);
	SaveNewDockData(hwnd, pD->dockData);
	hTray = MoveDockWindow(hwnd, dockType, pTemp->index);
	if(hTray)
		SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

	if(g_RTHandles.hbSkin){
		if(g_DockSettings.fCustSkin){
			ApplyNewSkin(hwnd, &g_DRTHandles);
		}
	}
	else{
		if(g_DockSettings.fCustColor || g_DockSettings.fCustCaption){
			ApplyDockColors(MEMNOTE(hwnd));
		}
		GetWindowRect(hwnd, &rc);
		MeasureSkinlessNoteRectangles(pNote, rc.right - rc.left, rc.bottom - rc.top, pNote->pData->szName);

		HWND hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, (pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left) - 2, pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
		CreateSimpleMarksBitmap(hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}

	//if(!pD->onTop){
		SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//}

	if(dtPrev != DOCK_NONE){
		P_NOTE_DATA		ptd;
		RemoveDockItem(DHeader(dtPrev), hwnd);
		while(pRemain){
			ptd = NoteData(pRemain->hwnd);
			SetDockIndex(&ptd->dockData, pRemain->index);
			hTray = MoveDockWindow(pRemain->hwnd, dtPrev, pRemain->index);
			pRemain = pRemain->pNext;
		}
		if(hTray)
			SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}

	ShowNoteMarks(hwnd);
	pNote->pSavedHandles->rolled = rolled;
}

static void FreeNoteMenus(HMENU hMenu){
	HMENU					hFree;
	int						count;

	hFree = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_NOTE_GROUPS));
	count = GetMenuItemCount(hFree);
	FreeMenus(hFree);
	for(int i = count - 1; i >= 0; i--){
		DeleteMenu(hFree, i, MF_BYPOSITION);
	}
}

static void AddGroupsToCombo(HWND hCombo){
	LPPNGROUP				ppg;
	int						groupsCount, index;
	P_GROUP_MENU_STRUCT		pGroups, pTemp;

	ppg = g_PGroups;
	groupsCount = PNGroupsCount(g_PGroups);
	if(ppg){
		pGroups = calloc(groupsCount, sizeof(GROUP_MENU_STRUCT));
		if(pGroups){
			pTemp = pGroups;
			while(ppg){
				pTemp->id = ppg->id;
				pTemp->pName = ppg->szName;
				pTemp++;
				//move next
				ppg = ppg->next;
			}
			pTemp = pGroups;
			if(groupsCount > 1){
				qsort(pTemp, groupsCount, sizeof(GROUP_MENU_STRUCT), GroupMenusCompare);
			}
			for(int i = 0; i < groupsCount; i++){
				index = SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)pTemp->pName);
				SendMessageW(hCombo, CB_SETITEMDATA, index, (LPARAM)pTemp->id);
				pTemp++;
			}
			free(pGroups);
		}
	}
}

static void SetFlippedMenu(UINT id, UINT idCaption, int image, PMITEM pMenus, HMENU hMenu, wchar_t * lpLangFile, P_HK_TYPE lpKeys, int keysCount, wchar_t * lpDefault){
	P_HK_TYPE				pHK;
	wchar_t					szText[128], szBuffer[128];
	PMITEM					pmi;

	pHK = GetHotKeyByID(id, lpKeys, keysCount);

	if(pHK){
		if(pHK->fsModifiers || pHK->vk)
			SetMenuTextWithAccelerator(idCaption, S_MENU, lpLangFile, lpDefault, szText, pHK->szKey);
		else{
			*szBuffer = '\0';
			GetMIReserved(pMenus, keysCount, id, szBuffer);
			if(*szBuffer)
				SetMenuTextWithAccelerator(idCaption, S_MENU, lpLangFile, lpDefault, szText, szBuffer);
			else
				SetMenuText(idCaption, S_MENU, lpLangFile, lpDefault, szText);
		}
	}		
	else{
		*szBuffer = '\0';
		GetMIReserved(pMenus, keysCount, id, szBuffer);
		if(*szBuffer)
			SetMenuTextWithAccelerator(idCaption, S_MENU, lpLangFile, lpDefault, szText, szBuffer);
		else
			SetMenuText(idCaption, S_MENU, lpLangFile, lpDefault, szText);
	}
		
	SetMIText(pMenus, keysCount, id, szText);
	pmi = GetMItem(pMenus, keysCount, id);
	if(pmi){
		pmi->xPos = image;
		SetMenuItemProperties(pmi, hMenu, id, FALSE);
	}
}

static void AddGroupsMenuItems(HWND hwnd, HMENU hMenu){
	HMENU					hGroups;
	LPPNGROUP				ppg;
	MENUITEMINFOW			mi;
	int						pos = 0, groupsCount;
	PMEMNOTE				pNote = MEMNOTE(hwnd);
	wchar_t					szDefault[128];
	MITEM					mit, *pmi;
	P_GROUP_MENU_STRUCT		pGroups, pTemp;
	UINT					idCaption;
	int						image;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = 49;

	hGroups = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_NOTE_GROUPS));

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA | MIIM_ID;
	//favorites status
	mi.wID = IDM_ADD_TO_FAVORITES;
	GetMenuItemInfoW(g_hNotePopUp, IDM_ADD_TO_FAVORITES, FALSE, &mi);
	pmi = (PMITEM)mi.dwItemData;
	if(pmi){
		if(pNote->pRTHandles->favorite == GROUP_FAVORITES){
			wcscpy(szDefault, L"Remove From Favorites");
			image = 48;
			idCaption = IDM_REMOVE_FAVORITES;
		}
		else{
			wcscpy(szDefault, L"Add To Favorites");
			image = 47;
			idCaption = IDM_ADD_TO_FAVORITES;
		}
		SetFlippedMenu(mi.wID, idCaption, image, NoteMenu(), hMenu, g_NotePaths.CurrLanguagePath, g_NoteHotkeys, NoteMenuCount(), szDefault);
	}

	//locked status
	mi.wID = IDM_LOCK_NOTE;
	GetMenuItemInfoW(g_hNotePopUp, IDM_LOCK_NOTE, FALSE, &mi);
	pmi = (PMITEM)mi.dwItemData;
	if(pmi){
		if(pNote->pFlags->locked){
			wcscpy(szDefault, L"Remove Note Password");
			image = 54;
			idCaption = IDM_UNLOCK_NOTE;
		}
		else{
			wcscpy(szDefault, L"Set Note Password");
			image = 53;
			idCaption = IDM_LOCK_NOTE;
		}
		SetFlippedMenu(mi.wID, idCaption, image, NoteMenu(), hMenu, g_NotePaths.CurrLanguagePath, g_NoteHotkeys, NoteMenuCount(), szDefault);
	}

	//pinned status
	mi.wID = IDM_ADD_PIN;
	GetMenuItemInfoW(g_hNotePopUp, IDM_ADD_PIN, FALSE, &mi);
	pmi = (PMITEM)mi.dwItemData;
	if(pmi){
		if(!pNote->pPin){
			wcscpy(szDefault, L"Pin To Window");
			image = 57;
			idCaption = IDM_ADD_PIN;
		}
		else{
			wcscpy(szDefault, L"Unpin");
			image = 58;
			idCaption = IDM_REMOVE_PIN;
		}
		SetFlippedMenu(mi.wID, idCaption, image, NoteMenu(), hMenu, g_NotePaths.CurrLanguagePath, g_NoteHotkeys, NoteMenuCount(), szDefault);
	}

	ppg = g_PGroups;
	groupsCount = PNGroupsCount(g_PGroups);
	if(ppg){
		pGroups = calloc(groupsCount, sizeof(GROUP_MENU_STRUCT));
		if(pGroups){
			pTemp = pGroups;
			while(ppg){
				pTemp->id = ppg->id;
				pTemp->pName = ppg->szName;
				pTemp++;
				//move next
				ppg = ppg->next;
			}
			pTemp = pGroups;
			if(groupsCount > 1){
				qsort(pTemp, groupsCount, sizeof(GROUP_MENU_STRUCT), GroupMenusCompare);
			}
			for(int i = 0; i < groupsCount; i++){
				mit.id = pTemp->id + GROUP_ADDITION;
				wcscpy(mit.szText, pTemp->pName);
				if(pTemp->id == pNote->pData->idGroup){
					AppendMenuW(hGroups, MF_STRING | MF_CHECKED, mit.id, mit.szText);
				}
				else{
					AppendMenuW(hGroups, MF_STRING , mit.id, mit.szText);
				}
				SetMenuItemProperties(&mit, hGroups, pos++, TRUE);
				pTemp++;
			}
			free(pGroups);
		}
	}
}

static void Note_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	RemoveBell(hwnd);
}

static void Note_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	
	RemoveBell(hwnd);
	pt.x = x;
	pt.y = y;
	if(pNote->pRTHandles->hbSkin){
		pNote->pFlags->inDrag = TRUE;
		SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &m_fFullDrag, 0);
	}
	else{
		if(PtInRect(&pNote->pRTHandles->rcCaption, pt)){
			pNote->pFlags->inDrag = TRUE;
			SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &m_fFullDrag, 0);
		}
		else
			pNote->pFlags->inDrag = FALSE;
	}
}

static void Note_OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	SetFocus((HWND)GetPropW(hwnd, PH_EDIT));
}

static void Note_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PMEMNOTE			pNote;
	int					result = 0;
	BOOL				delete = FALSE, changed = FALSE;
	ADJUST_STRUCT		as;

	switch(id){
		case SUGG_START:{
			MENUITEMINFOW		mi = {0};
			PMITEM				pmi;

			mi.cbSize = sizeof(mi);
			mi.fMask = MIIM_DATA;
			GetMenuItemInfoW(m_hMenuSuggestions, id, false, &mi);
			pmi = (PMITEM)mi.dwItemData;
			if(pmi){
				AddWordToDictionary(pmi->szReserved, true);
				DestroySuggestionsMenu();
				RedrawWindow((HWND)GetPropW(hwnd, PH_EDIT), NULL, NULL, RDW_INVALIDATE);
			}
			break;
		}
		case IDM_SPELL_NOW:
			if(!g_hSpellChecking)
				CheckRESpelling((HWND)GetPropW(hwnd, PH_EDIT));
			else
				BringWindowToTop(g_hSpellChecking);
			break;
		case IDM_SPELL_AUTO:{
			if(g_Spell.spellAuto){
				g_Spell.spellAuto = false;
				WritePrivateProfileStringW(S_NOTE, IK_SPELL_AUTO, L"0", g_NotePaths.INIFile);
			}
			else{
				g_Spell.spellAuto = true;
				WritePrivateProfileStringW(S_NOTE, IK_SPELL_AUTO, L"1", g_NotePaths.INIFile);
			}
			ApplyRedrawEdit();
			break;
		}
		case IDM_SPELL_DOWNLOAD:
			OpenPage(hwnd, SPELL_DOWNLOAD);
			break;
		case IDM_ADD_PIN:
			pNote = MEMNOTE(hwnd);
			if(!pNote->pPin)
				AddPin(hwnd, pNote);
			else
				RemovePin(pNote);
			break;
		case IDM_LOCK_NOTE:
			pNote = MEMNOTE(hwnd);
			if(pNote->pFlags->locked){
				//remove password
				if(ShowLoginDialog(g_hInstance, hwnd, DLG_LOGIN_MAIN, NULL, LDT_NOTE, pNote) == IDOK){
					pNote->pFlags->locked = false;
					WritePrivateProfileStringW(pNote->pFlags->id, IK_LOCK, NULL, g_NotePaths.DataFile);
				}
			}
			else{
				//set password
				wchar_t			szNewPwrd[256];

				*szNewPwrd = '\0';
				if(ShowLoginDialog(g_hInstance, hwnd, DLG_LOGIN_CREATE, szNewPwrd, LDT_NOTE, pNote) == IDOK){
					pNote->pFlags->locked = true;
					WritePrivateProfileStringW(pNote->pFlags->id, IK_LOCK, szNewPwrd, g_NotePaths.DataFile);
				}
			}
			if(pNote->pData->visible){
				RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(pNote->hwnd);
			}
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
			break;
		case IDM_REPLY:
			pNote = MEMNOTE(hwnd);
			SendNoteToContact(hwnd, pNote, pNote->pSRStatus->recFrom);
			break;
		case IDM_SELECT_CONT_GROUP:{
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUPS_SEND), hwnd, Multigroups_DlgProc, (LPARAM)hwnd);
			if(result == IDOK){
				pNote = MEMNOTE(hwnd);
				if(!pNote->pFlags->saved && IsBitOn(g_NextSettings.flags1, SB3_SAVE_BEFORE_SEND)){
					SaveNote(pNote);
				}
				LPPCONTACT		lpc = g_PContacts;
				while(lpc){
					if(lpc->prop.send)
						SendNoteToContact(hwnd, pNote, lpc->prop.name);
					lpc = lpc->next;
				}
				ClearMultipleNames();
			}
			break;
		}
		case IDM_SEND_TO_SEVERAL:{
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SEND_SEVERAL), hwnd, Multicontacts_DlgProc, (LPARAM)hwnd);
			if(result == IDOK){
				pNote = MEMNOTE(hwnd);
				if(!pNote->pFlags->saved && IsBitOn(g_NextSettings.flags1, SB3_SAVE_BEFORE_SEND)){
					SaveNote(pNote);
				}
				LPPCONTACT		lpc = g_PContacts;
				while(lpc){
					if(lpc->prop.send)
						SendNoteToContact(hwnd, pNote, lpc->prop.name);
					lpc = lpc->next;
				}
				ClearMultipleNames();
			}
			break;
		}
		case IDM_ADD_CONT_GROUP:{
			ZeroMemory(&g_ContGroup, sizeof(PCONTGROUP));
			if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTGROUPS), hwnd, ContGroups_DlgProc, 2) == IDOK){
				PContGroupsAdd(&g_PContGroups, &g_ContGroup, true);
				SaveContactsGroups();
			}
			break;
		}
		case IDM_ADD_CONTACT:{
			ZeroMemory(&g_Contact, sizeof(PCONTPROP));
			result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOSTS), hwnd, Hosts_DlgProc, 2);
			if(result == IDOK || result == IDYES){
				g_PContacts = PContactsAdd(g_PContacts, &g_Contact);
				SaveContacts();
				if(result == IDYES){
					pNote = MEMNOTE(hwnd);
					if(!pNote->pFlags->saved && IsBitOn(g_NextSettings.flags1, SB3_SAVE_BEFORE_SEND)){
						SaveNote(pNote);
					}
					SendNoteToContact(hwnd, pNote, g_Contact.name);
				}
			}
			break;
		}
		case IDM_MANAGE_LINKS:
			pNote = MEMNOTE(hwnd);
			if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_LINKS), hwnd, ChooseLink_DlgProc, (LPARAM)pNote) == IDOK){
				SaveLinks(pNote);
			} 
			break;
		case IDM_SAVE_AS_TEXT:{
			SaveNoteAsTextFile(MEMNOTE(hwnd));
			break;
		}
		case IDM_TAGS:
			pNote = MEMNOTE(hwnd);
			if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_TAGS), hwnd, Tags_DlgProc, (LPARAM)pNote) == IDOK){
				if(pNote->pFlags->fromDB){
					SaveTags(pNote);
				}
				if(g_hCPDialog)
					SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
			break;
		case IDM_DUPLICATE_NOTE:
			DuplicateNote(MEMNOTE(hwnd));
			break;
		case IDM_RESTORE_BACKUP:
			RestoreFromBackup(MEMNOTE(hwnd));
			break;
		case IDM_ROLL_UNROLL:
			pNote = MEMNOTE(hwnd);
			if(pNote->pRTHandles->rolled){
				UnrollNote(pNote);
			}
			else{
				RollNote(pNote);
			}
			break;
		case IDM_DROP_SHORTCUT:
			PasteLinkToFile(hwnd);
			break;
		case IDM_DROP_CONTENT:
			PasteContentsOfFile(hwnd);
			break;
		case IDM_INSERT_DATETIME:
			InsertDateTime(hwnd);
			break;
		case IDM_INSERT_SMILIE:
			DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SMILIES), hwnd, Smilies_DlgProc, 0);
			break;
		case IDM_INSERT_PICTURE:{
			wchar_t		szPicture[MAX_PATH];
			if(ChoosePictureFile(hwnd, szPicture) == IDOK){
				InsertPicture(hwnd, szPicture);
			}
			break;
		}
		case IDM_ADJUST_APPEARANCE:
		case IDM_ADJUST_SCHEDULE:
			pNote = MEMNOTE(hwnd);
			as.pNote = pNote;
			if(id == IDM_ADJUST_APPEARANCE)
				as.type = 0;
			else
				as.type = 1;
			m_Reload = FALSE;
			DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_ADJUST), hwnd, Adjust_DlgProc, (LPARAM)&as);
			if(!m_Reload && !g_IsClosing){
				//make note opaque
				SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
			g_hAdjust = NULL;
			break;
		case IDM_SAVE:
			pNote = MEMNOTE(hwnd);
			SavePNote(pNote);
			//make note opaque
			if(g_NoteSettings.transAllow){
				if(GetFocus() == (HWND)GetPropW(hwnd, PH_EDIT)){
					SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
					pNote->pFlags->transparent = FALSE;
				}
			}
			break;
		case IDM_ATTACHMENT:{
			pNote = MEMNOTE(hwnd);
			if(!pNote->pFlags->saved){
				if(MessageBoxW(hwnd, g_Strings.SaveBeforeSend, g_Strings.SCaption, MB_YESNO | MB_ICONQUESTION) == IDNO){
					break;
				}
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
			}
			SendNoteAsAttachment(pNote);
			break;
		}
		case IDM_SAVE_AS:
			pNote = MEMNOTE(hwnd);
			if(pNote->pFlags->fromDB)
				SaveNoteAs(pNote, 1);
			break;
		case IDM_HIDE:
			if(IsBitOn(g_NoteSettings.reserved1, SB1_PLAY_HIDE_SOUND)){
				PlaySound(MAKEINTRESOURCE(IDS_HIDE_SOUND), g_hInstance, SND_RESOURCE | SND_ASYNC);
			}
			if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_FLUENTLY)){
				pNote = MEMNOTE(hwnd);
				if(g_RTHandles.hbSkin){
					SetWindowRgn(hwnd, RegionFromSkin(pNote->pRTHandles->hbSkin, pNote->pRTHandles->crMask), TRUE);
				}
				SetTimer(hwnd, CLOSE_TIMER_ID, 100, CloseTimerProc);
			}
			else
				HideNote(MEMNOTE(hwnd), false);
			break;
		case IDM_DEL:{
			BOOL		delComplete = FALSE;
			wchar_t		szMessage[256];
			int			iGroup;

			pNote = MEMNOTE(hwnd);
			
			iGroup = pNote->pData->idGroup;

			if((GetKeyState(VK_LSHIFT) & 0x8000) == 0x8000){
				delComplete = TRUE;
				wcscpy(szMessage, g_Strings.DMessageComplete);
			}
			else if(!pNote->pFlags->fromDB){
				delComplete = TRUE;
				wcscpy(szMessage, g_Strings.DMessageComplete);
			}
			else{
				wcscpy(szMessage, g_Strings.DMessageBin);
			}
			if(codeNotify == CHN_DEL_WITH_GROUP){
				//do not ask question when delete with group
				delete = TRUE;
			}
			else{
				if(g_NoteSettings.confirmDelete){
					result = MessageBoxW(hwnd, szMessage, g_Strings.DCaption, MB_ICONQUESTION | MB_YESNO);
					if (result == IDYES){
						delete = TRUE;
					}
				}
				else
					delete = TRUE;
			}

			if(delete){
				if(g_hCPDialog){
					if(codeNotify != CHN_DEL_WITH_GROUP){
						//refresh control panel only when delete note (not group in whole)
						SendMessageW(g_hCPDialog, PNM_CTRL_DEL, 0, (LPARAM)pNote);
					}
					SendMessageW(g_hCPDialog, PNM_UPDATE_CTRL_STATUS, 0, 0);
				}
				//remove note from all possible links
				RemoveNoteFromLinks(pNote);

				if(delComplete){
					DeleteNoteCompletely(pNote);
				}
				else{
					if(codeNotify == CHN_DEL_WITH_GROUP)
						DeleteToBin(pNote, 0);
					else
						DeleteToBin(pNote, -1);
				}
				if(g_hCPDialog){
					if(codeNotify != CHN_DEL_WITH_GROUP){
						//refresh control panel only when delete note (not group in whole)
						SendMessageW(g_hCPDialog, PNM_RELOAD, 0, 0);
					}
				}
			}
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, iGroup, GROUP_RECYCLE);
			}
			break;
		}
		case IDM_EMAIL:
			SendNoteByEmail(MEMNOTE(hwnd));
			break;
		case IDM_HIGH_PRIORITY:
			ToggleNotePriority(MEMNOTE(hwnd));
			break;
		case IDM_TOGGLE_PROTECTION:
			pNote = MEMNOTE(hwnd);
			ToggleNoteProtection(pNote);
			break;
		case IDM_MARK_AS_COMPLETED:
			ToggleNoteCompleted(MEMNOTE(hwnd));
			break;
		case IDM_PRINT_NOTE:
			pNote = MEMNOTE(hwnd);
			PrintEdit((HWND)GetPropW(pNote->hwnd, PH_EDIT), pNote->pData->szName);
			break;
		case IDM_ON_TOP:
			pNote = MEMNOTE(hwnd);
			
			if(codeNotify == 0){
				result = GetMenuState(g_hNotePopUp, IDM_ON_TOP, MF_BYCOMMAND);
				if((result & MF_CHECKED) != MF_CHECKED){
					CheckMenuItem(g_hNotePopUp, IDM_ON_TOP, MF_CHECKED | MF_BYCOMMAND);
				}
				else{
					CheckMenuItem(g_hNotePopUp, IDM_ON_TOP, MF_UNCHECKED | MF_BYCOMMAND);
				}
				changed = TRUE;
				ToggleNoteOnTop(pNote);
			}
			else{
				CheckMenuItem(g_hNotePopUp, IDM_ON_TOP, MF_CHECKED | MF_BYCOMMAND);
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}

			if(changed && codeNotify == 0){
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(hwnd);
				if(g_hCPDialog)
					SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
			break;
		case IDC_NOTE_EDIT:
			if(codeNotify == EN_CHANGE && !m_JustCreated){
				pNote = MEMNOTE(hwnd);		
				pNote->pFlags->maskChanged |= F_TEXT;
				if(pNote->pFlags->saved){
					pNote->pFlags->saved = FALSE;
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(hwnd);
					if(g_hCPDialog){
						SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
					}
				}
			}
			break;
		case IDM_UNDO:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_UNDO, 0, 0);
			break;
		case IDM_REDO:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_REDO, 0, 0);
			break;
		case IDM_CUT:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_CUT, 0, 0);
			break;
		case IDM_COPY:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_COPY, 0, 0);
			break;
		case IDM_COPY_AS_TEXT:{
			CHARRANGE		chr = {0};

			chr.cpMax = -1;
			//copy text into clipboard
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_COPY, 0, 0);
			//paste text from clipboard into hidden richtext box as plain text
			SendMessageW(g_hTEditSimple, WM_PASTE, 0, 0);
			//cut plain text into clipboard
			SendMessageW(g_hTEditSimple, EM_EXSETSEL, 0, (LPARAM)&chr);
			SendMessageW(g_hTEditSimple, WM_CUT, 0, 0);
			break;
		}
		case IDM_PASTE:
			if(IsClipboardFormatAvailable(CF_BITMAP)){
			// if(IsClipboardFormatAvailable(CF_BITMAP) 
			// || IsClipboardFormatAvailable(CF_ENHMETAFILE)){
				LPRICHEDITOLE 	pREOle = NULL;
				SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_GETOLEINTERFACE, 0, (LPARAM)&pREOle);
				if(pREOle != NULL){
					InsertBitmapFromClipboard(pREOle);
					//save edit box contents to temporary file and load from it
					ExchangeSameRE((HWND)GetPropW(hwnd, PH_EDIT));
				}
			}
			else{
				SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_PASTE, 0, 0);
			}
			break;
		case IDM_PASTE_AS_TEXT:{
			CHARRANGE			chr = {0};
			GETTEXTLENGTHEX		gtl = {0};
			//clear simple richtext box
			SendMessageW(g_hTEditSimple, WM_SETTEXT, 0, (LPARAM)L"");
			//paste text from clipboard into hidden richtext box as plain text
			SendMessageW(g_hTEditSimple, WM_PASTE, 0, 0);
			//clear rich richtext box
			SendMessageW(g_hTEditRich, WM_SETTEXT, 0, (LPARAM)L"");
			//paste text from clipboard into hidden richtext box as rich text
			SendMessageW(g_hTEditRich, WM_PASTE, 0, 0);
			//cut and paste plain text
			chr.cpMax = -1;
			SendMessageW(g_hTEditSimple, EM_EXSETSEL, 0, (LPARAM)&chr);
			SendMessageW(g_hTEditSimple, WM_CUT, 0, 0);
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_PASTE, 0, 0);
			//cut rich text back to clipboard
			chr.cpMin = 0;
			gtl.flags = GTL_NUMCHARS | GTL_PRECISE;
			gtl.codepage = 1200;
			chr.cpMax = SendMessageW(g_hTEditRich, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0);
			SendMessageW(g_hTEditRich, EM_EXSETSEL, 0, (LPARAM)&chr);
			SendMessageW(g_hTEditRich, WM_CUT, 0, 0);
			break;
		}
		case IDM_SELECT_ALL:{
			CHARRANGE	chr;
			chr.cpMin = 0;
			chr.cpMax = -1;
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_EXSETSEL, 0, (LPARAM)&chr);
			break;
		}
		case IDM_SEARCH_NOTE:
		case IDM_REPLACE_NOTE:{
			NFIND nf;
			nf.hNote = hwnd;
			nf.iFindReplace = id;
			if(g_hFindDialog == NULL)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_FIND_SINGLE), g_hMain, Find_DlgProc, (LPARAM)&nf);
			else
				SendMessageW(g_hFindDialog, PNM_NEW_FIND_PARENT, 0, (LPARAM)&nf);
			break;
		}
		case IDM_SEARCH_NOTE_NEXT:
			if(SharedFindProc((HWND)GetPropW(hwnd, PH_EDIT)) == -1){
				wchar_t		szBuffer[256], szKey[16];
				_itow(DLG_FIND_SINGLE, szKey, 10);
				GetPrivateProfileStringW(S_OPTIONS, szKey, L"Find", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				MessageBoxW(hwnd, g_Strings.NoMatchesFound, szBuffer, MB_OK | MB_ICONINFORMATION);
			}
			break;
		case IDM_DOCK_LEFT:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_LEFT, pNote->pData);
			break;
		case IDM_DOCK_TOP:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_TOP, pNote->pData);
			break;
		case IDM_DOCK_RIGHT:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_RIGHT, pNote->pData);
			break;
		case IDM_DOCK_BOTTOM:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_BOTTOM, pNote->pData);
			break;
		case IDM_DOCK_NONE:
			UndockNote(MEMNOTE(hwnd));
			break;
		case IDM_ADD_TO_FAVORITES:
			AddToFavorites(MEMNOTE(hwnd));
			break;
		case IDM_FORMAT_FONT_FAMILY:{
			P_NCMD_BAR			pC;
			CBCREATION 			cbr;

			pC = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS);
			cbr.hParent = g_hMain;
			cbr.hButton = pC->hCmd;
			cbr.hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
			SetCurrentFontFace((HWND)GetPropW(hwnd, PH_EDIT));
			CreateComboDialog(&cbr, g_hInstance);
			break;
		}
		case IDM_FORMAT_FONT_BOLD:
		case IDM_FORMAT_FONT_ITALIC:
		case IDM_FORMAT_FONT_UNDER:
		case IDM_FORMAT_FONT_STRIKE:
		case IDM_FORMAT_FONT_HILIT:
		case IDM_FORMAT_ALIGN_LEFT:
		case IDM_FORMAT_ALIGN_CENTER:
		case IDM_FORMAT_ALIGN_RIGHT:
		case IDM_FORMAT_BULLETS:
			SendMessageW((HWND)GetPropW(hwnd, PH_COMMANDS), WM_COMMAND, MAKEWPARAM(id, 0), 0);
			break;
		case IDM_NO_BULLETS:
			RemoveAllBulletsNumbering(hwnd);
			SetFocus(hwnd);
			break;
		case IDM_SIMPLE_BULLETS:
		case IDM_NUMBERS_PERIOD:
		case IDM_NUMBERS_PARENS:
		case IDM_LC_PERIOD:
		case IDM_LC_PARENS:
		case IDM_UC_PERIOD:
		case IDM_UC_PARENS:
		case IDM_ROMAN_LC:
		case IDM_ROMAN_UC:
			SetREBulletsAndNumbering(hwnd, id);
			SetFocus(hwnd);
			break;
		default:
			if(codeNotify == 0 && hwndCtl == 0){
				if(id > SPELL_DICT_ADDITION && id <= SPELL_DICT_ADDITION + SPELL_DICT_MAX){
					HMENU	h1 = GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_SPELL_CHECK));
					if((GetMenuState(h1, id, MF_BYCOMMAND) && MF_CHECKED) != MF_CHECKED){
						MENUITEMINFOW		mi = {0};
						PMITEM				pmi;
						int					count = GetMenuItemCount(h1);

						mi.cbSize = sizeof(mi);
						mi.fMask = MIIM_DATA;
						GetMenuItemInfoW(h1, id, FALSE, &mi);
						pmi = (PMITEM)mi.dwItemData;
						if(pmi){
							PHENTRY		pl = getEntry(g_LangNames, pmi->szReserved);
							if(pl){
								wcscpy(g_Spell.spellCulture, pl->key);
								WritePrivateProfileStringW(S_NOTE, IK_SPELL_CULTURE, g_Spell.spellCulture, g_NotePaths.INIFile);
								InitSpeller();
								for(int i = 6; i < count; i++)
									CheckMenuItem(h1, i, MF_UNCHECKED | MF_BYPOSITION);
								CheckMenuItem(h1, id, MF_CHECKED | MF_BYCOMMAND);
								if(g_Spell.spellAuto){
									ApplyRedrawEdit();
								}
							}
						}
					}
				}
				else if(id > SUGG_START && id <= SUGG_ADDITION + SUGG_MAX){
					MENUITEMINFOW		mi = {0};
					PMITEM				pmi;

					mi.cbSize = sizeof(mi);
					mi.fMask = MIIM_DATA;
					GetMenuItemInfoW(m_hMenuSuggestions, id, false, &mi);
					pmi = (PMITEM)mi.dwItemData;
					if(pmi){
						CHARRANGE		chr;
						chr.cpMin = pmi->xCheck;
						chr.cpMax = pmi->yCheck;
						HWND			hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
						SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
						SendMessageW(hEdit, EM_REPLACESEL, TRUE, (LPARAM)pmi->szText);
						DestroySuggestionsMenu();
					}
				}
				else if(id >= IDI_FORMAT_COLOR_START && id <= IDI_FORMAT_COLOR_END){
					m_CurrColorsChoice = CC_TEXT;
					ColorChanged(hwnd, id);
				}
				else if(id >= IDI_FORMAT_HIGHLIGHT_START && id <= IDI_FORMAT_HIGHLIGHT_END){
					m_CurrColorsChoice = CC_HIGHLIGHT;
					ColorChanged(hwnd, id);
				}
				else if(id > IDI_SIZE_START && id <= IDI_SIZE_START + MAX_FONT_SIZE){
					//size menu
					SetREFontSize(hwnd, id - IDI_SIZE_START);
					SetFocus(hwnd);
				}
				else if(id >= SEARCH_ENGINE_ADDITION && id < SEARCH_ENGINE_ADDITION + SEARCH_ENGINE_MAX){
					HMENU			hMenu = GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_SEARCH_WEB));
					wchar_t			szBuffer[128], szText[1024], szCommand[1024 * 2];
					LPPSENG			pSeng;
					TEXTRANGEW		tr = {0};

					tr.lpstrText = szText;
					SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_EXGETSEL, 0, (LPARAM)&tr.chrg);
					SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_GETTEXTRANGE, 0, (LPARAM)&tr);
					GetMenuStringW(hMenu, id, szBuffer, 128, MF_BYCOMMAND);
					pSeng = SEngByName(g_PSengs, szBuffer);
					if(pSeng){
						wcscpy(szCommand, pSeng->query);
						_wcstrm(szText);
						wcscat(szCommand, szText);
						if(wcslen(g_DefBrowser) == 0)
							ShellExecuteW(g_hMain, L"open", szCommand, NULL, NULL, SW_SHOWDEFAULT);
						else{
							if((int)ShellExecuteW(g_hMain, NULL, g_DefBrowser, szCommand, NULL, SW_SHOWDEFAULT) <= 32){
								ShellExecuteW(g_hMain, L"open", szCommand, NULL, NULL, SW_SHOWDEFAULT);
							}
						}
					}
				}
				else if(id >= GROUP_ADDITION && id <= GROUP_ADDITION + MAX_GROUP_COUNT){
					//from groups menu
					HMENU			hGroups;
					NOTE_DATA		data;
					int				iCurrent, iNew;

					pNote = MEMNOTE(hwnd);
					hGroups = GetSubMenu(g_hNotePopUp, GetMenuPosition(g_hNotePopUp, IDM_NOTE_GROUPS));	
					if((GetMenuState(hGroups, id, MF_BYCOMMAND) & MFS_CHECKED) != MFS_CHECKED){
						iCurrent = pNote->pData->idGroup;
						iNew = id - GROUP_ADDITION;
						pNote->pData->idGroup = iNew;
						ChangeNoteLookByGroup(pNote, iNew);
						if(pNote->pFlags->fromDB){
							if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
								data.idGroup = pNote->pData->idGroup;
								WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
							}
						}
						if(g_hCPDialog){
							//SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
							SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, iCurrent, iNew);
						}
					}
				}
				else if(id >= LINKS_ADDITION && id < LINKS_ADDITION + LINKS_MAX){
					HMENU			hMenu = GetSubMenu(g_hNotePopUp, GetMenuPosition(g_hNotePopUp, IDM_LINKS));
					MENUITEMINFOW	mi = {0};
					PMITEM			pmi;

					mi.cbSize = sizeof(mi);
					mi.fMask = MIIM_DATA;
					GetMenuItemInfoW(hMenu, id, false, &mi);
					pmi = (PMITEM)mi.dwItemData;
					pNote = MemNoteById(pmi->szReserved);
					if(pNote){
						if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
							ShowNote(pNote);
					}
				}
				else if(id >= CONTACTS_ADDITION && id < CONTACTS_ADDITION + CONTACTS_MAX){
					wchar_t			szBuffer[256];
					HMENU			hMenu = GetSubMenu(g_hNotePopUp, GetMenuPosition(g_hNotePopUp, IDM_SEND_INTRANET));

					pNote = MEMNOTE(hwnd);
					GetMenuStringW(hMenu, id, szBuffer, 256, MF_BYCOMMAND);
					SendNoteToContact(hwnd, pNote, szBuffer);
				}
			}
			else
				DefWindowProcW(hwnd, WM_COMMAND, MAKEWPARAM(codeNotify, id), (LPARAM)hwndCtl);
			// else{
				// ULONG		styleID = MAKELONG(id, codeNotify);
				// BOOL			fFromStyles = FALSE;
				// PNSTYLE_TYPE	styleType = (g_RTHandles.hbSkin) ? STYLE_SKINABLE : STYLE_SKINLESS;
				// switch(styleType){
					// case STYLE_SKINABLE:
						// fFromStyles = IsIDFromStyles(styleID, g_StylesSkinnable);
						// break;
					// case STYLE_SKINLESS:
						// fFromStyles = IsIDFromStyles(styleID, g_StylesSkinless);
						// break;
				// }
				// if(fFromStyles){
					// HMENU		hStyles = GetSubMenu(g_hNotePopUp, GetMenuPosition(g_hNotePopUp, IDM_STYLES));
					// if((GetMenuState(hStyles, styleID, MF_BYCOMMAND) & MFS_CHECKED) != MFS_CHECKED){
					// }	
				// }
				// else
					// DefWindowProcW(hwnd, WM_COMMAND, MAKEWPARAM(codeNotify, id), (LPARAM)hwndCtl);
			// }
	}
}

static void Note_OnClose(HWND hwnd)
{
	PMEMNOTE		pNote = MEMNOTE(hwnd);

	if(pNote){
		//delete commands bitmap for skinless note
		if(pNote->pRTHandles && !pNote->pRTHandles->hbSkin){
			DeleteBitmap(pNote->pRTHandles->hbCommand);
			DeleteBitmap(pNote->pRTHandles->hbDelHide);
			DeleteBitmap(pNote->pRTHandles->hbSys);
		}
	}
	EnumPropsW(hwnd, PropEnumProc);
	free((P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS));
	free((P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_DH));
	free((HWND *)GetWindowLongPtrW(hwnd, OFFSET_SYS));
	DestroyWindow(hwnd);
}

static BOOL Note_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	CenterWindow(hwnd, FALSE);
	return TRUE;
}

static void Note_OnPaint(HWND hwnd)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	PAINTSTRUCT			ps;

	if(pNote){
		BeginPaint(hwnd, &ps);
		if(pNote->pRTHandles->hbSkin){
			if(DockType(pNote->pData->dockData) == DOCK_NONE){
				PaintSkinnableWindow(hwnd, ps.hdc, pNote->pRTHandles);
			}
			else{
				if(g_DockSettings.fCustSkin){
					PaintSkinnableWindow(hwnd, ps.hdc, &g_DRTHandles);
				}
				else{
					if((pNote->pAppearance->nPrivate & F_SKIN) != F_SKIN){
						PaintSkinnableWindow(hwnd, ps.hdc, pNote->pRTHandles);
					}
					else{
						PaintSkinnableWindow(hwnd, ps.hdc, &g_DRTHandles);
					}
				}
			}
		}
		else
			PaintSimpleWindow(hwnd, ps.hdc, pNote->pRTHandles);
		EndPaint(hwnd, &ps);
	}
}

static void Note_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	BOOL				dragIt = FALSE;

	if(keyFlags == MK_LBUTTON){
		pt.x = x;
		pt.y = y;
		if(pNote->pRTHandles->hbSkin){
			if(pNote->pFlags->inDrag){
				dragIt = TRUE;
			}
		}
		else{
			if(PtInRect(&pNote->pRTHandles->rcCaption, pt) && pNote->pFlags->inDrag){
				dragIt = TRUE;
			}
		}
		//drag full window
		if(dragIt && DockType(pNote->pData->dockData) == DOCK_NONE){
			ReleaseCapture();
			SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, TRUE, 0, 0);
			SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, m_fFullDrag, 0, 0);
			if(pNote->pFlags->fromDB)
				WriteNotePlacement(hwnd, TRUE);
			else
				WriteNotePlacement(hwnd, FALSE);
		}
	}
	else if(keyFlags == 0){
		//check for docked notes
		if(DockType(pNote->pData->dockData) != DOCK_NONE && !pNote->pFlags->mouseIn){
			int result = ArrowRequired(hwnd, DockType(pNote->pData->dockData), pNote->pData->dockData);
			if(result != -1){
				HWND hTemp = GetNeededArrow(result);
				// ShowWindow(hTemp, SW_SHOWNORMAL);
				SetWindowPos(hTemp, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
				pNote->pFlags->mouseIn = TRUE;
				TRACKMOUSEEVENT	tm;
				tm.cbSize = sizeof(tm);
				tm.hwndTrack = hwnd;
				tm.dwHoverTime = 0;
				tm.dwFlags = TME_LEAVE;
				TrackMouseEvent(&tm);
			}
		}
	}
}

void UnrollNote(PMEMNOTE pNote){
	RECT		rc, rcDesktop;
	NOTE_DATA	data;

	if(g_NoteSettings.rollOnDblClick){
		if(!pNote->pRTHandles->hbSkin && DockType(pNote->pData->dockData) == DOCK_NONE && pNote->pData->visible){
			if(pNote->pRTHandles->rolled){
				SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcDesktop, FALSE);
				CopyRect(&rc, &pNote->pData->rcp);
				if(rc.bottom > rcDesktop.bottom)
					rc.top = rcDesktop.bottom - (rc.bottom - rc.top);
				MoveWindow(pNote->hwnd, rc.left, rc.top, rc.right - rc.left, pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top, TRUE);

				pNote->pRTHandles->rolled = FALSE;
				pNote->pData->rolled = FALSE;

				RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
				if(pNote->pFlags->fromDB){
					if(!GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
						memcpy(&data, pNote->pData, sizeof(NOTE_DATA));
					}
					data.rolled = FALSE;
					WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				}
			}
		}
	}
}

void FitToCaption(PMEMNOTE pNote, BOOL copyRects){
	RECT		rc;
	HDC			hdc = GetDC(pNote->hwnd);
	SIZE		sz;
	HFONT		hFont;
	int			width, height;
	HWND 		hToolbar;
	NOTE_DATA	data;

	m_DblClickSize = TRUE;
	if(copyRects){
		GetWindowRect(pNote->hwnd, &rc);
		CopyRect(&pNote->pRTHandles->rcSize, &rc);
		CopyRect(&pNote->pData->rcp, &rc);
	}
	hFont = SelectFont(hdc, pNote->pRTHandles->hFCaption);
	GetTextExtentPoint32W(hdc, pNote->pData->szName, wcslen(pNote->pData->szName), &sz);
	if(!DelHideInvisible())
		width = sz.cx + (pNote->pRTHandles->rcDH.right - pNote->pRTHandles->rcDH.left) + 8;
	else
		width = sz.cx + 8;
	SelectFont(hdc, hFont);
	ReleaseDC(pNote->hwnd, hdc);
	MoveWindow(pNote->hwnd, pNote->pData->rcp.left, pNote->pData->rcp.top, width, pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top, TRUE);

	if(!DelHideInvisible()){
		hToolbar = (HWND)GetPropW(pNote->hwnd, PH_DELHIDE);
		height = pNote->pRTHandles->rcDH.bottom - pNote->pRTHandles->rcDH.top;
		MoveWindow(hToolbar, width - (pNote->pRTHandles->rcDH.right - pNote->pRTHandles->rcDH.left), pNote->pRTHandles->rcDH.top, pNote->pRTHandles->rcDH.right - pNote->pRTHandles->rcDH.left, height, TRUE);
		m_DblClickSize = FALSE;
	}

	pNote->pRTHandles->rolled = TRUE;
	pNote->pData->rolled = TRUE;

	if(pNote->pFlags->fromDB){
		if(!GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			memcpy(&data, pNote->pData, sizeof(NOTE_DATA));
		}
		data.rolled = TRUE;
		WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
	}
}

void RollNote(PMEMNOTE pNote){
	RECT		rc;
	NOTE_DATA	data;

	if(g_NoteSettings.rollOnDblClick){
		if(!pNote->pRTHandles->hbSkin && DockType(pNote->pData->dockData) == DOCK_NONE && pNote->pData->visible){
			if(!pNote->pRTHandles->rolled){
				if(!IsBitOn(g_NoteSettings.reserved1, SB1_FIT_TO_CAPTION)){
					m_DblClickSize = TRUE;
					GetWindowRect(pNote->hwnd, &rc);
					CopyRect(&pNote->pRTHandles->rcSize, &rc);
					CopyRect(&pNote->pData->rcp, &rc);
					MoveWindow(pNote->hwnd, rc.left, rc.top, rc.right - rc.left, pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top, TRUE);
					m_DblClickSize = FALSE;

					pNote->pRTHandles->rolled = TRUE;
					pNote->pData->rolled = TRUE;

					if(pNote->pFlags->fromDB){
						if(!GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
							memcpy(&data, pNote->pData, sizeof(NOTE_DATA));
						}
						data.rolled = TRUE;
						WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
					}
				}
				else{
					FitToCaption(pNote, TRUE);
				}
				RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
			}
		}
	}
	
}

// static void ReApplayInterLinks(HWND hEdit){
	// CHARFORMAT2		ch1 = {0}, ch2 = {0};
	// long			len;
	// GETTEXTLENGTHEX	gtl = {0};
	// CHARRANGE		chr = {0};

	// ch1.cbSize = sizeof(ch1);
	// ch2.cbSize = sizeof(ch2);
	// ch1.dwMask = CFM_ANIMATION | CFM_LCID | CFM_STYLE | CFM_KERNING;
	// ch2.dwMask = CFM_LINK;
	// ch2.dwEffects = CFE_LINK;

	// chr.cpMin = 0; chr.cpMax = 1;
	// gtl.flags = GTL_DEFAULT | GTL_NUMCHARS;
	// gtl.codepage = 1200;

	// len = SendMessageW(hEdit, EM_GETTEXTLENGTHEX,(WPARAM)&gtl, 0);
	// for(long i = 0; i < len; i++, chr.cpMin++, chr.cpMax++){
		// SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
		// RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &ch1);
		// if(ch1.bAnimation){
			// RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &ch2);
		// }
	// }
	// chr.cpMin = 0, chr.cpMax = 0;
	// SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
// }

// static void ApplyInterLink(HWND hwnd){
	// HWND			hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
	// PMEMNOTE		pNote = MemNoteByHwnd(hwnd);
	// CHARFORMAT2		chr = {0};
	// LPPLINK			pLink;
	// BYTE			order;
	
	// chr.cbSize = sizeof(chr);
	// chr.dwMask = CFM_ANIMATION;
	// RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chr);
	// if(chr.bAnimation > 0){
		// pLink = PLinksItem(pNote->pLinks, chr.bAnimation);
		// wcscpy(pLink->id, g_TempLink);
	// }
	// else{
		// order = PLinksLastOrder(pNote->pLinks) + 1;
		// ZeroMemory(&chr, sizeof(chr));
		// chr.cbSize = sizeof(chr);
		// chr.dwMask = CFM_ANIMATION | CFM_LINK;
		// chr.dwEffects = CFE_LINK;
		// chr.bAnimation = order;
		// RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chr);
		// pNote->pLinks = PLinksAdd(pNote->pLinks, order, g_TempLink);
	// }
// }

static int ArrowRequired(HWND hwnd, dock_type dockType, int dockData){
	RECT			rcDesktop, rc, rcTray, rcReal;
	int				mcount, index;
	short 			count = DHeader(dockType)->count;
	HWND			hTray;

	GetWindowRect(hwnd, &rc);
	GetClientRect(GetDesktopWindow(), &rcDesktop);
	mcount = GetSystemMetrics(SM_CMONITORS);
	if(mcount > 1){
		rcDesktop.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rcDesktop.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	hTray = FindWindow("Shell_TrayWnd", NULL);
	GetWindowRect(hTray, &rcTray);
	SubtractRect(&rcReal, &rcDesktop, &rcTray);

	index = DockIndex(dockData);
	switch(dockType){
		case DOCK_LEFT:
			if(rc.top < rcReal.top){
				return DA_LEFT_UP;
			}
			else if(rc.bottom > rcReal.bottom){
				return DA_LEFT_DOWN;
			}
			else if(index < count - 1 && rc.bottom == rcReal.bottom){
				return DA_LEFT_DOWN;
			}
			else if(index > 0 && rc.top == rcReal.top){
				return DA_LEFT_UP;
			}
			break;
		case DOCK_TOP:
			if(rc.left < rcReal.left){
				return DA_TOP_LEFT;
			}
			else if(rc.right > rcReal.right){
				return DA_TOP_RIGHT;
			}
			else if(index < count - 1 && rc.right == rcReal.right){
				return DA_TOP_RIGHT;
			}
			else if(index > 0 && rc.left == rcReal.left){
				return DA_TOP_LEFT;
			}
			break;
		case DOCK_RIGHT:
			if(rc.top < rcReal.top){
				return DA_RIGHT_UP;
			}
			else if(rc.bottom > rcReal.bottom){
				return DA_RIGHT_DOWN;
			}
			else if(index < count - 1 && rc.bottom == rcReal.bottom){
				return DA_RIGHT_DOWN;
			}
			else if(index > 0 && rc.top == rcReal.top){
				return DA_RIGHT_UP;
			}
			break;
		case DOCK_BOTTOM:
			if(rc.left < rcReal.left){
				return DA_BOTTOM_LEFT;
			}
			else if(rc.right > rcReal.right){
				return DA_BOTTOM_RIGHT;
			}
			else if(index < count - 1 && rc.right == rcReal.right){
				return DA_BOTTOM_RIGHT;
			}
			else if(index > 0 && rc.left == rcReal.left){
				return DA_BOTTOM_LEFT;
			}
			break;
		case DOCK_NONE:
			break;
	}
	
	return -1;
}

static void OnResizeNote(HWND hwnd, LPRECT lpRc){

	RECT				rc;
	PMEMNOTE			pNote = MEMNOTE(hwnd);

	if(pNote->pRTHandles && pNote->pRTHandles->hbSkin){
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			if(g_DockSettings.fCustSkin){
				memcpy(pNote->pRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
			}
		}
	}

	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
		CopyRect(&rc, &pNote->pRTHandles->rcEdit);
	else
		CopyRect(&rc, lpRc);
	//resize edit box when skin is changed
	MoveWindow((HWND)GetPropW(hwnd, PH_EDIT), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	//move commands bar
	if(pNote->pRTHandles && pNote->pRTHandles->hbSkin)
		SetNoteCommandsBar(hwnd, pNote->pRTHandles);
}

static void SetNoteCommandsBar(HWND hwnd, P_NOTE_RTHANDLES pH){
	P_NCMD_BAR			pC, pDH;
	int					width, height;
	HRGN				rBase, rMark;
	HWND				hToolbar, *phMarks;
	BITMAP				bm;

	hToolbar = (HWND)GetPropW(hwnd, PH_COMMANDS);
	if(pH->hbSkin){
		rBase = RegionFromSkin(pH->hbCommand, pH->crMask);
		SetWindowRgn(hToolbar, rBase, TRUE);
	}
	if(!pH->vertTbr){
		height = pH->rcCommands.bottom - pH->rcCommands.top;
		width = (pH->rcCommands.right - pH->rcCommands.left) / CMD_TBR_BUTTONS;
	}
	else{
		height = (pH->rcCommands.bottom - pH->rcCommands.top) / CMD_TBR_BUTTONS;
		width = pH->rcCommands.right - pH->rcCommands.left;
	}
	if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDETOOLBAR)){
		if(!pH->hbSkin)
			MoveWindow(hToolbar, pH->rcGrip.left - (pH->rcCommands.right - pH->rcCommands.left), pH->rcCommands.top, pH->rcCommands.right - pH->rcCommands.left, pH->rcCommands.bottom - pH->rcCommands.top, false);
		else
			MoveWindow(hToolbar, pH->rcCommands.left, pH->rcCommands.top, pH->rcCommands.right - pH->rcCommands.left, pH->rcCommands.bottom - pH->rcCommands.top, FALSE);
	}
	else{
		if(!pH->hbSkin)
			MoveWindow(hToolbar, pH->rcGrip.left - (pH->rcCommands.right - pH->rcCommands.left), pH->rcCommands.top, pH->rcCommands.right - pH->rcCommands.left, pH->rcCommands.bottom - pH->rcCommands.top, true);
		else
			MoveWindow(hToolbar, pH->rcCommands.left, pH->rcCommands.top, pH->rcCommands.right - pH->rcCommands.left, pH->rcCommands.bottom - pH->rcCommands.top, TRUE);
	}
	pC = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS);
	for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
		HWND hw = pC->hCmd;
		if(!pH->vertTbr)
			MoveWindow(hw, width * i, 0, width, height, TRUE);		
		else
			MoveWindow(hw, 0, height * i, width, height, TRUE);		
		pC++;
	}
	
	//set delete/hide toolbar
	hToolbar = (HWND)GetPropW(hwnd, PH_DELHIDE);
	if(DelHideInvisible()){
		ShowWindow(hToolbar, SW_HIDE);
	}
	else{
		if(pH->hbSkin){
			SetWindowRgn(hToolbar, RegionFromSkin(pH->hbDelHide, pH->crMask), TRUE);
		}
		height = pH->rcDH.bottom - pH->rcDH.top;
		width = (pH->rcDH.right - pH->rcDH.left) / 2;
		MoveWindow(hToolbar, pH->rcDH.left, pH->rcDH.top, pH->rcDH.right - pH->rcDH.left, height, TRUE);
		// MoveWindow(hToolbar, pH->rcDH.left + width, pH->rcDH.top, pH->rcDH.right - pH->rcDH.left, height, TRUE);
		pDH = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_DH);
		for(int i = 0; i < CMD_SYS_BUTTONS; i++){
			MoveWindow(pDH->hCmd, width * i, 0, width, height, TRUE);
			if(i == CMD_SYS_BUTTONS - 1 && IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_DELETE)){
				ShowWindow(pDH->hCmd, SW_HIDE);
				if(!pH->hbSkin)
					MoveWindow((--pDH)->hCmd, width * i, 0, width, height, TRUE);
				else
					MoveWindow(hToolbar, pH->rcDH.left + width * i, pH->rcDH.top, width, height, TRUE);
			}
			else if(i == 0 && IsBitOn(g_NextSettings.flags1, SB3_HIDE_HIDE)){
				ShowWindow(pDH->hCmd, SW_HIDE);
			}
			pDH++;
		}
	}
	//set note marks buttons
	phMarks = (HWND *)GetWindowLongPtrW(hwnd, OFFSET_SYS);
	if(phMarks && pH->hbSys && pH->hbSkin){
		HDC		hdc, hdcDest, hdcSrc;
		HBITMAP	hbmp, hold, ho;
		GetObject(pH->hbSys, sizeof(bm), &bm);
		
		hdc = GetDC(hwnd);
		hdcDest = CreateCompatibleDC(hdc);
		hdcSrc = CreateCompatibleDC(hdc);
		ho = SelectBitmap(hdcSrc, pH->hbSys);
		for(int i = 0; i < pH->marks; i++){
			hbmp = CreateCompatibleBitmap(hdc, bm.bmWidth / pH->marks, bm.bmHeight);
			hold = SelectBitmap(hdcDest, hbmp);
			BitBlt(hdcDest, 0, 0, bm.bmWidth / pH->marks, bm.bmHeight, hdcSrc, i * (bm.bmWidth / pH->marks), 0, SRCCOPY);
			SelectBitmap(hdcDest, hold);
			rMark = RegionFromSkin(hbmp, pH->crMask);
			SetWindowPos(*phMarks, 0, 0, 0, bm.bmWidth / pH->marks, bm.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
			SetWindowRgn(*phMarks++, rMark, TRUE);
			DeleteBitmap(hbmp);
		}
		SelectBitmap(hdcSrc, ho);
		DeleteDC(hdcDest);
		DeleteDC(hdcSrc);
		ReleaseDC(hwnd, hdc);
	}
}

static LRESULT CALLBACK Grip_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, Grip_OnMouseMove);

		case WM_PAINT:
		{
			PAINTSTRUCT			ps;
			PMEMNOTE			pNote = MEMNOTE(GetParent(hwnd));

			BeginPaint(hwnd, &ps);
			if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustColor)
				PaintGrip(hwnd, ps.hdc, g_DockSettings.crWindow);
			else
				PaintGrip(hwnd, ps.hdc, pNote->pAppearance->crWindow);
			EndPaint(hwnd, &ps);
			return FALSE;
		}
		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
		}
}

static void Grip_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(GetParent(hwnd));

	if(keyFlags == MK_LBUTTON && DockType(pNote->pData->dockData) == DOCK_NONE){
		ReleaseCapture();
		SendMessageW(GetParent(hwnd), WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
		if(pNote->pFlags->fromDB)
			WriteNotePlacement(GetParent(hwnd), TRUE);
		else
			WriteNotePlacement(GetParent(hwnd), FALSE);
	}
}

static void ShowHideNoteToolbar(HWND hNote){
	P_NCMD_BAR			pC;

	pC = (P_NCMD_BAR)GetWindowLongPtrW(hNote, OFFSET_COMMANDS);
	if(pC){
		if(!IsBitOn(g_NoteSettings.reserved1, SB1_HIDETOOLBAR))
			ShowWindow((HWND)GetPropW(hNote, PH_COMMANDS), SW_SHOW);
	}
}

static LRESULT CALLBACK Edit_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PMEMNOTE			pNote;
	P_NCMD_BAR			pC;
	int					idMod = 0, key = 0, id;
	// CReCallback			* pCallback = NULL;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_RBUTTONUP, Edit_OnRButtonUp);
		case WM_CHAR:
			if(wParam == '\t' && ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000))
				return FALSE;
			else
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			//get modifiers
			idMod = GetModifiers(NULL);
			//get the very first other key pressed
			key = GetOtherKey(NULL);
			if((idMod == 0 && (key >= VK_F1 && key <= VK_F24)) || (idMod && key)){
				id = GetHotkeyID(g_EditHotkeys, COUNT_EDIT_HOTKEYS, idMod, key);
				if(id > 0){
					if(IsNoteHotkeyAvailable(MemNoteByHwnd(GetParent(hwnd)), hwnd, id)){
						SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(id, 0), 0);
						return FALSE;
					}
				}
				else{
					id = GetHotkeyID(g_NoteHotkeys, NELEMS(m_NMenus), idMod, key);
					if(id > 0){
						if(IsNoteHotkeyAvailable(MemNoteByHwnd(GetParent(hwnd)), hwnd, id)){
							SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(id, 0), 0);
							return FALSE;
						}
					}
				}
			}
			if(idMod == MOD_CONTROL){
				HWND		hToolbar;
				switch(wParam){
					case VK_B:
						hToolbar = (HWND)GetPropW(GetParent(hwnd), PH_COMMANDS);
						PostMessageW(hToolbar, WM_COMMAND, MAKEWPARAM(CMD_BOLD + 5000, 0), 0);
						return FALSE;
					case VK_I:
						hToolbar = (HWND)GetPropW(GetParent(hwnd), PH_COMMANDS);
						PostMessageW(hToolbar, WM_COMMAND, MAKEWPARAM(CMD_ITALIC + 5000, 0), 0);
						return FALSE;
					case VK_U:
						hToolbar = (HWND)GetPropW(GetParent(hwnd), PH_COMMANDS);
						PostMessageW(hToolbar, WM_COMMAND, MAKEWPARAM(CMD_UNDERLINE + 5000, 0), 0);
						return FALSE;
					case VK_K:
						hToolbar = (HWND)GetPropW(GetParent(hwnd), PH_COMMANDS);
						PostMessageW(hToolbar, WM_COMMAND, MAKEWPARAM(CMD_STRIKETHROUGH + 5000, 0), 0);
						return FALSE;
					case VK_V:
						if(IsClipboardFormatAvailable(CF_BITMAP)){
						// if(IsClipboardFormatAvailable(CF_BITMAP) 
						// || IsClipboardFormatAvailable(CF_ENHMETAFILE)){
							PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_PASTE, 0), 0);
							return FALSE;
						}
						else
							return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
					case VK_C:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_COPY, 0), 0);
						return FALSE;
					case VK_P:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_PRINT_NOTE, 0), 0);
						return FALSE;
					case VK_S:{
						PMEMNOTE	pNote = MEMNOTE(GetParent(hwnd));
						if(pNote && pNote->pFlags->maskChanged && !pNote->pFlags->saved)
							PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
						return FALSE;
					}
					case VK_F:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SEARCH_NOTE, 0), 0);
						return FALSE;
					case VK_H:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_REPLACE_NOTE, 0), 0);
						return FALSE;
					default:
						return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
				}
			}
			else{
				if((GetKeyState(VK_SHIFT) & 0x8000) != 0x8000 && (GetKeyState(VK_MENU) & 0x8000) != 0x8000){
					if(wParam == VK_F3){
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SEARCH_NOTE_NEXT, 0), 0);
						return FALSE;
					}
				}
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_KILLFOCUS:
			if((HWND)wParam != GetParent(hwnd)){
				if(g_NoteSettings.transAllow){
					//return transparency
					pNote = MEMNOTE(GetParent(hwnd));
					if(pNote->pAddNApp->transValue != 0)
						SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, pNote->pAddNApp->transValue, LWA_COLORKEY | LWA_ALPHA);
					else
						SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, g_NoteSettings.transValue, LWA_COLORKEY | LWA_ALPHA);
					pNote->pFlags->transparent = TRUE;
				}
				pC = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(hwnd), OFFSET_COMMANDS);
				if(pC){
					if(!g_hComboDialog || (g_hComboDialog && (HWND)wParam != g_hComboDialog)){
						if(!IsBitOn(g_NoteSettings.reserved1, SB1_HIDETOOLBAR)){
							//hide toolbar and update toolbar rectangle on note
							RECT	rc;
							GetWindowRect((HWND)GetPropW(GetParent(hwnd), PH_COMMANDS), &rc);
							ShowWindow((HWND)GetPropW(GetParent(hwnd), PH_COMMANDS), SW_HIDE);
							MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc, 2);
							RedrawWindow(GetParent(hwnd), &rc, NULL, RDW_INVALIDATE);
						}
					}
				}
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_SETFOCUS:
			RemoveBell(GetParent(hwnd));
			pNote = MEMNOTE(GetParent(hwnd));
			if(pNote->pFlags->transparent){
				//draw window opaque 
				SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
			ShowHideNoteToolbar(GetParent(hwnd));
			// pC = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(hwnd), OFFSET_COMMANDS);
			// if(pC){
				// if(!IsBitOn(g_NoteSettings.reserved1, SB_HIDETOOLBAR))
					// ShowWindow((HWND)GetPropW(GetParent(hwnd), PH_COMMANDS), SW_SHOW);
			// }
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_VSCROLL:
		case WM_MOUSEWHEEL:
			pNote = MEMNOTE(GetParent(hwnd));
			if(pNote->pFlags->transparent){
				//draw window opaque 
				SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_COPY:
			m_InCopy = TRUE;
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_PAINT:{
			LRESULT		result = CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
			if(g_Spell.spellAuto){
				AutoCheckRESpelling(hwnd);
			}
			return result;
		}
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static void Edit_OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	HMENU			hMenu = NULL;
	CHARRANGE		chr;
	PMEMNOTE		pNote = MEMNOTE(GetParent(hwnd));
	PSUGGESTION		ps = NULL;
	POINT			pt;

	pt.x = x;
	pt.y = y;
	if(g_Spell.spellAuto){
		ps = SuggestionsForRightClick(hwnd, pt);
	}
	if(!ps){
		hMenu = g_hEditPopUp;
		m_hTempMenu = hMenu;
		if(SendMessageW(hwnd, EM_CANUNDO, 0, 0))
			EnableMenuItem(hMenu, IDM_UNDO, MF_BYCOMMAND | MF_ENABLED);
		else
			EnableMenuItem(hMenu, IDM_UNDO, MF_BYCOMMAND | MF_GRAYED);

		if(SendMessageW(hwnd, EM_CANREDO, 0, 0))
			EnableMenuItem(hMenu, IDM_REDO, MF_BYCOMMAND | MF_ENABLED);
		else
			EnableMenuItem(hMenu, IDM_REDO, MF_BYCOMMAND | MF_GRAYED);

		HMENU	h1 = GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_SPELL_CHECK));
		int		count = GetMenuItemCount(h1);
		BOOL	fEnable = false;

		for(int i = 6; i < count; i++){
			if((GetMenuState(h1, i, MF_BYPOSITION) & MF_CHECKED) == MF_CHECKED){
				fEnable = true;
				break;
			}
		}

		if(fEnable){
			EnableMenuItem(hMenu, IDM_SPELL_NOW, MF_BYCOMMAND | MF_ENABLED);
		}
		else{
			EnableMenuItem(hMenu, IDM_SPELL_NOW, MF_BYCOMMAND | MF_GRAYED);
		}

		if(g_Spell.isCheckLib){
			EnableMenuItem(hMenu, IDM_SPELL_CHECK, MF_BYCOMMAND | MF_ENABLED);
		}
		else{
			EnableMenuItem(hMenu, IDM_SPELL_CHECK, MF_BYCOMMAND | MF_GRAYED);
		}

		if(g_Spell.spellAuto){
			CheckMenuItem(hMenu, IDM_SPELL_AUTO, MF_CHECKED | MF_BYCOMMAND);
		}
		else{
			CheckMenuItem(hMenu, IDM_SPELL_AUTO, MF_UNCHECKED | MF_BYCOMMAND);
		}

		ZeroMemory(&chr, sizeof(chr));
		SendMessageW(hwnd, EM_EXGETSEL, 0, (LPARAM)&chr);
		if(chr.cpMin == chr.cpMax){
			EnableMenuItem(hMenu, IDM_CUT, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_COPY_AS_TEXT, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_SEARCH_WEB, MF_BYCOMMAND | MF_GRAYED);
			// //internal links
			// EnableMenuItem(hMenu, IDM_ADD_LINK_INTERNAL, MF_BYCOMMAND | MF_GRAYED);
			// EnableMenuItem(hMenu, IDM_REM_LINK_INTERNAL, MF_BYCOMMAND | MF_GRAYED);
		}
		else{
			if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
				EnableMenuItem(hMenu, IDM_CUT, MF_BYCOMMAND | MF_GRAYED);
			else
				EnableMenuItem(hMenu, IDM_CUT, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, IDM_COPY_AS_TEXT, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, IDM_SEARCH_WEB, MF_BYCOMMAND | MF_ENABLED);
			BuildSEngsMenu();
		}

		if(SendMessageW(hwnd, EM_CANPASTE, CF_TEXT, 0) 
		|| SendMessageW(hwnd, EM_CANPASTE, CF_UNICODETEXT, 0) 
		|| SendMessageW(hwnd, EM_CANPASTE, CF_OEMTEXT, 0) 
		|| IsClipboardFormatAvailable(CF_BITMAP) 
		|| IsClipboardFormatAvailable(CF_ENHMETAFILE)){
			if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
				EnableMenuItem(hMenu, IDM_PASTE, MF_BYCOMMAND | MF_GRAYED);
			else
				EnableMenuItem(hMenu, IDM_PASTE, MF_BYCOMMAND | MF_ENABLED);
		}
		else
			EnableMenuItem(hMenu, IDM_PASTE, MF_BYCOMMAND | MF_GRAYED);

		if(SendMessageW(hwnd, EM_CANPASTE, CF_TEXT, 0) 
		|| SendMessageW(hwnd, EM_CANPASTE, CF_UNICODETEXT, 0) 
		|| SendMessageW(hwnd, EM_CANPASTE, CF_OEMTEXT, 0)){
			if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
				EnableMenuItem(hMenu, IDM_PASTE_AS_TEXT, MF_BYCOMMAND | MF_GRAYED);
			else
				EnableMenuItem(hMenu, IDM_PASTE_AS_TEXT, MF_BYCOMMAND | MF_ENABLED);
		}
		else
			EnableMenuItem(hMenu, IDM_PASTE_AS_TEXT, MF_BYCOMMAND | MF_GRAYED);

		if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
			EnableMenuItem(hMenu, IDM_REPLACE_NOTE, MF_BYCOMMAND | MF_GRAYED);
		else
			EnableMenuItem(hMenu, IDM_REPLACE_NOTE, MF_BYCOMMAND | MF_ENABLED);

		CheckBulletsMenuItems(hwnd);
	}
	else{
		m_hMenuSuggestions = CreateSuggestionsMenu(ps);
		hMenu = m_hMenuSuggestions;
		FreeSuggestions(&ps);
	}
	ShowPopUp(GetParent(hwnd), hMenu);
}

static HMENU CreateSuggestionsMenu(PSUGGESTION ps){
	wchar_t				szTemp[256];
	HMENU				hMenu = CreatePopupMenu();
	MITEM				mit = {0};
	PSUGGESTION			p;
	int					id = SUGG_START, i1, i2;

	mit.xPos = -1;
	mit.yPos = -1;

	DestroySuggestionsMenu();

	i1 = ps->value;
	i2 = i1 + wcslen(ps->word);
	wcscpy(szTemp, ps->word);
	p = ps;

	//always add Add To Dictionary
	mit.id = id++;
	wcscpy(mit.szText, g_Strings.AddToDictionary);
	wcscpy(mit.szReserved, szTemp);
	AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, hMenu, 0, true);
	//add separator
	AppendMenuW(hMenu, MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR, 0, NULL);
	if(p){
		if(!p->next){
			//if there is no suggestions - add No suggestions menu
			mit.id = id++;
			wcscpy(mit.szText, g_Strings.NoSuggestion);
			AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
			SetMenuItemProperties(&mit, hMenu, 2, true);
			EnableMenuItem(hMenu, 2, MF_BYPOSITION | MF_GRAYED);
		}
		else{
			//add suggestions
			p = p->next;
			while(p){
				mit.id = id++;
				wcscpy(mit.szText, p->word);
				mit.xCheck = i1;
				mit.yCheck = i2;
				AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
				SetMenuItemProperties(&mit, hMenu, GetMenuPosition(hMenu, mit.id), true);
				p = p->next;
			}
		}
	}

	// do{
		// mit.id = id++;				
		// if(count == 0){
			// wcscpy(mit.szText, g_Strings.NoSuggestion);
		// }
		// else if(count == 1){
			// wcscpy(mit.szText, g_Strings.AddToDictionary);
			// wcscpy(mit.szReserved, szTemp);
			// AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
			// SetMenuItemProperties(&mit, hMenu, 1, true);
			// goto _next;
		// }
		// else{
			// wcscpy(mit.szText, p->word);
			// mit.xCheck = i1;
			// mit.yCheck = i2;
		// }
		// AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
		// SetMenuItemProperties(&mit, hMenu, GetMenuPosition(hMenu, mit.id), true);
		// p = p->next;
// _next:
		// count++;
	// }while(p);
	// if(count == 1)
		// EnableMenuItem(hMenu, 0, MF_BYPOSITION | MF_GRAYED);
	// else{
		// FreeSingleMenu(hMenu, 0);
		// DeleteMenu(hMenu, 0, MF_BYPOSITION);
		// InsertMenuW(hMenu, 1, MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR, 0, NULL);
	// }
	return hMenu;
}

static void DestroySuggestionsMenu(void){
	int				count;

	if(m_hMenuSuggestions){
		count = GetMenuItemCount(m_hMenuSuggestions);
		for(int i = count - 1; i >= 0; i--){
			FreeSingleMenu(m_hMenuSuggestions, i);
			DeleteMenu(m_hMenuSuggestions, i, MF_BYPOSITION);
		}
		DestroyMenu(m_hMenuSuggestions);
		m_hMenuSuggestions = NULL;
	}
}

static void ApplyNewSkin(HWND hNote, P_NOTE_RTHANDLES pH){

	RECT				rc;
	BITMAP				bm;

	GetWindowRect(hNote, &rc);
	GetObjectW(pH->hbSkin, sizeof(bm), &bm);
	//resize controls on note window
	SendMessage(hNote, PNM_RESIZE, (WPARAM)&pH->rcEdit, 0);
	//resize note window
	MoveWindow(hNote, rc.left, rc.top, bm.bmWidth, bm.bmHeight, TRUE);
	//update tooltip
	SendMessageW(hNote, PNM_TT_UPDATE, 0, 0);
}

static BOOL ApplyNoteChanges(HWND hwnd){
	PMEMNOTE			pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	int					wChanged = 0, index, tempID;
	BOOL				fReload = FALSE;
	wchar_t				szTemp[128];

	if((pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT)
		wChanged |= F_RTFONT;
	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
		wChanged |= F_SKIN;
	if((pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT)
		wChanged |= F_C_FONT;
	if((pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR)
		wChanged |= F_C_COLOR;
	if((pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR)
		wChanged |= F_B_COLOR;
	if(ScheduleChanged(pNote))
		wChanged |= F_SCHEDULE;
	//check alarm type
	if(!IsBitOn(pNote->pSchedule->params, SP_USE_TTS)){
		if(IsDlgButtonChecked(m_Dlgs.hSchedule, IDC_OPT_USE_TTS) == BST_CHECKED){
			//file before and text after
			BitOn(&pNote->pSchedule->params, SP_USE_TTS);
			wChanged |= F_SCHEDULE;
		}
	}
	else{
		if(IsDlgButtonChecked(m_Dlgs.hSchedule, IDC_OPT_USE_WAV_FILE) == BST_CHECKED){
			//text before and file after
			BitOff(&pNote->pSchedule->params, SP_USE_TTS);
			wChanged |= F_SCHEDULE;
		}
	}
	//check alarm sound
	if(IsDlgButtonChecked(m_Dlgs.hSchedule, IDC_OPT_USE_WAV_FILE) == BST_CHECKED){
		//check file name
		tempID = IDC_CBO_CUST_SOUND;
	}
	else{
		//check voice name
		tempID = IDC_CBO_NOTE_VOICES;
	}
	index = SendDlgItemMessageW(m_Dlgs.hSchedule, tempID, CB_GETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dlgs.hSchedule, tempID, CB_GETLBTEXT, index, (LPARAM)szTemp);
	if(wcscmp(pNote->pSchedule->szSound, szTemp) != 0){
		wcscpy(pNote->pSchedule->szSound, szTemp);
		wChanged |= F_SCHEDULE;
	}

	if(wChanged){
		//if appearance changed
		if((wChanged & F_SKIN) == F_SKIN 
		|| (wChanged & F_RTFONT) == F_RTFONT 
		|| (wChanged & F_C_FONT) == F_C_FONT
		|| (wChanged & F_C_COLOR) == F_C_COLOR
		|| (wChanged & F_B_COLOR) == F_B_COLOR){
			memcpy(pNote->pAppearance, &m_TNAppearance, sizeof(NOTE_APPEARANCE));
		}
		if((wChanged & F_SKIN) == F_SKIN){	//skin changed
			if(pNote->pRTHandles->crMask == m_TNHandles.crMask){
				if((pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN)			
					//if previous skin was from separate bitmap
					GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, TRUE);
				else
					GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, FALSE);
				pNote->pAppearance->nPrivate |= F_SKIN;
				if(pNote->pData->visible){
					ApplyNewSkin(pNote->hwnd, pNote->pRTHandles);
				}
				if(pNote->pFlags->fromDB){
					WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, pNote->pAppearance, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
				}
				else{
					pNote->pFlags->saved = FALSE;
				}
			}
			else{
				fReload = TRUE;
			}
		}
		
		if((wChanged & F_C_FONT) == F_C_FONT){
			if((pNote->pAppearance->nPrivate & F_C_FONT) == F_C_FONT)
				DeleteFont(pNote->pRTHandles->hFCaption);
			pNote->pRTHandles->hFCaption = CreateFontIndirectW(&pNote->pAppearance->lfCaption);
			pNote->pAppearance->nPrivate |= F_C_FONT;
			if(pNote->pFlags->fromDB){
				SaveNewCFont(pNote, &pNote->pAppearance->lfCaption);
			}
			else{
				pNote->pFlags->saved = FALSE;
			}
		}

		if((wChanged & F_C_COLOR) == F_C_COLOR){
			pNote->pAppearance->nPrivate |= F_C_COLOR;
			if(pNote->pFlags->fromDB){
				SaveNewCColor(pNote, pNote->pAppearance->crCaption);
			}
			else{
				pNote->pFlags->saved = FALSE;
			}
		}

		if((wChanged & F_B_COLOR) == F_B_COLOR){
			pNote->pAppearance->nPrivate |= F_B_COLOR;
			if(pNote->pFlags->fromDB){
				SaveNewBColor(pNote, pNote->pAppearance->crWindow);
			}
			else{
				pNote->pFlags->saved = FALSE;
			}
		}

		if((wChanged & F_SCHEDULE) == F_SCHEDULE){	//schedule changed
			if(pNote->pFlags->fromDB){
				//just save new schedule without prompting
				WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
			}
			else{
				pNote->pFlags->maskChanged |= F_SCHEDULE;
				pNote->pFlags->saved = FALSE;
			}
		}

		//recreate toolbar for skinless window
		if((wChanged & F_C_FONT) == F_C_FONT || (wChanged & F_C_COLOR) == F_C_COLOR || (wChanged & F_B_COLOR) == F_B_COLOR) {
			if(pNote->pData->visible){
				CreateSkinlessToolbarBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
				CreateSkinlessSysBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
				CreateSimpleMarksBitmap(pNote->hwnd, &pNote->pRTHandles->hbSys, MARKS_COUNT);
				if(!g_RTHandles.hbSkin){
					MeasureSkinlessNoteRectangles(pNote, pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left, pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top, pNote->pData->szName);
					HWND hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
					MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, (pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left) - 2, pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
				}
			}
		}
		//set new richedit background color
		if((wChanged & F_B_COLOR) == F_B_COLOR){
			if(pNote->pData->visible){
				RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
			}
		}
		if(pNote->pData->visible){
			RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
			ShowNoteMarks(pNote->hwnd);
		}
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
		if(g_hOverdueDialog){
			SendMessageW(g_hOverdueDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
	//check additional appearance
	if(pNote->pAddNApp->transValue != m_TAddAppearance.transValue){
		pNote->pAddNApp->transValue = m_TAddAppearance.transValue;
		if(pNote->pFlags->fromDB){
			ADDITIONAL_NAPP		addapp;
			GetPrivateProfileStructW(pNote->pFlags->id, S_ADD_APPEARANE, &addapp, sizeof(ADDITIONAL_NAPP), g_NotePaths.DataFile);
			addapp.transValue = pNote->pAddNApp->transValue;
			WritePrivateProfileStructW(pNote->pFlags->id, S_ADD_APPEARANE, &addapp, sizeof(ADDITIONAL_NAPP), g_NotePaths.DataFile);
		}
		else{
			pNote->pFlags->saved = FALSE;
		}
	}
	return fReload;
}

BOOL CALLBACK Adjust_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMHDR		* nm;

	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_CLOSE, Adjust_OnClose);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Adjust_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, Adjust_OnCommand);
	case WM_NOTIFY:
		nm = (NMHDR *)lParam;
		if(nm->code == TCN_SELCHANGE){
			//show/hide controls on tabs selection
			switch(SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_GETCURSEL, 0, 0)){
				case 0:
					ShowWindow(m_Dlgs.hSchedule, SW_HIDE);
					ShowWindow(m_Dlgs.hAppearance, SW_SHOW);
					ShowWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), SW_SHOW);
					break;
				case 1:
					ShowWindow(m_Dlgs.hSchedule, SW_SHOW);
					ShowWindow(m_Dlgs.hAppearance, SW_HIDE);
					ShowWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), SW_HIDE);
					break;
			}
		}
		return TRUE;
	default: return FALSE;
	}
}

static void Adjust_OnClose(HWND hwnd)
{
	g_hAdjust = NULL;
	if(g_hLastModal == hwnd)
		EndDialog(hwnd, 0);
	else
		DestroyWindow(hwnd);
}

static BOOL Adjust_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	P_ADJUST_STRUCT		pAS = (P_ADJUST_STRUCT)lParam;
	PMEMNOTE			pNote = pAS->pNote;
	RECT				rc;
	wchar_t				szCaption[256];

	g_hLastModal = hwnd;

	//store handle of calling window
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pNote);
	
	//store parameters
	memcpy(&m_TNAppearance, pNote->pAppearance, sizeof(NOTE_APPEARANCE));
	memcpy(&m_TNHandles, pNote->pRTHandles, sizeof(NOTE_RTHANDLES));
	memcpy(&m_TAddAppearance, pNote->pAddNApp, sizeof(ADDITIONAL_NAPP));

	m_Dlgs.hAppearance = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NOTE_APP), hwnd, NApp_DlgProc, 0); 
	m_Dlgs.hSchedule = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NOTE_SCHEDULE), hwnd, NSched_DlgProc, 0); 

	//create schedule part dialogs
	m_Dns.hOnce = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_ONCE), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hEveryDay = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_EVERYDAY), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hRepeat = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_REPEATEVERY), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hWeekly = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_WEEKLY), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hAfter = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_AFTER), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hMonthExact = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_MONTHLY_EXACT), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hMonthRel = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_MONTHLY_RELATIVE), m_Dlgs.hSchedule, Dns_DlgProc, 0); 

	m_DataRes1 = pNote->pData->res1;

	AddTabs(hwnd);
	GetClientRect(hwnd, &rc);
	SetSizes(hwnd, rc.right - rc.left, rc.bottom -rc.top);

	if(DockType(pNote->pData->dockData) != DOCK_NONE){
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_LST_SKIN), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_BMP), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_CFONT), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_BCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_ADJUST_DISABLE), SW_SHOW);
	}
	else{
		if(m_TNHandles.hbSkin){
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_LST_SKIN), SW_SHOW);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_CFONT), SW_HIDE);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_BCOLOR), SW_HIDE);
		}
		else{
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_LST_SKIN), SW_HIDE);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_CFONT), SW_SHOW);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_BCOLOR), SW_SHOW);
		}
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_BMP), SW_SHOW);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_ADJUST_DISABLE), SW_HIDE);
	}


	InitAppearancePart(m_Dlgs.hAppearance);
	InitSchedulePart(m_Dlgs.hSchedule);

	//enable "Standard view" button
	if((pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN 
	|| (pNote->pAppearance->nPrivate & F_RTFONT) == F_RTFONT 
	|| (pNote->pAppearance->nPrivate & F_C_COLOR) == F_C_COLOR
	|| (pNote->pAppearance->nPrivate & F_C_FONT) == F_C_FONT
	|| (pNote->pAppearance->nPrivate & F_B_COLOR) == F_B_COLOR
	|| pNote->pAddNApp->transValue > 0){
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), TRUE);
		}
		else{
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), FALSE);
		}
	}
	else{
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), FALSE);
	}

	GetWindowTextW(hwnd, szCaption, 128);
	
	wcscat(szCaption, L" - ");
	wcscat(szCaption, pNote->pData->szName);
	SetWindowTextW(hwnd, szCaption);

	if(pAS->type == 1){
		NMHDR		nm;

		nm.code = TCN_SELCHANGE;
		nm.idFrom = IDC_TAB_ADJUST;
		nm.hwndFrom = GetDlgItem(hwnd, IDC_TAB_ADJUST);
		SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_SETCURSEL, 1, 0);
		SendMessageW(hwnd, WM_NOTIFY, IDC_TAB_ADJUST, (LPARAM)&nm);
	}
	//prevent another window appearance
	g_hAdjust = hwnd;

	return FALSE;
}

static void Adjust_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PMEMNOTE		pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch(id){
		case IDCANCEL:
			ClearOnAdjustClose(pNote);
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDOK:
			//store flags (because of not-track flag)
			if(pNote->pData->res1 != m_DataRes1){
				NOTE_DATA			data;
				GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				data.res1 = m_DataRes1;
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				pNote->pData->res1 = m_DataRes1;
			}
			
			m_Reload = ApplyNoteChanges(hwnd);

			if(m_Reload){
				//the note should be reloaded
				wchar_t			szID[28];
				wcscpy(szID, pNote->pFlags->id);
				SaveNote(pNote);
				if(pNote->pData->visible){
					SendMessageW(pNote->hwnd, WM_CLOSE, 0, 0);
					LoadNoteProperties(pNote, &g_RTHandles, szID, FALSE);
					CreateNote(pNote, g_hInstance, FALSE, NULL);
				}
				if(g_hCPDialog){
					SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
				}
				if(g_hOverdueDialog){
					SendMessageW(g_hOverdueDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
				}
			}
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDC_CMD_STD_VIEW:
			if(pNote->pData->visible){
				RestoreStandardView(pNote->hwnd);
			}
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
	}
}

static void ClearOnAdjustClose(PMEMNOTE pNote){
	if(pNote->pRTHandles->hFCaption != m_TNHandles.hFCaption)
		DeleteFont(m_TNHandles.hFCaption);
	if(pNote->pRTHandles->hFont != m_TNHandles.hFont)
		DeleteFont(m_TNHandles.hFont);
	if(pNote->pRTHandles->hbSkin != m_TNHandles.hbSkin)
		DeleteBitmap(m_TNHandles.hbSkin);
	if(pNote->pRTHandles->hbDelHide != m_TNHandles.hbDelHide)
		DeleteBitmap(m_TNHandles.hbDelHide);
	if(pNote->pRTHandles->hbSys != m_TNHandles.hbSys)
		DeleteBitmap(m_TNHandles.hbSys);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RestoreStandardView
 Created  : Sun Sep  9 12:36:00 2007
 Modified : Sun Sep  9 12:36:00 2007

 Synopsys : Restores standard view for note
 Input    : hNote - note handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RestoreStandardView(HWND hNote){
	PMEMNOTE				pNote = MEMNOTE(hNote);
	RECT					rc;
	BITMAP					bm;
	HWND					hEdit;
	BOOL					fDeletePrevious = FALSE;
	LPPNGROUP				ppg;

	ppg = PNGroupsGroupById(g_PGroups, pNote->pData->idGroup);
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	//copy standard appearance
	if((pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN)
		fDeletePrevious = TRUE;
	memcpy(pNote->pAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));
	pNote->pAppearance->crWindow = ppg->crWindow;
	pNote->pAppearance->crCaption = ppg->crCaption;
	pNote->pAppearance->crFont = ppg->crFont;
	wcscpy(pNote->pAppearance->szSkin, ppg->szSkin);

	//clear note's flags
	pNote->pFlags->maskChanged &= ~F_SKIN;
	pNote->pFlags->maskChanged &= ~F_RTFONT;
	pNote->pFlags->maskChanged &= ~F_C_FONT;
	pNote->pFlags->maskChanged &= ~F_C_COLOR;
	pNote->pFlags->maskChanged &= ~F_B_COLOR;
	//get standard font and skin
	pNote->pRTHandles->hFont = g_RTHandles.hFont;
	pNote->pRTHandles->hbSkin = g_RTHandles.hbSkin;
	//clear "saved" flag, if note's text has not been changed
	if((pNote->pFlags->maskChanged & F_TEXT) != F_TEXT)
		pNote->pFlags->saved = TRUE;
	if(!pNote->pRTHandles->hbSkin){
		//in case of skinless window - get caption font
		pNote->pRTHandles->hFCaption = g_RTHandles.hFCaption;
		//prepare needed rectangles
		GetClientRect(hNote, &rc);
		DeleteBitmap(pNote->pRTHandles->hbCommand);
		CreateSkinlessToolbarBitmap(hNote, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
		CreateSkinlessSysBitmap(hNote, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
		CreateSimpleMarksBitmap(hNote, &pNote->pRTHandles->hbSys, MARKS_COUNT);
		MeasureSkinlessNoteRectangles(pNote, rc.right - rc.left, rc.bottom - rc.top, pNote->pData->szName);
		if(pNote->pData->visible){
			HWND hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
			MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, (pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left) - 2, pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
		}
		RichEdit_SetBkgndColor(hEdit, FALSE, pNote->pAppearance->crWindow);
	}
	else{
		//get standard skin properties
		GetSkinProperties(hNote, pNote->pRTHandles, pNote->pAppearance->szSkin, fDeletePrevious);
		GetWindowRect(hNote, &rc);
		GetObjectW(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		//resize controls on note window
		SendMessage(hNote, PNM_RESIZE, (WPARAM)&pNote->pRTHandles->rcEdit, 0);
		//resize note window
		MoveWindow(hNote, rc.left, rc.top, bm.bmWidth, bm.bmHeight, TRUE);
		//update tooltip
		SendMessageW(hNote, PNM_TT_UPDATE, 0, 0);
	}
	//clear private appearance
	pNote->pAppearance->nPrivate = 0;
	//remove appearance entry from data file
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, NULL, 0, g_NotePaths.DataFile);
	SetREFontCommon(hEdit, &pNote->pAppearance->lfFont);
	SetREFontColorCommon(hEdit, pNote->pAppearance->crFont);

	//restore transparency level
	if(m_TAddAppearance.transValue != 0){
		m_TAddAppearance.transValue = 0;
		pNote->pAddNApp->transValue = 0;
	}
	SetTransValue(m_Dlgs.hAppearance, g_NoteSettings.transValue);

	//redraw note
	RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
	ShowNoteMarks(hNote);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ScheduleChanged
 Created  : Sun May 27 15:33:17 2007
 Modified : Sun May 27 15:33:17 2007

 Synopsys : Checks whether schedule of note has been changed
 Input    : hwnd - note handle
 Output   : TRUE if changed, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static BOOL ScheduleChanged(PMEMNOTE pNote){

	SYSTEMTIME			st, stSt;
	int					prevType, weekday, ordinal;
	int					scType, index, data;
	BOOL				translated, result = FALSE, loopChecked = FALSE;
	short				combType = 0;

	GetLocalTime(&st);
	scType = SendDlgItemMessageW(m_Dlgs.hSchedule, IDC_CBO_NOTE_SCHEDULE, CB_GETCURSEL, 0, 0);

	index = SendDlgItemMessageW(m_Dlgs.hSchedule, IDC_CBO_STOP_LOOP, CB_GETCURSEL, 0, 0);
	if(index != CB_ERR){
		data = SendDlgItemMessageW(m_Dlgs.hSchedule, IDC_CBO_STOP_LOOP, CB_GETITEMDATA, index, 0);
		if((int)pNote->pSchedule->stopLoop != data){
			pNote->pSchedule->stopLoop = (short)data;
			result = true;
		}
	}
	if(IsDlgButtonChecked(m_Dlgs.hSchedule, IDC_CHK_LOOP_SOUND) == BST_CHECKED)
		loopChecked = TRUE;
	if(!IsBitOn(pNote->pSchedule->params, SP_SOUND_IN_LOOP) && loopChecked){
		result = TRUE;
		BitOn(&pNote->pSchedule->params, SP_SOUND_IN_LOOP);
	}
	else if(IsBitOn(pNote->pSchedule->params, SP_SOUND_IN_LOOP) && !loopChecked){
		result = TRUE;
		BitOff(&pNote->pSchedule->params, SP_SOUND_IN_LOOP);
	}
	switch(scType){
		case SCH_NO:					//no schedule checked
			if(pNote->pSchedule->scType != 0){		//note was scheduled before
				wchar_t		szTemp[128];
				wcscpy(szTemp, pNote->pSchedule->szSound);
				ZeroMemory(pNote->pSchedule, sizeof(SCHEDULE_TYPE));//remove scheduling	
				wcscpy(pNote->pSchedule->szSound, szTemp);
				if(pNote->pRTHandles->idTimer)			//stop timer
					KillTimer(NULL, pNote->pRTHandles->idTimer);
				return TRUE;
			}
			break;
		case SCH_ONCE:					//schedule once checked
			if(pNote->pSchedule->scType != SCH_ONCE){		//different schedule type before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_ONCE;
				//get selected time
				SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy time
				// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				pNote->pSchedule->scDate = st;
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				return TRUE;
			}
			else{			//the previous scheduling was of the same type - must check times difference
				//get selected time
				SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wDay != st.wDay || pNote->pSchedule->scDate.wMonth != st.wMonth
					|| pNote->pSchedule->scDate.wYear != st.wYear || pNote->pSchedule->scDate.wHour != st.wHour 
					|| pNote->pSchedule->scDate.wMinute != st.wMinute){
					//copy time
					// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					pNote->pSchedule->scDate = st;
					return TRUE;
				}
			}
			break;
		case SCH_ED:			//schedule every day checked
			if(pNote->pSchedule->scType != SCH_ED){		//different schedule type before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_ED;
				//get selected time
				SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy time
				// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				pNote->pSchedule->scDate = st;
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				//need for further checking
				GetLocalTime(&pNote->pSchedule->scStart);
				return TRUE;
			}
			else{			//the previous scheduling was of the same type - must check times difference
				//get selected time
				SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wHour != st.wHour || pNote->pSchedule->scDate.wMinute != st.wMinute){
					//copy time
					// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					pNote->pSchedule->scDate = st;
					//need for further checking
					GetLocalTime(&pNote->pSchedule->scStart);
					return TRUE;
				}
			}
			break;
		case SCH_REP:					//schedule repeat checked
			//check what option is selected
			if(IsDlgButtonChecked(m_Dns.hRepeat, IDC_OPT_TIME_START) == BST_CHECKED)
				combType = SCH_REP;
			else if(IsDlgButtonChecked(m_Dns.hRepeat, IDC_OPT_COMP_START) == BST_CHECKED)
				combType = SCH_REP + START_COMP;
			else if(IsDlgButtonChecked(m_Dns.hRepeat, IDC_OPT_PROG_START) == BST_CHECKED)
				combType = SCH_REP + START_PROG;
			if(pNote->pSchedule->scType != combType){		//different schedule before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = combType;
				//get selected time
				GetPesterPart(&st);
				//copy time
				// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				pNote->pSchedule->scDate = st;
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);		//set repeat timer according to timer interval
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				if(combType == SCH_REP){
					//store starting date/time
					SendDlgItemMessageW(m_Dns.hRepeat, IDC_DTP_REPEAT_START, DTM_GETSYSTEMTIME, 0, (LPARAM)&pNote->pSchedule->scStart);
					pNote->pSchedule->scStart.wSecond = 0;
					pNote->pSchedule->scStart.wMilliseconds = 0;
					//need for further checking
					// memcpy(&pNote->pSchedule->scLastRun, &pNote->pSchedule->scStart, sizeof(SYSTEMTIME));
					pNote->pSchedule->scLastRun = pNote->pSchedule->scStart;
				}
				else if(combType - SCH_REP == START_PROG){
					// memcpy(&pNote->pSchedule->scStart, &g_StartTimeProg, sizeof(SYSTEMTIME));
					pNote->pSchedule->scStart = g_StartTimeProg;
					// memcpy(&pNote->pSchedule->scLastRun, &g_StartTimeProg, sizeof(SYSTEMTIME));
					pNote->pSchedule->scLastRun = g_StartTimeProg;
				}
				else if(combType - SCH_REP == START_COMP){
					// memcpy(&pNote->pSchedule->scStart, &g_StartTimeSystem, sizeof(SYSTEMTIME));
					pNote->pSchedule->scStart = g_StartTimeSystem;
					// memcpy(&pNote->pSchedule->scLastRun, &g_StartTimeSystem, sizeof(SYSTEMTIME));
					pNote->pSchedule->scLastRun = g_StartTimeSystem;
				}
				return TRUE;
			}
			else{			//the previous scheduling was of the same type - must check times difference
				//get selected time
				GetPesterPart(&st);
				if(combType != SCH_REP){	//in case of combined style we don't need DTP part for checking
					if(pNote->pSchedule->scDate.wYear != st.wYear || pNote->pSchedule->scDate.wMonth != st.wMonth 
					|| pNote->pSchedule->scDate.wDayOfWeek != st.wDayOfWeek || pNote->pSchedule->scDate.wHour != st.wHour 
					|| pNote->pSchedule->scDate.wMinute != st.wMinute || pNote->pSchedule->scDate.wSecond != st.wSecond){
						//copy time
						// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
						pNote->pSchedule->scDate = st;
						pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);		//set repeat timer according to timer interval
					}
				}
				else{	//check both pester part and DTP part
					//get starting date/time
					SYSTEMTIME		stTemp;
					SendDlgItemMessageW(m_Dns.hRepeat, IDC_DTP_REPEAT_START, DTM_GETSYSTEMTIME, 0, (LPARAM)&stTemp);
					if(pNote->pSchedule->scDate.wYear != st.wYear || pNote->pSchedule->scDate.wMonth != st.wMonth 
					|| pNote->pSchedule->scDate.wDayOfWeek != st.wDayOfWeek || pNote->pSchedule->scDate.wHour != st.wHour 
					|| pNote->pSchedule->scDate.wMinute != st.wMinute || pNote->pSchedule->scDate.wSecond != st.wSecond 
					|| pNote->pSchedule->scStart.wYear != stTemp.wYear || pNote->pSchedule->scStart.wMonth != stTemp.wMonth 
					|| pNote->pSchedule->scStart.wDay != stTemp.wDay || pNote->pSchedule->scStart.wHour != stTemp.wHour 
					|| pNote->pSchedule->scStart.wMinute != stTemp.wMinute){
						//copy time
						// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
						pNote->pSchedule->scDate = st;
						pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);		//set repeat timer according to timer interval
						stTemp.wSecond = 0;
						stTemp.wMilliseconds = 0;
						//need for further checking
						// memcpy(&pNote->pSchedule->scLastRun, &stTemp, sizeof(SYSTEMTIME));
						pNote->pSchedule->scLastRun = stTemp;
						//store starting date/time
						// memcpy(&pNote->pSchedule->scStart, &stTemp, sizeof(SYSTEMTIME));
						pNote->pSchedule->scStart = stTemp;
						return TRUE;
					}
				}
			}
			break;
		case SCH_PER_WD:		//weekly schedule checked
			//get weekday
			weekday = SelectedWeekdays();
			if(weekday == 0)
				return TRUE;
			// weekday = SendDlgItemMessageW(hdlg, IDC_CBO_EVERY_WD, CB_GETITEMDATA, SendDlgItemMessageW(hdlg, IDC_CBO_EVERY_WD, CB_GETCURSEL, 0, 0), 0);
			if(pNote->pSchedule->scType != SCH_PER_WD){		//different schedule type
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_PER_WD;
				//get selected time
				SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//store weekday
				st.wDayOfWeek = weekday;
				//copy time
				// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				pNote->pSchedule->scDate = st;
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				//need for further checking
				GetLocalTime(&pNote->pSchedule->scLastRun);
				pNote->pSchedule->scLastRun.wSecond = 0;
				pNote->pSchedule->scLastRun.wMilliseconds = 0;
				if(pNote->pSchedule->scLastRun.wMinute > 0){
					pNote->pSchedule->scLastRun.wMinute--;
				}
				else{
					if(pNote->pSchedule->scLastRun.wHour > 0)
						pNote->pSchedule->scLastRun.wHour--;
					else
						pNote->pSchedule->scLastRun.wHour = 23;
					pNote->pSchedule->scLastRun.wMinute = 59;
				}
				return TRUE;
			}
			else{
				//get selected time
				SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wDayOfWeek != weekday || pNote->pSchedule->scDate.wHour != st.wHour || pNote->pSchedule->scDate.wMinute != st.wMinute){
					st.wDayOfWeek = weekday;
					//copy time
					// memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					pNote->pSchedule->scDate = st;
					//need for further checking
					GetLocalTime(&pNote->pSchedule->scLastRun);
					pNote->pSchedule->scLastRun.wSecond = 0;
					pNote->pSchedule->scLastRun.wMilliseconds = 0;
					if(pNote->pSchedule->scLastRun.wMinute > 0){
						pNote->pSchedule->scLastRun.wMinute--;
					}
					else{
						if(pNote->pSchedule->scLastRun.wHour > 0)
							pNote->pSchedule->scLastRun.wHour--;
						else
							pNote->pSchedule->scLastRun.wHour = 23;
						pNote->pSchedule->scLastRun.wMinute = 59;
					}
					return TRUE;
				}
			}
			break;
		case SCH_AFTER:				//'after' schedule checked
			//check what option is selected
			if(IsDlgButtonChecked(m_Dns.hAfter, IDC_OPT_TIME_START) == BST_CHECKED)
				combType = SCH_AFTER;
			else if(IsDlgButtonChecked(m_Dns.hAfter, IDC_OPT_COMP_START) == BST_CHECKED)
				combType = SCH_AFTER + START_COMP;
			else if(IsDlgButtonChecked(m_Dns.hAfter, IDC_OPT_PROG_START) == BST_CHECKED)
				combType = SCH_AFTER + START_PROG;
			if(pNote->pSchedule->scType != combType){	//different schedule type
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = combType;
				if(combType == SCH_AFTER){
					//get selected time
					SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
					//copy start time
					// memcpy(&pNote->pSchedule->scStart, &st, sizeof(SYSTEMTIME));	
					pNote->pSchedule->scStart = st;
				}
				else if(combType - SCH_AFTER == START_PROG){
					// memcpy(&pNote->pSchedule->scStart, &g_StartTimeProg, sizeof(SYSTEMTIME));
					pNote->pSchedule->scStart = g_StartTimeProg;
				}
				else if(combType - SCH_AFTER == START_COMP){
					// memcpy(&pNote->pSchedule->scStart, &g_StartTimeSystem, sizeof(SYSTEMTIME));
					pNote->pSchedule->scStart = g_StartTimeSystem;
				}
				//store date parts - wDayOfWeek is used for weeks count
				pNote->pSchedule->scDate.wYear = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_YEARS, &translated, FALSE);
				pNote->pSchedule->scDate.wMonth = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MONTHS, &translated, FALSE);
				pNote->pSchedule->scDate.wDayOfWeek = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_WEEKS, &translated, FALSE);
				pNote->pSchedule->scDate.wDay = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_DAYS, &translated, FALSE);
				pNote->pSchedule->scDate.wHour = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_HOURS, &translated, FALSE);
				pNote->pSchedule->scDate.wMinute = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MINS, &translated, FALSE);
				pNote->pSchedule->scDate.wSecond = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_SECS, &translated, FALSE);
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				return TRUE;
			}
			else{
				//store date parts in temporary variable - wDayOfWeek is used for weeks count
				stSt.wYear = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_YEARS, &translated, FALSE);
				stSt.wMonth = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MONTHS, &translated, FALSE);
				stSt.wDayOfWeek = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_WEEKS, &translated, FALSE);
				stSt.wDay = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_DAYS, &translated, FALSE);
				stSt.wHour = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_HOURS, &translated, FALSE);
				stSt.wMinute = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MINS, &translated, FALSE);
				stSt.wSecond = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_SECS, &translated, FALSE);
				if(combType != SCH_AFTER){	//no need for checking DTC part for combined type
					if(pNote->pSchedule->scDate.wYear != stSt.wYear || pNote->pSchedule->scDate.wMonth != stSt.wMonth 
					|| pNote->pSchedule->scDate.wDayOfWeek != stSt.wDayOfWeek || pNote->pSchedule->scDate.wDay != stSt.wDay 
					|| pNote->pSchedule->scDate.wHour != stSt.wHour || pNote->pSchedule->scDate.wMinute != stSt.wMinute 
					|| pNote->pSchedule->scDate.wSecond != stSt.wSecond){
						//copy dates parts
						// memcpy(&pNote->pSchedule->scDate, &stSt, sizeof(SYSTEMTIME));
						pNote->pSchedule->scDate = stSt;
						return TRUE;
					}	
				}
				else{
					//get selected time
					SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
					if(pNote->pSchedule->scStart.wYear != st.wYear || pNote->pSchedule->scStart.wMonth != st.wMonth 
					|| pNote->pSchedule->scStart.wDayOfWeek != st.wDayOfWeek || pNote->pSchedule->scStart.wDay != st.wDay 
					|| pNote->pSchedule->scStart.wHour != st.wHour || pNote->pSchedule->scStart.wMinute != st.wMinute 
					|| pNote->pSchedule->scStart.wSecond != st.wSecond 
					|| pNote->pSchedule->scDate.wYear != stSt.wYear || pNote->pSchedule->scDate.wMonth != stSt.wMonth 
					|| pNote->pSchedule->scDate.wDayOfWeek != stSt.wDayOfWeek || pNote->pSchedule->scDate.wDay != stSt.wDay 
					|| pNote->pSchedule->scDate.wHour != stSt.wHour || pNote->pSchedule->scDate.wMinute != stSt.wMinute 
					|| pNote->pSchedule->scDate.wSecond != stSt.wSecond){
						//copy start time
						// memcpy(&pNote->pSchedule->scStart, &st, sizeof(SYSTEMTIME));
						pNote->pSchedule->scStart = st;
						//copy dates parts
						// memcpy(&pNote->pSchedule->scDate, &stSt, sizeof(SYSTEMTIME));
						pNote->pSchedule->scDate = stSt;
						return TRUE;
					}	
				}
			}
			break;
		case SCH_MONTHLY_EXACT:
			//get selected time
			SendDlgItemMessageW(m_Dns.hMonthExact, IDC_DTP_MONTH_EXACT, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
			if(pNote->pSchedule->scType != SCH_MONTHLY_EXACT){	//different schedule type before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_MONTHLY_EXACT;
				//need for further checking
				pNote->pSchedule->scLastRun.wMonth = 0;
				GetLocalTime(&pNote->pSchedule->scStart);
				//copy SYSTEMTIME structure, because we must have all its fields initialized
				// memcpy(&pNote->pSchedule->scDate, &pNote->pSchedule->scStart, sizeof(SYSTEMTIME));
				pNote->pSchedule->scDate = pNote->pSchedule->scStart;
				//set time parameters
				pNote->pSchedule->scDate.wHour = st.wHour;
				pNote->pSchedule->scDate.wMinute = st.wMinute;
				pNote->pSchedule->scDate.wDay = SendDlgItemMessageW(m_Dns.hMonthExact, IDC_CBO_MONTH_EXACT, CB_GETCURSEL, 0, 0) + 1;
				return TRUE;
			}
			else{
				//save time parameters in temporary variables
				stSt.wHour = st.wHour;
				stSt.wMinute = st.wMinute;
				stSt.wDay = SendDlgItemMessageW(m_Dns.hMonthExact, IDC_CBO_MONTH_EXACT, CB_GETCURSEL, 0, 0) + 1;
				//check whether any change occured
				if(stSt.wHour != pNote->pSchedule->scDate.wHour 
				|| stSt.wMinute != pNote->pSchedule->scDate.wMinute 
				|| stSt.wDay != pNote->pSchedule->scDate.wDay){
					//need for further checking
					pNote->pSchedule->scLastRun.wMonth = 0;
					pNote->pSchedule->scDate.wHour = stSt.wHour;
					pNote->pSchedule->scDate.wMinute = stSt.wMinute;
					pNote->pSchedule->scDate.wDay = stSt.wDay;
					return TRUE;
				}
			}
			break;
		case SCH_MONTHLY_RELATIVE:
			//get day of week
			weekday = SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_GETITEMDATA, SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_GETCURSEL, 0, 0), 0);
			//get ordinal
			ordinal = SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_ORDINAL, CB_GETCURSEL, 0, 0);
			//get selected time
			SendDlgItemMessageW(m_Dns.hMonthRel, IDC_DTP_MONTH_EXACT, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
			if(pNote->pSchedule->scType != SCH_MONTHLY_RELATIVE){	//different schedule type before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_MONTHLY_RELATIVE;
				//need for further checking
				pNote->pSchedule->scLastRun.wMonth = 0;
				GetLocalTime(&pNote->pSchedule->scStart);
				//copy SYSTEMTIME structure, because we must have all its fields initialized
				// memcpy(&pNote->pSchedule->scDate, &pNote->pSchedule->scStart, sizeof(SYSTEMTIME));
				pNote->pSchedule->scDate = pNote->pSchedule->scStart;
				//set time
				pNote->pSchedule->scDate.wHour = st.wHour;
				pNote->pSchedule->scDate.wMinute = st.wMinute;
				//set day of week
				pNote->pSchedule->scDate.wDayOfWeek = weekday;
				//set ordinal
				pNote->pSchedule->scDate.wMilliseconds = ordinal;
				return TRUE;
			}
			else{
				//check whether any change occured
				if(st.wHour != pNote->pSchedule->scDate.wHour 
				|| st.wMinute != pNote->pSchedule->scDate.wMinute 
				|| pNote->pSchedule->scDate.wDayOfWeek != weekday 
				|| pNote->pSchedule->scDate.wMilliseconds != ordinal){
					//need for further checking
					pNote->pSchedule->scLastRun.wMonth = 0;
					pNote->pSchedule->scDate.wHour = st.wHour;
					pNote->pSchedule->scDate.wMinute = st.wMinute;
					//set day of week
					pNote->pSchedule->scDate.wDayOfWeek = weekday;
					//set ordinal
					pNote->pSchedule->scDate.wMilliseconds = ordinal;
					return TRUE;
				}
			}
			break;
	}
	return result;
}

static void FillScheduleTypes(HWND hwnd, wchar_t * lpFile){
	wchar_t 		szBuffer[256], szId[12];
	
	_itow(IDC_OPT_NO_SCH, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"No schedule", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_ONCE, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Once at:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_EV_DAY, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Every day at:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_REPEAT, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Repeat every:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_EVERY_WD, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Weekly on:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_AFTER, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"After:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_MONTH_EXACT, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Monthly (exact)", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_MONTH_RELATIVE, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Monthly (relative)", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
}

static void AddTabs(HWND hwnd){

	TCITEMW		tci;
	wchar_t 	szBuffer[256];
	HWND		hCombo;

	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1003", DS_CAP_SINGLE, szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_CMD_STD_VIEW, g_NotePaths.CurrLanguagePath, L"Standard view");
	//tabs
	ZeroMemory(&tci, sizeof(tci));
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = szBuffer;
	tci.cchTextMax = 256;
	GetPrivateProfileStringW(L"tabs", L"0", L"Skin", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_INSERTITEMW, 0, (LPARAM)&tci);
	GetPrivateProfileStringW(L"tabs", L"2", L"Schedule", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_INSERTITEMW, 1, (LPARAM)&tci);

	//appearance part
	SetDlgCtlText(m_Dlgs.hAppearance, IDC_ST_ADJUST_DISABLE, g_NotePaths.CurrLanguagePath, L"Settings are available for not docked notes only");

	//-----------	schedule part	-----------
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_CHK_NO_TRACK, g_NotePaths.CurrLanguagePath, L"Do not track the note if it is overdue");
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_GRP_CUST_SOUND, g_NotePaths.CurrLanguagePath, L"Sound alert");
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_CHK_LOOP_SOUND, g_NotePaths.CurrLanguagePath, L"Repeat in loop");
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_OPT_USE_TTS, g_NotePaths.CurrLanguagePath, L"Text of note");
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_OPT_USE_WAV_FILE, g_NotePaths.CurrLanguagePath, L"File");
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_ST_STOP_LOOP, g_NotePaths.CurrLanguagePath, L"Stop the loop after:");
	FillStopLoop(GetDlgItem(m_Dlgs.hSchedule, IDC_CBO_STOP_LOOP));

	//'repeat' dialog
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_YEARS, g_NotePaths.CurrLanguagePath, L"Years");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_MONTHS, g_NotePaths.CurrLanguagePath, L"Months");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_WEEKS, g_NotePaths.CurrLanguagePath, L"Weeks");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_DAYS, g_NotePaths.CurrLanguagePath, L"Days");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_HOURS, g_NotePaths.CurrLanguagePath, L"Hours");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_MINS, g_NotePaths.CurrLanguagePath, L"Minutes");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_SECS, g_NotePaths.CurrLanguagePath, L"Seconds");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_STARTING, g_NotePaths.CurrLanguagePath, L"Starting from:");
	SetDlgCtlText(m_Dns.hRepeat, IDC_OPT_TIME_START, g_NotePaths.CurrLanguagePath, L"Exact time");
	SetDlgCtlText(m_Dns.hRepeat, IDC_OPT_COMP_START, g_NotePaths.CurrLanguagePath, L"Next system start");
	SetDlgCtlText(m_Dns.hRepeat, IDC_OPT_PROG_START, g_NotePaths.CurrLanguagePath, L"Next program start");
	CheckDlgButton(m_Dns.hRepeat, IDC_OPT_TIME_START, BST_CHECKED);
	EnableWindow(GetDlgItem(m_Dns.hRepeat, IDC_DTP_REPEAT_START), TRUE);
	CheckDlgButton(m_Dns.hRepeat, IDC_OPT_COMP_START, BST_UNCHECKED);
	CheckDlgButton(m_Dns.hRepeat, IDC_OPT_PROG_START, BST_UNCHECKED);
	//'after' dialog
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_DAYS, g_NotePaths.CurrLanguagePath, L"Days");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_HOURS, g_NotePaths.CurrLanguagePath, L"Hours");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_MINS, g_NotePaths.CurrLanguagePath, L"Minutes");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_SECS, g_NotePaths.CurrLanguagePath, L"Seconds");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_YEARS, g_NotePaths.CurrLanguagePath, L"Years");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_MONTHS, g_NotePaths.CurrLanguagePath, L"Months");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_WEEKS, g_NotePaths.CurrLanguagePath, L"Weeks");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_STARTING, g_NotePaths.CurrLanguagePath, L"Starting from:");
	SetDlgCtlText(m_Dns.hAfter, IDC_OPT_TIME_START, g_NotePaths.CurrLanguagePath, L"Exact time");
	SetDlgCtlText(m_Dns.hAfter, IDC_OPT_COMP_START, g_NotePaths.CurrLanguagePath, L"Start of computer");
	SetDlgCtlText(m_Dns.hAfter, IDC_OPT_PROG_START, g_NotePaths.CurrLanguagePath, L"Start of program");
	CheckDlgButton(m_Dns.hAfter, IDC_OPT_TIME_START, BST_CHECKED);
	EnableWindow(GetDlgItem(m_Dns.hAfter, IDC_DTP_AFTER), TRUE);
	CheckDlgButton(m_Dns.hAfter, IDC_OPT_COMP_START, BST_UNCHECKED);
	CheckDlgButton(m_Dns.hAfter, IDC_OPT_PROG_START, BST_UNCHECKED);
	//monthly - exact dialog
	SetDlgCtlText(m_Dns.hMonthExact, IDC_MONTH_ST_DAY, g_NotePaths.CurrLanguagePath, L"Date");
	SetDlgCtlText(m_Dns.hMonthExact, IDC_MONTH_ST_TIME, g_NotePaths.CurrLanguagePath, L"Time");
	SetDlgCtlText(m_Dns.hMonthExact, IDC_ST_WARN_MONTH_EX, g_NotePaths.CurrLanguagePath, L"If there is no such date in current month, the nearest date will be used instead.");
	//monthly - relative dialog
	SetDlgCtlText(m_Dns.hMonthRel, IDC_MONTH_ST_ORDINAL, g_NotePaths.CurrLanguagePath, L"Ordinal number");
	SetDlgCtlText(m_Dns.hMonthRel, IDC_MONTH_ST_DOFWEEK, g_NotePaths.CurrLanguagePath, L"Day of week");
	SetDlgCtlText(m_Dns.hMonthRel, IDC_MONTH_ST_TIME, g_NotePaths.CurrLanguagePath, L"Time");
	//'weekly' dialog
	SetDlgCtlText(m_Dns.hWeekly, IDC_ST_TIME_EVERY_WD, g_NotePaths.CurrLanguagePath, L"At:");
	//format datetime pickers
	SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.DateFormat);
	SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.TimeFormat);
	SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.TimeFormat);
	SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.DateFormat);
	SendDlgItemMessageW(m_Dns.hMonthExact, IDC_DTP_MONTH_EXACT, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.TimeFormat);
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_DTP_MONTH_EXACT, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.TimeFormat);
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_DTP_REPEAT_START, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.DateFormat);
	//prepare textboxes for 'repeat' dialog
	for(int i = IDC_EDT_DAYS; i <= IDC_EDT_SECS; i++)
		SendDlgItemMessageW(m_Dns.hRepeat, i, EM_LIMITTEXT, 2, 0);
	for(int i = IDC_EDT_YEARS; i <= IDC_EDT_WEEKS; i++)
		SendDlgItemMessageW(m_Dns.hRepeat, i, EM_LIMITTEXT, 2, 0);
	//prepare textboxes for 'after' dialog
	for(int i = IDC_EDT_DAYS; i <= IDC_EDT_SECS; i++)
		SendDlgItemMessageW(m_Dns.hAfter, i, EM_LIMITTEXT, 2, 0);
	for(int i = IDC_EDT_YEARS; i <= IDC_EDT_WEEKS; i++)
		SendDlgItemMessageW(m_Dns.hAfter, i, EM_LIMITTEXT, 2, 0);
	//prepare days of week
	FillDaysOfWeek(g_NotePaths.CurrLanguagePath);
	//prepare full days of week
	FillFullDaysOfWeek(g_NotePaths.CurrLanguagePath);
	FillScheduleTypes(m_Dlgs.hSchedule, g_NotePaths.CurrLanguagePath);
	//fill sounds
	hCombo = GetDlgItem(m_Dlgs.hSchedule, IDC_CBO_CUST_SOUND);
	SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)DS_DEF_SOUND);
	for(int i = 1; i < 256; i++){
		if(!g_Sounds[i])
			break;
		SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)g_Sounds[i]);
	}
}

static void FillStopLoop(HWND hCombo){
	wchar_t		*defStop[] = {L"0", L"5", L"10", L"15", L"30", L"45", L"60", L"120", L"180", L"300", L"900", L"1800"};
	wchar_t		*defPeriods[] = {L"Never", L"5 sec", L"10 sec", L"15 sec", L"30 sec", 
								L"45 sec", L"1 min", L"2 min", L"3 min", L"5 min", L"15 min", L"30 min"};
	wchar_t		*pStops, *pTemp, *pDef, szBuffer[32];
	int			ub = 1024, result, index;

	pStops = calloc(ub, sizeof(wchar_t));
	if(pStops){
		result = GetPrivateProfileStringW(S_STOP_LOOP, NULL, NULL, pStops, ub, g_NotePaths.CurrLanguagePath);
		while(result == ub - 2){
			ub *= 2;
			free(pStops);
			pStops = calloc(ub, sizeof(wchar_t));
			if(pStops){
				result = GetPrivateProfileStringW(S_STOP_LOOP, NULL, NULL, pStops, ub, g_NotePaths.CurrLanguagePath);
			}
		}
		if(!*pStops){
			pDef = pStops;
			for(int i = 0; i < NELEMS(defStop); i++){
				wcscpy(pDef, defStop[i]);
				pDef += (wcslen(defStop[i]) + 1);
			}
		}
		if(*pStops){
			pTemp = pStops;
			int		i = 0;
			while(*pTemp){
				GetPrivateProfileStringW(S_STOP_LOOP, pTemp, NULL, szBuffer, 32, g_NotePaths.CurrLanguagePath);
				if(!*szBuffer){
					wcscpy(szBuffer, defPeriods[i]);
				}
				index = SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)szBuffer);
				if(index != CB_ERR){
					SendMessageW(hCombo, CB_SETITEMDATA, index, _wtoi(pTemp));
				}
				i++;
				pTemp += wcslen(pTemp) + 1;
			}
		}
		free(pStops);
	}
}

static void FillFullDaysOfWeek(wchar_t * lpLang){
	wchar_t			szBuffer[128];

	//add ordinals
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_ORDINAL, CB_RESETCONTENT, 0, 0);
	for(int i = first; i <= last; i++){
		AddOrdinalToCombo(i, lpLang);
	}
	//add weekdays
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_RESETCONTENT, 0, 0);
	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:
			for(int i = 0; i <= 6; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			break;
		case 1:
			for(int i = 1; i <= 6; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			AddWeekdayToCombo(0, lpLang);
			break;
		case 2:
			for(int i = 2; i <= 6; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			for(int i = 0; i <= 1; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			break;
		case 3:
			for(int i = 3; i <= 6; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			for(int i = 0; i <= 2; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			break;
		case 4:
			for(int i = 4; i <= 6; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			for(int i = 0; i <= 3; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			break;
		case 5:
			for(int i = 5; i <= 6; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			for(int i = 0; i <= 4; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			break;
		case 6:
			AddWeekdayToCombo(6, lpLang);
			for(int i = 0; i <= 5; i++){
				AddWeekdayToCombo(i, lpLang);
			}
			break;
	}
}

static void FillDaysOfWeek(wchar_t * lpLang){
	wchar_t			szBuffer[128];

	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:
			for(int i = IDC_CHK_WD_0, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 1:
			for(int i = IDC_CHK_WD_0, j = 1; i <= IDC_CHK_WD_5; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			SetWeekdayText(IDC_CHK_WD_6, 0, lpLang);
			break;
		case 2:
			for(int i = IDC_CHK_WD_0, j = 2; i <= IDC_CHK_WD_4; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_5, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 3:
			for(int i = IDC_CHK_WD_0, j = 3; i <= IDC_CHK_WD_3; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_4, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 4:
			for(int i = IDC_CHK_WD_0, j = 4; i <= IDC_CHK_WD_2; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_3, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 5:
			for(int i = IDC_CHK_WD_0, j = 5; i <= IDC_CHK_WD_1; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_2, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 6:
			SetWeekdayText(IDC_CHK_WD_0, 6, lpLang);
			for(int i = IDC_CHK_WD_1, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
	}
}

static short SelectedWeekdays(void){
	int			result = 0;

	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_0))
		// return 1;
		BitOn(&result, 1);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_1))
		// return 2;
		BitOn(&result, 2);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_2))
		// return 3;
		BitOn(&result, 3);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_3))
		// return 4;
		BitOn(&result, 4);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_4))
		// return 5;
		BitOn(&result, 5);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_5))
		// return 6;
		BitOn(&result, 6);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_6))
		// return 7;
		BitOn(&result, 7);
	return (short)result;
}

static void SetWeekdayText(int id, int day, const wchar_t * lpFile){

	wchar_t 		szBuffer[24], szDefault[24], szId[12];

	_itow(day, szId, 10);
	GetDlgItemTextW(m_Dns.hWeekly, id, szDefault, 24);
	GetPrivateProfileStringW(S_WEEKDAYS, szId, szDefault, szBuffer, 24, lpFile);
	SetDlgItemTextW(m_Dns.hWeekly, id, szBuffer);
}

static void AddOrdinalToCombo(int id, const wchar_t * lpFile){
	wchar_t 		szBuffer[64], szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(S_ORDINALS, szId, NULL, szBuffer, 64, lpFile);
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_ORDINAL, CB_ADDSTRING, 0, (LPARAM)szBuffer);
}

static void AddWeekdayToCombo(int day, const wchar_t * lpFile){
	wchar_t 		szBuffer[64], szId[12];
	int				index;

	_itow(day, szId, 10);
	GetPrivateProfileStringW(S_WEEKDAYS_FULL, szId, NULL, szBuffer, 64, lpFile);
	index = SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_SETITEMDATA, index, day);
}

static void SetPesterPart(LPSYSTEMTIME lps){
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_YEARS, lps->wYear, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_MONTHS, lps->wMonth, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_WEEKS, lps->wDayOfWeek, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_DAYS, lps->wDay, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_HOURS, lps->wHour, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_MINS, lps->wMinute, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_SECS, lps->wSecond, FALSE);
}

static void GetPesterPart(LPSYSTEMTIME lps){
	lps->wYear = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_YEARS, NULL, FALSE);
	lps->wMonth = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_MONTHS, NULL, FALSE);
	lps->wDayOfWeek = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_WEEKS, NULL, FALSE);
	lps->wDay = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_DAYS, NULL, FALSE);
	lps->wHour = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_HOURS, NULL, FALSE);
	lps->wMinute = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_MINS, NULL, FALSE);
	lps->wSecond = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_SECS, NULL, FALSE);
}

static void SetSizes(HWND hwnd, int cx, int cy){
	
	RECT		rc, rcd;

	//size and move controls on adjust dialog
	SetRect(&rc, 0, 0, cx, cy);
	MoveWindow(GetDlgItem(hwnd, IDC_TAB_ADJUST), 0, 0, cx, cy, TRUE);
	SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);
	MoveWindow(GetDlgItem(hwnd, IDC_ST_TAB), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	GetWindowRect(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), &rcd);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcd, 2);
	MoveWindow(m_Dlgs.hSchedule, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top - (rcd.bottom - rcd.top + 2 * (rc.bottom - rcd.bottom)), TRUE);
	MoveWindow(m_Dlgs.hAppearance, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top - (rcd.bottom - rcd.top + 2 * (rc.bottom - rcd.bottom)), TRUE);
}

static void SetNoteCaption(HWND hwnd){
	P_NOTE_DATA			pD;
	wchar_t				szText[256];

	pD = NoteData(hwnd);
	wcscpy(szText, NOTE_CAPTION_FIRST_PART);
	wcscat(szText, pD->szName);
	wcscat(szText, L"]");
	SetWindowTextW(hwnd, szText);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PaintGrip
 Created  : Sun Sep  9 13:51:36 2007
 Modified : Sun Sep  9 13:51:36 2007

 Synopsys : Paints "sizeable" grip
 Input    : hwnd - note's handle
            hdcPS - device context
            pA - note's appearance handles
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PaintGrip(HWND hwnd, HDC hdcPS, COLORREF crWindow){
	RECT				rc;
	HPEN				hPen, hOldPen;
	POINT				pt;
	HBRUSH				hBrush;

	GetClientRect(hwnd, &rc);
	hBrush = CreateSolidBrush(crWindow);
	FillRect(hdcPS, &rc, hBrush);
	hPen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_BTNSHADOW));
	hOldPen = SelectPen(hdcPS, hPen);
	MoveToEx(hdcPS, rc.right - 5, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 5);
	MoveToEx(hdcPS, rc.right - 10, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 10);
	MoveToEx(hdcPS, rc.right - 15, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 15);
	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
	SelectPen(hdcPS, hPen);
	MoveToEx(hdcPS, rc.right - 6, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 6);
	MoveToEx(hdcPS, rc.right - 11, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 11);
	MoveToEx(hdcPS, rc.right - 16, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 16);
	SelectPen(hdcPS, hOldPen);
	DeletePen(hPen);
	DeleteBrush(hBrush);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PaintSimpleWindow
 Created  : Sun Sep  9 13:44:08 2007
 Modified : Sun Sep  9 13:44:08 2007

 Synopsys : Paints skinless note
 Input    : hwnd - note's handle
            hdcPS - device context
            pH - note's run-time handles
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PaintSimpleWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	RECT				rc;//, rc1, rc2;
	HPEN				hPen, hOldPen;
	HBRUSH				hBrush, hOldBrush, hTextBrush;
	HFONT				hFont, hOldFont;
	COLORREF			crWindow, crCaption;
	// int					flag = 0;

	//get needed parameters
	if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustColor)
		crWindow = g_DockSettings.crWindow;
	else
		crWindow = pNote->pAppearance->crWindow;
	if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustCaption){
		crCaption = g_DockSettings.crCaption;
		hFont = g_DRTHandles.hFCaption;
	}
	else{
		crCaption = pNote->pAppearance->crCaption;
		hFont = pNote->pRTHandles->hFCaption;
	}
	//get note's dimensions
	GetClientRect(hwnd, &rc);
	//create solid brush with needed color
	hBrush = CreateSolidBrush(crWindow);
	//fill note's surface with brush
	FillRect(hdcPS, &rc, hBrush);
	//draw note's caption
	FillSkinlessNoteCaption(hdcPS, &pNote->pRTHandles->rcCaption, crWindow, &pNote->pRTHandles->rcGrip);
	//save original brush
	hOldBrush = SelectBrush(hdcPS, hBrush);
	//draw caption text
	hTextBrush = CreateSolidBrush(crCaption);
	SetBkMode(hdcPS, TRANSPARENT);
	SetTextColor(hdcPS, crCaption);
	hOldFont = SelectFont(hdcPS, hFont);
	DrawTextW(hdcPS, pNote->pData->szName, -1, &pH->rcTextCaption, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	//create solid pen with caption font color
	hPen = CreatePen(PS_SOLID, 1, crCaption);
	hOldPen = SelectPen(hdcPS, hPen);
	//frame note
	FrameSkinlessNote(hdcPS, &rc, crWindow);
	//restore DC
	SelectBrush(hdcPS, hOldBrush);
	SelectFont(hdcPS, hOldFont);
	SelectPen(hdcPS, hOldPen);
	DeletePen(hPen);
	DeleteBrush(hBrush);
	DeleteBrush(hTextBrush);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PaintSkinnableWindow
 Created  : Sun Sep  9 13:39:04 2007
 Modified : Sun Sep  9 13:39:04 2007

 Synopsys : Paints skinnable note
 Input    : hwnd - note handle
            hdcPS - device context
            pH - note's run-time handles
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PaintSkinnableWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	HDC					hdc, hdcSave;
	HBITMAP				hOriginal, hBmSave, hBmSaveOld;
	RECT				rc;

	//get note's dimensions
	GetClientRect(hwnd, &rc);
	//save original bitmap
	hOriginal = pNote->pRTHandles->hbSkin;
	//prepare compatible DC
	hdc = CreateCompatibleDC(hdcPS);
	//set back mode to transparent
	SetBkMode(hdc, TRANSPARENT);
	//select original bitmap in temporary DC
	SelectBitmap(hdc, hOriginal);
	//create DC to store original bitmap
	hdcSave = CreateCompatibleDC(hdcPS);
	hBmSave = CreateCompatibleBitmap(hdcPS, rc.right - rc.left, rc.bottom - rc.top);
	hBmSaveOld = SelectBitmap(hdcSave, hBmSave);
	//Save the original bitmap, because it will be overwritten
	BitBlt(hdcSave, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
	//copy prepared bitmap onto destination
	BitBlt(hdcPS, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
	//Place the original bitmap back into the bitmap sent here
	BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcSave, 0, 0, SRCCOPY);
	//clean up
	DeleteBitmap(SelectBitmap(hdcSave, hBmSaveOld));
	DeleteDC(hdcSave);
	DeleteDC(hdc);
}

void ShowNoteMarks(HWND hwnd){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	HWND				*phMarks;
	int					flag = 0, count = 0;

	phMarks = (HWND *)GetWindowLongPtrW(hwnd, OFFSET_SYS);

	if(pNote->pSchedule->scType != 0){
		flag |= 1;
		count++;
		SetWindowPos(phMarks[MARK_SCHEDULE], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	}
	else{
		ShowWindow(phMarks[MARK_SCHEDULE], SW_HIDE);
	}
	if(!pNote->pFlags->saved){
		if((flag & 1) == 1){
			//schedule mark is visible - show changed mark at its original location
			SetWindowPos(phMarks[MARK_CHANGED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else{
			//schedule mark is not visible - show changed mark at schedule mark location
			SetWindowPos(phMarks[MARK_CHANGED], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		flag |= 2;
		count++;
	}
	else{
		ShowWindow(phMarks[MARK_CHANGED], SW_HIDE);
	}
	if(pNote->pRTHandles->marks > 2){
		//new skins
		if(IsBitOn(pNote->pData->res1, NB_PROTECTED)){
			if((flag & 1) == 1){
				//schedule mark is visible
				if((flag & 2) == 2){
					//changed flag is visible - show protected mark at its original location
					SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else{
					//only schedule flag is visible - show protected mark at changed flag location
					SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
			}
			else{
				//schedule flag is not visible
				if((flag & 2) == 2){
					//changed flag is visible - show protected mark at changed flag location
					SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else{
					//only schedule flag is visible - show protected mark at schedule flag location
					SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
			}
			flag |= 4;
			count++;
		}
		else{
			ShowWindow(phMarks[MARK_PROTECTED], SW_HIDE);
		}
		if(pNote->pRTHandles->marks > 3){
			//priority mark
			if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY)){
				if((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4){
					//show mark on its original location
					SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcPriority.left, pNote->pRTHandles->rcPriority.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else if(((flag & 1) == 1 && (flag & 2) == 2)
				|| ((flag & 1) == 1 && (flag & 4) == 4)
				|| ((flag & 2) == 2 && (flag & 4) == 4)){
					//shift mark one step left
					SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else if(flag == 1 || flag == 2 || flag == 4){
					//shift mark two steps left
					SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else{
					//shift mark three steps left
					SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				flag |= 8;
				count++;
			}
			else{
				ShowWindow(phMarks[MARK_PRIORITY], SW_HIDE);
			}
			//completed mark
			if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
				if((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4 && (flag & 8) == 8){
					//show mark on its original location
					SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcCompleted.left, pNote->pRTHandles->rcCompleted.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else if(((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4)
				|| ((flag & 1) == 1 && (flag & 2) == 2 && (flag & 8) == 8)
				|| ((flag & 1) == 1 && (flag & 4) == 4 && (flag & 8) == 8)
				|| ((flag & 2) == 2 && (flag & 4) == 4 && (flag & 8) == 8)){
					//shift mark one step left
					SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcPriority.left, pNote->pRTHandles->rcPriority.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else if(((flag & 1) == 1 && (flag & 2) == 2)
				|| ((flag & 1) == 1 && (flag & 4) == 4)
				|| ((flag & 2) == 2 && (flag & 4) == 4)
				|| ((flag & 1) == 1 && (flag & 8) == 8)
				|| ((flag & 2) == 2 && (flag & 8) == 8)
				|| ((flag & 4) == 4 && (flag & 8) == 8)){
					//shift mark two steps left
					SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else if(flag == 1 || flag == 2 || flag == 4 || flag == 8){
					//shift mark three steps left
					SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				else{
					//shift mark four steps left
					SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
				}
				flag |= 16;
				count++;
			}
			else{
				ShowWindow(phMarks[MARK_COMPLETED], SW_HIDE);
			}
			if(pNote->pRTHandles->marks > 5){
				//password mark
				if(pNote->pFlags->locked){
					switch(count){
						case 5:
							//show mark on its original location
							SetWindowPos(phMarks[MARK_PASSWORD], HWND_TOP, pNote->pRTHandles->rcPassword.left, pNote->pRTHandles->rcPassword.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 4:
							//shift mark one step left
							SetWindowPos(phMarks[MARK_PASSWORD], HWND_TOP, pNote->pRTHandles->rcCompleted.left, pNote->pRTHandles->rcCompleted.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 3:
							//shift mark tw steps left
							SetWindowPos(phMarks[MARK_PASSWORD], HWND_TOP, pNote->pRTHandles->rcPriority.left, pNote->pRTHandles->rcPriority.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 2:
							//shift mark three steps left
							SetWindowPos(phMarks[MARK_PASSWORD], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 1:
							//shift mark four steps left
							SetWindowPos(phMarks[MARK_PASSWORD], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 0:
							//shift mark five steps left
							SetWindowPos(phMarks[MARK_PASSWORD], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
					}
					flag |= 32;
					count++;
				}
				else{
					ShowWindow(phMarks[MARK_PASSWORD], SW_HIDE);
				}
			}
			if(pNote->pRTHandles->marks > 6){
				//pin mark
				if(pNote->pPin){
					switch(count){
						case 6:
							//show mark on its original location
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcPin.left, pNote->pRTHandles->rcPin.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 5:
							//shift mark one step left
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcPassword.left, pNote->pRTHandles->rcPassword.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 4:
							//shift mark tw steps left
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcCompleted.left, pNote->pRTHandles->rcCompleted.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 3:
							//shift mark three steps left
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcPriority.left, pNote->pRTHandles->rcPriority.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 2:
							//shift mark four steps left
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 1:
							//shift mark five steps left
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
						case 0:
							//shift mark six steps left
							SetWindowPos(phMarks[MARK_PIN], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
							break;
					}
					flag |= 64;
					count++;
				}
				else{
					ShowWindow(phMarks[MARK_PIN], SW_HIDE);
				}
			}
		}
	}
	else{
		ShowWindow(phMarks[MARK_PROTECTED], SW_HIDE);
		ShowWindow(phMarks[MARK_PRIORITY], SW_HIDE);
		ShowWindow(phMarks[MARK_COMPLETED], SW_HIDE);
	}
}

static DWORD OutStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb){
	int				limit;

	if(m_CurrType == CODE_UNICODE_BE){
		//we should reverse bytes in case of big endian
		if(cb){
			__asm{
				mov ecx, cb
				mov limit, ecx
				mov edx, pbBuff
				xor ecx, ecx
				_loop:
					mov al, byte ptr [edx]
					mov ah, byte ptr [edx+1]
					mov [edx], ah
					mov [edx+1], al
					add edx, 2
					add ecx, 2
					cmp ecx, limit
				jl _loop
			}
		}
	}
	WriteFile((HANDLE)dwCookie, pbBuff, cb, (PULONG)pcb, NULL);
	return 0;
}

static LRESULT CALLBACK Marks_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_PAINT, Marks_OnPaint);
		HANDLE_MSG (hwnd, WM_LBUTTONUP, Marks_OnLButtonUp);

		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Marks_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	HWND				*phMarks;

	phMarks = (HWND *)GetWindowLongPtrW(GetParent(hwnd), OFFSET_SYS);	
	if(phMarks[MARK_SCHEDULE] == hwnd && g_hAdjust == NULL){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_ADJUST_SCHEDULE, 0), 0);
	}
	else if(phMarks[MARK_CHANGED] == hwnd){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
	}
	else if(phMarks[MARK_PRIORITY] == hwnd){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_HIGH_PRIORITY, 0), 0);
	}
	else if(phMarks[MARK_PROTECTED] == hwnd){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_TOGGLE_PROTECTION, 0), 0);
	}
	else if(phMarks[MARK_COMPLETED] == hwnd){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_MARK_AS_COMPLETED, 0), 0);
	}
	else if(phMarks[MARK_PASSWORD] == hwnd){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_LOCK_NOTE, 0), 0);
	}
	else if(phMarks[MARK_PIN] == hwnd){
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_ADD_PIN, 0), 0);
	}
}

static void Marks_OnPaint(HWND hwnd)
{
	PMEMNOTE			pNote = MEMNOTE(GetParent(hwnd));
	P_NOTE_RTHANDLES	pH;
	PAINTSTRUCT			ps;
	int					index;
	RECT				rc;

	if(pNote && pNote->pRTHandles){
		BeginPaint(hwnd, &ps);
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			pH = pNote->pRTHandles;
		}
		else{
			if(g_DockSettings.fCustSkin){
				pH = &g_DRTHandles;
			}
			else{
				if((pNote->pAppearance->nPrivate & F_SKIN) != F_SKIN){
					pH = pNote->pRTHandles;
				}
				else{
					pH = &g_DRTHandles;
				}
			}
		}
		GetWindowRect(hwnd, &rc);
		index = (int)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
		DrawTransparentBitmap(pH->hbSys, ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, (rc.right - rc.left) * index, 0, CLR_MASK);
		EndPaint(hwnd, &ps);
	}
}

static LRESULT CALLBACK Tbr_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
	HANDLE_MSG (hwnd, WM_DRAWITEM, Tbr_OnDrawItem);
	HANDLE_MSG (hwnd, WM_MEASUREITEM, Tbr_OnMeasureItem);
	HANDLE_MSG (hwnd, WM_COMMAND, Tbr_OnCommand);
	HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Tbr_OnInitMenuPopup);

	case PNM_NEW_FONT_FACE:
		SetREFontFace(GetParent(hwnd), &g_SelectedFont);
		SetFocus(GetParent(hwnd));
		return FALSE;
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Tbr_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
}

static void Tbr_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	RemoveBell(GetParent(hwnd));
	if(id > IDI_SIZE_START && id <= IDI_SIZE_START + MAX_FONT_SIZE){
		SetREFontSize(GetParent(hwnd), id - IDI_SIZE_START);
		RemoveSizeMenu();
		SetFocus(GetParent(hwnd));
		//if(id - IDI_SIZE_START != GetCFSize()){
			//SetREFontSize(GetParent(hwnd), id - IDI_SIZE_START);
			//RemoveSizeMenu();
			//SetFocus(GetParent(hwnd));
		//}
	}
	else if(id >= IDI_FORMAT_COLOR_START && id <= IDI_FORMAT_COLOR_END){
		m_CurrColorsChoice = CC_TEXT;
		ColorChanged(GetParent(hwnd), id);
	}
	else if(id >= IDI_FORMAT_HIGHLIGHT_START && id <= IDI_FORMAT_HIGHLIGHT_END){
		m_CurrColorsChoice = CC_HIGHLIGHT;
		ColorChanged(GetParent(hwnd), id);
	}
	else{
		switch(id){
			case IDM_NO_BULLETS:
				RemoveAllBulletsNumbering(GetParent(hwnd));
				SetFocus(GetParent(hwnd));
				break;
			case IDM_SIMPLE_BULLETS:
			case IDM_NUMBERS_PERIOD:
			case IDM_NUMBERS_PARENS:
			case IDM_LC_PERIOD:
			case IDM_LC_PARENS:
			case IDM_UC_PERIOD:
			case IDM_UC_PARENS:
			case IDM_ROMAN_LC:
			case IDM_ROMAN_UC:
				SetREBulletsAndNumbering(GetParent(hwnd), id);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_BOLD + 5000:
				SetREFontDecoration(GetParent(hwnd), FD_BOLD);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_ITALIC + 5000:
				SetREFontDecoration(GetParent(hwnd), FD_ITALIC);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_UNDERLINE + 5000:
				SetREFontDecoration(GetParent(hwnd), FD_UNDERLINE);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_STRIKETHROUGH + 5000:
				SetREFontDecoration(GetParent(hwnd), FD_STRIKETHROUGH);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_A_LEFT + 5000:
				SetREAlignment(GetParent(hwnd), PFA_LEFT);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_A_CENTER + 5000:
				SetREAlignment(GetParent(hwnd), PFA_CENTER);
				SetFocus(GetParent(hwnd));
				break;
			case CMD_A_RIGHT + 5000:
				SetREAlignment(GetParent(hwnd), PFA_RIGHT);
				SetFocus(GetParent(hwnd));
				break;
		}
	}
}

static void Tbr_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void Tbr_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static LRESULT CALLBACK Command_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
		HANDLE_MSG (hwnd, WM_PAINT, Command_OnPaint);
		HANDLE_MSG (hwnd, WM_LBUTTONUP, Command_OnLButtonUp);

		case WM_RBUTTONUP:
		case WM_MBUTTONUP:{
			RecreateCommandTooltip(hwnd);
			return FALSE;
		}
		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Command_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	POINT		pt;
	int			id;
	BOOL		delete = FALSE;

	GetCursorPos(&pt);
	id = GetDlgCtrlID(hwnd);
	switch(id){
		case CMD_FONT + 5000:{
			CBCREATION cbr;
			cbr.hParent = g_hMain;
			cbr.hButton = hwnd;
			cbr.hEdit = (HWND)GetPropW(cbr.hParent, PH_EDIT);
			SetCurrentFontFace((HWND)GetPropW(GetParent(GetParent(hwnd)), PH_EDIT));
			CreateComboDialog(&cbr, g_hInstance);
			break;
		}
		case CMD_COLOR + 5000:
		case CMD_HIGHLIGHT + 5000:{
			if(id == CMD_COLOR + 5000){
				m_CurrColorsChoice = CC_TEXT;
				TrackPopupMenu(m_hMenuTColor, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
			}
			else{
				m_CurrColorsChoice = CC_HIGHLIGHT;
				TrackPopupMenu(m_hMenuTHighlight, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
			}
			break;
		}
		case CMD_SIZE + 5000:{
			MENUITEMINFOW mi;
			int size = GetCurrentFontSize((HWND)GetPropW(GetParent(GetParent(hwnd)), PH_EDIT));
			SetCFSize(size);
			CreateSizeMenu(&g_hSizePopUp, TRUE);
			ZeroMemory(&mi, sizeof(mi));
			mi.cbSize = sizeof(mi);
			mi.fMask = MIIM_STATE;
			mi.fState = MFS_HILITE;
			SetMenuItemInfoW(g_hSizePopUp, IDI_SIZE_START + size, FALSE, &mi);
			TrackPopupMenu(g_hSizePopUp, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
			break;
		}
		case CMD_BULLETS + 5000:
			CheckBulletsMenuItems((HWND)GetPropW(GetParent(GetParent(hwnd)), PH_EDIT));
			TrackPopupMenu(g_hBulletsMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
			break;
		case CMD_BOLD + 5000:
		case CMD_ITALIC + 5000:
		case CMD_UNDERLINE + 5000:
		case CMD_STRIKETHROUGH + 5000:
		case CMD_A_LEFT + 5000:
		case CMD_A_CENTER + 5000:
		case CMD_A_RIGHT + 5000:
		// case CMD_BULLETS + 5000:
			//forward processing to toolbar window
			SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(id, 0), (LPARAM)hwnd);
			break;
		case IDM_DEL:
			if(!m_InDblClick){
				delete = TRUE;
				SendMessageW(GetParent(GetParent(hwnd)), WM_COMMAND, MAKEWPARAM(IDM_DEL, 0), 0);
			}
			break;
		case IDM_HIDE:
			if(!m_InDblClick){
				SendMessageW(GetParent(GetParent(hwnd)), WM_COMMAND, MAKEWPARAM(IDM_HIDE, 0), 0);
			}
			break;
	}
	if(!delete)
		RecreateCommandTooltip(hwnd);
}

static void Command_OnPaint(HWND hwnd)
{
	PMEMNOTE			pNote = MEMNOTE(GetParent(GetParent(hwnd)));
	PAINTSTRUCT			ps;
	RECT				rc;
	int					id;

	BeginPaint(hwnd, &ps);
	id = GetDlgCtrlID(hwnd);
	if(pNote->pRTHandles){
		//check for docked note
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			if(g_DockSettings.fCustSkin){
				if(g_RTHandles.hbSkin){
					memcpy(pNote->pRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
				}
			}
		}
		GetClientRect(hwnd, &rc);
		switch(id){
		case IDM_DEL:
		case IDM_HIDE:
			if(pNote->pRTHandles->hbDelHide){
				DrawTransparentBitmap(pNote->pRTHandles->hbDelHide, ps.hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), (id - IDM_HIDE) * (rc.right - rc.left), 0, pNote->pRTHandles->crMask);
			}
			break;
		default:
			if(pNote->pRTHandles->hbCommand){
				if(!pNote->pRTHandles->vertTbr)
					DrawTransparentBitmap(pNote->pRTHandles->hbCommand, ps.hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), (id - 5000) * (rc.right - rc.left), 0, pNote->pRTHandles->crMask);
				else
					DrawTransparentBitmap(pNote->pRTHandles->hbCommand, ps.hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), 0, (id - 5000) * (rc.bottom - rc.top), pNote->pRTHandles->crMask);
			}
		}
	}
	EndPaint(hwnd, &ps);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetCommandsTooltips
 Created  : Tue Jan  8 12:06:50 2008
 Modified : Tue Jan  8 12:06:50 2008

 Synopsys : Creates toolbar tooltips
 Input    : hwnd - note window handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetCommandsTooltips(HWND hwnd){

	wchar_t 		szBuffer[256], szKey[12];
	P_NCMD_BAR		pC, pDH;
	TOOLINFOW		ti;

	ZeroMemory(&ti, sizeof(ti));
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	pC = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS);
	for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
		_itow(5000 + i, szKey, 10);
		GetPrivateProfileStringW(S_COMMANDS, szKey, NULL, szBuffer, 256, g_NotePaths.CurrLanguagePath);
		ti.uId = (UINT)pC->hCmd;
		ti.hwnd = pC->hCmd;
		if(pC->hTtp != NULL){
			SendMessageW(pC->hTtp, TTM_DELTOOLW, 0, (LPARAM)&ti);
			DestroyWindow(pC->hTtp);
		}
		ti.lpszText = szBuffer;
		pC->hTtp = CreateWindowExW(WS_EX_TOPMOST, L"Tooltips_class32", NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);
		SendMessageW(pC->hTtp, TTM_ADDTOOLW, 0, (LPARAM)&ti);
		pC++;
	}
	pDH = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_DH);
	for(int i = 0; i < CMD_SYS_BUTTONS; i++){
		_itow(IDM_HIDE + i, szKey, 10);
		GetPrivateProfileStringW(S_MENU, szKey, NULL, szBuffer, 256, g_NotePaths.CurrLanguagePath);
		ti.uId = (UINT)pDH->hCmd;
		if(pDH->hTtp != NULL){
			SendMessageW(pDH->hTtp, TTM_DELTOOLW, 0, (LPARAM)&ti);
			DestroyWindow(pDH->hTtp);
		}
		ti.lpszText = szBuffer;
		pDH->hTtp = CreateWindowExW(WS_EX_TOPMOST, L"Tooltips_class32", NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);
		SendMessageW(pDH->hTtp, TTM_ADDTOOLW, 0, (LPARAM)&ti);
		pDH++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RecreateCommandTooltip
 Created  : Tue Jan  8 12:05:48 2008
 Modified : Tue Jan  8 12:05:48 2008

 Synopsys : Recreates toolbar tooltips
 Input    : hwnd - note window handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RecreateCommandTooltip(HWND hwnd){

	wchar_t 		szBuffer[256], szKey[12], szSection[64];
	P_NCMD_BAR		pCmd;
	TOOLINFOW		ti;
	int				id;

	if(MEMNOTE(GetParent(GetParent(hwnd)))){
		ZeroMemory(&ti, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;

		id = GetDlgCtrlID(hwnd);
		switch(id){
		case IDM_HIDE:
		case IDM_DEL:
			pCmd = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(GetParent(hwnd)), OFFSET_DH);
			pCmd += (id - IDM_HIDE);
			wcscpy(szSection, S_MENU);
			break;
		default:
			pCmd = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(GetParent(hwnd)), OFFSET_COMMANDS);
			pCmd += (id - 5000);
			wcscpy(szSection, S_COMMANDS);
		}
		_itow(id, szKey, 10);
		GetPrivateProfileStringW(szSection, szKey, NULL, szBuffer, 256, g_NotePaths.CurrLanguagePath);
		ti.uId = (UINT)hwnd;
		ti.hwnd = hwnd;
		if(pCmd->hTtp != NULL){
			SendMessageW(pCmd->hTtp, TTM_DELTOOLW, 0, (LPARAM)&ti);
			DestroyWindow(pCmd->hTtp);
		}
		ti.lpszText = szBuffer;
		pCmd->hTtp = CreateWindowExW(WS_EX_TOPMOST, L"Tooltips_class32", NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);
		SendMessageW(pCmd->hTtp, TTM_ADDTOOLW, 0, (LPARAM)&ti);
	}
}

void DeleteRTFile(wchar_t * id){
	wchar_t		szPath[MAX_PATH];

	if(NotesDirExists(g_NotePaths.DataDir)){
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, id);
		wcscat(szPath, NOTE_EXTENTION);
		DeleteFileW(szPath);
		DeleteAutoSavedFile(id);
	}
}

static void DeleteAutoSavedFile(wchar_t * id){
	wchar_t		szPath[MAX_PATH], szDBFile[MAX_PATH];
	HANDLE		hFile;
	BOOL		fDelete = false;
	DWORD		dwSize = 0;

	if(NotesDirExists(g_NotePaths.DataDir)){
		//remove section from index file
		wcscpy(szDBFile, g_NotePaths.DataDir);
		wcscat(szDBFile, AUTOSAVE_INDEX_FILE);
		WritePrivateProfileSectionW(id, NULL, szDBFile);
		//delete temp file
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, L"~");
		wcscat(szPath, id);
		wcscat(szPath, L".no~");
		DeleteFileW(szPath);
		//delete index file if it's empty
		hFile = CreateFileW(szDBFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			dwSize = GetFileSize(hFile, NULL);
			if(dwSize == 0)
				fDelete = true;
			CloseHandle(hFile);
			if(fDelete)
				DeleteFileW(szDBFile);
		}
	}
}

int GetFirstNoteCharacters(HWND hNote, wchar_t * lpBuffer, int nMax){
	HWND		hEdit;
	wchar_t		szBuffer[nMax + 1];
	int			count = 0;

	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	count = GetWindowTextLengthW(hEdit);
	if(count > nMax)
		count = GetWindowTextW(hEdit, szBuffer, nMax + 1);
	else
		count = GetWindowTextW(hEdit, szBuffer, count + 1);
	if(count > 0){
		wcscpy(lpBuffer, szBuffer);
		while(*lpBuffer++){
			if(*lpBuffer == '\r' || *lpBuffer == '\n'){
				*lpBuffer = '\0';
				break;
			}
		}
	}
	return count;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetTemporarySaveName
 Created  : Tue Jan  8 12:04:33 2008
 Modified : Tue Jan  8 12:04:33 2008

 Synopsys : Creates temporary name when note is saving for the first time 
            (the first nMax characters from note text)
 Input    : hNote - note window handle
            lpName - buffer for note name
            nMax - max count of characters to get from note text
 Output   : Temporary name length
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
int GetTemporarySaveName(HWND hNote, wchar_t * lpName, int nMax){

	wchar_t		szTemp[nMax + 1];
	int			count = 0;

	switch(GetSmallValue(g_SmallValues, SMS_DEF_NAMING)){
		case 0:
			count = GetFirstNoteCharacters(hNote, lpName, nMax);
			break;
		case 1:
			ConstructDateTimeString(NULL, lpName);
			// GetFormattedDateTime(lpName);
			count = wcslen(lpName);
			break;
		case 2:
			ConstructDateTimeString(NULL, lpName);
			// GetFormattedDateTime(lpName);
			wcscat(lpName, L" ");
			GetFirstNoteCharacters(hNote, szTemp, nMax - wcslen(lpName));
			wcscat(lpName, szTemp);
			count = wcslen(lpName);
			break;
	}
	return count;
}

static void SetCurrentFontFace(HWND hEdit){

	CHARFORMAT2W		chf;

	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_FACE;
	RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	wcscpy(g_CurrentFontFace, chf.szFaceName);

}

static int GetCurrentFontSize(HWND hEdit){

	CHARFORMAT2W		chf;
	int					size;
	HDC					hdc;

	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_SIZE;
	RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	hdc = GetDC(hEdit);
	size = -(chf.yHeight * GetDeviceCaps(hdc, LOGPIXELSY)) / (20 * 72);
	size = -MulDiv(size, 72, GetDeviceCaps(hdc, LOGPIXELSY));
	ReleaseDC(hEdit, hdc);
	return size;
}

static void InitAppearancePart(HWND hwnd){
	//fill skins
	if(m_TNHandles.hbSkin){
		FillSkins(hwnd, m_TNAppearance.szSkin, TRUE, FALSE);
	}
	else{
		FillSkins(hwnd, m_TNAppearance.szSkin, FALSE, TRUE);
		RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
		SendMessageW(GetDlgItem(hwnd, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TNAppearance.crWindow);
	}
}

static void InitSchedulePart(HWND hwnd){

	PMEMNOTE			pNote = (PMEMNOTE)GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);
	P_SCHEDULE_TYPE		pS;
	wchar_t				szDay[16];
	int					index, count, data;

	//schedule part
	if(IsBitOn(pNote->pData->res1, NB_NOT_TRACK))
		CheckDlgButton(hwnd, IDC_CHK_NO_TRACK, BST_CHECKED);
	else
		CheckDlgButton(hwnd, IDC_CHK_NO_TRACK, BST_UNCHECKED);

	pS = pNote->pSchedule;
	if(IsBitOn(pS->params, SP_SOUND_IN_LOOP)){
		CheckDlgButton(hwnd, IDC_CHK_LOOP_SOUND, BST_CHECKED);
		// EnableWindow(GetDlgItem(hwnd, IDC_ST_STOP_LOOP), true);
		// EnableWindow(GetDlgItem(hwnd, IDC_CBO_STOP_LOOP), true);
	}
	else{
		CheckDlgButton(hwnd, IDC_CHK_LOOP_SOUND, BST_UNCHECKED);
		// EnableWindow(GetDlgItem(hwnd, IDC_ST_STOP_LOOP), false);
		// EnableWindow(GetDlgItem(hwnd, IDC_CBO_STOP_LOOP), false);
	}
	count = SendDlgItemMessageW(hwnd, IDC_CBO_STOP_LOOP, CB_GETCOUNT, 0, 0);
	for(int i = 0; i < count; i++){
		data = SendDlgItemMessageW(hwnd, IDC_CBO_STOP_LOOP, CB_GETITEMDATA, i, 0);
		if((int)pS->stopLoop == data){
			SendDlgItemMessageW(hwnd, IDC_CBO_STOP_LOOP, CB_SETCURSEL, i, 0);
			break;
		}
	}

	if(IsBitOn(pS->params, SP_USE_TTS)){
		CheckDlgButton(hwnd, IDC_OPT_USE_TTS, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_CUST_SOUND), false);
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_NOTE_VOICES), true);
		// EnableWindow(GetDlgItem(hwnd, IDC_CHK_LOOP_SOUND), false);
		SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_FILE, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_SOUND, false);
		SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_TEXT, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_VOICE, true);
		//fill voices
		AddVoicesToControl(GetDlgItem(hwnd, IDC_CBO_NOTE_VOICES), CB_ADDSTRING, CB_SETITEMDATA, CB_SETCURSEL, pS->szSound);
	}
	else{
		CheckDlgButton(hwnd, IDC_OPT_USE_WAV_FILE, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_CUST_SOUND), true);
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_NOTE_VOICES), false);
		// EnableWindow(GetDlgItem(hwnd, IDC_CHK_LOOP_SOUND), true);
		SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_FILE, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_SOUND, true);
		SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_TEXT, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_VOICE, false);
		//fill voices
		AddVoicesToControl(GetDlgItem(hwnd, IDC_CBO_NOTE_VOICES), CB_ADDSTRING, CB_SETITEMDATA, CB_SETCURSEL, g_DefVoice);
	}

	if(pS->scType - START_PROG != SCH_REP && pS->scType - START_COMP != SCH_REP && pS->scType - START_PROG != SCH_AFTER && pS->scType - START_COMP != SCH_AFTER)
		SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_SETCURSEL, pS->scType, 0);
	else if(pS->scType - START_PROG == SCH_REP || pS->scType - START_COMP == SCH_REP)
		SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_SETCURSEL, SCH_REP, 0);
	else if(pS->scType - START_PROG == SCH_AFTER || pS->scType - START_COMP == SCH_AFTER)
		SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_SETCURSEL, SCH_AFTER, 0);

	SendMessageW(m_Dlgs.hSchedule, WM_COMMAND, MAKEWPARAM(IDC_CBO_NOTE_SCHEDULE, CBN_SELENDOK), (LPARAM)GetDlgItem(m_Dlgs.hSchedule, IDC_CBO_NOTE_SCHEDULE));
	if(*pS->szSound != '\0')
		index = SendDlgItemMessageW(hwnd, IDC_CBO_CUST_SOUND, CB_FINDSTRINGEXACT, -1, (LPARAM)pS->szSound);
	else
		index = SendDlgItemMessageW(hwnd, IDC_CBO_CUST_SOUND, CB_FINDSTRINGEXACT, -1, (LPARAM)g_Sound.custSound);
	if(index == CB_ERR)
		SendDlgItemMessageW(hwnd, IDC_CBO_CUST_SOUND, CB_SETCURSEL, 0, 0);
	else
		SendDlgItemMessageW(hwnd, IDC_CBO_CUST_SOUND, CB_SETCURSEL, index, 0);
	//'monthly - exact' dialog
	SendDlgItemMessageW(m_Dns.hMonthExact, IDC_CBO_MONTH_EXACT, CB_RESETCONTENT, 0, 0);
	for(int i = 1; i <= 31; i++){
		_itow(i, szDay, 10);
		SendDlgItemMessageW(m_Dns.hMonthExact, IDC_CBO_MONTH_EXACT, CB_ADDSTRING, 0, (LPARAM)szDay);
	}
	SendDlgItemMessageW(m_Dns.hMonthExact, IDC_CBO_MONTH_EXACT, CB_SETCURSEL, 0, 0);
	//'monthly' - relative dialog
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_SETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_ORDINAL, CB_SETCURSEL, 0, 0);
	if(pS->scType == SCH_ED){
		SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
	}
	else if(pS->scType == SCH_ONCE){
		SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
	}
	else if(pS->scType == SCH_REP || pS->scType - START_PROG == SCH_REP || pS->scType - START_COMP == SCH_REP){
		SetPesterPart(&pS->scDate);
		if(pS->scType == SCH_REP){
			SendDlgItemMessageW(m_Dns.hRepeat, IDC_DTP_REPEAT_START, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scStart); 
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_TIME_START, BST_CHECKED);
			EnableWindow(GetDlgItem(m_Dns.hRepeat, IDC_DTP_REPEAT_START), TRUE);
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_COMP_START, BST_UNCHECKED);
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_PROG_START, BST_UNCHECKED);
		}
		else if(pS->scType - START_PROG == SCH_REP){
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_TIME_START, BST_UNCHECKED);
			EnableWindow(GetDlgItem(m_Dns.hRepeat, IDC_DTP_REPEAT_START), FALSE);
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_COMP_START, BST_UNCHECKED);
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_PROG_START, BST_CHECKED);
		}
		else{
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_TIME_START, BST_UNCHECKED);
			EnableWindow(GetDlgItem(m_Dns.hRepeat, IDC_DTP_REPEAT_START), FALSE);
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_COMP_START, BST_CHECKED);
			CheckDlgButton(m_Dns.hRepeat, IDC_OPT_PROG_START, BST_UNCHECKED);
		}
	}
	else if(pS->scType == SCH_PER_WD){
		for(int i = 0; i <= 6; i++){
			if(IsBitOn(pS->scDate.wDayOfWeek, i + 1)){
				CheckDlgButton(m_Dns.hWeekly, IDC_CHK_WD_0 + i, BST_CHECKED);
			}
			else{
				CheckDlgButton(m_Dns.hWeekly, IDC_CHK_WD_0 + i, BST_UNCHECKED);
			}
		}
		SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
	}
	else if(pS->scType == SCH_AFTER || pS->scType - START_PROG == SCH_AFTER || pS->scType - START_COMP == SCH_AFTER){
		if(pS->scType == SCH_AFTER)
			SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scStart); 
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_YEARS, pS->scDate.wYear, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_MONTHS, pS->scDate.wMonth, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_WEEKS, pS->scDate.wDayOfWeek, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_DAYS, pS->scDate.wDay, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_HOURS, pS->scDate.wHour, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_MINS, pS->scDate.wMinute, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_SECS, pS->scDate.wSecond, FALSE);
		if(pS->scType == SCH_AFTER){
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_TIME_START, BST_CHECKED);
			EnableWindow(GetDlgItem(m_Dns.hAfter, IDC_DTP_AFTER), TRUE);
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_COMP_START, BST_UNCHECKED);
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_PROG_START, BST_UNCHECKED);
		}
		else if(pS->scType - START_PROG == SCH_AFTER){
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_TIME_START, BST_UNCHECKED);
			EnableWindow(GetDlgItem(m_Dns.hAfter, IDC_DTP_AFTER), FALSE);
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_COMP_START, BST_UNCHECKED);
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_PROG_START, BST_CHECKED);
		}
		else{
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_TIME_START, BST_UNCHECKED);
			EnableWindow(GetDlgItem(m_Dns.hAfter, IDC_DTP_AFTER), FALSE);
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_COMP_START, BST_CHECKED);
			CheckDlgButton(m_Dns.hAfter, IDC_OPT_PROG_START, BST_UNCHECKED);
		}
	}
	else if(pS->scType == SCH_MONTHLY_EXACT){
		SendDlgItemMessageW(m_Dns.hMonthExact, IDC_DTP_MONTH_EXACT, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
		SendDlgItemMessageW(m_Dns.hMonthExact, IDC_CBO_MONTH_EXACT, CB_SETCURSEL, pS->scDate.wDay - 1, 0);
	}
	else if(pS->scType == SCH_MONTHLY_RELATIVE){
		SendDlgItemMessageW(m_Dns.hMonthRel, IDC_DTP_MONTH_EXACT, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate);
		//select day of week
		for(int i = 0; i <= 6; i++){
			if(SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_GETITEMDATA, i, 0) == pS->scDate.wDayOfWeek){
				SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_WEEKDAYS, CB_SETCURSEL, i, 0);
				break;
			}
		}
		//select ordinal part
		int		count = SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_ORDINAL, CB_GETCOUNT, 0, 0);
		for(int i = 0; i < count; i++){
			if(i == pS->scDate.wMilliseconds){
				SendDlgItemMessageW(m_Dns.hMonthRel, IDC_CBO_ORDINAL, CB_SETCURSEL, i, 0);
				break;
			}
		}
	}
	//'repeat' dialog
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_YEARS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 99, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_MONTHS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 12, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_WEEKS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 48, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_DAYS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 31, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_HOURS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 24, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_MINS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_SECS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
	//'after' dialog
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_YEARS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 99, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_MONTHS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 12, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_WEEKS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 48, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_DAYS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 31, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_HOURS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 24, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_MINS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_SECS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
}

static void SetTransValue(HWND hwnd, int value){
	wchar_t			szBuffer[256], szValue[32];

	_itow(value, szValue, 10);
	wcscat(szValue, L" %");
	wcscpy(szBuffer, m_TransCaption);
	wcscat(szBuffer, L": ");
	wcscat(szBuffer, szValue);
	SetDlgItemTextW(hwnd, IDC_GRP_TRANS, szBuffer);
	m_TAddAppearance.transValue = 255 - value * 2.55;
}

static BOOL CALLBACK NApp_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_DRAWITEM, NApp_OnDrawItem);
		HANDLE_MSG (hwnd, WM_COMMAND, NApp_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, NApp_OnInitDialog);
		HANDLE_MSG (hwnd, WM_HSCROLL, NApp_OnHScroll);

		// HANDLE_MSG (hwnd, WM_CTLCOLORSTATIC, NApp_OnCtlColorStatic);
		// case PNM_DRAW_SN_PVW:
			// DrawSimpleNotePreview((HWND)wParam, (DRAWITEMSTRUCT *)lParam, &m_TNAppearance, &m_TNHandles);
			// return TRUE;
		case GCN_COLOR_SELECTED:
			if(m_TNAppearance.crWindow != (COLORREF)lParam){
				PMEMNOTE	pNote = (PMEMNOTE)GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);
				m_TNAppearance.crWindow = (COLORREF)lParam;
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
				pNote->pFlags->maskChanged |= F_B_COLOR;
			}
			return TRUE;
		default: return FALSE;
	}
}

static void NApp_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(code == TB_THUMBPOSITION || code == TB_THUMBTRACK){
		SetTransValue(hwnd, pos);
	}
	else if(code == TB_LINEUP || code == TB_LINEDOWN || code == TB_PAGEUP || code == TB_PAGEDOWN || code == TB_BOTTOM || code == TB_TOP){
		SetTransValue(hwnd, SendDlgItemMessageW(hwnd, IDC_TRANS_TRACK_BAR, TBM_GETPOS, 0, 0));
	}
}

static BOOL NApp_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t		szId[12];
	RECT		rc1, rc2, rc3;
	HWND		hPicker;
	HWND		hTrack;
	int			transVal;

	GetWindowRect(GetDlgItem(hwnd, IDC_ST_BMP), &rc2);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc2, 2);

	GetWindowRect(GetDlgItem(hwnd, IDC_PL_1), &rc3);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc3, 2);
	hPicker = CreateGCPickerWindowW(rc3.left, rc3.top, (rc3.right - rc3.left), (rc3.bottom - rc3.top), hwnd, IDC_CHOOSE_COLOR, m_TNAppearance.crWindow);

	if(!g_RTHandles.hbSkin){
		GetClientRect(hwnd, &rc1);
		ShowWindow(GetDlgItem(hwnd, IDC_LST_SKIN), SW_HIDE);
		SetDlgCtlText(hwnd, IDC_CMD_SKNLESS_BCOLOR, g_NotePaths.CurrLanguagePath, L"Background color");
		SetDlgCtlText(hwnd, IDC_CMD_SKNLESS_CFONT, g_NotePaths.CurrLanguagePath, L"Caption font");
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_BCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), SW_SHOW);
		MoveWindow(GetDlgItem(hwnd, IDC_ST_BMP), ((rc1.right - rc1.left) - (rc2.right - rc2.left)) / 2, rc2.top, rc2.right - rc2.left, rc2.bottom - rc2.top, TRUE);
		ShowWindow(hPicker, SW_SHOW);
		BringWindowToTop(hPicker);
	}
	else{
		GetWindowRect(GetDlgItem(hwnd, IDC_PLACEHOLDER), &rc1);
		MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc1, 2);
		MoveWindow(GetDlgItem(hwnd, IDC_ST_BMP), rc1.left, rc2.top, rc2.right - rc2.left, rc2.bottom - rc2.top, TRUE);
		ShowWindow(hPicker, SW_HIDE);
	}

	//set up track values
	hTrack = GetDlgItem(hwnd, IDC_TRANS_TRACK_BAR);
	SendMessageW(hTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 10);
	_itow(IDC_GRP_TRANS, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Transparency", m_TransCaption, 128, g_NotePaths.CurrLanguagePath);
	EnableWindow(GetDlgItem(hwnd, IDC_GRP_TRANS), (g_NoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_LEFT_ST), (g_NoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(hTrack, (g_NoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_RIGHT_ST), (g_NoteSettings.transAllow ? TRUE : FALSE));
	if(m_TAddAppearance.transValue != 0)
		transVal = 100 - m_TAddAppearance.transValue / 2.55;
	else if(g_NoteSettings.transValue != 0)
		transVal = 100 - g_NoteSettings.transValue / 2.55;
	else
		transVal = 0;
	SetTransValue(hwnd, transVal);
	SendMessageW(hTrack, TBM_SETPOS, TRUE, transVal);

	return TRUE;
}

static void NApp_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PMEMNOTE	pNote = (PMEMNOTE)GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);

	switch(id){
		case IDC_CMD_SKNLESS_CFONT:{
				int res = ChangeSkinlessCaptionFont(hwnd, &m_TNAppearance.lfCaption, &m_TNAppearance.crCaption, &m_TNHandles, IDC_ST_BMP, FALSE);
				if((res & F_C_FONT) == F_C_FONT)
					pNote->pFlags->maskChanged |= F_C_FONT;
				if((res & F_C_COLOR) == F_C_COLOR)
					pNote->pFlags->maskChanged |= F_C_COLOR;
			}
			break;
		case IDC_LST_SKIN:	//show skin preview
			if(codeNotify == LBN_SELCHANGE){
				ChangeSkinPreview(hwnd, IDC_LST_SKIN, IDC_ST_BMP, &m_TNHandles, m_TNAppearance.szSkin, (pNote->pRTHandles->hbSkin == m_TNHandles.hbSkin) ? FALSE : TRUE);
				pNote->pFlags->maskChanged |= F_SKIN;
			}
			break;
	}
}

static void NApp_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC && lpDrawItem->CtlID == IDC_ST_BMP)
		//draw skin preview
		DrawSkinPreview(GetDlgItem(hwnd, IDC_ST_BMP), &m_TNHandles, &m_TNAppearance, m_TNHandles.crMask, lpDrawItem, TRUE);
}

static BOOL CALLBACK NSched_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, NSched_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, NSched_OnInitDialog);
		case WM_NOTIFY:{
			LPNMHDR		lpnm = (LPNMHDR)lParam;
			switch(lpnm->code){
				case TTN_NEEDTEXTW:{
					TOOLTIPTEXTW 	*ttp = (TOOLTIPTEXTW *)lParam;
					GetPrivateProfileStringW(S_OPTIONS, L"8064", L"Listen", ttp->szText, 79, g_NotePaths.CurrLanguagePath);
					break;
				}
			}
			return true;
		}
		default: return FALSE;
	}
}

static BOOL NSched_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND		hToolbar;

	hToolbar = GetDlgItem(hwnd, IDC_TBR_SOUND_FILE);
	DoToolbarW(hToolbar, g_hImlPvwNormal, g_hImlPvwGray, NELEMS(m_SoundButtonsF), m_SoundButtonsF);
	hToolbar = GetDlgItem(hwnd, IDC_TBR_SOUND_TEXT);
	DoToolbarW(hToolbar, g_hImlPvwNormal, g_hImlPvwGray, NELEMS(m_SoundButtonsV), m_SoundButtonsV);
	return TRUE;
}

static void NSched_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t			szBuffer[128];
	int				index;

	switch(id){
		case IDC_CBO_NOTE_SCHEDULE:
			if(codeNotify == CBN_SELENDOK){
				HWND		*pDlg = (HWND *)&m_Dns;
				int			count = SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_GETCOUNT, 0, 0);
				int			selected = SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_GETCURSEL, 0, 0);

				for(int i = 1; i < count; i++){
					if(i == selected)
						ShowWindow(*pDlg, SW_SHOW);
					else
						ShowWindow(*pDlg, SW_HIDE);
					pDlg++;
				}
			}
			break;
		case IDC_CHK_NO_TRACK:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_CHK_NO_TRACK) == BST_CHECKED)
					BitOn(&m_DataRes1, NB_NOT_TRACK);
				else
					BitOff(&m_DataRes1, NB_NOT_TRACK);
			}
			break;
		// case IDC_CHK_LOOP_SOUND:
			// if(codeNotify == BN_CLICKED){
				// if(IsDlgButtonChecked(hwnd, IDC_CHK_LOOP_SOUND) == BST_CHECKED){
					// EnableWindow(GetDlgItem(hwnd, IDC_ST_STOP_LOOP), true);
					// EnableWindow(GetDlgItem(hwnd, IDC_CBO_STOP_LOOP), true);
				// }
				// else{
					// EnableWindow(GetDlgItem(hwnd, IDC_ST_STOP_LOOP), false);
					// EnableWindow(GetDlgItem(hwnd, IDC_CBO_STOP_LOOP), false);
				// }
			// }
			// break;
		case IDC_CMD_PVW_CUST_SOUND:
			index = SendDlgItemMessageW(hwnd, IDC_CBO_CUST_SOUND, CB_GETCURSEL, 0, 0);
			SendDlgItemMessageW(hwnd, IDC_CBO_CUST_SOUND, CB_GETLBTEXT, index, (LPARAM)szBuffer);
			PreviewSound(szBuffer);
			break;
		case IDC_CMD_PVW_CUST_VOICE:{
			index = SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_VOICES, CB_GETCURSEL, 0, 0);
			DWORD	token = SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_VOICES, CB_GETITEMDATA, index, 0);
			SpeakNote((PMEMNOTE)GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA), (void *)token);
			break;
		}
		case IDC_OPT_USE_TTS:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_USE_TTS) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_CUST_SOUND), false);
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_NOTE_VOICES), true);
					// CheckDlgButton(hwnd, IDC_CHK_LOOP_SOUND, BST_UNCHECKED);
					// EnableWindow(GetDlgItem(hwnd, IDC_CHK_LOOP_SOUND), false);
					SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_FILE, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_SOUND, false);
					SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_TEXT, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_VOICE, true);
				}
			}
			break;
		case IDC_OPT_USE_WAV_FILE:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_USE_WAV_FILE) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_CUST_SOUND), true);
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_NOTE_VOICES), false);
					// EnableWindow(GetDlgItem(hwnd, IDC_CHK_LOOP_SOUND), true);
					SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_FILE, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_SOUND, true);
					SendDlgItemMessageW(hwnd, IDC_TBR_SOUND_TEXT, TB_ENABLEBUTTON, IDC_CMD_PVW_CUST_VOICE, false);
				}
			}
			break;
	}
}

static void RemoveBell(HWND hwnd){
	HWND				hBell;
	PMEMNOTE			pNote = MemNoteByHwnd(hwnd);

	if(pNote){
		if(pNote->pRTHandles->stopAlarmCounter > 0){
			KillTimer(pNote->hwnd, STOP_LOOP_TIMER_ID);
			pNote->pRTHandles->stopAlarmCounter = 0;
		}
		if(IsBitOn(pNote->pSchedule->params, SP_IN_ALARM)){
			BitOff(&pNote->pSchedule->params, SP_IN_ALARM);
			if(IsBitOn(pNote->pSchedule->params, SP_SOUND_IN_LOOP)){
				if(!IsBitOn(pNote->pSchedule->params, SP_USE_TTS)){
					PlaySoundW(NULL, 0, 0);
				}
				else{
					RemoveVoiceNotification(pNote->pVoice);
					StopSpeak(pNote->pVoice);
					ReleaseVoice(pNote->pVoice);
					pNote->pVoice = CreateVoice();
				}
			}
			else{
				StopSpeak(pNote->pVoice);
			}
		}
	}
	if(g_NoteSettings.visualAlert){
		//destoy bell window, if any
		hBell = (HWND)GetPropW(hwnd, PH_BELL);
		if(hBell && IsWindow(hBell)){
			//destroy bell window only if it exists
			DestroyWindow(hBell);
		}
	}
}

static void SaveNewCFont(PMEMNOTE pNote, LPLOGFONTW lplf){
	NOTE_APPEARANCE		app = {0};

	GetPrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	memcpy(&app.lfCaption, lplf, sizeof(LOGFONTW));
	app.fFontSet = TRUE;
	app.nPrivate |= F_C_FONT;
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
}

static void SaveNewCColor(PMEMNOTE pNote, COLORREF crCaption){
	NOTE_APPEARANCE	app = {0};

	GetPrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	app.crCaption = crCaption;
	app.fFontSet = TRUE;
	app.nPrivate |= F_C_COLOR;
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
}

static void SaveNewBColor(PMEMNOTE pNote, COLORREF crWindow){
	NOTE_APPEARANCE	app = {0};

	GetPrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	app.crWindow = crWindow;
	app.nPrivate |= F_B_COLOR;
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
}

void SaveVisibility(HWND hwnd, BOOL visible){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	NOTE_DATA			data;

	if(pNote->pData){
		pNote->pData->visible = visible;
		//save only the visibility data
		if(pNote->pFlags->fromDB){
			//version 4.0
			if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
				data.visible = visible;
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			}
			else{
				//version 3.5
				if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA) - sizeof(RECT) - sizeof(BOOL), g_NotePaths.DataFile)){
					data.visible = visible;
					WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				}
			}
		}		
	}
}

static void WriteNotePlacement(HWND hwnd, BOOL fSave){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	RECT				rcNote;
	NOTE_DATA			data;
	NOTE_REL_POSITION	nrp;
	SIZE				sz;

	GetWindowRect(hwnd, &rcNote);
	if(pNote->pData){
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			sz = GetScreenMetrics();
			//check rectangle metrics and repair them if needed
			CheckSavedRectangle(&rcNote);

			//save current absolute position
			if(pNote->pRTHandles->rolled){
				rcNote.bottom = rcNote.top + (pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_FIT_TO_CAPTION)){
					rcNote.right = rcNote.left + (pNote->pRTHandles->rcSize.right - pNote->pRTHandles->rcSize.left);
				}
			}
			CopyRect(&pNote->pData->rcp, &rcNote);
			if(fSave){
				if(!GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
					memcpy(&data, pNote->pData, sizeof(NOTE_DATA));
				}
				CopyRect(&data.rcp, &rcNote);
				//save rolled status
				if(pNote->pRTHandles->rolled != data.rolled){
					data.rolled = pNote->pRTHandles->rolled;
				}
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				//save current relational position
				nrp.left = (double)rcNote.left / (double)sz.cx;
				nrp.top = (double)rcNote.top / (double)sz.cy;
				nrp.width = rcNote.right - rcNote.left;
				nrp.height = rcNote.bottom - rcNote.top;
				WritePrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), g_NotePaths.DataFile);
			}
		}
	}
}

void AddToFavorites(PMEMNOTE pNote){
	// wchar_t			szBuffer[256];

	// GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	// if(wcslen(szBuffer) == 0)
	if(pNote->pRTHandles->favorite == 0){
		WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, pNote->pData->szName, g_NotePaths.INIFile);
		pNote->pRTHandles->favorite = GROUP_FAVORITES;
	}
	else{
		WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, g_NotePaths.INIFile);
		pNote->pRTHandles->favorite = 0;
	}

	SendMessageW(g_hMain, PNM_FAVORITES_CHANGED, 0, 0);
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, PNM_CTRL_FAV_STATUS_CHANGED, 0, (LPARAM)pNote);
}

PMITEM NoteMenu(void){
	return m_NMenus;
}

PMITEM EditMenu(void){
	return m_EMenus;
}

PMITEM DropMenu(void){
	return m_DMenus;
}

int NoteMenuCount(void){
	return NELEMS(m_NMenus);
}

int EditMenuCount(void){
	return NELEMS(m_EMenus);
}

int DropMenuCount(void){
	return NELEMS(m_DMenus);
}

static BOOL CALLBACK Dns_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Dns_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Dns_OnInitDialog);
	// HANDLE_MSG (hwnd, WM_DRAWITEM, Dns_OnDrawItem);
	default: return FALSE;
	}
}

static BOOL Dns_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc1, rc2, rc3, rc4;

	GetClientRect(GetParent(hwnd), &rc1);
	GetClientRect(hwnd, &rc2);
	GetWindowRect(GetDlgItem(GetParent(hwnd), IDC_CBO_NOTE_SCHEDULE), &rc3);
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc3, 2);
	GetWindowRect(GetDlgItem(GetParent(hwnd), IDC_CHK_NO_TRACK), &rc4);
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc4, 2);
	MoveWindow(hwnd, ((rc1.right - rc1.left) - (rc2.right - rc2.left)) / 2, rc3.bottom, rc2.right - rc2.left, rc2.bottom - rc2.top - (rc4.bottom - rc4.top) + 16, TRUE);
	return FALSE;
}

// static void Dns_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
// {
	// if(lpDrawItem->CtlType == ODT_COMBOBOX)
		// DrawComboItem(lpDrawItem);
// }

static void Dns_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_EDT_DAYS:
		case IDC_EDT_HOURS:
		case IDC_EDT_MINS:
		case IDC_EDT_SECS:
			if(hwnd == m_Dns.hRepeat){
				if(codeNotify == EN_UPDATE){
					BOOL	translated;
					//prevent empty control
					if(GetDlgItemInt(hwnd, id, &translated, FALSE) == 0 && translated == FALSE)
						SetDlgItemInt(hwnd, id, 0, FALSE);
				}
			}
			break;
		case IDC_OPT_TIME_START:
		case IDC_OPT_COMP_START:
		case IDC_OPT_PROG_START:
			if(IsDlgButtonChecked(hwnd, IDC_OPT_TIME_START) == BST_CHECKED){
				if(hwnd == m_Dns.hAfter)
					EnableWindow(GetDlgItem(hwnd, IDC_DTP_AFTER), TRUE);
				else if(hwnd == m_Dns.hRepeat)
					EnableWindow(GetDlgItem(hwnd, IDC_DTP_REPEAT_START), TRUE);
			}
			else{
				if(hwnd == m_Dns.hAfter)
					EnableWindow(GetDlgItem(hwnd, IDC_DTP_AFTER), FALSE);
				else if(hwnd == m_Dns.hRepeat)
					EnableWindow(GetDlgItem(hwnd, IDC_DTP_REPEAT_START), FALSE);
			}
			break;
	}
}

static void InsertDateTime(HWND hwnd){
	wchar_t			szDate[128];
	
	ConstructDateTimeString(NULL, szDate);
	// GetFormattedDateTime(szDate);
	SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_REPLACESEL, TRUE, (LPARAM)szDate);
}

void DeleteNoteCompletely(PMEMNOTE pNote){
	WIN32_FIND_DATAW		fd;
	HANDLE					handle = INVALID_HANDLE_VALUE;
	BOOL					result = TRUE;
	wchar_t					szPath[MAX_PATH], szId[128];

	wcscpy(szId, pNote->pFlags->id);

	wcscpy(szPath, g_NotePaths.BackupDir);
	wcscat(szPath, pNote->pFlags->id);
	wcscat(szPath, L"_*");
	wcscat(szPath, BACK_NOTE_EXTENTION);

	WritePrivateProfileSectionW(pNote->pFlags->id, NULL, g_NotePaths.DataFile);
	DeleteRTFile(pNote->pFlags->id);
	if(pNote->pData->visible){
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_DOCK_NONE, 0), 0);
		}
		SendMessageW(pNote->hwnd, WM_CLOSE, 0, 0);
	}
	DeleteMemNote(pNote);
	//delete all backup copies
	handle = FindFirstFileW(szPath, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szPath, g_NotePaths.BackupDir);
			wcscat(szPath, fd.cFileName);
			DeleteFileW(szPath);
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
	// if(g_hCPDialog)
		// SendMessageW(g_hCPDialog, PNM_CTRL_BACK_DELETE, 0, (LPARAM)szId);
}

void CenterRestorePlacementNote(PMEMNOTE pNote, int operation, BOOL leaveAtPosition){
	BOOL isVisible = FALSE;

	if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
		isVisible = pNote->pData->visible;
		if(isVisible){
			if(DockType(pNote->pData->dockData) != DOCK_NONE){
				SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_DOCK_NONE, 0), 0);
			}
		}
		else{
			CreateNote(pNote, g_hInstance, FALSE, NULL);
			SaveVisibility(pNote->hwnd, TRUE);
		}

		if(operation == IDM_SHOW_CENTER)
			if(leaveAtPosition)
				RestorePlacement(pNote->hwnd, FALSE);
			else
				CenterWindow(pNote->hwnd, FALSE);
		else{
			if(leaveAtPosition){
				//do not restore size for skinned notes
				if(pNote->pRTHandles->hbSkin)
					RestorePlacement(pNote->hwnd, FALSE);
				else
					RestorePlacement(pNote->hwnd, TRUE);
			}
			else{
				//do not restore size for skinned notes
				if(pNote->pRTHandles->hbSkin)
					CenterWindow(pNote->hwnd, FALSE);
				else
					CenterWindow(pNote->hwnd, TRUE);
			}
		}

		if(!isVisible){
			ShowWindow(pNote->hwnd, SW_SHOW);
		}

		SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		if(!pNote->pData->onTop){
			SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		//have to reapply topmost for all appropriate windows, because this flag is gone by some mystery reason
		PMEMNOTE	pTemp = MemoryNotes();
		while(pTemp){
			if(pTemp->pData->visible && pTemp->pData->onTop && pTemp->hwnd != pNote->hwnd)
				SetWindowPos(pTemp->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			pTemp = pTemp->next;
		}

		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
}

void DuplicateNote(PMEMNOTE pSrc){
	HWND		hNote = NULL;
	PMEMNOTE	pNote;

	if(!UnlockNote(pSrc) || !UnlockGroup(pSrc->pData->idGroup)){
		return;
	}
	pNote = AddMemNote();
	if(pNote){
		DuplicateMemNote(pSrc, pNote);
		LoadDuplicatedNoteProperties(pNote, &g_RTHandles, pSrc);
		hNote = CreateNote(pNote, g_hInstance, FALSE, NULL);
		//load RTF file contents
		if(pSrc->pFlags->fromDB && pSrc->pFlags->saved){
			ReadNoteRTFFile(pSrc->pFlags->id, (HWND)GetPropW(hNote, PH_EDIT), NULL);
		}
		else{
			//save edit box contents to temporary file and load from it
			ExchangeREContents((HWND)GetPropW(pSrc->hwnd, PH_EDIT), (HWND)GetPropW(hNote, PH_EDIT));
		}
		if(pNote->pSchedule->scType != SCH_NO)
			pNote->pFlags->maskChanged |= F_SCHEDULE;
		RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, PNM_CTRL_INS, 0, (LPARAM)pNote);
			SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
		}
	}
}

static void LoadDuplicatedNoteProperties(PMEMNOTE pNote, P_NOTE_RTHANDLES pHandles, PMEMNOTE pSrc){
	BOOL				fSaveSchedule = FALSE;
	wchar_t				szID[128];

	pNote->pFlags->saved = TRUE;

	//check for possible schedule from versions prior to 5.5 in order to set start time
	if(pNote->pSchedule->scType == SCH_REP){
		if(pNote->pSchedule->scStart.wYear == 0 && pNote->pSchedule->scStart.wMonth == 0 && pNote->pSchedule->scStart.wDay == 0 
		&& pNote->pSchedule->scStart.wHour == 0 && pNote->pSchedule->scStart.wMinute == 0){
			GetLocalTime(&pNote->pSchedule->scStart);
			fSaveSchedule = TRUE;
		}
	}
	//reset changed flag
	pNote->pFlags->maskChanged = 0;

	FormatNewNoteID(szID);
	//set note id
	wcscpy(pNote->pFlags->id, szID);
	//check schedule
	if(pNote->pSchedule->scType != 0 && pNote->pData->idGroup != GROUP_RECYCLE){
		if(pNote->pSchedule->scType == SCH_REP || pNote->pSchedule->scType - SCH_REP == START_COMP 
		|| pNote->pSchedule->scType - SCH_REP == START_PROG){	//set repeat count according to timer interval
			pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
		}
		pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
	}
	//docking
	if(DockType(pNote->pData->dockData) == DOCK_NONE || pNote->pData->idGroup == GROUP_RECYCLE){
		//set dock index to -777 for notes from previous version
		if(DockIndex(pNote->pData->dockData) != -777)
			SetDockIndex(&pNote->pData->dockData, -777);
	}
	if(fSaveSchedule){
		//save changed schedule (from versions prior to 5.5)
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
	}
	//set duplicated flag
	BitOn(&pNote->pData->res1, NB_DUPLICATED);
	//set visibility
	if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
		//new note is always visible
		pNote->pData->visible = TRUE;
	}
	if((pSrc->pAppearance->nPrivate & F_B_COLOR) == F_B_COLOR)
		pNote->pFlags->maskChanged |= F_B_COLOR;
	if((pSrc->pAppearance->nPrivate & F_SKIN) == F_SKIN)
		pNote->pFlags->maskChanged |= F_SKIN;
	if((pSrc->pAppearance->nPrivate & F_RTFONT) == F_RTFONT)
		pNote->pFlags->maskChanged |= F_RTFONT;
	if((pSrc->pAppearance->nPrivate & F_C_COLOR) == F_C_COLOR)
		pNote->pFlags->maskChanged |= F_C_COLOR;
	if((pSrc->pAppearance->nPrivate & F_C_FONT) == F_C_FONT)
		pNote->pFlags->maskChanged |= F_C_FONT;
}

void ExchangeSameRE(HWND hEdit){
	CHARRANGE			chr = {0};

	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&chr);
	ExchangeREContents(hEdit, hEdit);
	SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
}

static void ExchangeREContents(HWND hEditSrc, HWND hEditDest){
	wchar_t				szTempPath[MAX_PATH + 128];
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	EDITSTREAM			esm;

	GetNoteTempFileName(szTempPath);
	hFile = CreateFileW(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		esm.dwCookie = (DWORD)hFile;
		esm.dwError = 0;
		esm.pfnCallback = OutStreamCallback;
		RichEdit_StreamOut(hEditSrc, SF_RTF | SF_UNICODE, &esm);
		CloseHandle(hFile);
		hFile = CreateFileW(szTempPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			esm.dwCookie = (DWORD)hFile;
			esm.dwError = 0;
			esm.pfnCallback = InStreamCallback;
			CReCallback * pCallback = new_CReCallback();
			SendMessageW(hEditDest, EM_SETOLECALLBACK, 0, (LPARAM)pCallback);
			RichEdit_StreamIn(hEditDest, SF_RTF | SF_UNICODE, &esm);
			if(pCallback)
				delete_CReCallback(pCallback);
			CloseHandle(hFile);
		}
		DeleteFileW(szTempPath);
	}
}

static void InsertPicture(HWND hNote, wchar_t * lpFile){
	wchar_t			* lpExt;
	HBITMAP 		hBmp = NULL;
	HWND			hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	PMEMNOTE		pNote = MemNoteByHwnd(hNote);
	LPRICHEDITOLE 	pREOle = NULL;
					
	if(!hEdit || !pNote)
		return;
	SendMessageW(hEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&pREOle);
	if(pREOle == NULL)
		return;
	lpExt = PathFindExtensionW(lpFile);
	if(_wcsicmp(lpExt, L".png") == 0){
		int			dwSize;
		char		* lpAFile = NULL;
		// First of all we need to convert input UNICODE string to ASCII string
		// Get needed size for character string
		dwSize = WideCharToMultiByte(CP_ACP, 0, lpFile, -1, lpAFile, 0, NULL, NULL);
		if(dwSize > 0){
			// allocate string buffer
			lpAFile = (char *)calloc(MAX_PATH, sizeof(char));
			if(lpAFile){
				// convert UNICODE string
				dwSize = WideCharToMultiByte(CP_ACP, 0, lpFile, -1, lpAFile, MAX_PATH, NULL, NULL);
				if(dwSize > 0){
					hBmp = LoadPNGFile(lpAFile);
				}
				free(lpAFile);
			}
		}
	}
	else if(_wcsicmp(lpExt, L".ico") == 0){
		if(g_RTHandles.hbSkin == NULL){
			HBRUSH	hbr = CreateSolidBrush(pNote->pAppearance->crWindow);
			hBmp = LoadIconFile(lpFile, hbr);
			DeleteBrush(hbr);
		}
		else{
			SIZE	sz = {0, 0};
			GetIconObjectSize(lpFile, &sz);
			if(sz.cx != 0 && sz.cy != 0){
				HBRUSH	hbr = PatternBrushFromSkin(hNote, sz);
				hBmp = LoadIconFile(lpFile, hbr);
				DeleteBrush(hbr);
			}
		}
	}
	else if(_wcsicmp(lpExt, L".cur") == 0){
		if(g_RTHandles.hbSkin == NULL){
			HBRUSH	hbr = CreateSolidBrush(pNote->pAppearance->crWindow);
			hBmp = LoadCursorFile(lpFile, hbr);
			DeleteBrush(hbr);
		}
		else{
			SIZE	sz = {0, 0};
			GetCursorObjectSize(lpFile, &sz);
			if(sz.cx != 0 && sz.cy != 0){
				HBRUSH	hbr = PatternBrushFromSkin(hNote, sz);
				hBmp = LoadCursorFile(lpFile, hbr);
				DeleteBrush(hbr);
			}
		}
	}
	else{
		if(g_RTHandles.hbSkin == NULL){
			HBRUSH	hbr = CreateSolidBrush(pNote->pAppearance->crWindow);
			hBmp = LoadPictureFile(lpFile, hbr);
			DeleteBrush(hbr);
		}
		else{
			SIZE	sz = {0, 0};
			GetPictureObjectSize(lpFile, &sz);
			if(sz.cx != 0 && sz.cy != 0){
				HBRUSH	hbr = PatternBrushFromSkin(hNote, sz);
				hBmp = LoadPictureFile(lpFile, hbr);
				DeleteBrush(hbr);
			}
		}
	}
	if(hBmp){
		InsertBitmap(pREOle, hBmp);
		DeleteBitmap(hBmp);
		//save edit box contents to temporary file and load from it
		ExchangeSameRE((HWND)GetPropW(hNote, PH_EDIT));
	}
}

static HBRUSH PatternBrushFromSkin(HWND hNote, SIZE sz){
	HBRUSH		hBrush = NULL;
	HDC			hdcTemp, hdcNote, hdcSkin;
	HBITMAP		hBmpBack, hBmpOld, hBmpSkinOld;
	PMEMNOTE	pNote = MemNoteByHwnd(hNote);
	HWND		hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	CHARRANGE	cr;
	POINT		pt;

	if(pNote == NULL)
		return NULL;
	//get current rich edit box selection index
	cr.cpMax = cr.cpMin = 0;
	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&cr);
	//get coordinates of current rich edit box selection
	pt.x = pt.y = 0;
	SendMessageW(hEdit, EM_POSFROMCHAR, (WPARAM)&pt, cr.cpMin);
	hdcNote = GetDC(hNote);
	hdcTemp = CreateCompatibleDC(hdcNote);
	hdcSkin = CreateCompatibleDC(hdcNote);
	hBmpBack = CreateCompatibleBitmap(hdcNote, sz.cx, sz.cy);
	ReleaseDC(hNote, hdcNote);
	hBmpOld = SelectBitmap(hdcTemp, hBmpBack);
	hBmpSkinOld = SelectBitmap(hdcSkin, pNote->pRTHandles->hbSkin);
	BitBlt(hdcTemp, 0, 0, sz.cx, sz.cy, hdcSkin, pNote->pRTHandles->rcEdit.left + pt.x, pNote->pRTHandles->rcEdit.top + pt.y, SRCCOPY);
	SelectBitmap(hdcSkin, hBmpSkinOld);
	SelectBitmap(hdcTemp, hBmpOld);
	hBrush = CreatePatternBrush(hBmpBack);
	DeleteDC(hdcTemp);
	DeleteDC(hdcSkin);
	return hBrush;
}

static int ChoosePictureFile(HWND hNote, wchar_t * lpFile){
	ZeroMemory(lpFile, sizeof(wchar_t) * wcslen(lpFile));
	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
	ofn.lStructSize	= sizeof(OPENFILENAMEW);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	ofn.hwndOwner = hNote;
	ofn.lpstrFilter	= OPEN_PICTURE_FILTER;
	ofn.lpstrTitle = g_Strings.InsPictureCaption;
	ofn.lpstrFile = lpFile;
	ofn.nMaxFile = MAX_PATH;
	return GetOpenFileNameW(&ofn);
}

BOOL CALLBACK Tags_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Tags_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Tags_OnInitDialog);
	default: return FALSE;
	}
}

static void Tags_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t			szBuffer[128];
	int				index, count;

	switch(id){
		case IDCANCEL:
			RemovePropW(hwnd, PN_PROP);
			g_hTags = NULL;
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:
			AddTags(hwnd);
			RemovePropW(hwnd, PN_PROP);
			g_hTags = NULL;
			EndDialog(hwnd, IDOK);
			break;
		case IDC_LST_PRE_TAGS:
			if(codeNotify == LBN_SELCHANGE){
				if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0) >= 0)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), FALSE);
			}
			else if(codeNotify == LBN_DBLCLK){
				if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0) >= 0){
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_TAGS_PRETOCURR, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR));
				}
			}
			break;
		case IDC_LST_CURR_TAGS:
			if(codeNotify == LBN_SELCHANGE){
				if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCURSEL, 0, 0) >= 0)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), FALSE);
			}
			else if(codeNotify == LBN_DBLCLK){
				if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCURSEL, 0, 0) >= 0){
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_TAGS_CURRTOPRE, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE));
				}
			}
			break;
		case IDC_CMD_TAGS_PRETOCURR:
			if(codeNotify == BN_CLICKED){
				count = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, FALSE, -1);
				for(int i = count - 1; i >= 0; i--){
					if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETSEL, i, 0) > 0){
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_DELETESTRING, i, 0);
						index = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, TRUE, index);
					}
				}
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_CURR_TAGS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_CURR_TAGS));
				// if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0) > 0)
					// SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, TRUE, 0);
				// else
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, FALSE, -1);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), FALSE);
			}
			break;
		case IDC_CMD_TAGS_CURRTOPRE:
			if(codeNotify == BN_CLICKED){
				count = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, FALSE, -1);
				for(int i = count - 1; i >= 0; i--){
					if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETSEL, i, 0) > 0){
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_DELETESTRING, i, 0);
						index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, TRUE, index);
					}
				}
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_PRE_TAGS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_PRE_TAGS));
				// if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0) > 0)
					// SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, TRUE, 0);
				// else
				SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, FALSE, -1);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), FALSE);
			}
			break;
	}
}

static BOOL Tags_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256], szKey[12];
	PMEMNOTE		pNote = (PMEMNOTE)lParam;
	LPPTAG			pTemp;

	g_hTags = hwnd;
	SetPropW(hwnd, PN_PROP, (HANDLE)pNote);
	_itow(IDM_TAGS, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Tags", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	wcscat(szBuffer, L" - ");
	wcscat(szBuffer, pNote->pData->szName);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_PRE_TAGS, g_NotePaths.CurrLanguagePath, L"Available tags");
	SetDlgCtlText(hwnd, IDC_ST_CURR_TAGS, g_NotePaths.CurrLanguagePath, L"Current tags");
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, L"OK");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	pTemp = pNote->pTags;
	while(pTemp){
		SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_ADDSTRING, 0, (LPARAM)pTemp->text);
		pTemp = pTemp->next;
	}
	if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0) > 0){
		SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETCURSEL, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), TRUE);
	}
	pTemp = g_PTagsPredefined;
	while(pTemp){
		if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_FINDSTRINGEXACT, -1, (LPARAM)pTemp->text) == LB_ERR){
			SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_ADDSTRING, 0, (LPARAM)pTemp->text);
		}
		pTemp = pTemp->next;
	}
	if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0) > 0){
		SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETCURSEL, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), TRUE);
	}
	return FALSE;
}

static void AddTags(HWND hwnd){
	PMEMNOTE		pNote = (PMEMNOTE)GetPropW(hwnd, PN_PROP);
	wchar_t			szBuffer[128];
	int				count;

	if(pNote->pTags){
		TagsFree(pNote->pTags);
		pNote->pTags = NULL;
	}
	count = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0);
	for(int i = 0; i < count; i++){
		SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
		pNote->pTags = TagsAdd(pNote->pTags, szBuffer, NULL);
	}
}

void SaveTags(PMEMNOTE pNote){
	wchar_t			*pTags;
	LPPTAG			pTemp;
	int				count = TagsCount(pNote->pTags);
	
	if(count == 0){
		WritePrivateProfileStringW(pNote->pFlags->id, IK_TAGS, NULL, g_NotePaths.DataFile);
		return;
	}
	pTags = calloc(count * 128, sizeof(wchar_t));
	if(pTags){
		pTemp = pNote->pTags;
		while(pTemp){
			wcscat(pTags, pTemp->text);
			wcscat(pTags, L",");
			pTemp = pTemp->next;
		}
		if(*pTags){
			wchar_t *p = wcsrchr(pTags, ',');
			if(p){
				*p = '\0';
			}
			WritePrivateProfileStringW(pNote->pFlags->id, IK_TAGS, pTags, g_NotePaths.DataFile);
		}
		else
		{
			WritePrivateProfileStringW(pNote->pFlags->id, IK_TAGS, NULL, g_NotePaths.DataFile);
		}
		free(pTags);
	}
}

static void LoadTags(PMEMNOTE pNote){
	wchar_t			*pTags;
	int				ub = 1024, result;
	pTags = calloc(ub, sizeof(wchar_t));
	if(pTags){
		result = GetPrivateProfileStringW(pNote->pFlags->id, IK_TAGS, NULL, pTags, ub, g_NotePaths.DataFile);
		while(result == ub - 1){
			ub *= 2;
			result = GetPrivateProfileStringW(pNote->pFlags->id, IK_TAGS, NULL, pTags, ub, g_NotePaths.DataFile);
		}
		if(*pTags){
			ReadTagsFromString(pNote, pTags);
		}
		free(pTags);
	}
}

static void ReadTagsFromString(PMEMNOTE pNote, wchar_t * pText){
	wchar_t			*w, *ptr;

	if(pNote->pTags){
		TagsFree(pNote->pTags);
	}
	pNote->pTags = NULL;
	w = wcstok(pText, L",", &ptr);
	while(w){
		pNote->pTags = TagsAdd(pNote->pTags, w, NULL);
		w = wcstok(NULL, L",", &ptr);
	}
}

static void BuildSEngsMenu(void){
	HMENU					hMenu;
	int						count, id = SEARCH_ENGINE_ADDITION, j = 0;
	MITEM					mit = {0};
	LPPSENG					pSeng;

	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;
	hMenu = GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_SEARCH_WEB));
	count = GetMenuItemCount(hMenu);
	for(int i = count - 1; i >= 0; i--){
		FreeSingleMenu(hMenu, i);
		DeleteMenu(hMenu, i, MF_BYPOSITION);
	}
	pSeng = g_PSengs;
	while(pSeng){
		mit.id = id++;
		wcscpy(mit.szText, pSeng->name);
		AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
		SetMenuItemProperties(&mit, hMenu, j++, TRUE);
		pSeng = pSeng->next;
	}
}

static void RemoveNoteFromLinks(PMEMNOTE pNote){
	PMEMNOTE				pTemp = MemoryNotes();

	while(pTemp){
		if(pTemp->pLinks && wcscmp(pTemp->pFlags->id, pNote->pFlags->id) != 0){
			if(PLinksContains(pTemp->pLinks, pNote->pFlags->id)){
				pTemp->pLinks = PLinksRemove(pTemp->pLinks, pNote->pFlags->id);
				SaveLinks(pTemp);
			}
		}
		pTemp = pTemp->next;
	}
}

static void BuildLinksMenu(PMEMNOTE pNote){
	HMENU					hMenu;
	int						count, id = LINKS_ADDITION, j = 2;
	MITEM					mit = {0};
	LPPLINK					pLink;
	PMEMNOTE				pmn;

	if(!pNote)
		return;
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;
	hMenu = GetSubMenu(g_hNotePopUp, GetMenuPosition(g_hNotePopUp, IDM_LINKS));
	count = GetMenuItemCount(hMenu);
	for(int i = count - 1; i > 0; i--){
		FreeSingleMenu(hMenu, i);
		DeleteMenu(hMenu, i, MF_BYPOSITION);
	}
	pLink = pNote->pLinks;
	if(pLink){
		AppendMenuW(hMenu, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	}
	while(pLink){
		pmn = MemNoteById(pLink->id);
		if(pmn){
			mit.id = id++;
			wcscpy(mit.szText, pmn->pData->szName);
			wcscpy(mit.szReserved, pLink->id);
			AppendMenuW(hMenu, MF_STRING, mit.id, mit.szText);
			SetMenuItemProperties(&mit, hMenu, j++, TRUE);
		}
		pLink = pLink->next;
	}
}

static void LoadLinks(PMEMNOTE pNote){
	wchar_t		*pString = NULL, *w, *ptr;
	int			ub = 1024, result = 0;

	pString = calloc(ub, sizeof(wchar_t));
	if(pString){
		result = GetPrivateProfileStringW(pNote->pFlags->id, IK_LINKS, NULL, pString, ub, g_NotePaths.DataFile);
		while(result == ub - 1){
			ub *= 2;
			free(pString);
			pString = pString = calloc(ub, sizeof(wchar_t));
			if(pString){
				result = GetPrivateProfileStringW(pNote->pFlags->id, IK_LINKS, NULL, pString, ub, g_NotePaths.DataFile);
			}
		}
		if(*pString){
			w = wcstok(pString, L"|", &ptr);
			while(w){
				pNote->pLinks = PLinksAdd(pNote->pLinks, w);
				w = wcstok(NULL, L"|", &ptr);
			}
		}
		free(pString);
	}
}

static void SaveLinks(PMEMNOTE pNote){
	LPPLINK		pLink = NULL;
	wchar_t		*pString = NULL;
	int			count = 0;

	WritePrivateProfileStringW(pNote->pFlags->id, IK_LINKS, NULL, g_NotePaths.DataFile);
	if(pNote->pLinks){
		count = PLinksCount(pNote->pLinks);
		pString = calloc(count * 28 + count + 1, sizeof(wchar_t));
		if(pString){
			pLink = pNote->pLinks;
			while(pLink){
				wcscat(pString, pLink->id);
				wcscat(pString, L"|");
				pLink = pLink->next;
			}
			WritePrivateProfileStringW(pNote->pFlags->id, IK_LINKS, pString, g_NotePaths.DataFile);
			free(pString);
		}
	}
}

static BOOL IsNoteHotkeyAvailable(PMEMNOTE pNote, HWND hEdit, int command){
	switch(command){
		case IDM_RESTORE_BACKUP:
			if(!NotesDirExists(g_NotePaths.BackupDir) || !IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP))
				return FALSE;
			break;
		case IDM_PASTE_AS_TEXT:{
			if(SendMessageW(hEdit, EM_CANPASTE, CF_TEXT, 0) 
			|| SendMessageW(hEdit, EM_CANPASTE, CF_UNICODETEXT, 0) 
			|| SendMessageW(hEdit, EM_CANPASTE, CF_OEMTEXT, 0)){
				if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
					return FALSE;
			}
			else
				return FALSE;
			break;
		}
		case IDM_COPY_AS_TEXT:{
			CHARRANGE	chr = {0};
			SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&chr);
			if(chr.cpMin == chr.cpMax)
				return FALSE;
			break;
		}
		default:
			return TRUE;
	}
	return TRUE;
}

void SaveNoteAsTextFile(PMEMNOTE pNote){
	wchar_t				szBuffer[256];
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	HWND				hwnd;

	if(!pNote->pFlags->saved){
		if(!SaveNote(pNote)){
			return;
		}
	}
	if(pNote->pData->visible)
		hwnd = pNote->hwnd;
	else
		hwnd = g_hMain;

	GetPrivateProfileStringW(L"captions", L"save_text_file", L"Save as text file", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	wcscpy(szFileName, pNote->pData->szName);
	m_CurrType = ShowCustomSaveFileDlg(hwnd, szFileName, szFileTitle, TEXT_FILE_FILTER, szBuffer, TEXT_FILE_EXTENTION);
	if(m_CurrType > -1){
		HANDLE		hFile;
		EDITSTREAM	eStream;
		int			flags = SF_TEXT;
		DWORD		written;
		ReadSimpleRTFFile(g_hTEditContent, pNote->pFlags->id);
		
		hFile = CreateFileW(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			eStream.dwCookie = (DWORD)hFile;
			eStream.pfnCallback = OutStreamCallback;
			eStream.dwError = 0;
			if(m_CurrType == CODE_UNICODE){
				WriteFile(hFile, m_BOMUnicode, 2, &written, NULL);
				flags |= SF_UNICODE;
			}
			else if(m_CurrType == CODE_UNICODE_BE){
				WriteFile(hFile, m_BOMBE, 2, &written, NULL);
				flags |= SF_UNICODE;
			}
			else if(m_CurrType == CODE_UTF8){
				WriteFile(hFile, m_BOMUTF8, 3, &written, NULL);
				flags = (CP_UTF8 << 16) | SF_USECODEPAGE | SF_TEXT;
			}
			RichEdit_StreamOut(g_hTEditContent, flags, &eStream);
			CloseHandle(hFile);
			GetPrivateProfileStringW(S_MESSAGES, L"file_save_success", L"File saved successfully", szBuffer, 255, g_NotePaths.CurrLanguagePath);
			MessageBoxW(hwnd, szBuffer, PROG_NAME, MB_OK);
		}
	}
}

BOOL IsHotkeyAvailable(PMEMNOTE pNote, int command){
	switch(command){
		case IDM_RESTORE_BACKUP:
			if(!NotesDirExists(g_NotePaths.BackupDir) || !IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP))
				return FALSE;
			break;
		case IDM_PASTE_AS_TEXT:{
			HWND		hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
			if(SendMessageW(hEdit, EM_CANPASTE, CF_TEXT, 0) 
			|| SendMessageW(hEdit, EM_CANPASTE, CF_UNICODETEXT, 0) 
			|| SendMessageW(hEdit, EM_CANPASTE, CF_OEMTEXT, 0)){
				if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
					return FALSE;
			}
			else
				return FALSE;
			break;
		}
		case IDM_COPY_AS_TEXT:{
			HWND		hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
			CHARRANGE	chr = {0};
			SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&chr);
			if(chr.cpMin == chr.cpMax)
				return FALSE;
			break;
		}
		default:
			return TRUE;
	}
	return TRUE;
}

static BOOL DelHideInvisible(void){
	return (IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_DELETE) & IsBitOn(g_NextSettings.flags1, SB3_HIDE_HIDE));
}

void RemovePin(PMEMNOTE pNote){
	free(pNote->pPin);
	pNote->pPin = NULL;
	WritePrivateProfileStructW(pNote->pFlags->id, IK_PIN, NULL, 0, g_NotePaths.DataFile);
	if(pNote->pRTHandles->pinTimer != 0)
		KillTimer(NULL, pNote->pRTHandles->pinTimer);
	if(g_hCPDialog){
		SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	}
	if(pNote->pData->visible)
		ShowNoteMarks(pNote->hwnd);
}

void AddPin(HWND hParent, PMEMNOTE pNote){
	if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_WINDOWS), hParent, WinList_DlgProc, (LPARAM)pNote) == IDOK){
		WritePrivateProfileStructW(pNote->pFlags->id, IK_PIN, pNote->pPin, sizeof(PPIN), g_NotePaths.DataFile);
		pNote->pRTHandles->pinTimer = SetTimer(NULL, 0, PIN_TIMER_INTERVAL, PinTimerProc);
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
		if(pNote->pData->visible)
			ShowNoteMarks(pNote->hwnd);
	}
}

static void CALLBACK PinTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	PMEMNOTE			pNote = NULL;
	HWND				hPin = NULL;
	WINDOWPLACEMENT		wp = {0};

	wp.length = sizeof(WINDOWPLACEMENT);
	pNote = MemNoteByPinTimer(idEvent);

	if(!pNote)
		return;

	if(pNote->pPin){
		hPin = FindWindowW(pNote->pPin->class, pNote->pPin->text);
		if(hPin){
			GetWindowPlacement(hPin, &wp);
			if(wp.showCmd != SW_SHOWMINIMIZED){
				if(GetForegroundWindow() == hPin){
					if(!pNote->pData->visible){
						ShowNote(pNote);
					}
					if(!pNote->pRTHandles->pinActive){
						if(!pNote->pData->onTop){
							SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
							SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						}
						pNote->pRTHandles->pinActive = true;
					}
				}
				else{
					pNote->pRTHandles->pinActive = false;
				}
			}
			else{
				if(pNote->pData->visible){
					if(!IsBitOn(pNote->pSchedule->params, SP_IN_ALARM)){
						HideNote(pNote, true);
						pNote->pRTHandles->pinActive = false;
					}
				}
			}
		}
		else{
			if(pNote->pData->visible){
				if(!IsBitOn(pNote->pSchedule->params, SP_IN_ALARM)){
					HideNote(pNote, true);
					pNote->pRTHandles->pinActive = false;
				}
			}
		}
	}
}

static BOOL CALLBACK WinList_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR			lpnmhdr;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, WinList_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, WinList_OnInitDialog);
		case WM_CLOSE:
			g_hPinToWindow = NULL;
			EndDialog(hwnd, IDCANCEL);
			return FALSE;
		case WM_NOTIFY:
			lpnmhdr = (LPNMHDR)lParam;
			switch(lpnmhdr->code){
				case LVN_ITEMCHANGED:{
					LPNMLISTVIEW		lpnm = (LPNMLISTVIEW)lParam;
					if(lpnm->uNewState == 3 || lpnm->uNewState == 2){
						EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
					}
					else{
						EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
					}
					break;
				}
				case NM_DBLCLK:
					if(lpnmhdr->idFrom == IDC_LST_WINDOWS)
						SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
					break;
			}
			return FALSE;
		default: return FALSE;
	}
}

static BOOL WinList_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t				szBuffer[256];
	LVCOLUMNW			lvc = {0};
	PMEMNOTE			pNote;
	
	pNote = (PMEMNOTE)lParam;
	g_hPinToWindow = hwnd;
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)lParam);
	GetPrivateProfileStringW(S_CAPTIONS, IK_WINDOWS, L"Pin note to window", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	wcscat(szBuffer, L" [");
	wcscat(szBuffer, pNote->pData->szName);
	wcscat(szBuffer, L"]");
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_AV_WINDOWS, g_NotePaths.CurrLanguagePath, L"Available windows");
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 200;
	lvc.pszText = szBuffer;
	GetPrivateProfileStringW(S_WINDOWS_COLUMNS, L"0", L"Window text", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_LST_WINDOWS, LVM_INSERTCOLUMNW, 0, (LPARAM)&lvc);
	lvc.iSubItem = 1;
	GetPrivateProfileStringW(S_WINDOWS_COLUMNS, L"1", L"Window class", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_LST_WINDOWS, LVM_INSERTCOLUMNW, 1, (LPARAM)&lvc);
	SendDlgItemMessageW(hwnd, IDC_LST_WINDOWS, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	EnumWindows(EnumWindowsProc, (LPARAM)hwnd);
	return TRUE;
}

static void WinList_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			g_hPinToWindow = NULL;
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			PMEMNOTE		pNote;
			wchar_t			szBuffer[MAX_PATH];
			LVITEMW			lvi = {0};
			int				index = SendDlgItemMessageW(hwnd, IDC_LST_WINDOWS, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED);

			pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			if(pNote){
				pNote->pPin = calloc(1, sizeof(PPIN));
				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.iItem = index;
				lvi.pszText = szBuffer;
				lvi.cchTextMax = MAX_PATH;
				SendDlgItemMessageW(hwnd, IDC_LST_WINDOWS, LVM_GETITEMW, 0, (LPARAM)&lvi);
				wcscpy(pNote->pPin->text, szBuffer);
				// wcscpy(pnhook.text, szBuffer);
				// pnhook.hwnd = (HWND)lvi.lParam;
				lvi.mask = LVIF_TEXT;
				lvi.iSubItem = 1;
				SendDlgItemMessageW(hwnd, IDC_LST_WINDOWS, LVM_GETITEMTEXTW, index, (LPARAM)&lvi);
				wcscpy(pNote->pPin->class, szBuffer);
				// wcscpy(pnhook.class, szBuffer);
				g_hPinToWindow = NULL;
				EndDialog(hwnd, IDOK);
			}
			break;
		}
	}
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
	wchar_t				szBuffer[MAX_PATH], szClass[MAX_PATH];
	LVITEMW				lvi = {0};

	if(IsWindowVisible(hwnd)){
		*szBuffer = '\0';
		*szClass = '\0';
		GetWindowTextW(hwnd, szBuffer, MAX_PATH);
		GetClassNameW(hwnd, szClass, MAX_PATH);
		if(wcslen(szBuffer) > 0 && wcslen(szClass) > 0){
			lvi.mask = LVIF_TEXT | LVIF_PARAM;
			lvi.iItem = SendDlgItemMessageW((HWND)lParam, IDC_LST_WINDOWS, LVM_GETITEMCOUNT, 0, 0);
			lvi.pszText = szBuffer;
			lvi.lParam = (LPARAM)hwnd;
			SendDlgItemMessageW((HWND)lParam, IDC_LST_WINDOWS, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = szClass;
			SendDlgItemMessageW((HWND)lParam, IDC_LST_WINDOWS, LVM_SETITEMW, 0, (LPARAM)&lvi);
		}
	}
	return TRUE;
}

static COLORREF MakeRandomColor(void){
	BYTE	r, g, b;

	rseed((unsigned)time(NULL));
	r = rnd(255);
	g = rnd(255);
	b = rnd(255);
	return RGB(r, g, b);
}

static int rnd(int base){
	int		result = 0;
	__asm{
		mov eax, rnd_seed
		test eax, 80000000h
    	jz  _next
    	add eax, 7fffffffh
		_next:
		xor edx, edx
	    mov ecx, 127773
	    div ecx
	    mov ecx, eax
	    mov eax, 16807
	    mul edx
	    mov edx, ecx
	    mov ecx, eax
	    mov eax, 2836
	    mul edx
	    sub ecx, eax
	    xor edx, edx
	    mov eax, ecx
	    mov rnd_seed, ecx
	    div base

	    mov eax, edx
		mov result, eax
	}
	return result;
}

static void rseed(int seed){
	rnd_seed = 12345678;

	rnd_seed = seed;
}
