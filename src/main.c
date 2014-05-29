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

#ifndef	COBJMACROS
#define COBJMACROS
#endif

#include <stdio.h>

#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "note.h"
#include "about.h"
#include "transparentbitmap.h"
#include "shared.h"
#include "memorynotes.h"
#include "controlpanel.h"
#include "search.h"
#include "docking.h"
#include "toolbar.h"
#include "darrow.h"
#include "hotkeys.h"
#include "overdue.h"
#include "groups.h"
#include "registryclean.h"
#include "shortcut.h"
#include "linklabel.h"
#include "update.h"
#include "download.h"
#include "skinlessdrawing.h"
#include "print.h"
#include "login.h"
#include "gtoolbar.h"
#include "encryption.h"
#include "gcolors.h"
#include "gcpicker.h"
#include "ctreeview.h"
#include "enums.h"
#include "diary.h"
#include "searchtags.h"
#include "sengs.h"
#include "backuprestore.h"
#include "sync.h"
#include "fdialogs.h"
#include "progress.h"
#include "contacts.h"
#include "hosts.h"
#include "sockerrors.h"
#include "sockets.h"
#include "searchdates.h"
#include "contgroups.h"
#include "localsync.h"
#include "htable.h"

#include <wininet.h>

/** Macros *********************************************************/
#define HANDLE_WM_HOTKEY(hwnd, wParam, lParam, fn)  ((fn)((hwnd), (int)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)

/** Constants *********************************************************/
#define	MAIN_HOTKEYS_IND	100
#define	NOTE_HOTKEYS_IND	200
#define	EDIT_HOTKEYS_IND	300
#define	NEXT_HOTKEYS_IND	400

#define	TIMER_AUTOSAVE_ID	333
#define	TIMER_PROTECT_ID	777
#define	TIMER_PROGRESS_ID	999
#define	TIMER_CLEAN_BIN_ID	2222
#define	TIMER_DBL_CLICK		1111

#define	DBL_CLICK_COUNT		11
#define	CLEAN_BIN_INTERVAL	60000

#define	SENG_PROP			L"SENG_PROP"
#define	EXTERNAL_PROP		L"EXT_PROP"
#define	CONTACTS_GROUP_PROP	L"CONTACTS_GROUP_PROP"

/** register logon/logoff messages procedures *********************************************************/
typedef BOOL (*REGSESSIONMESSAGES)(HWND, DWORD);
typedef BOOL (*UNREGSESSIONMESSAGES)(HWND);

/** Toolbar buttons ********************************************************/
#if defined(_WIN64)
static TBBUTTON				m_ScheduleButtons[] = {
							{0,IDC_CMD_ADD_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{1,IDC_CMD_DEL_SOUND,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{3,IDC_CMD_LISTEN,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0}
							};
static TBBUTTON				m_HotKeyButtons[] = {
							{0,IDC_CMD_ADD_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{2,IDC_CMD_MODIFY,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{1,IDC_CMD_DEL_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0}
							};
#else
static TBBUTTON				m_ScheduleButtons[] = {
							{0,IDC_CMD_ADD_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{1,IDC_CMD_DEL_SOUND,0,TBSTYLE_BUTTON,0,0,0,0},
							{3,IDC_CMD_LISTEN,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}
							};
static TBBUTTON				m_HotKeyButtons[] = {
							{0,IDC_CMD_ADD_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{2,IDC_CMD_MODIFY,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{1,IDC_CMD_DEL_SOUND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}
							};
#endif

/** Enums *************************************************************/
typedef enum _enDiaryCustEnableStatus {
	ENDC_ENABLED_WITH_SKIN, 
	ENDC_ENABLED_WITHOUT_SKIN, 
	ENDC_DISABLED}enDiaryCustEnableStatus;

typedef enum _bhChecks {
	bhSaveRoot, 
	bhSaveAlways, 
	bhAskBefore, 
	bhConfirmDel, 
	bhHideWithout, 
	bhPositionRoot, 
	bhAlwaysOnTop, 
	bhRelational, 
	bhNoWindowList, 
	bhExcAeroPeek, 
	bhCleanBin,
	bhHideCompleted,
	bhBigIcons};
typedef enum _skChecks {
	skSkinlessRoot, 
	skRandomColor, 
	skInvertColor, 
	skRollUnroll, 
	skFittToCaption};
typedef enum _msChecks {
	msStartupRoot, 
	msStartWithWindows, 
	msShowCPOnStart, 
	msCheckNewVOnStart, 
	msEffectsRoot, 
	msHideFluently, 
	msPlaySoundOnHide};
typedef enum _apChecks {
	apAppRoot, 
	apTransparency, 
	apHideToolbar, 
	apCustomFonts, 
	apShowScroll, 
	apHideDelete, 
	apCrossInstTrg,
	apHideHide};
typedef enum _scChecks {
	scScheduleRoot, 
	scAllowSound, 
	scVisualAlert, 
	scTrackOverdue, 
	scDontMove};
typedef enum _prChecks {
	prEncryptRoot, 
	prEncryption,  
	prLocked, 
	prHideFromTray, 
	prBackup, 
	prBackupNotes, 
	prSilentBackup,
	prProtectMisc,
	prDontShowContent,
	prSyncRoot,
	prSyncLocalBin};
typedef enum _drChecks {
	drDiaryRoot, 
	drDiaryCust, 
	drAddWeekday, 
	drFullWeekday, 
	drWeekdayAtEnd, 
	drNoPages, 
	drAscSort};
typedef enum _dcChecks {
	dcSkinsRoot, 
	dcCustomSkins, 
	dcSkinlessRoot, 
	dcCustSize, 
	dcCustColor, 
	dcCustFont, 
	dcMiscRoot, 
	dcInverseOrder};
typedef enum _ntChecks {
	ntSyncRoot, 
	ntIncDelSync, 
	ntSyncStart, 
	ntSaveBeforeSync,
	ntExchRoot,
	ntSaveBeforeSending,
	ntNotShowNotifyNewMessage,
	ntNotPlaySoundNewMessage,
	ntShowNoteClickNotify,
	ntShowCPClickNotify,
	ntNotShowSendMessage,
	ntHideAfterSend,
	ntNotShowContactsInMenu,
	ntShowAfterReceiving,
	ntEnableNetwork};

/** Checks arrays *************************************************************/
static HTREEITEM		m_ChecksTBehavior[13] = {0};
static wchar_t			*m_DefTBehavior[13] = {
	L"Saving/Deletion", 
	L"Always save unsaved notes on exit", 
	L"Ask before saving", 
	L"Confirmation before note deletion", 
	L"Save notes while hiding without confirmation", 
	L"Miscellaneous", 
	L"New note always on top", 
	L"Relational notes positioning", 
	L"Do not show notes in windows list (Alt+Tab) ( * )", 
	L"Keep notes visible while hovering over \"Show desktop\" button ( *** )", 
	L"Warn when note is deleted automatically from Recycle Bin",
	L"Hide note marked as \"Completed\"",
	L"Show big icons on PNotes Control Panel's toolbar"};
static HTREEITEM		m_ChecksTSkins[5] = {0};
static wchar_t			*m_DefTSkins[5] = {
	L"Common skinless settings", 
	L"Use random background color", 
	L"Invert text color", 
	L"Roll/unroll note by double click on caption", 
	L"Fit to caption when rolled"};
static HTREEITEM		m_ChecksTMisc[7] = {0};
static wchar_t			*m_DefTMisc[7] = {
	L"Startup options", 
	L"Start program with Windows", 
	L"Show Control Panel on program start", 
	L"Check for new version on program start", 
	L"Effects", 
	L"Hide notes fluently", 
	L"Play sound when hide note"};
static HTREEITEM		m_ChecksTApp[8] = {0};
static wchar_t			*m_DefTApp[8] = {
	L"Appearance", 
	L"Allow transparency", 
	L"Hide note's toolbar", 
	L"Use custom fonts", 
	L"Show scrollbar ( * )", 
	L"Hide 'Delete' button ( * )", 
	L"Change appearance of 'Hide' button to 'Delete' button ( * )",
	L"Hide 'Hide' button ( * )"};
static HTREEITEM		m_ChecksTSchedule[5] = {0};
static wchar_t			*m_DefTSchedule[5] = {
	L"Conduct", 
	L"Allow sound alert", 
	L"Visual alert", 
	L"Track overdue reminders", 
	L"Do not move notes to the center of screen"};
static HTREEITEM		m_ChecksTProtection[11] = {0};
static wchar_t			*m_DefTProtection[11] = {
	L"Encryption", 
	L"Store notes as encrypted files", 
	L"When program is locked", 
	L"Hide icon from system tray", 
	L"Backup", 
	L"Backup notes before saving", 
	L"Silent full backup",
	L"Miscellaneous", 
	L"Do not show note's content at Control Panel if note or its group is password protected",
	L"Local synchronization",
	L"Include notes from Recycle Bin in local synchronization"};
static HTREEITEM		m_ChecksTDiary[7] = {0};
static wchar_t			*m_DefTDiary[7] = {
	L"\"Diary\" group settings", 
	L"Custom \"Diary\" goup settings", 
	L"Add weekday name to note's name", 
	L"Full weekday name", 
	L"Add weekday name at the end of note's name", 
	L"Do not show diary \"pages\" from previous dates in \"Diary\" menu", 
	L"Ascending sort of \"pages\" in \"Diary\" menu (starting from most early)"};
static HTREEITEM		m_ChecksTDock[8] = {0};
static wchar_t			*m_DefTDock[8] = {
	L"Skins", 
	L"Custom skin", 
	L"Skinless notes", 
	L"Custom size", 
	L"Custom back color", 
	L"Custom caption font", 
	L"Miscellaneous", 
	L"Inverse docking order"};
static HTREEITEM		m_ChecksTNetwork[15] = {0};
static wchar_t			*m_DefTNetwork[15] = {
	L"Synchronization", 
	L"Include notes from Recycle Bin in synchronization", 
	L"Synchronize notes on program start", 
	L"Save notes before syncronization",
	L"Notes exchange",
	L"Save note before sending",
	L"Do not show notification when new note arrives",
	L"Do not play sound when new note arrives",
	L"Show received note after click on notification message",
	L"Show \"Incoming\" group after click on notification message",
	L"Do not show notification when note is sending",
	L"Hide note after sending",
	L"Do not show contacts in context menu",
	L"Show note after receiving",
	L"Enable exchange"};

static wchar_t			*m_DiaryFormats[] = {
	L"MMMM dd, yyyy", 
	L"MMMM dd yyyy", 
	L"MMMM dd yy", 
	L"MMM dd yyyy", 
	L"MMM dd yy", 
	L"MM dd yyyy", 
	L"MM dd yy", 
	L"MMM-dd-yyyy", 
	L"MMM-dd-yy", 
	L"MM-dd-yyyy", 
	L"MM-dd-yy", 
	L"MM/dd/yy", 
	L"MM/dd/yyyy", 
	L"MM.dd.yyyy", 
	L"MM.dd.yy", 
	L"yyyy-MM-dd", 
	L"yy-MM-dd", 
	L"yyyy.MM.dd", 
	L"yy.MM.dd", 
	L"yyyy/MM/dd", 
	L"yy/MM/dd", 
	L"dd/MM/yyyy", 
	L"dd/MM/yy", 
	L"dd.MM.yyyy", 
	L"dd.MM.yy", 
	L"dd-MM-yyyy", 
	L"dd-MM-yy", 
	L"dd MMMM yyyy", 
	L"dd MMM yyyy", 
	L"dd MMMM yy", 
	L"dd MMM yy"};

static wchar_t			*m_EngCols[] = {
	L"Name", 
	L"Query line"};
static wchar_t			*m_ExtProgsColumns[] = {
	L"Name", 
	L"Command line", 
	L"Command line parameters"};
static wchar_t			*m_DefNaming[] = {
	L"First characters of note", 
	L"Current date/time", 
	L"Current date/time and first characters of note"};
static wchar_t			*m_ContactCols[] = {
	L"Name",
	L"Computer name",
	L"IP address"};
static wchar_t			*m_LocalSyncCols[] = {
	L"Computer name",
	L"Notes files location",
	L"Index file location"};
/** Structures ********************************************************/
typedef struct _LSTCOMP {
	HWND		hList;
	int			iSubItem;
	int			iSortOrder;
}LSTCOMP, *PLSTCOMP;

typedef struct _TBRTOOLTIP {
	int			id;
	wchar_t		szTip[128];
}TBRTOOLTIP, *PTBRTOOLTIP;

typedef struct _DLG_WNDS {
	HWND		hAppearance;
	HWND		hSkins;
	HWND		hSchedule;
	HWND 		hDocks;
	HWND		hBehavior;
	HWND		hMisc;
	HWND		hProtection;
	HWND		hDiary;
	HWND		hNetwork;
}DLG_WNDS;

typedef struct _NAV_BUTTON {
	int			id;
	HWND		hwnd;
}NAV_BUTTON, *PNAV_BUTTON;

typedef struct _CUST_FONT	*P_CUST_FONT;
typedef struct _CUST_FONT {
	wchar_t		*lpName;
	P_CUST_FONT	next;
}CUST_FONT;

/** Prototypes ********************************************************/
static LRESULT CALLBACK NotesOwnerWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void Main_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Main_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Main_OnClose(HWND hwnd);
static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void Main_OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk);
static void Main_OnNCDestroy(HWND hwnd);
static void Main_OnEndSession(HWND hwnd, BOOL fEnding);
static void Main_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void Main_OnDestroy(HWND hwnd);
static void CreateDataFile(void);
static BOOL CreateAppearance(HWND hwnd);
static void GetAllNotes(BOOL checkPassword);
static void CreateDefaultFont(PLOGFONTW plfFont, BOOL fBold);
static void CreateMenuFont(void);
static LRESULT CALLBACK OptionsDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void Options_OnClose(HWND hwnd);
static void Options_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Options_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static BOOL CALLBACK Externals_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Externals_OnClose(HWND hwnd);
static BOOL Externals_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static BOOL CALLBACK SEngs_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Externals_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void SEngs_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void SEngs_OnClose(HWND hwnd);
static BOOL SEngs_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static BOOL CALLBACK Hot_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Hot_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Hot_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Hot_OnClose(HWND hwnd);
static BOOL CALLBACK EditHotProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL ApplyChanges(HWND hwnd);
static void ShowAllNotes(void);
static void AddNotifyIcon(void);
static void ApplyMainDlgLanguage(HWND hwnd);
static void CleanUp(BOOL freeMemNotes);
static void CreatePopUp(void);
static void PrepareDateFormatsMessages(void);
static BOOL FileExistsByFullPath(wchar_t * lpFile);
static LRESULT CALLBACK Font_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void RestoreDefaultSettings(void);
static void EnableSkinlessProperties(BOOL fEnabled);
static HRESULT CreatePNotesShortcut(void);
static void LoadMenuLanguages(HMENU	hMenu);
static void CreateShowHideGroupsMenus(HMENU hMenu);
static void LangPathByID(int id, wchar_t * lpPath);
static void GetFileNameFromPath(wchar_t * lpPath, wchar_t * lpName);
// static void AddOptionsTabs(HWND hwnd);
// static void SetMainDlgSizes(HWND hwnd);
static void SetShowDateTime(HWND hwnd);
static void PrepareMessages(void);
static void RegisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count);
static void UnregisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count);
static void PrepareHotKeys(void);
static void PrepareNoteHotKeys(void);
static void PrepareEditHotKeys(void);
static void SaveHotKeys(const wchar_t * lpSection, P_HK_TYPE lpKeys, int count);
static void DrawListItem(const DRAWITEMSTRUCT * lpd, int index);
static int GetHKId(P_HK_TYPE lpKeys, int count, int identifier);
static BOOL CheckHotKeysChanges(P_HK_TYPE lpKeys, P_HK_TYPE lpTempKeys, int count, BOOL reg);
static BOOL CheckGroupsHotkeysChanges(LPPNGROUP pg1, LPPNGROUP pg2);
static void LoadSounds(HWND hwnd);
static BOOL CopySoundFile(wchar_t * lpSrcFull, wchar_t * lpSrcFile);
static BOOL DeleteSoundFile(wchar_t * lpFile);
static int ParsePNCommandLine(wchar_t * pINIPath, wchar_t * pDataPath, wchar_t * pProgPath, wchar_t * pDBPath, wchar_t * pSkinsPath, wchar_t * pBackPath, wchar_t * pLangPath, wchar_t * pSoundPath, wchar_t * pFontsPath, wchar_t * pDictPath);
static BOOL CALLBACK EnumPNotes(HWND hwnd, LPARAM lParam);
static BOOL CALLBACK EnumHelpWindows(HWND hwnd, LPARAM lParam);
static void ReloadNotes(BOOL bSaveBefore);
static void ClearOnOptionsClose(void);
static BOOL IsLastBackslash(wchar_t * src);
static void CALLBACK AutosaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void CALLBACK CleanBinTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void CALLBACK DblClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void LoadAutosaved(void);
static void DeleteAutosaved(void);
// static void GetPrevious(wchar_t * lpFile);
static BOOL CALLBACK Skins_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Skins_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Skins_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Skins_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void InitDlgSkins(HWND hwnd);
static BOOL CALLBACK Styles_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Styles_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Styles_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Styles_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Styles_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static BOOL CALLBACK Appearance_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Appearance_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Appearance_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static void Appearance_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static HBRUSH Appearance_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static void InitDlgAppearance(HWND hwnd);
static BOOL CALLBACK Schedule_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void Schedule_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Schedule_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Schedule_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static void InitDlgSchedule(HWND hwnd);
static BOOL CALLBACK Misc_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Misc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Appearance_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static int CALLBACK ExternalNameCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static void InitDlgMisc(HWND hwnd);
static BOOL CALLBACK Network_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void InitDlgNetwork(HWND hwnd);
static void Network_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL CALLBACK Protection_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void InitDlgProtection(HWND hwnd);
static void Protection_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL CALLBACK Diary_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Diary_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Diary_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static BOOL Diary_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void InitDlgDiary(HWND hwnd);
static BOOL CALLBACK Docks_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Docks_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static BOOL Docks_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Docks_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Docks_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static void InitDlgDocks(HWND hwnd);
// static void FillComboSkins(HWND hwnd, int id, const wchar_t * lpSkinCurrent);
// static void SetDockDistance(HWND hwnd, int val);
static BOOL CALLBACK Behavior_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void InitDlgBehavior(HWND hwnd);
static void Behavior_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static HWND CreateNewNote(BOOL fCreateDiary);
static void MakeDefaultGroup(void);
static void RefreshDefaultGroupName(void);
// static void PrepareLeftPaneView(void);
// static void PrepareLeftImageList(void);
static void ShowHideOptionsDialogs(HWND hwnd);
static void MoveOptionsDialogs(int x, int y);
static void SetTransValue(HWND hwnd, int value, BYTE * transValue);
static void GetVersionNumber(void);
static void StartUpdateProcess(void);
static void ShowNewVersionBaloon(wchar_t * szNewVersion);
static BOOL IsNewDLLVersion(char * szLib);
// static void MoveButtonOnOptions(HWND hwnd, int id, int offset);
static win_version WinVer(void);
static win_version ShellVersion(void);
static void ChangeDockingDefSkin(void);
static void ChangeDiarySkinProperties(void);
static void ChangeDockingDefSize(int id, int val);
static void InitDockHeaders(void);
static void FreeDockHeaders(void);
static void RearrangeDockWindowsOnstart(dock_type dockType);
static BOOL CALLBACK DockAllProc(HWND hwnd, LPARAM lParam);
static void GetDockSkinProperties(HWND hwnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkin, BOOL fDeletePrev);
static LRESULT CALLBACK HiddenProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void UnregisterLogonMessage(HWND hwnd);
static void RegisterLogonMessage(HWND hwnd);
static void BuildFavoritesMenu(void);
static void FreeFavoritesMenu(void);
static void ShowFavorite(int id);
static void ShowDiaryItem(int id);
static void ExecuteExternal(int id);
static void RegisterCustomFonts(void);
static void ClearCustomFonts(void);
static BOOL IsAnyAutosavedExists(void);
static void FindFavorite(wchar_t * id);
static void LoadWeekdays(wchar_t * lpLang);
static void SetWeekday(int day, int index, const wchar_t * lpFile);
static void FreeWeekdays(void);
static HWND NoteFromClipboard(void);
static void AddDblCommands(wchar_t * lpFile);
static bool_settings_2 GetIndexOfDblClick(void);
static void SetDefaultItemForDblClick(void);
static BOOL CheckDblClickChanges(void);
static BOOL IsNeededEditID(int id);
static void CheckOverdueReminders(void);
static void PrepareRepParts(wchar_t * lpFile);
static void FreeRepParts(void);
static int DaysBetweenAlerts(const SYSTEMTIME stLast, const SYSTEMTIME stNow, const SYSTEMTIME stDate);
static void FillNavigationBar(wchar_t * lpFile);
static BOOL CALLBACK HideShowWhenLockedProc(HWND hwnd, LPARAM lParam);
static PNAV_BUTTON GetNavButton(int id);
static BOOL SmallValuesChanged(void);
static void PrepareBulletsMenu(void);
static void FreeAllSounds(void);
static void LoadAllSounds(void);
static void AddSound(wchar_t * lpSound);
static void RemoveSound(wchar_t * lpSound);
static void ShowAllFavorites(void);
static void ToggleAllRolled(BOOL flag);
static void ToggleAllOnTop(BOOL flag);
static void ToggleAllPriorityProtectComplete(int member, BOOL flag);
static void WarningScrollbarsTransparency(void);
static void ShowFirstBaloon(void);
static void ShowReceiveNotification(wchar_t * lpInfo);
static void ShowNotification(wchar_t * lpInfo, wchar_t * lpCaption, int timeout);
static void AddChecksBehavior(void);
static void AddChecksSkins(void);
static void AddChecksMisc(void);
static void AddChecksDock(void);
static void AddChecksDiary(void);
static void AddChecksAppearance(void);
static void AddChecksSchedule(void);
static void AddChecksProtection(void);
static void AddChecksNetwork(void);
static void ApplyChecksLanguage(wchar_t *lpFile);
static void PrepareSmallToolbars(void);
static void GetTTText(int id, wchar_t * lpText);
static void EnableSmallToolbar(HWND hwnd, int id, BOOL fHideAdd);
static void EnableSmallToolbarFull(HWND hDlg, int idTbr, BOOL fAdd, BOOL fModify, BOOL fDelete);
static void ShowDiaryDateSample(HWND hwnd);
static void GetDiarySkin(NOTE_APPEARANCE diaryApp, P_NOTE_RTHANDLES diaryRH);
static void CreateDiaryAppearance(void);
static void EnableDiaryCustPart(enDiaryCustEnableStatus en);
static void GetPredefinedTags(void);
static void SavePredefinedTags(void);
static void CheckAllDeletedTags(void);
static void CheckAllModifiedTags(void);
static void SaveExternals(void);
static void SaveSearchEngines(void);
static void GetSearchEngines(void);
static void GetLocalSyncs(void);
static void SaveLocalSyncs(void);
static void SaveVoices(void);
static void RestoreDefaultVoices(void);
static void GetContacts(void);
static void GetContGroups(void);
static void GetExternals(void);
static void UpdateDefContGroupsName(void);
static BOOL CALLBACK HelpMissing_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void HelpMissing_OnClose(HWND hwnd);
static void HelpMissing_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL HelpMissing_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void DownloadHelpFile(HWND hwnd, wchar_t * lpURL, wchar_t * lpFile);
static void UpdateDownloadProgress(HWND hwnd, int total, int ready);
static void ClearHelpMissing(HWND hwnd);
static void CALLBACK ProgressTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void FullBackup(HWND hwnd, BOOL fAutomatic, BOOL fMessage);
static void BackupOnCommandLine(wchar_t * lpDirectory);
static void ConstructAutomaticBackupName(wchar_t * lpDirectory, wchar_t * lpFileName);
static void PrepareScheduleImagelists(HMODULE hIcons);
static void ShowHideByTag(wchar_t * lpTag, BOOL fShow);
static void SynchronizeNow(HWND hwnd, LPARAM lParam);
static void CheckAndCreatePath(wchar_t * path);
static void EnableMainMenus(void);
static PROGRESS_STRUCT PrepareSyncStruct(HWND hwnd, LPARAM lParam);
static void InsertContactIntoList(HWND hList, PCONTPROP cp);
static void InsertLocalSyncIntoList(HWND hList, LPPLOCALSYNC ls);
static void UpdateLocalSyncList(HWND hList, LPPLOCALSYNC lpls, LVITEMW lvi);
static void InsertContGroupToTree(HWND hTree, LPPCONTGROUP pcg);
static void UpdateContactInList(HWND hList, PCONTPROP cp, LVITEMW lvi);
static int CALLBACK ListStringCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static void StartWSServer(HWND hwnd);
static void ReceiveNewNote(void);
static BOOL IsGroupChecked(int id);
static void CheckGroup(int id);
// static void Hotdlg_OnClose(HWND hwnd);
// static void Hotdlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
// static BOOL Hotdlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
// static BOOL CALLBACK Hotdlg_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void InsertCommands(HWND hTree, HMENU hMenu, HTREEITEM htiParent, int tab);
static void InsertHotGroups(HWND hTree, HTREEITEM htiParent, int idParent);
static HTREEITEM InsertSingleHotGroup(HWND hTree, HTREEITEM hItem, LPPNGROUP pTemp);
static void CleanHotdlgOnClose(void);
static void DisplaySelectedHotkey(TVITEMW tvi, HWND hwnd, int tab);
static BOOL IsSuitableHKSelected(HWND hwnd, int tvwId);
static void ApplyHotKeys(void);
static void GetGroupChecks(void);
static void FreeGroupChecks(void);
static void GetAllGroups(void);
// static BOOL CALLBACK ContGroups_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// static void ContGroups_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
// static void ContGroups_OnClose(HWND hwnd);
// static BOOL ContGroups_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void ContGroups_EnableOK(HWND hwnd);
static HTREEITEM FindTVGroup(HWND hTree, HTREEITEM hStart, int id);
static HTREEITEM FindTVContact(HWND hTree, HTREEITEM hGStart, HTREEITEM hCStart, wchar_t * lpName);
static void AddContactToGroups(HWND hTree, LPPCONTACT lpc);
static void RemoveContactFromGroup(HWND hTree, wchar_t * lpName);
static void ReplaceContactInFroups(HWND hTree, LPPCONTACT lpc);
static void TransferContacts(HWND hTree, int groupOld, int groupNew);
static BOOL IsFTPEnabled(void);

static void LoadLanguageNames(void);
static void LoadDictionaries(void);
static int GetCodePage(char * key);
static void LoadCodePages(void);
static void CheckAndCreateDictionariesFile(void);
static void CheckAndCreateDictionariesDirectory(void);
static void AddDictFiles(wchar_t * szSpellDir, wchar_t * szAff);
static void BuildSpellMenu(void);
static BOOL SpellerExists(void);
// static void ReplaceCharacters(wchar_t * dst, wchar_t * src);
// static void CleanPrefetch(const wchar_t * lpcProgName);

/** Module variables ********************************************************/
static MITEM				m_MainMenus[] = {
	{IDM_NEW, 0, 0, -1, -1, MT_REGULARITEM, false, L"New Note", L""}, 
	{IDM_LOAD_NOTE, 1, 0, -1, -1, MT_REGULARITEM, false, L"Load Note", L""}, 
	{IDM_NOTE_FROM_CLIPBOARD, 2, 0, -1, -1, MT_REGULARITEM, false, L"New Note From Clipboard", L""}, 
	{IDM_DIARY, 3, 0, -1, -1, MT_REGULARITEM, false, L"Diary", L""},
	{IDM_TODAY_DIARY, -1, 0, -1, 0, MT_REGULARITEM, false, L"Today", L""},
	{IDM_OPTIONS, 4, 0, -1, -1, MT_REGULARITEM, false, L"Preferences", L""}, 
	{IDM_CTRL, 5, 0, -1, -1, MT_REGULARITEM, false, L"Control Panel", L""},
	{IDM_HOT_KEYS, 59, 0, -1, -1, MT_REGULARITEM, false, L"Hot Keys", L""},
	{IDM_SHOW_HIDE_ALL, 52, 0, -1, -1, MT_REGULARITEM, false, L"Show/Hide", L""},
	{IDM_SHOW_GROUPS, 6, 0, -1, -1, MT_REGULARITEM, false, L"Show", L""}, 
	{IDM_SHOW_ALL, -1, -1, -1, -1, MT_REGULARITEM, false, L"Show All", L""},
	{IDM_SHOW_INCOMING, -1, -1, -1, -1, MT_REGULARITEM, false, L"Incoming", L""},
	{IDM_HIDE_GROUPS, 7, 0, -1, -1, MT_REGULARITEM, false, L"Hide", L""},
	{IDM_HIDE_ALL, -1, -1, -1, -1, MT_REGULARITEM, false, L"Hide All", L""},
	{IDM_HIDE_INCOMING, -1, -1, -1, -1, MT_REGULARITEM, false, L"Incoming", L""},
	{IDM_SHOW_BY_TAG, 8, 0, -1, -1, MT_REGULARITEM, false, L"Show By Tag", L""},
	{IDM_HIDE_BY_TAG, 9, 0, -1, -1, MT_REGULARITEM, false, L"Hide By Tag", L""},
	{IDM_SHOW_BY_DATE, 51, 0, -1, -1, MT_REGULARITEM, false, L"Last Modified Notes", L""},
	{IDM_SBD_0, -1, -1, -1, -1, MT_REGULARITEM, false, L"Today", L""},
	{IDM_SBD_1, -1, -1, -1, -1, MT_REGULARITEM, false, L"1 day ago", L""},
	{IDM_SBD_2, -1, -1, -1, -1, MT_REGULARITEM, false, L"2 days ago", L""},
	{IDM_SBD_3, -1, -1, -1, -1, MT_REGULARITEM, false, L"3 days ago", L""},
	{IDM_SBD_4, -1, -1, -1, -1, MT_REGULARITEM, false, L"4 days ago", L""},
	{IDM_SBD_5, -1, -1, -1, -1, MT_REGULARITEM, false, L"5 days ago", L""},
	{IDM_SBD_6, -1, -1, -1, -1, MT_REGULARITEM, false, L"6 days ago", L""},
	{IDM_SBD_7, -1, -1, -1, -1, MT_REGULARITEM, false, L"7 days ago", L""},
	{IDM_ALL_TO_FRONT, 10, -1, -1, -1, MT_REGULARITEM, false, L"Bring All To Front", L""},
	{IDM_SAVE_ALL, 11, 0, -1, -1, MT_REGULARITEM, false, L"Save All", L""},
	{IDM_BACK_SYNC, 12, 0, -1, -1, MT_REGULARITEM, false, L"Backup/Synchronize", L""},
	{IDM_BACKUP_FULL, -1, -1, -1, -1, MT_REGULARITEM, false, L"Create Full Backup", L""},
	{IDM_RESTORE_FULL, -1, -1, -1, -1, MT_REGULARITEM, false, L"Restore From Full Backup", L""},
	{IDM_SYNC_NOW, -1, -1, -1, -1, MT_REGULARITEM, false, L"Synchronize", L""},
	{IDM_LOCAL_SYNC, -1, -1, -1, -1, MT_REGULARITEM, false, L"Manual Local Synchronization", L""},
	{IDM_RELOAD_ALL, 13, 0, -1, -1, MT_REGULARITEM, false, L"Reload All", L""},
	{IDM_DOCK_ALL, 14, 0, -1, -1, MT_REGULARITEM, false, L"Docking (All Notes)", L""},
	{IDM_DOCK_NONE_ALL, -1, 0, -1, -1, MT_REGULARITEM, false, L"None", L""},
	{IDM_DOCK_LEFT_ALL, 15, 0, -1, -1, MT_REGULARITEM, false, L"Left", L""},
	{IDM_DOCK_TOP_ALL, 16, 0, -1, -1, MT_REGULARITEM, false, L"Top", L""},
	{IDM_DOCK_RIGHT_ALL, 17, 0, -1, -1, MT_REGULARITEM, false, L"Right", L""},
	{IDM_DOCK_BOTTOM_ALL, 18, 0, -1, -1, MT_REGULARITEM, false, L"Bottom", L""},
	{IDM_SWITCH_ON_ALL, 19, 0, -1, -1, MT_REGULARITEM, false, L"Switch On (All Notes)", L""},
	{IDM_ALL_PRIORITY_ON, -1, 0, -1, 0, MT_REGULARITEM, false, L"High Priority", L""},
	{IDM_ALL_PROTECTION_ON, -1, 0, -1, 0, MT_REGULARITEM, false, L"Protection Mode", L""},
	{IDM_ALL_COMPLETE_ON, -1, 0, -1, 0, MT_REGULARITEM, false, L"Complete Mark", L""},
	{IDM_ALL_ROLL_ON, -1, 0, -1, 0, MT_REGULARITEM, false, L"Roll", L""},
	{IDM_ALL_ON_TOP_ON, -1, 0, -1, 0, MT_REGULARITEM, false, L"\"On Top\" Status", L""},
	{IDM_SWITCH_OFF_ALL, 20, 0, -1, -1, MT_REGULARITEM, false, L"Switch Off (All Notes)", L""},
	{IDM_ALL_PRIORITY_OFF, -1, 0, -1, 0, MT_REGULARITEM, false, L"High Priority", L""},
	{IDM_ALL_PROTECTION_OFF, -1, 0, -1, 0, MT_REGULARITEM, false, L"Protection Mode", L""},
	{IDM_ALL_COMPLETE_OFF, -1, 0, -1, 0, MT_REGULARITEM, false, L"Complete Mark", L""},
	{IDM_ALL_ROLL_OFF, -1, 0, -1, 0, MT_REGULARITEM, false, L"Unroll", L""},
	{IDM_ALL_ON_TOP_OFF, -1, 0, -1, 0, MT_REGULARITEM, false, L"\"On Top\" Status", L""},
	{IDM_SEARCH_SUBMENU, 21, 0, -1, -1, MT_REGULARITEM, false, L"Search", L""},
	{IDM_SEARCH_IN_NOTES, -1, 0, -1, -1, MT_REGULARITEM, false, L"Search In Notes", L""},
	{IDM_SEARCH_BY_TAGS, -1, 0, -1, -1, MT_REGULARITEM, false, L"Search By Tags", L""},
	{IDM_SEARCH_BY_DATE, -1, 0, -1, -1, MT_REGULARITEM, false, L"Search By Dates", L""},
	{IDM_FAVORITES, 22, 0, -1, -1, MT_REGULARITEM, false, L"Favorites", L""},
	{IDM_SHOW_ALL_FAVORITES, -1, -1, -1, -1, MT_REGULARITEM, false, L"Show All", L""},
	{IDM_PASSWORD, 56, 0, -1, -1, MT_REGULARITEM, false, L"Password", L""},
	{IDM_CREATE_PASSWORD, -1, 0, -1, -1, MT_REGULARITEM, false, L"Create Password", L""},
	{IDM_CHANGE_PASSWORD, -1, 0, -1, -1, MT_REGULARITEM, false, L"Change Password", L""},
	{IDM_REMOVE_PASSWORD, -1, 0, -1, -1, MT_REGULARITEM, false, L"Remove Password", L""},
	{IDM_LOCK_PROGRAM, 23, 0, -1, -1, MT_REGULARITEM, false, L"Lock Program", L""},
	{IDM_RUN_PROG, 24, 0, -1, -1, MT_REGULARITEM, false, L"Run", L""},
	{IDM_LANGUAGES, 25, 0, -1, -1, MT_REGULARITEM, false, L"Language", L""},
	{IDM_HELP, 26, 0, -1, -1, MT_REGULARITEM, false, L"Help", L""}, 
	{IDM_ABOUT, 27, 0, -1, -1, MT_REGULARITEM, false, L"About", L""}, 
	{IDM_PAYPAL, 28, 0, -1, -1, MT_REGULARITEM, false, L"Support PNotes Project", L""},
	{IDM_ON_THE_WEB, 29, -1, -1, -1, MT_REGULARITEM, false, PROG_PAGE_MENU_ITEM, L""},
	{IDM_EXIT, 55, -1, -1, -1, MT_REGULARITEM, false, L"Exit", L""}};

static HWND					m_hHotDlg = 0, m_hTopPanel;
static NOTIFYICONDATAW		m_nData;
static HMENU				m_hMenu = 0, m_hPopUp, m_hPrefsMenu = NULL;
static BOOL					m_InTray = false, m_InCheckOverdue = false;
static wchar_t				m_sTempSkinFile[256];
static wchar_t				m_sNoWindowMessage[256], m_sPathCHM[MAX_PATH], m_sPathPDF[MAX_PATH];
static wchar_t				m_sInvalidDate[256], m_sInvalidTime[256], m_sUnsuccessfull[512];
static wchar_t				m_sDefSize[256], m_sTempdefBrowser[MAX_PATH];
static wchar_t				m_sSound1[128], m_sSound2[128], m_sSound3[128], m_sSound4[128], m_sSound5[128];
static USHORT				m_TempWSPort;
static NOTE_RTHANDLES		m_TempRHandles, m_TempDRTHandles, m_TempDiaryRTHandles;
static NOTE_APPEARANCE		m_TempAppearance, m_TempDiaryAppearance;
static NOTE_SETTINGS		m_TempNoteSettings;
static NEXT_SETTINGS		m_TempNextSettings;
static SMALLVALUES			m_TempSmallValues;
static NT_DT_FORMATS		m_TempFormats;
static PNDOCK				m_TempDockingSettings;
static HANDLE				m_Mutex;
static SOUND_TYPE			m_TempSound, m_PrevSound;
static HK_TYPE				m_HKCurrent;
static int					m_Args;				//command line arguments
static int					m_TimerAutosave = 0;		//autosave timer id
static HIMAGELIST			m_hImlLeft = NULL, m_hImlTreeCheck = NULL, m_hImlSmallBars = NULL, m_hImlSmallBarsGray = NULL, m_hImlDefCheck = NULL, m_hImlHotKeys = NULL;
static P_CUST_FONT			m_pFonts = NULL;
static wchar_t				* m_PanelDefs[] = {L"Appearance", L"Skins", L"Schedule", L"Docking", L"Behavior", L"Protection", L"Diary", L"Network", L"Misc"};
static HK_TYPE				m_Hotkeys[NELEMS(m_MainMenus)], m_TempHotkeys[NELEMS(m_MainMenus)];
static LPPNGROUP			m_TempGroups = NULL;
static PNCOMMAND 			m_DblCommands[DBL_CLICK_COUNT];
static P_HK_TYPE			m_TempNoteHotkeys = NULL;
static P_HK_TYPE			m_TempEditHotkeys = NULL;
static DLG_WNDS				m_Dialogs;
static char					m_Version[12];
static BOOL					m_CheckingFromButton = false, m_TrackBaloonClick, m_NewDLLVersion, m_TrackLogon, m_SettingsDialogLoaded;
static int					m_CurrentOption = IDR_ICO_APPEARANCE;
static NAV_BUTTON			m_NavButtons[] = {{IDR_ICO_APPEARANCE, NULL}, {IDR_ICO_SKINS, NULL}, {IDR_ICO_SCHEDULE, NULL}, {IDR_ICO_DOCK, NULL}, {IDR_ICO_BEHAVIOR, NULL}, {IDR_ICO_PROTECTION, NULL}, {IDR_ICO_DIARY, NULL}, {IDR_ICO_NETWORK, NULL}, {IDR_ICO_MISC, NULL}};
static UINT					WM_TASKBARCREATED, WM_BALOONMOUSEMESSAGE;
static HWND					m_hTreeBehavior, m_hTreeSkins, m_hTreeMisc, m_hTreeApp, m_hTreeSchedule, m_hTreeProtection, m_hTreeDiary, m_hTreeDock, m_hTreeNetwork;
static TBRTOOLTIP			m_Tooltips[4] = {{IDC_CMD_ADD_SOUND, L"Add"}, {IDC_CMD_DEL_SOUND, L"Remove"}, {IDC_CMD_MODIFY, L"Modify"}, {IDC_CMD_LISTEN, L"Listen"}};
static int					m_TempDiaryFormatIndex;
static int					m_ShowHideIndex, m_ProgressIconIndex;
static LPPTAG				m_TempPTagsPredefined, m_TagsDeleted, m_TagsModified;
static LPPSENG				m_TempSengs, m_TempExternals;
static PSENG				m_Seng, m_External;
static HICON				m_ProgressIcons[6];		//for progress animation
static BOOL					m_IsProgressTimer = false;
static FTP_DATA				m_TempFTPData;
static wchar_t				m_TempVoice[128];
static int					m_TimerCleanBin;
static BOOL					m_CleanBinProgress;
static void					*m_pVoice = NULL;
static FTP_DATA				m_FTPData;
static PGRCHECKED			m_GChecks;
static BOOL					m_DisableNetwork = false;
static win_version			m_ShellVersion;
static COLORREF				m_TempSpellColor;
static wchar_t				m_BackupDirCL[MAX_PATH];
static BOOL					m_InDblClick = false;
static UINT					m_Elapsed = 0;

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX 	icc;
    WNDCLASSEXW 			wcx;
	MSG						msg;
	int						args;
	wchar_t					szINIPath[MAX_PATH], szDataPath[MAX_PATH], szProgPath[MAX_PATH], szDBPath[MAX_PATH];
	wchar_t					szSkinsPath[MAX_PATH], szBackPath[MAX_PATH], szLangPath[MAX_PATH], szSoundPath[MAX_PATH], szFontsPath[MAX_PATH], szDictPath[MAX_PATH];

	args = ParsePNCommandLine(szINIPath, szDataPath, szProgPath, szDBPath, szSkinsPath, szBackPath, szLangPath, szSoundPath, szFontsPath, szDictPath);

	m_Mutex = CreateMutexW(NULL, true, NOTES_MUTEX);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		//close previous instance of program and exit (continue if there is backup switch)
		if((args & ARG_BACKUP) != ARG_BACKUP){
			if(((args & ARG_EXIT) == ARG_EXIT) || ((args & ARG_CREATE_NEW) == ARG_CREATE_NEW))
				EnumWindows(EnumPNotes, args);
			return 0;
		}
	}
	else{
		//just exit if there was any 'exit' command line switch
		if((args & ARG_EXIT) == ARG_EXIT)
			return 0;
	}
	
	//get program start time and system start time
	GetLocalTime(&g_StartTimeProg);
	GetSystemStartTime();

	//check whether main INI path has been sent as command line argument
	g_NotePaths.DataDir[0] = '\0';
	g_NotePaths.INIFile[0] = '\0';
	if((args & ARG_INI_PATH) == ARG_INI_PATH){
		wcscpy(g_NotePaths.INIFile, szINIPath);
		if(!_wcsistr(g_NotePaths.INIFile, L"Notes.ini")){
			if(IsLastBackslash(g_NotePaths.INIFile))
				wcscat(g_NotePaths.INIFile, L"Notes.ini");
			else
				wcscat(g_NotePaths.INIFile, L"\\Notes.ini");
		}
	}
	if((args & ARG_DATA_PATH) == ARG_DATA_PATH){
		if(!IsLastBackslash(szDataPath))
			wcscat(szDataPath, L"\\");
		wcscpy(g_NotePaths.DataDir, szDataPath);
	}
	else{
		GetSubPathW(g_NotePaths.DataDir, L"\\data\\");
	}
	CheckAndCreatePath(g_NotePaths.DataDir);

	if((args & ARG_PROG_PATH) == ARG_PROG_PATH){
		wcscpy(g_NotePaths.ProgFullPath, szProgPath);
	}
	else{
		GetModuleFileNameW(NULL, g_NotePaths.ProgFullPath, MAX_PATH);
	}

	if((args & ARG_DB_PATH) == ARG_DB_PATH){
		wcscpy(g_NotePaths.DataFile, szDBPath);
		if(!_wcsistr(g_NotePaths.DataFile, L"notes.pnid")){
			if(IsLastBackslash(g_NotePaths.DataFile))
				wcscat(g_NotePaths.DataFile, L"notes.pnid");
			else
				wcscat(g_NotePaths.DataFile, L"\\notes.pnid");
		}
	}
	else{
		wcscpy(g_NotePaths.DataFile, g_NotePaths.DataDir);
		wcscat(g_NotePaths.DataFile, FN_DATA_FILE);
	}

	//get main INI file path
	if(wcslen(g_NotePaths.INIFile) == 0)
		GetSubPathW(g_NotePaths.INIFile, FN_INI_FILE);
	//other paths
	if((args & ARG_SKINS_PATH) == ARG_SKINS_PATH){
		wcscpy(g_NotePaths.SkinDir, szSkinsPath);
		wcscat(g_NotePaths.SkinDir, L"\\");
	}
	else
		g_NotePaths.SkinDir[0] = '\0';

	if((args & ARG_BACKUP_PATH) == ARG_BACKUP_PATH){
		wcscpy(g_NotePaths.BackupDir, szBackPath);
		wcscat(g_NotePaths.BackupDir, L"\\");
	}
	else{
		GetSubPathW(g_NotePaths.BackupDir, L"\\backup\\");
	}

	//create backup directory
	CheckAndCreatePath(g_NotePaths.BackupDir);
	if((args & ARG_BACKUP) == ARG_BACKUP){
		//backup
		if(m_BackupDirCL[0] == '\0')
			BackupOnCommandLine(NULL);
		else
			BackupOnCommandLine(m_BackupDirCL);
		//exit program
		return 0;
	}

	if((args & ARG_LANG_PATH) == ARG_LANG_PATH){
		wcscpy(g_NotePaths.LangDir, szLangPath);
		wcscat(g_NotePaths.LangDir, L"\\");
	}
	else
		g_NotePaths.LangDir[0] = '\0';
	if((args & ARG_SOUND_PATH) == ARG_SOUND_PATH){
		wcscpy(g_NotePaths.SoundDir, szSoundPath);
		wcscat(g_NotePaths.SoundDir, L"\\");
	}
	else
		g_NotePaths.SoundDir[0] = '\0';
	if((args & ARG_FONTS_PATH) == ARG_FONTS_PATH){
		wcscpy(g_NotePaths.FontsPath, szFontsPath);
		wcscat(g_NotePaths.FontsPath, L"\\");
	}
	else
		g_NotePaths.FontsPath[0] = '\0';
	if((args & ARG_DICT_PATH) == ARG_DICT_PATH){
		wcscpy(g_NotePaths.DictDir, szDictPath);
		wcscat(g_NotePaths.DictDir, L"\\");
	}
	else
		g_NotePaths.DictDir[0] = '\0';

	if((args & ARG_NONETWORK) == ARG_NONETWORK)
		m_DisableNetwork = true;

	g_hInstance = hInstance;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_COOL_CLASSES;
    InitCommonControlsEx(&icc);

	LoadLibraryW(L"riched20.dll");  // Rich Edit v2.0, v3.0

	//initializa COM
	CoInitialize(NULL);

	m_NewDLLVersion = IsNewDLLVersion("shell32.dll");
	g_WinVer = WinVer();
	m_ShellVersion = ShellVersion();

	WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
	WM_BALOONMOUSEMESSAGE = RegisterWindowMessageW(L"BaloonMouseMessage");

	ZeroMemory(&wcx, sizeof(wcx));
	//register owner window class
	wcx.cbSize = sizeof(wcx);
    wcx.hInstance = hInstance;
	wcx.lpfnWndProc = NotesOwnerWindowProc;
	wcx.hIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
	wcx.hIconSm = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
    wcx.lpszClassName = NOTES_OWNER_CLASS;
    if (!RegisterClassExW(&wcx))
        return 1;
	//create invisible owner window for notes
	g_hNotesParent = CreateWindowExW(0, NOTES_OWNER_CLASS, NULL, 0, 0, 0, 0, 0, NULL, NULL, g_hInstance, NULL);

	ZeroMemory(&wcx, sizeof(wcx));
    //register main window class
    wcx.cbSize = sizeof(wcx);
    wcx.hInstance = hInstance;
	wcx.lpfnWndProc = WindowProc;
	wcx.hIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
	wcx.hIconSm = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
    wcx.lpszClassName = NOTES_PROG_MAIN_CLASS;
    if (!RegisterClassExW(&wcx))
        return 1;

	//create invisible main window
	CreateWindowExW(0, NOTES_PROG_MAIN_CLASS, NULL, 0, 0, 0, 0, 0, NULL, NULL, g_hInstance, NULL);
	if(g_hMain){
		//get version for updates checking
		GetVersionNumber();
		//add notification icon
		AddNotifyIcon();
		g_SearchProc = Search_DlgProc;
		g_SearchTagsProc = SearchTags_DlgProc;
		if(g_NoteSettings.checkOnStart)
			StartUpdateProcess();
		//check overdue reminders
		if(IsBitOn(g_NoteSettings.reserved1, SB1_TRACK_OVERDUE)){
			CheckOverdueReminders();
		}
		// CreateMenuColors(g_WinVer);
	}
	if(g_LastStartTimeProg.wDay == 0 && NotesCount() == 0){
		//show baloon for the very first time
		ShowFirstBaloon();
		//add default search providers
		g_PSengs = SEngsAdd(g_PSengs, L"Google", L"http://www.google.com/search?q=");
		g_PSengs = SEngsAdd(g_PSengs, L"Yahoo", L"http://search.yahoo.com/search?p=");
		SaveSearchEngines();
	}

	//start message loop
	while(GetMessageW(&msg, NULL, 0, 0)){
		if(!IsDialogMessageW(g_hSearchDialog, &msg) 
		&& !IsDialogMessageW(g_hSearchTitlesDialog, &msg) 
		&& !IsDialogMessageW(g_hSearchEverywhereDialog, &msg) 
		&& !IsDialogMessageW(g_hSearchTags, &msg) 
		&& !IsDialogMessageW(g_hOptionsDlg, &msg)
		&& !IsDialogMessageW(g_hSearchDates, &msg)){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	CoUninitialize();
	//exit program
	return msg.wParam;
}

static void ReloadNotes(BOOL bSaveBefore){
	if(bSaveBefore){
		if(g_NoteSettings.saveOnExit & (m_Args & ARG_NO_SAVE) != ARG_NO_SAVE){
			ApplySaveOnUnload(m_Args, true);
		}
	}
	//close all notes in order to clean up memory
	ApplyCloseAllNotes();
	//free previous docking headers
	FreeDockHeaders();
	//free memory notes list
	FreeMemNotes();
	//prepare docking headers
	InitDockHeaders();
	GetAllNotes(true);
	if(g_Empties.count){
		LPPOINT	lpp = g_Empties.pPoint;
		LPINT	lpi = g_Empties.pDockData;
		for(int i = 0; i < g_Empties.count; i++){
			HWND hNote = CreateNewNote(false);
			if(hNote){
				SetWindowPos(hNote, HWND_TOP, lpp->x, lpp->y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER);
				lpp++;
				if(DockType(*lpi) != DOCK_NONE){
					P_NOTE_DATA		pD = NoteData(hNote);
					pD->dockData = *lpi;
					AddDockItem(DHeader(DockType(*lpi)), hNote, DockIndex(*lpi));
				}
				lpi++;
			}
		}
		free(g_Empties.pPoint);
		g_Empties.pPoint = 0;
		free(g_Empties.pDockData);
		g_Empties.pDockData = 0;
		g_Empties.count = 0;
	}
	//rearrange all dock notes
	RearrangeDockWindowsOnstart(DOCK_LEFT);
	RearrangeDockWindowsOnstart(DOCK_TOP);
	RearrangeDockWindowsOnstart(DOCK_RIGHT);
	RearrangeDockWindowsOnstart(DOCK_BOTTOM);
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, PNM_RELOAD, 0, 0);
}

static LRESULT CALLBACK NotesOwnerWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	if(g_StopInput && (uMsg != WM_COMMAND && LOWORD(wParam) != IDM_LOCK_PROGRAM))
		return false;
	switch(uMsg){
		HANDLE_MSG (hwnd, WM_DESTROY, Main_OnDestroy);
		HANDLE_MSG (hwnd, WM_CLOSE, Main_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG (hwnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Main_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, Main_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_HOTKEY, Main_OnHotKey);
		HANDLE_MSG (hwnd, WM_NCDESTROY, Main_OnNCDestroy);
		HANDLE_MSG (hwnd, WM_ENDSESSION, Main_OnEndSession);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Main_OnInitMenuPopup);

		case SPM_GETCODEPAGE:
			return GetCodePage((char *)lParam);
		case WM_POWERBROADCAST:
			if(wParam == PBT_APMRESUMEAUTOMATIC || wParam == PBT_APMRESUMESUSPEND || wParam == PBT_APMRESUMECRITICAL){
				//check overdue reminders after resuming
				if(IsBitOn(g_NoteSettings.reserved1, SB1_TRACK_OVERDUE)){
					CheckOverdueReminders();
				}
			}
			// else if(wParam == PBT_APMSUSPEND){
				// //have to silently save all unsaved notes

			// }
			break;
		case WM_WTSSESSION_CHANGE:
			if(m_TrackLogon){
				if(wParam == WTS_SESSION_UNLOCK || wParam == WTS_REMOTE_DISCONNECT){
					ApplySessionLogOn();
				}
			}
			break;
		case WSPM_DATA_SAVING_ERROR:
			MessageBoxW(hwnd, L"Error receiving data", PROG_NAME, MB_OK | MB_ICONERROR);
			break;
		case WSPM_RECEIVE_FINISHED:{
			ReceiveNewNote();
			g_wsPause = FALSE;
			break;
		}
		case UPDM_INETERROR:
			if(m_CheckingFromButton){
				if(strlen(gu_ErrDesc) > 0)
					MessageBox(hwnd, gu_ErrDesc, "PNotes", MB_OK | MB_ICONERROR);
				else
					MessageBoxW(hwnd, g_Strings.InternetUnavailable, PROG_NAME, MB_OK | MB_ICONERROR);
			}
			break;
		case UPDM_UPDATEFOUND:{
			wchar_t		temp[12];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)wParam, 12, temp, 12);
			ShowNewVersionBaloon(temp);
			break;
		}
		case UPDM_SAMEVERSION:
			if(m_CheckingFromButton)
				MessageBoxW(m_Dialogs.hMisc, g_Strings.SameVersion, g_Strings.CheckUpdate, MB_OK | MB_ICONINFORMATION);
			break;
		case UPDM_GETVERSION:
			strcpy((char *)wParam, m_Version);
			break;
		case UPDM_GETSTRINGS:
			strcpy((char *)wParam, UPDATE_CHECK_URL);
			strcpy((char *)lParam, "/version.txt");
			break;
		case PNM_IS_FTP_ENABLED:
			return IsFTPEnabled();
		case PNM_NOTES_RELOAD:
			ReloadNotes((BOOL)wParam);
			break;
		case PNM_OPEN_PAGE:
			OpenPage(hwnd, (wchar_t *)lParam);
			break;
		case PNM_NOTE_SENT:{
			wchar_t					szInfo[256], szDate[128];
					
			wcscpy(szInfo, g_Strings.Sent1);
			wcscat(szInfo, L" '");
			wcscat(szInfo, (wchar_t *)lParam);
			wcscat(szInfo, L"' ");
			wcscat(szInfo, g_Strings.Sent2);
			wcscat(szInfo, L"\n");
			wcscat(szInfo, g_Strings.Sent3);
			wcscat(szInfo, L" ");
			wcscat(szInfo, (wchar_t *)wParam);
			wcscat(szInfo, L"\n");
			wcscat(szInfo, g_Strings.Sent4);
			wcscat(szInfo, L" ");
			ConstructDateTimeString(NULL, szDate);
			wcscat(szInfo, szDate);
			ShowNotification(szInfo, g_Strings.SentCaption, 5000);
			break;
		}
		// case WM_SYSCOLORCHANGE:
			// CreateMenuColors(g_WinVer);
			// break;
		case WM_SHELLNOTIFY:
			if(wParam == IDI_TRAY){
				if(lParam == WM_LBUTTONDBLCLK){
					m_InDblClick = true;
					KillTimer(hwnd, TIMER_DBL_CLICK);
					m_Elapsed = 0;
					if(g_LockedState){
						if(g_hLoginDialog == NULL){
							if(IsPasswordSet()){
								if(ShowLoginDialog(g_hInstance, NULL, DLG_LOGIN_MAIN, NULL, LDT_MAIN, NULL) == IDCANCEL)
									return false;
								else{
									//unlock program
									g_UnlockFromMouse = true;
									SendMessageW(hwnd, WM_COMMAND, IDM_LOCK_PROGRAM, 0);
									return false;
								}
							}
							else{
								return false;
							}
						}
						else{
							return false;
						}
					}
					//choose default action
					switch(GetIndexOfDblClick()){
						case SB2_NEW_NOTE:
							SendMessageW(hwnd, WM_COMMAND, IDM_NEW, 0);
							break;
						case SB2_CONTROL_PANEL:
							SendMessageW(hwnd, WM_COMMAND, IDM_CTRL, 0);
							break;
						case SB2_PREFS:
							SendMessageW(hwnd, WM_COMMAND, IDM_OPTIONS, 0);
							break;
						case SB2_SEARCH_NOTES:
							SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_IN_NOTES, 0);
							break;
						case SB2_LOAD_NOTE:
							SendMessageW(hwnd, WM_COMMAND, IDM_LOAD_NOTE, 0);
							break;
						case SB2_FROM_CLIPBOARD:
							SendMessageW(hwnd, WM_COMMAND, IDM_NOTE_FROM_CLIPBOARD, 0);
							break;
						case SB2_ALL_TO_FRONT:
							SendMessageW(hwnd, WM_COMMAND, IDM_ALL_TO_FRONT, 0);
							break;
						case SB2_SAVE_ALL:
							SendMessageW(hwnd, WM_COMMAND, IDM_SAVE_ALL, 0);
							break;
						case SB2_SHOW_HIDE:
							if(m_ShowHideIndex == 0)
								SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SHOW_ALL, 0), 0);
							else
								SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIDE_ALL, 0), 0);
							break;
						case SB2_SEARCH_TAGS:
							SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_BY_TAGS, 0);
							break;
						case SB2_SEARCH_BY_DATES:
							SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_BY_DATE, 0);
							break;
						default:
							SendMessageW(hwnd, WM_COMMAND, IDM_NEW, 0);
							break;
					}
				}
				else if(lParam == WM_LBUTTONUP){
					if(!g_LockedState){
						if(m_InDblClick)
							m_InDblClick = false;
						else
							SetTimer(hwnd, TIMER_DBL_CLICK, 100, DblClickTimerProc);
					}
				}
				else if(lParam == WM_LBUTTONDOWN){
					if(g_LockedState){
						if(g_hLoginDialog == NULL){
							if(IsPasswordSet()){
								if(ShowLoginDialog(g_hInstance, NULL, DLG_LOGIN_MAIN, NULL, LDT_MAIN, NULL) == IDCANCEL)
									return false;
								else{
									//unlock program
									g_UnlockFromMouse = true;
									SendMessageW(hwnd, WM_COMMAND, IDM_LOCK_PROGRAM, 0);
									SetForegroundWindow(g_hMain);
									return false;
								}
							}
							else{
								return false;
							}
						}
						else{
							return false;
						}
					}
					else{
						SendMessageW(hwnd, WM_COMMAND, IDM_ALL_TO_FRONT, 0);
						return false;
					}
				}
				else if (lParam == WM_RBUTTONDOWN){
					if(g_LockedState){
						if(g_hLoginDialog == NULL){
							if(ShowLoginDialog(g_hInstance, NULL, DLG_LOGIN_MAIN, NULL, LDT_MAIN, NULL) == IDCANCEL)
								return false;
							else{
								//unlock program
								g_UnlockFromMouse = true;
								SendMessageW(hwnd, WM_COMMAND, IDM_LOCK_PROGRAM, 0);
								return false;
							}
						}
						else{
							return false;
						}
					}
					//show popup menu
					//build favorites menu
					BuildFavoritesMenu();
					//build diary menu
					BuildDiaryMenu(m_hPopUp);
					//build show/hide by tag menu
					BuildShowHideByTagsMenu(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SHOW_HIDE_ALL)));
					//build show/hide menus
					CreateShowHideGroupsMenus(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SHOW_HIDE_ALL)));
					//build external programs menu
					BuildExternalMenu(m_hPopUp);
					ShowPopUp(hwnd, m_hPopUp);
				}
				else if(lParam == NIN_BALLOONUSERCLICK){
					if(wcscmp(m_nData.szInfoTitle, g_Strings.CheckUpdate) == 0){
						OpenPage(g_hMain, DOWNLOAD_PAGE);
					}
					else if(wcscmp(m_nData.szInfoTitle, g_Strings.ReceivedCaption) == 0){
						if(IsBitOn(g_NextSettings.flags1, SB3_CLICK_BALOON_SHOW_NOTE)){
							PMEMNOTE	pNote = MemNoteById(g_wsID);
							ShowNote(pNote);
							// SaveVisibility(pNote->hwnd, TRUE);
							// CenterWindow(pNote->hwnd, false);
						}
						if(IsBitOn(g_NextSettings.flags1, SB3_CLICK_BALOON_SHOW_CP)){
							wchar_t			szKey[16];
							_itow(GROUP_INCOMING, szKey, 10);
							WritePrivateProfileStringW(S_CP_DATA, IK_CP_LAST_GROUP, szKey, g_NotePaths.INIFile);
							if(!g_hCPDialog){
								ShowControlPanel();
							}
							else{
								WINDOWPLACEMENT		wp;
								wp.length = sizeof(wp);
								GetWindowPlacement(g_hCPDialog, &wp);
								if(wp.showCmd == SW_SHOWMINIMIZED){
									wp.showCmd = SW_SHOWNORMAL;
									SetWindowPlacement(g_hCPDialog, &wp);
								}
								SetForegroundWindow(g_hCPDialog);
								SendMessageW(g_hCPDialog, PNM_SELECT_INCOMING, 0, 0);
							}
						}
					}
				}
			}
			break;
		case PNM_CREATE_NEW_NOTE:
			SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_NEW, 0), 0);
			break;
		case PNM_NEW_FROM_CLIPBOARD:
			SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_NOTE_FROM_CLIPBOARD, 0), 0);
			break;
		case PNM_DIARY_TODAY:
			SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_TODAY_DIARY, 0), 0);
			break;
		case PNM_MAIN_CLOSE:
			m_Args = lParam;
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case PNM_FAVORITES_CHANGED:
			BuildFavoritesMenu();
			break;
		case PNM_ON_TOP_CHANGED:
			ApplyOnTopChanged();
			break;
		case PNM_GET_MAIN_MENU:
			return (LRESULT)m_hPopUp;
		default:
			if(uMsg == WM_TASKBARCREATED)
				AddNotifyIcon();
			else
				return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

static void Main_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
	EnableMainMenus();
}

static void Main_OnEndSession(HWND hwnd, BOOL fEnding)
{
	//save all when Windows session ends
	CleanUp(false);
	if(g_NoteSettings.saveOnExit){
		if((m_Args & ARG_SILENT) != ARG_SILENT){
			m_Args |= ARG_SILENT;
		}
		ApplySaveOnUnload(m_Args, false);;
	}
	FreeMemNotes();
}

static void Main_OnNCDestroy(HWND hwnd)
{
	//unregister hot keys
	UnregisterHKeys(hwnd, m_Hotkeys, NELEMS(m_Hotkeys));
	//save hot keys
	SaveHotKeys(S_HOTKEYS, m_Hotkeys, NELEMS(m_Hotkeys));
}

static void Main_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void Main_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void Main_OnClose(HWND hwnd)
{
	if(g_NoteSettings.saveOnExit & (m_Args & ARG_NO_SAVE) != ARG_NO_SAVE){
		ApplySaveOnUnload(m_Args, false);
	}
	//close all notes in order to clean up memory
	ApplyCloseAllNotes();
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, WM_CLOSE, 0, 0);
	CleanUp(true);
	DestroyWindow(hwnd);
}

static void Main_OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk)
{
	int 	id, state;

	if(idHotKey >= MAIN_HOTKEYS_IND && idHotKey < NOTE_HOTKEYS_IND){
		id = GetHKId(m_Hotkeys, NELEMS(m_Hotkeys), idHotKey);
		if(id){
			if(m_hHotDlg != 0)
				//user press the hot key on the hot key creation dialog - just warn
				HKeysAlreadyRegistered(GetHotKeyByID(id, m_Hotkeys, NELEMS(m_Hotkeys))->szKey);
			else{
				if(id == IDM_LOCK_PROGRAM){
					if(IsPasswordSet()){
						//process hot key
						state = GetMenuState(m_hPopUp, id, MF_BYCOMMAND);
						if((state & MF_GRAYED) != MF_GRAYED)
							SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(id, 0), 0);
					}
				}
				else{
					if(!g_LockedState){
						//process hot key
						state = GetMenuState(m_hPopUp, id, MF_BYCOMMAND);
						if((state & MF_GRAYED) != MF_GRAYED)
							SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(id, 0), 0);
					}
				}
			}
		}
	}
	else{
		int		count = PNGroupsCount(g_PGroups);
		if((idHotKey >= SHOW_GROUP_ADDITION && idHotKey <= SHOW_GROUP_ADDITION + count)
		|| (idHotKey >= HIDE_GROUP_ADDITION && idHotKey <= HIDE_GROUP_ADDITION + count)){
			//process hot key
			SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(idHotKey, 0), 0);
		}
	}
}

static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HMENU				hTempMenu = NULL;
	int 				pos;
	MENUITEMINFOW		mi;
	wchar_t				szTemp[MAX_PATH], szBuffer[256], szOldPwrd[256], szLang[MAX_PATH];
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	HWND 				hNote;

	switch (id){
		case IDM_NEW:{
			hNote = CreateNewNote(false);
			PMEMNOTE	pNote = MEMNOTE(hNote);
			if(hNote && codeNotify != 0){
				pNote->pData->idGroup = codeNotify;
				LPPNGROUP	ppg = PNGroupsGroupById(g_PGroups, codeNotify);
				if(ppg && ppg->customCRWindow && !IsBitOn(g_NoteSettings.reserved1, SB1_RANDCOLOR)){
					pNote->pAppearance->crWindow = ppg->crWindow;
					SendMessageW(hNote, PNM_NEW_BGCOLOR, 0, 0);
				}
				if(ppg && ppg->customCRCaption && !IsBitOn(g_NoteSettings.reserved1, SB1_INVERT_TEXT)){
					pNote->pAppearance->crCaption = ppg->crCaption;
					SendMessageW(hNote, PNM_NEW_CAPTION_COLOR, 0, 0);
				}
				if(ppg && ppg->customCRFont && !IsBitOn(g_NoteSettings.reserved1, SB1_INVERT_TEXT)){
					pNote->pAppearance->crFont = ppg->crFont;
					SendMessageW(hNote, PNM_CHANGE_FONT_COLOR, (WPARAM)pNote->pAppearance->crFont, 0);
					RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(hNote);
				}
			}
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, PNM_CTRL_INS, 0, (LPARAM)pNote);
				SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
			}
			break;
		}
		case IDM_TODAY_DIARY:
			if(!IsDiaryOfToday()){
				hNote = CreateNewNote(true);
				PMEMNOTE	pNote = MEMNOTE(hNote);
				if(g_hCPDialog){
					SendMessageW(g_hCPDialog, PNM_CTRL_INS, 0, (LPARAM)pNote);
					SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
				}
			}
			break;
		case IDM_NOTE_FROM_CLIPBOARD:
			if(IsTextInClipboard()){
				hNote = NoteFromClipboard();
				if(hNote){
					PMEMNOTE	pNote = MEMNOTE(hNote);
					if(codeNotify != 0){
						pNote->pData->idGroup = codeNotify;
					}
					if(g_hCPDialog){
						SendMessageW(g_hCPDialog, PNM_CTRL_INS, 0, (LPARAM)pNote);
						SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
					}
				}
			}
			break;
		case IDM_CREATE_PASSWORD:
			if(ShowLoginDialog(g_hInstance, hwnd, DLG_LOGIN_CREATE, szBuffer, LDT_MAIN, NULL) == IDOK){
				WritePrivateProfileStringW(S_NOTE, IK_PASSWORD, szBuffer, g_NotePaths.INIFile);
				if(g_hOptionsDlg){
					CTreeView_SetEnable(m_hTreeProtection, m_ChecksTProtection[prEncryption], true);
					CTreeView_SetEnable(m_hTreeProtection, m_ChecksTProtection[prHideFromTray], true);
				}
			}
			break;
		case IDM_CHANGE_PASSWORD:
			GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szOldPwrd, 256, g_NotePaths.INIFile);
			if(ShowLoginDialog(g_hInstance, hwnd, DLG_LOGIN_CHANGE, szBuffer, LDT_MAIN, NULL) == IDOK){
				WritePrivateProfileStringW(S_NOTE, IK_PASSWORD, szBuffer, g_NotePaths.INIFile);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
					DecryptAll(szOldPwrd);
					EncryptAll(szBuffer);
				}
			}
			break;
		case IDM_REMOVE_PASSWORD:
			GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szOldPwrd, 256, g_NotePaths.INIFile);
			if(ShowLoginDialog(g_hInstance, hwnd, DLG_LOGIN_MAIN, NULL, LDT_MAIN, NULL) == IDOK){
				WritePrivateProfileStringW(S_NOTE, IK_PASSWORD, NULL, g_NotePaths.INIFile);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
					DecryptAll(szOldPwrd);
				}
				BitOff(&g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED);
				WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), g_NotePaths.INIFile);
				if(g_hOptionsDlg){
					BitOff(&m_TempNoteSettings.reserved1, SB1_STORE_ENCRYPTED);
					CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prEncryption], false);
					CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prHideFromTray], false);
					CTreeView_SetEnable(m_hTreeProtection, m_ChecksTProtection[prEncryption], false);
					CTreeView_SetEnable(m_hTreeProtection, m_ChecksTProtection[prHideFromTray], false);
				}
			}
			break;
		case IDM_HOT_KEYS:
			if(!g_hHotkeys)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOT), hwnd, Hotdlg_DlgProc, 0);
			else
				BringWindowToTop(g_hHotkeys);
			break;
		case IDM_SBD_0:
			ShowNotesByDate(DateSubtract(0));
			break;
		case IDM_SBD_1:
			ShowNotesByDate(DateSubtract(1));
			break;
		case IDM_SBD_2:
			ShowNotesByDate(DateSubtract(2));
			break;
		case IDM_SBD_3:
			ShowNotesByDate(DateSubtract(3));
			break;
		case IDM_SBD_4:
			ShowNotesByDate(DateSubtract(4));
			break;
		case IDM_SBD_5:
			ShowNotesByDate(DateSubtract(5));
			break;
		case IDM_SBD_6:
			ShowNotesByDate(DateSubtract(6));
			break;
		case IDM_SBD_7:
			ShowNotesByDate(DateSubtract(7));
			break;
		case IDM_SYNC_NOW:
			SynchronizeNow(hwnd, 0);
			break;
		case IDM_LOCAL_SYNC:
			if(g_hSyncFolders == NULL)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_LOCAL_SYNC), g_hMain, LocalSync_DlgProc, 0);
			break;
		case IDM_BACKUP_FULL:{
			FullBackup(hwnd, IsBitOn(g_NoteSettings.reserved1, SB1_SILENT_BACKUP), true);
			break;
		}
		case IDM_RESTORE_FULL:{
			GetPrivateProfileStringW(L"captions", L"open_full_backup", L"Restore from full backup", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, FULL_BACKUP_FILTER, szBuffer, g_NotePaths.BackupDir)){
				wchar_t		szMessage[320];
				wcscpy(szMessage, g_Strings.FullBackupWarnung);
				wcscat(szMessage, L"\n");
				wcscat(szMessage, g_Strings.ContinueQuestion);
				if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONEXCLAMATION) == IDYES){
					if(RestoreFullBackUp(szFileName)){
						ReloadNotes(false);
					}
				}
			}
			break;
		}
		case IDM_LOAD_NOTE:{
			GetPrivateProfileStringW(L"captions", L"load", L"Load Note", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, LOAD_NOTE_FILTER, szBuffer, NULL)){
				PathRemoveExtensionW(szFileTitle);
				GetPrivateProfileStringW(szFileTitle, S_DATA, NULL, szTemp, 256, g_NotePaths.DataFile);
				if(wcslen(szTemp) > 0){
					GetPrivateProfileStringW(L"messages", L"already_exists", L"The note with the same id already exists. Please, choose another file.", szFileName, MAX_PATH, g_NotePaths.CurrLanguagePath);
					GetPrivateProfileStringW(L"captions", L"loading_error", L"Loading Error", szBuffer, 256, g_NotePaths.CurrLanguagePath);
					MessageBoxW(hwnd, szFileName, szBuffer, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				PMEMNOTE	pNote = AddMemNote();
				LoadNoteProperties(pNote, &g_RTHandles, szFileTitle, true);
				//check whether loaded file is encrypted or not - get 7 first characters from file
				HANDLE		hFile;
				ULONG		read;
				char		szs[6];
				BOOL		fEncrypted = true;
				wchar_t		szTempPath[MAX_PATH + 128], szHash[256];

				hFile = CreateFileW(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if(hFile != INVALID_HANDLE_VALUE){
					GetNoteTempFileName(szTempPath);
					GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szHash, 256, g_NotePaths.INIFile);

					szs[5] = '\0';
					ReadFile(hFile, szs, 5, &read, NULL);
					if(_stricmp(szs, RTF_START) == 0){
						fEncrypted = false;
					}
					CloseHandle(hFile);
					if(!fEncrypted){
						//file is not encrypted
						if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
							//encrypt file before processing
							CryptEncryptFile(szFileName, szTempPath, szHash);
							hNote = CreateNote(pNote, g_hInstance, true, szTempPath);
						}
						else{
							//get file as is
							hNote = CreateNote(pNote, g_hInstance, true, szFileName);
						}
					}
					else{
						//file is encrypted
						if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
							//get file as is
							hNote = CreateNote(pNote, g_hInstance, true, szFileName);
						}
						else{
							//decrypt file before processing
							CryptDecryptFile(szFileName, szTempPath, szHash);
							hNote = CreateNote(pNote, g_hInstance, true, szTempPath);
						}
					}
					if(g_hCPDialog){
						SendMessageW(g_hCPDialog, PNM_CTRL_INS, 0, (LPARAM)pNote);
						SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
					}
				}
			}
			break;
		}
		case IDM_ALL_TO_FRONT:
			BringNotesToFront();
			break;
    	case IDM_EXIT:		//exit application
			g_IsClosing = true;
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDM_SEARCH_BY_TAGS:
			if(!g_hSearchTags)
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SEARCH_TAGS), NULL, (DLGPROC)SearchTags_DlgProc, 0);
			else
				BringWindowToTop(g_hSearchTags);
			SetForegroundWindow(g_hSearchTags);
			break;
		case IDM_SEARCH_BY_DATE:
			if(!g_hSearchDates)
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SEARCH_BY_DATE), NULL, (DLGPROC)SearchDates_DlgProc, 0);
			else
				BringWindowToTop(g_hSearchDates);
			SetForegroundWindow(g_hSearchDates);
			break;
		case IDM_SEARCH_IN_NOTES:
			if(!g_hSearchDialog)
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_FIND), NULL, (DLGPROC)Search_DlgProc, 0);
			else
				BringWindowToTop(g_hSearchDialog);
			SetForegroundWindow(g_hSearchDialog);
			break;
		case IDM_OPTIONS:	//restore or show options dialog
			if(!g_hOptionsDlg){
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_MAIN), NULL, (DLGPROC)OptionsDlgProc, 0);
			}
			else{
				WINDOWPLACEMENT		wp;
				wp.length = sizeof(wp);
				GetWindowPlacement(g_hOptionsDlg, &wp);
				if(wp.showCmd == SW_SHOWMINIMIZED){
					wp.showCmd = SW_SHOWNORMAL;
					SetWindowPlacement(g_hOptionsDlg, &wp);
				}
				SetForegroundWindow(g_hOptionsDlg);
				// BringWindowToTop(g_hOptionsDlg);
			}
			break;
		case IDM_ALL_PRIORITY_ON:
			ToggleAllPriorityProtectComplete(NB_HIGH_PRIORITY, true);
			break;
		case IDM_ALL_PROTECTION_ON:
			ToggleAllPriorityProtectComplete(NB_PROTECTED, true);
			break;
		case IDM_ALL_COMPLETE_ON:
			ToggleAllPriorityProtectComplete(NB_COMPLETED, true);
			break;
		case IDM_ALL_ROLL_ON:
			ToggleAllRolled(true);
			break;
		case IDM_ALL_ON_TOP_ON:
			ToggleAllOnTop(true);
			break;
		case IDM_ALL_PRIORITY_OFF:
			ToggleAllPriorityProtectComplete(NB_HIGH_PRIORITY, false);
			break;
		case IDM_ALL_PROTECTION_OFF:
			ToggleAllPriorityProtectComplete(NB_PROTECTED, false);
			break;
		case IDM_ALL_COMPLETE_OFF:
			ToggleAllPriorityProtectComplete(NB_COMPLETED, false);
			break;
		case IDM_ALL_ROLL_OFF:
			ToggleAllRolled(false);
			break;
		case IDM_ALL_ON_TOP_OFF:
			ToggleAllOnTop(false);
			break;
		case IDM_CTRL:
			if(!g_hCPDialog){
				ShowControlPanel();
			}
			else{
				WINDOWPLACEMENT		wp;
				wp.length = sizeof(wp);
				GetWindowPlacement(g_hCPDialog, &wp);
				if(wp.showCmd == SW_SHOWMINIMIZED){
					wp.showCmd = SW_SHOWNORMAL;
					SetWindowPlacement(g_hCPDialog, &wp);
				}
				SetForegroundWindow(g_hCPDialog);
			}
			break;
		case IDM_SHOW_ALL:	//show all notes
			ShowAllNotes();
			BringNotesToFront();
			m_ShowHideIndex = 1;
			WritePrivateProfileStringW(S_NOTE, IK_TOGGLE_SHOW_HIDE, L"1", g_NotePaths.INIFile);
			break;
		case IDM_HIDE_ALL:	//hide all notes
			ApplyHideAllNotes();
			m_ShowHideIndex = 0;
			WritePrivateProfileStringW(S_NOTE, IK_TOGGLE_SHOW_HIDE, L"0", g_NotePaths.INIFile);
			break;
		case IDM_SHOW_INCOMING:
			if(IsAnyHiddenInGroup(GROUP_INCOMING) && UnlockGroup(GROUP_INCOMING))
				ShowGroupOfNotes(GROUP_INCOMING);
			break;
		case IDM_HIDE_INCOMING:
			HideGroupOfNotes(GROUP_INCOMING);
			break;
		case IDM_RELOAD_ALL:
			ReloadNotes(true);
			break;
		case IDM_ABOUT:		//shohw about dialog
			if(!g_hAbout){
				CreateAboutDialog(NULL, g_hInstance, m_nData.hIcon, g_NotePaths.CurrLanguagePath);
			}
			break;
		case IDM_HELP:		//show help
			GetSubPathW(m_sPathCHM, HELP_SUBPATH);
			if((int)ShellExecuteW(hwnd, L"open", m_sPathCHM, NULL, NULL, SW_SHOWNORMAL) < 32){
				GetSubPathW(m_sPathPDF, HELP_PDF_SUBPATH);
				if((int)ShellExecuteW(hwnd, L"open", m_sPathPDF, NULL, NULL, SW_SHOWNORMAL) < 32){
					if(GetPrivateProfileIntW(S_NOTE, IK_SHOW_MISSING_HELP, 1, g_NotePaths.INIFile) == 1){
						//show missing help dialog
						int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_MISSING_HELP), hwnd, HelpMissing_DlgProc, 0);
						if(result == IDOK){
							if(wcslen(g_DefBrowser) == 0)
								ShellExecuteW(hwnd, L"open", HELP_PAGE, NULL, NULL, SW_SHOWDEFAULT);
							else
								if((int)ShellExecuteW(hwnd, NULL, g_DefBrowser, HELP_PAGE, NULL, SW_SHOWDEFAULT) <= 32)
									ShellExecuteW(hwnd, L"open", HELP_PAGE, NULL, NULL, SW_SHOWDEFAULT);
						}
					}
					else{
						OpenPage(hwnd, HELP_PAGE);
					}
				}
			}
			break;
		case IDM_ON_THE_WEB:
			OpenPage(hwnd, HOME_PAGE);
			break;
		case IDM_PAYPAL:
			OpenPage(hwnd, PAYPAL_PAGE);	
			break;
		case IDM_SHOW_ALL_FAVORITES:
			ShowAllFavorites();
			break;
		// case IDM_SAVE:
		// case IDM_SEARCH_NOTE:
		// case IDM_REPLACE_NOTE:
			// EnumThreadWindows(GetCurrentThreadId(), EnumActiveNotesProc, MAKELPARAM(id, 0));
			// break;
		case IDM_SAVE_ALL:
			ApplySaveAllNotes();
			break;
		case IDM_DOCK_LEFT_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_LEFT, DOCK_LEFT));
			break;
		case IDM_DOCK_TOP_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_TOP, DOCK_TOP));
			break;
		case IDM_DOCK_RIGHT_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_RIGHT, DOCK_RIGHT));
			break;
		case IDM_DOCK_BOTTOM_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_BOTTOM, DOCK_BOTTOM));
			break;
		case IDM_DOCK_NONE_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_NONE, DOCK_NONE));
			break;
		case IDM_LOCK_PROGRAM:
			if(!g_LockedState){
				g_LockedState = true;
				if(g_hCPDialog != NULL)
					ShowWindow(g_hCPDialog, SW_HIDE);
				if(g_hSearchDialog != NULL)
					ShowWindow(g_hSearchDialog, SW_HIDE);
				if(g_hAdjust != NULL)
					ShowWindow(g_hAdjust, SW_HIDE);
				if(g_hFindDialog != NULL)
					ShowWindow(g_hFindDialog, SW_HIDE);
				if(g_hSearchTitlesDialog != NULL)
					ShowWindow(g_hSearchTitlesDialog, SW_HIDE);
				if(g_hOverdueDialog != NULL)
					ShowWindow(g_hOverdueDialog, SW_HIDE);
				if(g_hOptionsDlg != NULL)
					ShowWindow(g_hOptionsDlg, SW_HIDE);
				EnumThreadWindows(GetCurrentThreadId(), HideShowWhenLockedProc, SW_HIDE);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_TRAY)){
					//delete tray icon
					if(m_InTray){
						m_InTray = false;
						Shell_NotifyIconW(NIM_DELETE, &m_nData);
					}
				}
				g_UnlockFromMouse = false;
			}
			else{
				if(!g_UnlockFromMouse && g_hLoginDialog == NULL){
					if(IsPasswordSet()){
						if(ShowLoginDialog(g_hInstance, g_hMain, DLG_LOGIN_MAIN, NULL, LDT_MAIN, NULL) == IDCANCEL){
							break;
						}
					}
				}
				if(g_hLoginDialog != NULL){
					break;
				}
				g_UnlockFromMouse = false;
				g_LockedState = false;
				if(g_hCPDialog != NULL)
					ShowWindow(g_hCPDialog, SW_SHOW);
				if(g_hSearchDialog != NULL)
					ShowWindow(g_hSearchDialog, SW_SHOW);
				if(g_hAdjust != NULL)
					ShowWindow(g_hAdjust, SW_SHOW);
				if(g_hFindDialog != NULL)
					ShowWindow(g_hFindDialog, SW_SHOW);
				if(g_hSearchTitlesDialog != NULL)
					ShowWindow(g_hSearchTitlesDialog, SW_SHOW);
				if(g_hOverdueDialog != NULL)
					ShowWindow(g_hOverdueDialog, SW_SHOW);
				if(g_hOptionsDlg != NULL)
					ShowWindow(g_hOptionsDlg, SW_SHOW);
				EnumThreadWindows(GetCurrentThreadId(), HideShowWhenLockedProc, SW_SHOW);
				if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_TRAY))
					//add tray icon
					AddNotifyIcon();
			}
			break;
		default:
			if(hwndCtl == 0 && codeNotify == 0){
				if(id >= LANG_ADDITION){	//sent from the languages menu
					hTempMenu = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_LANGUAGES));
					if((GetMenuState(hTempMenu, id, MF_BYCOMMAND) & MF_CHECKED) != MF_CHECKED){
						PMITEM		pmi;

						for(pos = 0; pos < GetMenuItemCount(hTempMenu); pos++){
							if(GetMenuItemID(hTempMenu, pos) == id)
								break;
						}
						ZeroMemory(&mi, sizeof(mi));
						mi.cbSize = sizeof(mi);
						mi.fMask = MIIM_DATA;
						GetMenuItemInfoW(hTempMenu, pos, true, &mi);
						pmi = (PMITEM)mi.dwItemData;
						wcscpy(szTemp, pmi->szReserved);
						GetFileNameFromPath(szTemp, szLang);
						CheckMenuRadioItem(hTempMenu, 0, GetMenuItemCount(hTempMenu) - 1, pos, MF_BYPOSITION);
						wcscpy(g_NotePaths.LangFile, szLang);
						//construct current language file full path
						wcscpy(g_NotePaths.CurrLanguagePath, g_NotePaths.LangDir);
						wcscat(g_NotePaths.CurrLanguagePath, szLang);
						PrepareMessages();
						//get program name for tray icon tooltip
						GetPrivateProfileStringW(L"program", L"name", NULL, g_NotePaths.ProgName, 256, szTemp);
						wcscpy(m_nData.szTip, g_NotePaths.ProgName);
						m_nData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
						Shell_NotifyIconW(NIM_MODIFY, &m_nData);
						//refresh default group name
						RefreshDefaultGroupName();
						//re-arrange menu
						CreatePopUp();
						//change names of all diary notes
						ChangeAllDiaryNames(m_DiaryFormats);
						if(g_hOptionsDlg)
							//apply new language
							ApplyMainDlgLanguage(g_hOptionsDlg);
						if(g_hCPDialog){
							//apply new language
							SendMessageW(g_hCPDialog, PNM_CTRL_UPD_LANG, 0, 0);
							SendMessageW(g_hCPDialog, PNM_CTRL_DIARY_CHANGE_LANG, 0, 0);
						}
						if(m_hHotDlg)
							//apply new language
							ApplyHotDialogLanguage(m_hHotDlg);
						if(g_hSearchDialog)
							//apply new language
							SendMessageW(g_hSearchDialog, PNM_CTRL_UPD_LANG, 0, 0);
						if(g_hSearchTitlesDialog)
							//apply new language
							SendMessageW(g_hSearchTitlesDialog, PNM_CTRL_UPD_LANG, 0, 0);
						if(g_hSearchTags)
							//apply new language
							SendMessageW(g_hSearchTags, PNM_CTRL_UPD_LANG, 0, 0);
						if(g_hSearchDates)
							//apply new language
							SendMessageW(g_hSearchDates, PNM_CTRL_UPD_LANG, 0, 0);
						if(g_hOverdueDialog)
							//apply new language
							SendMessageW(g_hOverdueDialog, PNM_CTRL_UPD_LANG, 0, 0);
						if(g_hSearchEverywhereDialog)
							//apply new language
							SendMessageW(g_hSearchEverywhereDialog, PNM_CTRL_UPD_LANG, 0, 0);
						//update default contacts group
						UpdateDefContGroupsName();
						//store new language
						WritePrivateProfileStringW(L"lang", S_FILE, szLang, g_NotePaths.INIFile);
						//notify all notes about new language
						ApplyNewLanguage();
					}
				}
				else if(id >= SHOW_GROUP_ADDITION && id < (SHOW_GROUP_ADDITION + MAX_GROUP_COUNT)){
					if(IsAnyHiddenInGroup(id - SHOW_GROUP_ADDITION) && UnlockGroup(id - SHOW_GROUP_ADDITION))
						ShowGroupOfNotes(id - SHOW_GROUP_ADDITION);
				}
				else if(id >= HIDE_GROUP_ADDITION && id < (HIDE_GROUP_ADDITION + MAX_GROUP_COUNT)){
					HideGroupOfNotes(id - HIDE_GROUP_ADDITION);
				}
				else if(id >= FAVORITES_ADDITION && id < (FAVORITES_ADDITION + FAVORITES_MAX)){	//favorites menu
					ShowFavorite(id);
				}
				else if(id > DIARY_ADDITION && id <= (DIARY_ADDITION + MAX_DIARY_PAGES + 1)){	//diary menu
					ShowDiaryItem(id);
				}
				else if(id >= EXTERNALS_ADDITION && id < EXTERNALS_ADDITION + EXTERNALS_MAX){
					ExecuteExternal(id);
				}
				else if(id >= SHOW_BY_TAG_ADDITION && id < SHOW_BY_TAG_ADDITION + TAGS_MAX_MENU){
					hTempMenu = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SHOW_HIDE_ALL));
					GetMenuStringW(GetSubMenu(hTempMenu, GetMenuPosition(hTempMenu, IDM_SHOW_BY_TAG)), id, szBuffer, 255, MF_BYCOMMAND);
					ShowHideByTag(szBuffer, true);
				}
				else if(id >= HIDE_BY_TAG_ADDITION && id < HIDE_BY_TAG_ADDITION + TAGS_MAX_MENU){
					hTempMenu = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SHOW_HIDE_ALL));
					GetMenuStringW(GetSubMenu(hTempMenu, GetMenuPosition(hTempMenu, IDM_HIDE_BY_TAG)), id, szBuffer, 255, MF_BYCOMMAND);
					ShowHideByTag(szBuffer, false);
				}
			}
	}
}

static BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	wchar_t		szMessage[256];
	int			exitParam;
	HMODULE		hIcons;

	//save main window handle
	g_hMain = hwnd;
	//get process heap
	g_hHeap = GetProcessHeap();
	// //register for logon/logoff messages
	if(m_TrackLogon)
		RegisterLogonMessage(hwnd);
	//check whether visual style is enabled
	g_VSEnabled = IsVisualStyleEnabled();

	// //get data path
	// if(wcslen(g_NotePaths.DataDir) == 0)
		// GetSubPathW(g_NotePaths.DataDir, L"\\data\\");
	
	// //get data file path
	// if(wcslen(g_NotePaths.DataFile) == 0){
		// wcscpy(g_NotePaths.DataFile, g_NotePaths.DataDir);
		// wcscat(g_NotePaths.DataFile, FN_DATA_FILE);
	// }

	//check whether paths exist
	CheckAndCreatePath(g_NotePaths.INIFile);
	// CheckAndCreatePath(g_NotePaths.DataDir);
	
	//get skins path
	if(wcslen(g_NotePaths.SkinDir) == 0)
		GetSubPathW(g_NotePaths.SkinDir, L"\\skins\\");
	CheckAndCreatePath(g_NotePaths.SkinDir);
	//get fonts path
	if(wcslen(g_NotePaths.FontsPath) == 0)
		GetSubPathW(g_NotePaths.FontsPath, L"\\fonts\\");
	CheckAndCreatePath(g_NotePaths.FontsPath);
	//get language files directory
	if(wcslen(g_NotePaths.LangDir) == 0)
		GetSubPathW(g_NotePaths.LangDir, L"\\lang\\");
	CheckAndCreatePath(g_NotePaths.LangDir);
	//get sound files directory
	if(wcslen(g_NotePaths.SoundDir) == 0)
		GetSubPathW(g_NotePaths.SoundDir, L"\\sound\\");
	CheckAndCreatePath(g_NotePaths.SoundDir);
	//backup directory is created in WinMain because of command line options, so there is no need to check and create it again
	// //get backup directory
	// if(wcslen(g_NotePaths.BackupDir) == 0)
		// GetSubPathW(g_NotePaths.BackupDir, L"\\backup\\");
	// CheckAndCreatePath(g_NotePaths.BackupDir);
	//get icons file
	GetSubPathW(g_NotePaths.IconsPath, ICONS_FILE);

	//get diary format index
	g_DiaryFormatIndex = GetPrivateProfileIntW(S_FORMAT, IK_DF_INDEX, 0, g_NotePaths.INIFile);
	//get last time run
	ZeroMemory(&g_LastStartTimeProg, sizeof(g_LastStartTimeProg));
	GetPrivateProfileStructW(S_EXITFLAG, IK_LAST_START_TIME, &g_LastStartTimeProg, sizeof(SYSTEMTIME), g_NotePaths.INIFile);

	//get language file short name
	GetPrivateProfileStringW(L"lang", S_FILE, L"english.lng", g_NotePaths.LangFile, 256, g_NotePaths.INIFile);
	//construct current language file full path
	wcscpy(g_NotePaths.CurrLanguagePath, g_NotePaths.LangDir);
	wcscat(g_NotePaths.CurrLanguagePath, g_NotePaths.LangFile);
	//get def browser
	GetPrivateProfileStringW(L"def_browser", L"def_browser", NULL, g_DefBrowser, MAX_PATH, g_NotePaths.INIFile);
	//prepare messages
	PrepareMessages();
	//allocate memory for edit hotkeys
	g_EditHotkeys = (P_HK_TYPE)calloc(COUNT_EDIT_HOTKEYS, sizeof(HK_TYPE));
	//prepare edit hotkeys
	PrepareEditHotKeys();
	//allocate memory for note hotkeys
	g_NoteHotkeys = (P_HK_TYPE)calloc(NoteMenuCount(), sizeof(HK_TYPE));
	//prepare note hotkeys
	PrepareNoteHotKeys();
	////register note hot keys
	//RegisterHKeys(hwnd, g_NoteHotkeys, NoteMenuCount());
	//prepare hot keys
	PrepareHotKeys();
	//register hot keys
	RegisterHKeys(hwnd, m_Hotkeys, NELEMS(m_Hotkeys));
	//get program name for tray icon tooltip
	GetPrivateProfileStringW(L"program", L"name", DEF_PROG_TOOLTIP, g_NotePaths.ProgName, 256, g_NotePaths.CurrLanguagePath);
	//register note window class
	if(!RegisterNoteClass(g_hInstance)){
		GetPNotesMessage(L"no_note_registered", L"Unable to register note window class", szMessage);
		MessageBoxW(hwnd, szMessage, NULL, MB_OK);
		CleanUp(true);
		return false;
	}

	//create date-time format strings
	GetPrivateProfileStringW(S_FORMAT, L"date_format", DTS_DATE_FORMAT, g_DTFormats.DateFormat, 128, g_NotePaths.INIFile);
	GetPrivateProfileStringW(S_FORMAT, L"time_format", DTS_TIME_FORMAT, g_DTFormats.TimeFormat, 128, g_NotePaths.INIFile);
	
	//get sounds
	LoadAllSounds();
	//get sound information
	g_Sound.allowSound = GetPrivateProfileIntW(S_SOUND, DSK_ALLOW, 1, g_NotePaths.INIFile);
	// g_Sound.defSound = GetPrivateProfileIntW(S_SOUND, DSK_SOUND_DEF, 0, g_NotePaths.INIFile);
	GetPrivateProfileStringW(S_SOUND, DSK_SOUND_CUST, DS_DEF_SOUND, g_Sound.custSound, MAX_PATH, g_NotePaths.INIFile);

	//try version more/equal 4.0 settings
	if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), g_NotePaths.INIFile)){
		g_NoteSettings.newOnTop = false;
		//try version 3.0.0 settings
		if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - sizeof(BOOL) - sizeof(int) * 2, g_NotePaths.INIFile)){
			g_NoteSettings.autoSave = false;
			g_NoteSettings.autoSValue = 5;
			// g_NoteSettings.allOnTop = false;
			g_NoteSettings.visualAlert = true;
			g_NoteSettings.rollOnDblClick = true;
			//try version 2.5.0 settings
			if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - sizeof(BOOL) * 4 - sizeof(BYTE), g_NotePaths.INIFile)){
				g_NoteSettings.confirmDelete = true;
				//try version 2.0.0 settings
				if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - sizeof(BOOL), g_NotePaths.INIFile)){
					//try version 1.0.1 structure size
					if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - (sizeof(BOOL) * 2), g_NotePaths.INIFile)){
						// g_NoteSettings.showOnStart = true;
						g_NoteSettings.saveOnExit = true;
						g_NoteSettings.confirmSave = true;
					}
				}
			}
		}
	}
	//get next settings
	GetPrivateProfileStructW(S_NOTE, IK_NEXT_SETTINGS, &g_NextSettings, sizeof(NEXT_SETTINGS), g_NotePaths.INIFile);
	if(m_DisableNetwork)
		BitOff(&g_NextSettings.flags1, SB3_ENABLE_NETWORK);
	//get spell underlining color
	g_Spell.color = GetPrivateProfileIntW(S_NOTE, IK_SPELL_COLOR, SPELL_COLOR, g_NotePaths.INIFile);
	//get spell culture
	GetPrivateProfileStringW(S_NOTE, IK_SPELL_CULTURE, L"en_US", g_Spell.spellCulture, 16, g_NotePaths.INIFile);
	//get spell auto
	g_Spell.spellAuto = GetPrivateProfileIntW(S_NOTE, IK_SPELL_AUTO, 0, g_NotePaths.INIFile);
	g_Spell.isCheckLib = SpellerExists();
	if(g_Spell.isCheckLib){
		//check and create dictionaries directory
		CheckAndCreateDictionariesDirectory();
		//create dictionaries file if needed
		CheckAndCreateDictionariesFile();
		//load spelling staff
		LoadCodePages();
		LoadDictionaries();
		InitSpeller();
	}
	//get small values
	ZeroMemory(&g_SmallValues, sizeof(g_SmallValues));
	GetPrivateProfileStructW(S_NOTE, IK_SMALL_VALUES, &g_SmallValues, sizeof(g_SmallValues), g_NotePaths.INIFile);
	//set default bullets indent
	if(GetSmallValue(g_SmallValues, SMS_BULLET_INDENT) == 0)
		SetSmallValue(&g_SmallValues, SMS_BULLET_INDENT, 4);
	//set default small values
	if(GetSmallValue(g_SmallValues, SMS_BACKUP_COUNT) == 0)
		SetSmallValue(&g_SmallValues, SMS_BACKUP_COUNT, DEF_BACK_COUNT);
	//get default margin size
	if(GetSmallValue(g_SmallValues, SMS_MARGIN_SIZE) == 0)
		SetSmallValue(&g_SmallValues, SMS_MARGIN_SIZE, 4);
	//get default diary pages count
	if(GetSmallValue(g_SmallValues, SMS_DIARY_PAGES) == 0)
		SetSmallValue(&g_SmallValues, SMS_DIARY_PAGES, DEF_DIARY_PAGES);
	//set default save characters
	if(GetSmallValue(g_SmallValues, SMS_DEF_SAVE_CHARACTERS) == 0)
		SetSmallValue(&g_SmallValues, SMS_DEF_SAVE_CHARACTERS, DEF_SAVE_CHARACTERS);
	//set default content length
	if(GetSmallValue(g_SmallValues, SMS_CONTENT_LENGTH) == 0)
		SetSmallValue(&g_SmallValues, SMS_CONTENT_LENGTH, DEF_CONTENT_CHARACTERS);
	//check password
	if(IsPasswordSet()){
		if(ShowLoginDialog(g_hInstance, hwnd, DLG_LOGIN_MAIN, NULL, LDT_MAIN, NULL) == IDCANCEL){
			CleanUp(false);
			return false;
		}
	}
	//set default action of double click in case of no action is set before
	if(!IsBitOn(g_NoteSettings.reserved2, SB2_NEW_NOTE) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_CONTROL_PANEL) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_PREFS) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_NOTES)
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_LOAD_NOTE) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_FROM_CLIPBOARD) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_ALL_TO_FRONT) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_SAVE_ALL)
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_SHOW_HIDE)
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_TAGS)){
		BitOn(&g_NoteSettings.reserved2, SB2_NEW_NOTE);
	}

	//prepare popup menu
	CreatePopUp();
	CreateMenuFont();

	//register custom fonts
	if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_FONTS))
		RegisterCustomFonts();
	//prepare default appearance
	if(!CreateAppearance(hwnd)){
		CleanUp(false);
		return false;
	}
	//get exit flag
	exitParam = GetPrivateProfileIntW(S_EXITFLAG, DSK_CURRENT_STATE, 0, g_NotePaths.INIFile);
	if(exitParam != 0){
		if(IsAnyAutosavedExists()){
			if(MessageBoxW(hwnd, m_sUnsuccessfull, PROG_NAME, MB_YESNO | MB_ICONEXCLAMATION) == IDYES){
				LoadAutosaved();
			}
		}
	}
	DeleteAutosaved();
	//write exit state parameter ("1" on start, "0" on succesfull end)
	WritePrivateProfileStringW(S_EXITFLAG, DSK_CURRENT_STATE, L"1", g_NotePaths.INIFile);
	//load groups
	MakeDefaultGroup();
	//start autosave timer
	if(g_NoteSettings.autoSave)
		m_TimerAutosave = SetTimer(hwnd, TIMER_AUTOSAVE_ID, g_NoteSettings.autoSValue * 1000 * 60, AutosaveTimerProc);
	//get diary appearance
	CreateDiaryAppearance();
	//get docking settings
	GetPrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
	//prepare dock appearance
	// memcpy(&g_DRTHandles, &g_RTHandles, sizeof(NOTE_RTHANDLES));
	g_DRTHandles = g_RTHandles;
	if(g_DockSettings.fCustSkin){
		GetDockSkinProperties(hwnd, &g_DRTHandles, g_DockSettings.szCustSkin, false);
	}
	if(!g_DockSettings.fCustCaption){
		// memcpy(&g_DockSettings.lfCaption, &g_Appearance.lfCaption, sizeof(g_Appearance.lfCaption));
		g_DockSettings.lfCaption = g_Appearance.lfCaption;
		g_DockSettings.crCaption = g_Appearance.crCaption;
	}
	else{
		g_DRTHandles.hFCaption = CreateFontIndirectW(&g_DockSettings.lfCaption);
	}
	if(!g_DockSettings.fCustColor){
		g_DockSettings.crWindow = g_Appearance.crWindow;
	}
	//prepare docking headers
	InitDockHeaders();
	//get ftp data
	GetPrivateProfileStructW(S_NOTE, IK_FTP_DATA, &m_FTPData, sizeof(FTP_DATA), g_NotePaths.INIFile);
	if(m_FTPData.port == 0)
		m_FTPData.port = INTERNET_DEFAULT_FTP_PORT;
	//perform possible synchronization
	if(IsBitOn(g_NoteSettings.reserved1, SB1_SYNC_ON_START)){
		PROGRESS_STRUCT	ps;
		ps = PrepareSyncStruct(hwnd, 0);
		DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_PROGRESS), hwnd, Progress_DlgProc, (LPARAM)&ps);
	}
	//get all local syncs
	GetLocalSyncs();
	//pefrom possible local synchronization (USB drive <==> hard disk)
	if(GetDriveTypeW(NULL) == DRIVE_REMOVABLE){
		wchar_t		szCompName[MAX_COMPUTERNAME_LENGTH + 1];
		ULONG		uLen = sizeof(szCompName);
		GetComputerNameW(szCompName, &uLen);
		LPPLOCALSYNC	lpls = PLocalSyncItem(g_PLocalSyncs, szCompName);
		if(lpls){
			wchar_t		szPathId[MAX_PATH];
			if(wcslen(lpls->pathId) > 0)
				wcscpy(szPathId, lpls->pathId);
			else
				wcscpy(szPathId, lpls->pathNotes);
			wcscat(szPathId, L"\\notes.pnid");
			LocalSyncFunc(g_NotePaths.DataDir, g_NotePaths.DataFile, lpls->pathNotes, szPathId);
		}
	}
	//prepare groups
	GetAllGroups();
	//load existing notes
	GetAllNotes(true);
	//rearrange all dock notes
	RearrangeDockWindowsOnstart(DOCK_LEFT);
	RearrangeDockWindowsOnstart(DOCK_TOP);
	RearrangeDockWindowsOnstart(DOCK_RIGHT);
	RearrangeDockWindowsOnstart(DOCK_BOTTOM);
	//create data file if it does not exist
	CreateDataFile();
	//get bitmaps for menu
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		g_hBmpMenuNormal = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_MENU_ST));
		g_hBmpMenuGray = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_MENU_ST_GRAY));
		g_hBmpEditNormal = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_MENU_EDIT));
		g_hBmpEditGray = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_MENU_EDIT_GRAY));
		g_hBmpMisc = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_COMMON_MISC));
		//prepare image list for checkbox treeviews
		HBITMAP	hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_CHECKS));
		m_hImlTreeCheck = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(m_hImlTreeCheck, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		//prepare image list for hot keys dialog
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_HOT_ICONS));
		m_hImlHotKeys = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(m_hImlHotKeys, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		//create image lists
		PrepareScheduleImagelists(hIcons);
		//load control panel cursors
		g_CurV = LoadCursorW(hIcons, MAKEINTRESOURCEW(IDR_CUR_V_SPLIT));
		g_CurH = LoadCursorW(hIcons, MAKEINTRESOURCEW(IDR_CUR_H_SPLIT));
		FreeLibrary(hIcons);
	}
	//initialize empty notes structure
	g_Empties.pPoint = 0;
	g_Empties.pDockData = 0;
	g_Empties.count = 0;
	//prepare find flags
	g_Flags |= FR_DOWN;
	//initialize printing
	InitPrintMembers();
	//get last options dialog page
	m_CurrentOption = GetPrivateProfileIntW(S_NOTE, IK_LAST_OPTION, IDR_ICO_APPEARANCE, g_NotePaths.INIFile);
	//save last start time
	WritePrivateProfileStructW(S_EXITFLAG, IK_LAST_START_TIME, &g_StartTimeProg, sizeof(SYSTEMTIME), g_NotePaths.INIFile);
	//create favorites menu
	BuildFavoritesMenu();
	//load smilies DLL
	g_hSmilies = LoadLibraryEx("smilies.images", NULL, LOAD_LIBRARY_AS_DATAFILE);
	//get predefined tags
	GetPredefinedTags();
	//get show/hide index
	m_ShowHideIndex = GetPrivateProfileIntW(S_NOTE, IK_TOGGLE_SHOW_HIDE, 0, g_NotePaths.INIFile);
	//get all search providers
	GetSearchEngines();
	//get all external programs
	GetExternals();
	//get all contacts
	GetContacts();
	//get all contacts groups
	GetContGroups();
	//build external programs menu
	BuildExternalMenu(m_hPopUp);
	//load available voices
	LoadVoices();
	//get possible default voice
	GetPrivateProfileStringW(S_NOTE, IK_VOICE_DEFAULT, NULL, g_DefVoice, 128, g_NotePaths.INIFile);
	//start auto clean Recycle Bin timer
	if(g_NextSettings.cleanDays > 0)
		m_TimerCleanBin = SetTimer(hwnd, TIMER_CLEAN_BIN_ID, CLEAN_BIN_INTERVAL, CleanBinTimerProc);
	//enable menus
	EnableMainMenus();
	//create voice
	m_pVoice = CreateVoice();
	//exchange initialization
	g_wsDefPort = (USHORT)GetPrivateProfileIntW(S_NOTE, IK_DEF_SOCKET_PORT, SOCKET_DEF_PORT, g_NotePaths.INIFile);
	
	if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
		//initialize Winsock
		InitializeWinsock();
		//start listening
		StartWSServer(hwnd);
		GetHostProperties(g_wsLocalHost);
	}

	//create invisible rich edit windows
	int style = WS_CHILD | ES_MULTILINE;
	g_hTEditSimple = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, g_hMain, NULL, g_hInstance, NULL);
	g_hTEditRich = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, g_hMain, NULL, g_hInstance, NULL);
	g_hTEditContent = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, g_hMain, NULL, g_hInstance, NULL);
	SendMessageW(g_hTEditSimple, EM_SETTEXTMODE, TM_PLAINTEXT | TM_MULTILEVELUNDO | TM_MULTICODEPAGE, 0);
	SendMessageW(g_hTEditSimple, EM_LIMITTEXT, -1, 0);
	SendMessageW(g_hTEditRich, EM_LIMITTEXT, -1, 0);
	SendMessageW(g_hTEditContent, EM_LIMITTEXT, -1, 0);
	SetREFontCommon(g_hTEditRich, &g_Appearance.lfFont);

	//show control panel
	if(IsBitOn(g_NoteSettings.reserved1, SB1_SHOW_CP))
		SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_CTRL, 0), 0);
	return true;
}

static void GetExternals(void){
	int			ub = 1024, result, pos;
	wchar_t		*pExternals, *pTemp, szBuffer[MAX_PATH + 1024], szProgram[1024], szCommand[MAX_PATH];

	pExternals = calloc(ub, sizeof(wchar_t));
	if(pExternals){
		result = GetPrivateProfileStringW(S_EXTERNAL_PROGRAMS, NULL, NULL, pExternals, ub, g_NotePaths.INIFile);
		while(result == ub - 2){
			ub *= 2;
			free(pExternals);
			pExternals = calloc(ub, sizeof(wchar_t));
			if(pExternals){
				result = GetPrivateProfileStringW(S_EXTERNAL_PROGRAMS, NULL, NULL, pExternals, ub, g_NotePaths.INIFile);
			}
		}
		if(*pExternals){
			pTemp = pExternals;
			while(*pTemp){
				wmemset(szBuffer, '\0', MAX_PATH + 1024);
				GetPrivateProfileStringW(S_EXTERNAL_PROGRAMS, pTemp, NULL, szBuffer, 1024, g_NotePaths.INIFile);
				pos = wcscspn(szBuffer, L"\1");
				if(pos == 0 || pos >= wcslen(szBuffer))
					g_PExternalls = SEngsAdd(g_PExternalls, pTemp, szBuffer);
				else{
					wcsncpy(szProgram, szBuffer, pos);
					szProgram[pos] = '\0';
					wchar_t		*p = wcschr(szBuffer, '\1');
					p++;
					wcscpy(szCommand, p);
					g_PExternalls = SEngsAddWithCommandLine(g_PExternalls, pTemp, szProgram, szCommand);
				}
				pTemp += wcslen(pTemp) + 1;
			}
		}
		free(pExternals);
	}
}

static void UpdateDefContGroupsName(void){
	LPPCONTGROUP	lpg;

	lpg = PContGroupsItemById(g_PContGroups, 0);
	if(lpg){
		GetPrivateProfileStringW(S_CAPTIONS, L"no_cont_group", DS_NO_CONT_GROUP, lpg->name, 127, g_NotePaths.CurrLanguagePath);
	}
}

static void GetContGroups(void){
	int			ub = 1024, result;
	wchar_t		*pContacts, *pTemp;
	PCONTGROUP	cp = {0};

	//add (None) group
	GetPrivateProfileStringW(S_CAPTIONS, L"no_cont_group", DS_NO_CONT_GROUP, cp.name, 127, g_NotePaths.CurrLanguagePath);
	PContGroupsAdd(&g_PContGroups, &cp, false);
	//add other groups
	pContacts = calloc(ub, sizeof(wchar_t));
	if(pContacts){
		result = GetPrivateProfileStringW(S_CONT_GROUPS, NULL, NULL, pContacts, ub, g_NotePaths.INIFile);
		while(result == ub - 2){
			ub *= 2;
			free(pContacts);
			pContacts = calloc(ub, sizeof(wchar_t));
			if(pContacts){
				result = GetPrivateProfileStringW(S_CONT_GROUPS, NULL, NULL, pContacts, ub, g_NotePaths.INIFile);
			}
		}
		if(*pContacts){
			pTemp = pContacts;
			while(*pTemp){
				GetPrivateProfileStructW(S_CONT_GROUPS, pTemp, &cp, sizeof(PCONTGROUP), g_NotePaths.INIFile);
				PContGroupsAdd(&g_PContGroups, &cp, false);
				pTemp += wcslen(pTemp) + 1;
			}
		}
		free(pContacts);
	}
}

static void GetContacts(void){
	int			ub = 1024, result;
	wchar_t		*pContacts, *pTemp;
	PCONTPROP	cp = {0};

	pContacts = calloc(ub, sizeof(wchar_t));
	if(pContacts){
		result = GetPrivateProfileStringW(S_CONTACTS, NULL, NULL, pContacts, ub, g_NotePaths.INIFile);
		while(result == ub - 2){
			ub *= 2;
			free(pContacts);
			pContacts = calloc(ub, sizeof(wchar_t));
			if(pContacts){
				result = GetPrivateProfileStringW(S_CONTACTS, NULL, NULL, pContacts, ub, g_NotePaths.INIFile);
			}
		}
		if(*pContacts){
			pTemp = pContacts;
			while(*pTemp){
				if(!GetPrivateProfileStructW(S_CONTACTS, pTemp, &cp, sizeof(PCONTPROP), g_NotePaths.INIFile)){
					GetPrivateProfileStructW(S_CONTACTS, pTemp, &cp, sizeof(PCONTPROP) - sizeof(int), g_NotePaths.INIFile);
				}
				g_PContacts = PContactsAdd(g_PContacts, &cp);
				pTemp += wcslen(pTemp) + 1;
			}
		}
		free(pContacts);
	}
}

static void GetLocalSyncs(void){
	int			ub = 1024, result;
	wchar_t		*pSyncs, *pTemp, *w, *ptr, szBuffer[MAX_PATH * 2 + 2];

	pSyncs = calloc(ub, sizeof(wchar_t));
	if(pSyncs){
		result = GetPrivateProfileStringW(S_LOCAL_SYNCS, NULL, NULL, pSyncs, ub, g_NotePaths.INIFile);
		while(result == ub - 2){
			ub *= 2;
			free(pSyncs);
			pSyncs = calloc(ub, sizeof(wchar_t));
			if(pSyncs){
				result = GetPrivateProfileStringW(S_LOCAL_SYNCS, NULL, NULL, pSyncs, ub, g_NotePaths.INIFile);
			}
		}
		if(*pSyncs){
			pTemp = pSyncs;
			while(*pTemp){
				GetPrivateProfileStringW(S_LOCAL_SYNCS, pTemp, NULL, szBuffer, MAX_PATH * 2 + 1, g_NotePaths.INIFile);
				w = wcstok(szBuffer, L"|", &ptr);
				g_PLocalSyncs = PLocalSyncAdd(&g_PLocalSyncs, pTemp, w, ptr);
				pTemp += wcslen(pTemp) + 1;
			}
		}
		free(pSyncs);
	}
}

static void GetSearchEngines(void){
	int			ub = 1024, result;
	wchar_t		*pSengs, *pTemp, szQuery[1024];

	pSengs = calloc(ub, sizeof(wchar_t));
	if(pSengs){
		result = GetPrivateProfileStringW(S_SEARCH_ENGINES, NULL, NULL, pSengs, ub, g_NotePaths.INIFile);
		while(result == ub - 2){
			ub *= 2;
			free(pSengs);
			pSengs = calloc(ub, sizeof(wchar_t));
			if(pSengs){
				result = GetPrivateProfileStringW(S_SEARCH_ENGINES, NULL, NULL, pSengs, ub, g_NotePaths.INIFile);
			}
		}
		if(*pSengs){
			pTemp = pSengs;
			while(*pTemp){
				GetPrivateProfileStringW(S_SEARCH_ENGINES, pTemp, NULL, szQuery, 1024, g_NotePaths.INIFile);
				g_PSengs = SEngsAdd(g_PSengs, pTemp, szQuery);
				pTemp += wcslen(pTemp) + 1;
			}
		}
		free(pSengs);
	}
}

static void GetPredefinedTags(void){
	int			ub = 1024, result;
	wchar_t		*pTags, *tag, *w, *ptr;

	pTags = calloc(ub, sizeof(wchar_t));
	if(pTags){
		result = GetPrivateProfileStringW(S_PRE_TAGS, IK_TAGS, NULL, pTags, ub, g_NotePaths.INIFile);
		while(result == ub - 1){
			ub *= 2;
			free(pTags);
			pTags = calloc(ub, sizeof(wchar_t));
			if(pTags){
				result = GetPrivateProfileStringW(S_PRE_TAGS, IK_TAGS, NULL, pTags, ub, g_NotePaths.INIFile);
			}
		}
		if(*pTags){
			w = wcstok(pTags, L",", &ptr);
			while(w){
				tag = _wcsdup(w);
				_wcstrm(tag);
				if(*tag){
					g_PTagsPredefined = TagsAdd(g_PTagsPredefined, tag, NULL);
				}
				free(tag);
				w = wcstok(NULL, L",", &ptr);
			}
		}
		free(pTags);
	}
}

static void CheckOverdueReminders(void){
	PMEMNOTE		* parr = NULL, pTemp = MemoryNotes();
	int				count = 0, days;
	BOOL			fFound;
	SYSTEMTIME		st, tStart, tLast, tDate;
	FILETIME		ftNow, ftRem, ftLast, f2;

	if(m_InCheckOverdue)
		return;

	m_InCheckOverdue = true;

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ftNow);

	while(pTemp){
		fFound = false;
		//check notes with schedule type, not from recycle bin and those should be tracked
		if(pTemp->pSchedule->scType != SCH_NO && pTemp->pData->idGroup != GROUP_RECYCLE && 
			!IsBitOn(pTemp->pData->res1, NB_NOT_TRACK)){
			switch(pTemp->pSchedule->scType){
				case SCH_ONCE:
					SystemTimeToFileTime(&pTemp->pSchedule->scDate, &ftRem);
					if(CompareFileTime(&ftRem, &ftNow) < 0)
						fFound = true;
					break;
				case SCH_ED:
					tStart = pTemp->pSchedule->scStart;
					tDate = pTemp->pSchedule->scDate;
					SystemTimeToFileTime(&pTemp->pSchedule->scLastRun, &ftLast);
					if(IsDateEmpty(&pTemp->pSchedule->scLastRun)){
						if(st.wMonth > tStart.wMonth || st.wDay > tStart.wDay 
						|| st.wHour > tStart.wHour || (st.wHour == tStart.wHour && st.wMinute > tStart.wMinute)){
							fFound = true;
						}
					}
					else if(CompareFileTime(&ftLast, &ftNow) < 0){
						if(DateDiff(st, pTemp->pSchedule->scLastRun, DAY) > 0){
							if(st.wHour > tDate.wHour || (st.wHour == tDate.wHour && st.wMinute > tDate.wMinute)){
								fFound = true;
							}
						}
					}
					break;
				case SCH_REP:
					tStart = pTemp->pSchedule->scStart;
					tLast = pTemp->pSchedule->scLastRun;
					tDate = pTemp->pSchedule->scDate;
					SystemTimeToFileTime(&pTemp->pSchedule->scStart, &f2);
					if(CompareFileTime(&ftNow, &f2) < 0)
						break;
					if((st.wYear > tStart.wYear) || (st.wYear == tStart.wYear && st.wMonth > tStart.wMonth)
					|| (st.wYear == tStart.wYear && st.wMonth == tStart.wMonth && st.wDay > tStart.wDay)
					|| (st.wYear == tStart.wYear && st.wMonth == tStart.wMonth && st.wDay == tStart.wDay && st.wHour > tStart.wHour)
					|| (st.wYear == tStart.wYear && st.wMonth == tStart.wMonth && st.wDay == tStart.wDay && st.wHour == tStart.wHour && st.wMinute > tStart.wMinute)){
						SystemTimeToFileTime(&tLast, &ftLast);
						if(!IsDateEmpty(&tLast) && CompareFileTime(&ftLast, &ftNow) < 0){
							if(DateDiff(st, tLast, YEAR) > tDate.wYear)
								fFound = true;
							else if((DateDiff(st, tLast, YEAR) == tDate.wYear)
							&& (DateDiff(st, tLast, MONTH) > tDate.wMonth))
								fFound == true;
							else if((DateDiff(st, tLast, YEAR) == tDate.wYear)
							&& (DateDiff(st, tLast, MONTH) == tDate.wMonth)
							&& (DateDiff(st, tLast, WEEK) > tDate.wDayOfWeek))
								fFound = true;
							else if((DateDiff(st, tLast, YEAR) == tDate.wYear)
							&& (DateDiff(st, tLast, MONTH) == tDate.wMonth)
							&& (DateDiff(st, tLast, WEEK) == tDate.wDayOfWeek)
							&& DateDiff(st, tLast, DAY) > tDate.wDay)
								fFound = true;
							else if((DateDiff(st, tLast, YEAR) == tDate.wYear)
							&& (DateDiff(st, tLast, MONTH) == tDate.wMonth)
							&& (DateDiff(st, tLast, WEEK) == tDate.wDayOfWeek)
							&& (DateDiff(st, tLast, DAY) == tDate.wDay) 
							&& (DateDiff(st, tLast, HOUR) > tDate.wHour))
								fFound = true;
							else if((DateDiff(st, tLast, YEAR) == tDate.wYear)
							&& (DateDiff(st, tLast, MONTH) == tDate.wMonth)
							&& (DateDiff(st, tLast, WEEK) == tDate.wDayOfWeek)
							&& (DateDiff(st, tLast, DAY) == tDate.wDay) 
							&& (DateDiff(st, tLast, HOUR) == tDate.wHour)
							&& (DateDiff(st, tLast, MINUTE) > tDate.wMinute))
								fFound = true;
							else if((DateDiff(st, tLast, YEAR) == tDate.wYear)
							&& (DateDiff(st, tLast, MONTH) == tDate.wMonth)
							&& (DateDiff(st, tLast, WEEK) == tDate.wDayOfWeek)
							&& (DateDiff(st, tLast, DAY) == tDate.wDay) 
							&& (DateDiff(st, tLast, HOUR) == tDate.wHour)
							&& (DateDiff(st, tLast, MINUTE) == tDate.wMinute)
							&& (DateDiff(st, tLast, SECOND) > tDate.wSecond))
								fFound = true;
						}
					}
					break;
				case SCH_AFTER:
					tStart = pTemp->pSchedule->scStart;
					tDate = pTemp->pSchedule->scDate;
					SystemTimeToFileTime(&pTemp->pSchedule->scStart, &f2);
					if(CompareFileTime(&ftNow, &f2) < 0)
						break;
					if(DateDiff(st, tStart, YEAR) > tDate.wYear)
						fFound = true;
					else if((DateDiff(st, tStart, YEAR) == tDate.wYear) 
					&& (DateDiff(st, tStart, MONTH) > tDate.wMonth))
						fFound = true;
					else if((DateDiff(st, tStart, YEAR) == tDate.wYear) 
					&& (DateDiff(st, tStart, MONTH) == tDate.wMonth)
					&& (DateDiff(st, tStart, WEEK) > tDate.wDayOfWeek))
						fFound = true;
					else if((DateDiff(st, tStart, YEAR) == tDate.wYear) 
					&& (DateDiff(st, tStart, MONTH) == tDate.wMonth)
					&& (DateDiff(st, tStart, WEEK) == tDate.wDayOfWeek)
					&& (DateDiff(st, tStart, DAY) > tDate.wDay))
						fFound = true;
					else if((DateDiff(st, tStart, YEAR) == tDate.wYear) 
					&& (DateDiff(st, tStart, MONTH) == tDate.wMonth)
					&& (DateDiff(st, tStart, WEEK) == tDate.wDayOfWeek)
					&& (DateDiff(st, tStart, DAY) == tDate.wDay)
					&& (DateDiff(st, tStart, HOUR) > tDate.wHour))
						fFound = true;
					else if((DateDiff(st, tStart, YEAR) == tDate.wYear) 
					&& (DateDiff(st, tStart, MONTH) == tDate.wMonth)
					&& (DateDiff(st, tStart, WEEK) == tDate.wDayOfWeek)
					&& (DateDiff(st, tStart, DAY) == tDate.wDay)
					&& (DateDiff(st, tStart, HOUR) == tDate.wHour)
					&& (DateDiff(st, tStart, MINUTE) > tDate.wMinute))
						fFound = true;
					else if((DateDiff(st, tStart, YEAR) == tDate.wYear) 
					&& (DateDiff(st, tStart, MONTH) == tDate.wMonth)
					&& (DateDiff(st, tStart, WEEK) == tDate.wDayOfWeek)
					&& (DateDiff(st, tStart, DAY) == tDate.wDay)
					&& (DateDiff(st, tStart, HOUR) == tDate.wHour)
					&& (DateDiff(st, tStart, MINUTE) == tDate.wMinute)
					&& (DateDiff(st, tStart, SECOND) > tDate.wSecond))
						fFound = true;
					break;
				case SCH_PER_WD:
					days = DaysBetweenAlerts(pTemp->pSchedule->scLastRun, st, pTemp->pSchedule->scDate);
					for(int i = 0; i <=6; i++){
						if(IsBitOn(days, i)){
							int dof = RealDayOfWeek(i);
							if(IsBitOn(pTemp->pSchedule->scDate.wDayOfWeek, dof + 1)){
								fFound = true;
								break;
							}
						}
					}
					break;
				case SCH_MONTHLY_EXACT:
					tLast = pTemp->pSchedule->scLastRun;
					tDate = pTemp->pSchedule->scDate;
					if(tLast.wMonth != st.wMonth){
						//not raised yet
						if(st.wDay > tDate.wDay){
							fFound = true;
						}
						else if(tDate.wDay <= 28 
						&& st.wDay == tDate.wDay 
						&& ((st.wHour > tDate.wHour) 
							|| (st.wHour == tDate.wHour 
								&& st.wMinute > tDate.wMinute))){
							fFound = true;
						}
						else if(tDate.wDay == 29){
							switch(st.wMonth){
								case 2:
									if(IsLeapYear(st.wYear)){
										if(st.wDay == tDate.wDay 
										&& ((st.wHour > tDate.wHour) 
											|| (st.wHour == tDate.wHour 
												&& st.wMinute > tDate.wMinute))){
											fFound = true;
										}
									}
									else if(st.wDay == 28){
										if(st.wHour > tDate.wHour 
											|| (st.wHour == tDate.wHour 
												&& st.wMinute > tDate.wMinute)){
											fFound = true;
										}
									}
									break;
								default:
									if(st.wDay == tDate.wDay 
									&& ((st.wHour > tDate.wHour) 
										|| (st.wHour == tDate.wHour 
											&& st.wMinute > tDate.wMinute))){
										fFound = true;
									}
									break;
							}
						} 
						else if(tDate.wDay == 30){
							switch(st.wMonth){
								case 2:
									if((IsLeapYear(st.wYear) && st.wDay == 29) || (!IsLeapYear(st.wYear) && st.wDay == 28)){
										if(st.wHour > tDate.wHour 
											|| (st.wHour == tDate.wHour 
												&& st.wMinute > tDate.wMinute)){
											fFound = true;
										}
									}
									break;
								default:
									if(st.wDay == tDate.wDay 
									&& ((st.wHour > tDate.wHour) 
										|| (st.wHour == tDate.wHour 
											&& st.wMinute > tDate.wMinute))){
										fFound = true;
									}
									break;
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
									&& ((st.wHour > tDate.wHour) 
										|| (st.wHour == tDate.wHour 
											&& st.wMinute > tDate.wMinute))){
										fFound = true;
									}
									break;
								case 4:
								case 6:
								case 9:
								case 11:
									if(st.wDay == 30 
									&& ((st.wHour > tDate.wHour) 
										|| (st.wHour == tDate.wHour 
											&& st.wMinute > tDate.wMinute))){
										fFound = true;
									}
									break;
								case 2:
									if((IsLeapYear(st.wYear) && st.wDay == 29) || (!IsLeapYear(st.wYear) && st.wDay == 28)){
										if(st.wHour > tDate.wHour 
											|| (st.wHour == tDate.wHour 
												&& st.wMinute > tDate.wMinute)){
											fFound = true;
										}
									}
									break;
							}
						}
					}
					break;
				case SCH_MONTHLY_RELATIVE:
					tLast = pTemp->pSchedule->scLastRun;
					tDate = pTemp->pSchedule->scDate;
					if(tLast.wMonth != st.wMonth){
						if(tDate.wMilliseconds < DayOfWeekOrdinal(st)){
							fFound = true;
						}
						else if(tDate.wMilliseconds == DayOfWeekOrdinal(st) 
						&& OrdinalDayOfWeek(tDate.wDayOfWeek) < RealDayOfWeek(st.wDayOfWeek)){
							fFound = true;
						}
						else if(tDate.wMilliseconds == DayOfWeekOrdinal(st) 
						&& OrdinalDayOfWeek(tDate.wDayOfWeek) == RealDayOfWeek(st.wDayOfWeek) 
						&& (st.wHour > tDate.wHour 
							|| (st.wHour == tDate.wHour 
								&& st.wMinute > tDate.wMinute))){
							fFound = true;
						}
					}
					break;
			}
		}
		if(fFound){
			if(count == 0)
				parr = malloc(sizeof(PMEMNOTE));
			else
				parr = realloc(parr, sizeof(PMEMNOTE) * (count + 1));
			parr[count] = pTemp;
			count++;
		}
		pTemp = pTemp->next;
	}
	if(count > 0){
		g_OverdueCount = count;
		DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_OVERDUE), g_hMain, Overdue_DlgProc, (LPARAM)parr);
		free(parr);
	}
	m_InCheckOverdue = false;
}

static int DaysBetweenAlerts(const SYSTEMTIME stLast, const SYSTEMTIME stNow, const SYSTEMTIME stDate){
	int		days = 0, day, from, to;
	
	if(stNow.wDayOfWeek > stLast.wDayOfWeek){
		if(stLast.wHour < stDate.wHour || (stLast.wHour == stDate.wHour && stLast.wMinute < stDate.wMinute)){
			from = stLast.wDayOfWeek;
		}
		else{
			from = stLast.wDayOfWeek + 1;
		}
		if(stNow.wHour > stDate.wHour || (stNow.wHour == stDate.wHour && stNow.wMinute > stDate.wMinute)){
			to = stNow.wDayOfWeek;
		}
		else{
			to = stNow.wDayOfWeek - 1;
		}
		for(day = from; day <= to; day++){
			BitOn(&days, day);
		}
	}
	else if(stNow.wDayOfWeek < stLast.wDayOfWeek){
		if(stLast.wHour < stDate.wHour || (stLast.wHour == stDate.wHour && stLast.wMinute < stDate.wMinute)){
			from = stLast.wDayOfWeek;
		}
		else{
			from = stLast.wDayOfWeek + 1;
		}
		for(day = from; day <= 6; day++){
			BitOn(&days, day);
		}
		if(stNow.wHour > stDate.wHour || (stNow.wHour == stDate.wHour && stNow.wMinute > stDate.wMinute)){
			to = stNow.wDayOfWeek;
		}
		else{
			to = stNow.wDayOfWeek - 1;
		}
		for(day = 0; day <= from; day++){
			BitOn(&days, day);
		}
	}
	else{
		if((stLast.wHour < stDate.wHour || (stLast.wHour == stDate.wHour && stLast.wMinute < stDate.wMinute)) && (stNow.wHour > stDate.wHour || (stNow.wHour == stDate.wHour && stNow.wMinute > stDate.wMinute))){
			BitOn(&days, stNow.wDayOfWeek);
		}
	}
	return days;
}

static void InitDockHeaders(void){
	g_DHeaders.pLeft = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
	g_DHeaders.pTop = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
	g_DHeaders.pRight = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
	g_DHeaders.pBottom = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
}

static void FreeDockHeaders(void){
	FreeDockList(g_DHeaders.pLeft);
	FreeDockList(g_DHeaders.pTop);
	FreeDockList(g_DHeaders.pRight);
	FreeDockList(g_DHeaders.pBottom);
}

static void MakeDefaultGroup(void){
	PNGROUP				pg = {0};
	int					result = 0;

	result = GetPrivateProfileStructW(S_GROUPS, L"0", &pg, sizeof(pg), g_NotePaths.INIFile);
	if(!result){
		result = GetPrivateProfileStructW(S_GROUPS, L"0", &pg, sizeof(pg) - sizeof(COLORREF) * 3 - sizeof(BOOL) * 4 - sizeof(wchar_t) * 64, g_NotePaths.INIFile);
	}
	if(!result){
		result = GetPrivateProfileStructW(S_GROUPS, L"0", &pg, sizeof(pg) - sizeof(COLORREF) * 3 - sizeof(BOOL) * 4 - sizeof(wchar_t) * 64 - sizeof(wchar_t) * 256, g_NotePaths.INIFile);
	}
	if(!result){
		result = GetPrivateProfileStructW(S_GROUPS, L"0", &pg, sizeof(pg) - sizeof(COLORREF) * 3 - sizeof(BOOL) * 4 - sizeof(wchar_t) * 64 - sizeof(HK_TYPE) * 2 - sizeof(wchar_t) * 256, g_NotePaths.INIFile);
	}

	if(!result){
		pg.parent = -1;
		GetPrivateProfileStringW(S_CAPTIONS, L"group_default", DS_GENERAL_GROUP_NAME, pg.szName, 128, g_NotePaths.CurrLanguagePath);
		WritePrivateProfileStructW(S_GROUPS, L"0", &pg, sizeof(pg), g_NotePaths.INIFile);
	}
}

static void RefreshDefaultGroupName(void){
	PNGROUP		png, *ppg;

	ZeroMemory(&png, sizeof(png));
	if(!GetPrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png), g_NotePaths.INIFile)){
		if(!GetPrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png) - sizeof(HK_TYPE) * 2, g_NotePaths.INIFile)){
			png.parent = -1;
		}
	}
	GetPrivateProfileStringW(S_CAPTIONS, L"group_default", DS_GENERAL_GROUP_NAME, png.szName, 128, g_NotePaths.CurrLanguagePath);
	WritePrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png), g_NotePaths.INIFile);
	ppg = PNGroupsGroupById(g_PGroups, png.id);
	if(ppg){
		wcscpy(ppg->szName, png.szName);
	}
}

static void GetFileNameFromPath(wchar_t * lpPath, wchar_t * lpName){
	while(*lpPath++)
		;
	while(*lpPath != '\\')
		*lpPath--;
	lpPath++;
	wcscpy(lpName, lpPath);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateDataFile
 Created  : Mon May 14 12:24:55 2007
 Modified : Mon May 14 12:24:55 2007

 Synopsys : Creates file for notes if it does not exist. Adds BOM bytes to 
            the beginning of file, since it must be Unicode
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreateDataFile(void){
	HANDLE		hFile;
	wchar_t		szTemp[MAX_PATH];

	wcscpy(szTemp, g_NotePaths.DataDir);
	if(!NotesDirExists(szTemp))
		NotesDirCreate(szTemp);
	if(!FileExistsByFullPath(g_NotePaths.DataFile)){
		hFile = CreateFileW(g_NotePaths.DataFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			CloseHandle(hFile);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateAppearance
 Created  : Mon May 14 12:26:19 2007
 Modified : Mon May 14 12:26:19 2007

 Synopsys : Loads skin, font and text color for general notes settings
 Input    : Main window handle
 Output   : true on success, false if skin file does not exist
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL CreateAppearance(HWND hwnd){

	//get skin file name
	GetPrivateProfileStringW(S_APPEARANCE, S_SKIN, NULL, g_Appearance.szSkin, 64, g_NotePaths.INIFile);

	//get font data
	if(!GetPrivateProfileStructW(S_APPEARANCE, S_FONT, &g_Appearance.lfFont, sizeof(g_Appearance.lfFont), g_NotePaths.INIFile)){
		//prepare default font
		CreateDefaultFont(&g_Appearance.lfFont, false);
	}
	g_Appearance.crFont = GetPrivateProfileIntW(S_APPEARANCE, S_COLOR, GetSysColor(COLOR_WINDOWTEXT), g_NotePaths.INIFile);
	if(g_Appearance.crFont == 0)
		g_Appearance.crFont = DEF_TEXT_COLOR;
	g_RTHandles.hFont = CreateFontIndirectW(&g_Appearance.lfFont);

	//get window caption font data
	if(!GetPrivateProfileStructW(S_APPEARANCE, S_CAPTION_FONT, &g_Appearance.lfCaption, sizeof(g_Appearance.lfCaption), g_NotePaths.INIFile)){
		//prepare default font
		CreateDefaultFont(&g_Appearance.lfCaption, true);
	}
	g_Appearance.crCaption = GetPrivateProfileIntW(S_APPEARANCE, S_CAPTION_COLOR, GetSysColor(COLOR_WINDOWTEXT), g_NotePaths.INIFile);
	if(g_Appearance.crCaption == 0)
		g_Appearance.crCaption = DEF_TEXT_COLOR;
	g_RTHandles.hFCaption = CreateFontIndirectW(&g_Appearance.lfCaption);
	//get window background color
	g_Appearance.crWindow = GetPrivateProfileIntW(S_APPEARANCE, S_BCKG_COLOR, DEF_COLOR, g_NotePaths.INIFile);

	//Load skin
	GetSkinProperties(hwnd, &g_RTHandles, g_Appearance.szSkin, true);

	//get default skinless note size
	g_RTHandles.szDef.cx = GetPrivateProfileIntW(S_DEFSIZE, L"w", DEF_FLAT_WIDTH, g_NotePaths.INIFile);
	g_RTHandles.szDef.cy = GetPrivateProfileIntW(S_DEFSIZE, L"h", DEF_FLAT_HEIGHT, g_NotePaths.INIFile);
	return true;
}

static void CreateDiaryAppearance(void){
	GetPrivateProfileStructW(S_DIARY, IK_DIARY_APPEARANCE, &g_DiaryAppearance, sizeof(NOTE_APPEARANCE), g_NotePaths.INIFile);
	if(!IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
		g_DiaryAppearance.crCaption = g_Appearance.crCaption;
		g_DiaryAppearance.crWindow = g_Appearance.crWindow;
		// memcpy(&g_DiaryAppearance.lfCaption, &g_Appearance.lfCaption, sizeof(LOGFONTW));
		g_DiaryAppearance.lfCaption = g_Appearance.lfCaption;
		wcscpy(g_DiaryAppearance.szSkin, g_Appearance.szSkin);
		if(wcscmp(g_DiaryAppearance.szSkin, DS_NO_SKIN) == 0)
			*g_DiaryAppearance.szSkin = '\0';
	}
	//always get common font
	// memcpy(&g_DiaryAppearance.lfFont, &g_Appearance.lfFont, sizeof(LOGFONTW));
	g_DiaryAppearance.lfFont = g_Appearance.lfFont;
	if(wcslen(g_DiaryAppearance.szSkin) > 0)
		GetSkinProperties(g_hMain, &g_DiaryRTHandles, g_DiaryAppearance.szSkin, false);
	g_DiaryRTHandles.hFCaption = CreateFontIndirectW(&g_DiaryAppearance.lfCaption);
	g_DiaryRTHandles.hFont = CreateFontIndirectW(&g_DiaryAppearance.lfFont);
	//always get common size
	g_DiaryRTHandles.szDef = g_RTHandles.szDef;
	GetDiarySkin(g_DiaryAppearance, &g_DiaryRTHandles);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RestoreDefaultSettings
 Created  : Thu Aug  9 17:36:43 2007
 Modified : Thu Aug  9 17:36:43 2007

 Synopsys : Restores default program settings (except language)
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RestoreDefaultSettings(void){

	int			index;
	wchar_t		szSkin[64];

	//-----------	appearance dialog	-----------
	//set default transparency
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apTransparency], false);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_LEFT_ST), false);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_RIGHT_ST), false);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_TRACK_BAR), false);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_GRP_TRANS), false);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_WARNING), false);
	m_TempNoteSettings.transAllow = false;
	m_TempNoteSettings.transValue = 255;
	SetTransValue(m_Dialogs.hAppearance, 0, &m_TempNoteSettings.transValue);
	SendDlgItemMessageW(m_Dialogs.hAppearance, IDC_TRANS_TRACK_BAR, TBM_SETPOS, true, 0);
	//set default font
	CreateDefaultFont(&m_TempAppearance.lfFont, false);
	if(g_RTHandles.hFont != m_TempRHandles.hFont)
		DeleteFont(m_TempRHandles.hFont);
	m_TempRHandles.hFont = CreateFontIndirectW(&m_TempAppearance.lfFont);
	m_TempAppearance.crFont = DEF_TEXT_COLOR;
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apShowScroll], false);
	m_TempNoteSettings.showScrollbar = false;
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apHideToolbar], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apHideDelete], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_DELETE);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], false);
	CTreeView_SetEnable(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_CROSS_INST_TRNGL);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apHideHide], false);
	BitOff(&m_TempNextSettings.flags1, SB3_HIDE_HIDE);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apCustomFonts], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_CUST_FONTS);
	SetSmallValue(&m_TempSmallValues, SMS_BULLET_INDENT, 4);
	SendDlgItemMessageW(m_Dialogs.hAppearance, IDC_CBO_BINDENT, CB_SETCURSEL, 3, 0);
	SetSmallValue(&m_TempSmallValues, SMS_MARGIN_SIZE, 4);
	SendDlgItemMessageW(m_Dialogs.hAppearance, IDC_CBO_MARGIN, CB_SETCURSEL, 4, 0);
	m_TempSpellColor = SPELL_COLOR;
	SendMessageW(GetDlgItem(m_Dialogs.hAppearance, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TempSpellColor);

	//-----------	skins dialog	-----------
	//select default skin (none)
	SendDlgItemMessageW(m_Dialogs.hSkins, IDC_LST_SKIN, LB_SETCURSEL, 0, 0);
	SendMessageW(m_Dialogs.hSkins, WM_COMMAND, MAKEWPARAM(IDC_LST_SKIN, LBN_SELCHANGE), (LPARAM)GetDlgItem(m_Dialogs.hAppearance, IDC_LST_SKIN));	
	//restore default skinless settings
	m_TempAppearance.crWindow = DEF_COLOR;
	SendMessageW(GetDlgItem(m_Dialogs.hSkins, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TempAppearance.crWindow);
	CreateDefaultFont(&m_TempAppearance.lfCaption, true); 
	if(g_RTHandles.hFCaption != m_TempRHandles.hFCaption)
		DeleteFont(m_TempRHandles.hFCaption);
	m_TempRHandles.hFCaption = CreateFontIndirectW(&m_TempAppearance.lfCaption);
	m_TempAppearance.crCaption = DEF_TEXT_COLOR;
	m_TempRHandles.szDef.cx = DEF_FLAT_WIDTH;
	m_TempRHandles.szDef.cy = DEF_FLAT_HEIGHT;
	SetDlgItemInt(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_W, DEF_FLAT_WIDTH, false);
	SetDlgItemInt(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_H, DEF_FLAT_HEIGHT, false);
	//set default random color property
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skRandomColor], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_RANDCOLOR);
	//set default inverting text color
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skInvertColor], false);
	CTreeView_SetEnable(m_hTreeSkins, m_ChecksTSkins[skInvertColor], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_INVERT_TEXT);
	//set default roll/unroll property
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skRollUnroll], true);
	m_TempNoteSettings.rollOnDblClick = true;
	//set default fit-to-caption
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skFittToCaption], false);
	CTreeView_SetEnable(m_hTreeSkins, m_ChecksTSkins[skFittToCaption], true);
	BitOff(&m_TempNoteSettings.reserved1, SB1_FIT_TO_CAPTION);
	//enable skinless properties
	EnableSkinlessProperties(true);

	//-----------	docking dialog	-----------
	m_TempDockingSettings.fInvOrder = false;
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcInverseOrder], false);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustomSkins], false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_LST_SKIN), false);
	wcscpy(szSkin, g_Appearance.szSkin);
	szSkin[wcslen(szSkin) - 4] = '\0';
	index = SendDlgItemMessageW(m_Dialogs.hDocks, IDC_LST_SKIN, LB_FINDSTRING, -1, (LPARAM)szSkin);
	SendDlgItemMessageW(m_Dialogs.hDocks, IDC_LST_SKIN, LB_SETCURSEL, index, 0);
	m_TempDockingSettings.fCustSkin = false;
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustSize], false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_W), false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_H), false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W), false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H), false);
	
	m_TempDockingSettings.fCustSize = false;
	if(m_TempRHandles.hbSkin){
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W, DEF_FLAT_WIDTH, false);
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H, DEF_FLAT_HEIGHT, false);
	}
	else{
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx, false);
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy, false);
	}
	SendDlgItemMessageW(m_Dialogs.hDocks, IDC_DOCK_SLIDER, TBM_SETPOS, true, 0);
	m_TempDockingSettings.dist = 0;
	m_TempDockingSettings.fCustCaption = false;
	m_TempDockingSettings.fCustColor = false;
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustColor], false);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustFont], false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CMD_SKNLESS_CFONT), false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CHOOSE_COLOR), false);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_BCKG_COLOR), false);
	m_TempDockingSettings.crWindow = DEF_COLOR;
	SendMessageW(GetDlgItem(m_Dialogs.hDocks, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TempDockingSettings.crWindow);
	CreateDefaultFont(&m_TempDockingSettings.lfCaption, true); 
	if(g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption)
		DeleteFont(m_TempDRTHandles.hFCaption);
	m_TempDRTHandles.hFCaption = CreateFontIndirectW(&m_TempDockingSettings.lfCaption);
	m_TempDockingSettings.crCaption = 1;
	RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
	//-----------	misc dialog	-----------
	//set default misc options
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msCheckNewVOnStart], false);
	m_TempNoteSettings.checkOnStart = false;
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msStartWithWindows], false);
	m_TempNoteSettings.onStartup = false;
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msShowCPOnStart], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_SHOW_CP);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msHideFluently], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_FLUENTLY);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msPlaySoundOnHide], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_PLAY_HIDE_SOUND);
	SetDlgItemTextW(m_Dialogs.hMisc, IDC_EDT_DEF_BROWSER, NULL);
	*m_sTempdefBrowser = '\0';
	
	//-----------	Behavior dialog	-----------
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhSaveAlways], true);
	m_TempNoteSettings.saveOnExit = true;
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhAskBefore], true);
	m_TempNoteSettings.confirmSave = true;
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhConfirmDel], true);
	m_TempNoteSettings.confirmDelete = true;
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhHideWithout], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_WO_PROMPT);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhAlwaysOnTop], false);
	m_TempNoteSettings.newOnTop = false;
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhHideCompleted], false);
	BitOff(&m_TempNextSettings.flags1, SB3_HIDE_COMPLETED);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhBigIcons], false);
	BitOff(&m_TempNextSettings.flags1, SB3_BIG_ICONS);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhRelational], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_RELPOSITION);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhNoWindowList], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_NO_ALT_TAB);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhCleanBin], false);
	BitOff(&m_TempNextSettings.flags1, SB3_CLEAN_BIN_WITH_WARNING);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhExcAeroPeek], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK);
	//restore default autosave settings
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CHK_AUTOSAVE, BM_SETCHECK, BST_UNCHECKED, 0);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_EDT_AUTOSAVE), false);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_UPD_AUTOSAVE), false);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_ST_AUTOSAVE), false);
	m_TempNoteSettings.autoSave = false;
	m_TempNoteSettings.autoSValue = 5;
	SetWindowTextW(GetDlgItem(m_Dialogs.hBehavior, IDC_EDT_AUTOSAVE), L"5");
	//erase double click action settings
	BitOff(&m_TempNoteSettings.reserved2, SB2_NEW_NOTE);
	BitOff(&m_TempNoteSettings.reserved2, SB2_CONTROL_PANEL);
	BitOff(&m_TempNoteSettings.reserved2, SB2_PREFS);
	BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_NOTES);
	BitOff(&m_TempNoteSettings.reserved2, SB2_LOAD_NOTE);
	BitOff(&m_TempNoteSettings.reserved2, SB2_FROM_CLIPBOARD);
	BitOff(&m_TempNoteSettings.reserved2, SB2_ALL_TO_FRONT);
	BitOff(&m_TempNoteSettings.reserved2, SB2_SAVE_ALL);
	BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_TAGS);
	//set default settings
	BitOn(&m_TempNoteSettings.reserved2, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETCURSEL, 0, 0);
	SetSmallValue(&m_TempSmallValues, SMS_DEF_SAVE_CHARACTERS, DEF_SAVE_CHARACTERS);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DEF_SAVE_CHARS, CB_SETCURSEL, DEF_SAVE_CHARACTERS - 1, 0);
	SetSmallValue(&m_TempSmallValues, SMS_CONTENT_LENGTH, DEF_CONTENT_CHARACTERS);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CONTENT_LENGTH, CB_SETCURSEL, DEF_CONTENT_CHARACTERS - 1, 0);
	SetSmallValue(&m_TempSmallValues, SMS_DEF_NAMING, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DEF_NAME_OPT, CB_SETCURSEL, 0, 0);
	m_TempNextSettings.cleanDays = 0;
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_SETCURSEL, 0, 0);
	m_TempNextSettings.flags2 = 0;
	//-----------	schedule dialog	-----------
	//set default date formats
	wcscpy(m_TempFormats.DateFormat, DTS_DATE_FORMAT);
	wcscpy(m_TempFormats.TimeFormat, DTS_TIME_FORMAT);
	SetDlgItemTextW(m_Dialogs.hSchedule, IDC_EDT_DATE_FMTS, m_TempFormats.DateFormat);
	SetDlgItemTextW(m_Dialogs.hSchedule, IDC_EDT_TIME_FMTS, m_TempFormats.TimeFormat);
	SetShowDateTime(m_Dialogs.hSchedule);
	//set default sound settings
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scAllowSound], true);
	m_TempSound.allowSound = true;
	wcscpy(m_TempSound.custSound, DS_DEF_SOUND);
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
	SendMessageW(GetDlgItem(m_Dialogs.hSchedule, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, false);
	//default voice
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_LST_VOICES, LB_SETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_LST_VOICES, LB_GETTEXT, 0, (LPARAM)m_TempVoice);
	EnableWindow(GetDlgItem(m_Dialogs.hSchedule, IDC_CMD_SET_DEF_VOICE), false);
	//set default visual alert property
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scVisualAlert], true);
	m_TempNoteSettings.visualAlert = true;
	//set default overdue tracking
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scTrackOverdue], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE);
	//set default do_not_center settings
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scDontMove], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_DONOT_CENTER);
	RestoreDefaultVoices();

	//-----------	Network dialog	-----------
	BitOff(&m_TempNoteSettings.reserved1, SB1_SILENT_BACKUP);
	BitOff(&m_TempNoteSettings.reserved1, SB1_SYNC_ON_START);
	BitOff(&m_TempNoteSettings.reserved1, SB1_INC_DEL_IN_SYNC);
	BitOff(&m_TempNoteSettings.reserved1, SB1_SAVE_BEFORE_SYNC);
	BitOff(&m_TempNextSettings.flags1, SB3_SAVE_BEFORE_SEND);
	BitOff(&m_TempNextSettings.flags1, SB3_RECEIVE_HIDE_BALOON);
	BitOff(&m_TempNextSettings.flags1, SB3_SEND_HIDE_BALOON);
	BitOff(&m_TempNextSettings.flags1, SB3_RECEIVE_NOT_PLAY_SOUND);
	BitOff(&m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_NOTE);
	BitOff(&m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_CP);
	BitOff(&m_TempNextSettings.flags1, SB3_HIDE_AFTER_SEND);
	BitOff(&m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONT_MENU);
	BitOff(&m_TempNextSettings.flags1, SB3_SHOW_AFTER_RECEIVING);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntSyncStart], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntIncDelSync], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSync], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntEnableNetwork], false);

	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSending], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotPlaySoundNewMessage], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowSendMessage], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntShowNoteClickNotify], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntShowCPClickNotify], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowContactsInMenu], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntHideAfterSend], false);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntShowAfterReceiving], false);

	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSending], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotPlaySoundNewMessage], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowSendMessage], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowNoteClickNotify], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowCPClickNotify], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowContactsInMenu], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntHideAfterSend], false);
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowAfterReceiving], false);
	
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_GRP_EXCHANGE), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_ST_CONTACTS), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_LST_CONTACTS), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_TBR_CONTACTS), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_TBR_CONT_GROUPS), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_ST_SOCKET_PORT), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_EDT_SOCKET_PORT), true);
	EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_TVW_CONT_GROUPS), true);

	m_TempWSPort = SOCKET_DEF_PORT;
	SetDlgItemInt(m_Dialogs.hNetwork, IDC_EDT_SOCKET_PORT, m_TempWSPort, false);

	//-----------	Protection dialog	-----------
	BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_TRAY);
	BitOff(&m_TempNoteSettings.reserved1, SB1_USE_BACKUP);
	BitOff(&m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONTENT);
	BitOff(&m_TempNextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prHideFromTray], false);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prBackupNotes], false);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prSilentBackup], false);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prDontShowContent], false);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prSyncLocalBin], false);

	//hide, show and enable controls
	EnableWindow(GetDlgItem(m_Dialogs.hProtection, IDC_ST_BACKUP_LEVELS), false);
	EnableWindow(GetDlgItem(m_Dialogs.hProtection, IDC_EDT_BACKUP_COUNT), false);
	EnableWindow(GetDlgItem(m_Dialogs.hProtection, IDC_UPD_BACKUP), false);
	SetSmallValue(&m_TempSmallValues, SMS_BACKUP_COUNT, DEF_BACK_COUNT);
	SetDlgItemInt(m_Dialogs.hProtection, IDC_EDT_BACKUP_COUNT, DEF_BACK_COUNT, false);

	//-----------	Diary dialog	-----------
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drAddWeekday], false);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drFullWeekday], false);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drWeekdayAtEnd], false);
	CTreeView_SetEnable(m_hTreeDiary, m_ChecksTDiary[drFullWeekday], false);
	CTreeView_SetEnable(m_hTreeDiary, m_ChecksTDiary[drWeekdayAtEnd], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_ADD_WEEKDAY_DIARY);
	BitOff(&m_TempNoteSettings.reserved1, SB1_FULL_WEEKDAY_NAME);
	BitOff(&m_TempNoteSettings.reserved1, SB1_WEEKDAY_DIARY_END);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drDiaryCust], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS);
	ClearRTHandles(&m_TempDiaryRTHandles);
	// memcpy(&m_TempDiaryAppearance, &m_TempAppearance, sizeof(NOTE_APPEARANCE));
	m_TempDiaryAppearance = m_TempAppearance;
	*m_TempDiaryAppearance.szSkin = '\0';
	SendMessageW(GetDlgItem(m_Dialogs.hDiary, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TempDiaryAppearance.crWindow);
	CreateDefaultFont(&m_TempDiaryAppearance.lfCaption, true); 
	if(g_DiaryRTHandles.hFCaption != m_TempDiaryRTHandles.hFCaption)
		DeleteFont(m_TempDiaryRTHandles.hFCaption);
	m_TempDiaryRTHandles.hFCaption = CreateFontIndirectW(&m_TempDiaryAppearance.lfCaption);
	RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
	SendDlgItemMessageW(m_Dialogs.hDiary, IDC_LST_SKIN, LB_SETCURSEL, -1, 0);
	EnableDiaryCustPart(ENDC_DISABLED);
	ShowDiaryDateSample(m_Dialogs.hDiary);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drNoPages], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_NO_DIARY_PAGES);
	EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_DIARY_COUNT), true);
	EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CBO_DIARY_COUNT), true);
	SetSmallValue(&m_TempSmallValues, SMS_DIARY_PAGES, DEF_DIARY_PAGES);
	SendDlgItemMessageW(m_Dialogs.hDiary, IDC_CBO_DIARY_COUNT, CB_SETCURSEL, DEF_DIARY_PAGES - 1, 0);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drAscSort], false);
	BitOff(&m_TempNoteSettings.reserved1, SB1_DIARY_SORT_ASC);
}

static void EnableSkinlessProperties(BOOL fEnabled){
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_GRP_SKINLESS_PROPS), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_GRP_DEF_SIZE), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_W), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_W), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_H), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_H), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_CMD_SKNLESS_CFONT), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_CHOOSE_COLOR), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_TVW_CHK_SKINLESS), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_BCKG_COLOR), fEnabled);
}

static void LoadAutosaved(void){
	wchar_t				szBuffer[10 * 1024], szSection[10 * 1024], *pw;
	wchar_t				szTemp[MAX_PATH], szPathNote[MAX_PATH], szTempNote[MAX_PATH];

	wcscpy(szTemp, g_NotePaths.DataDir);
	wcscat(szTemp, AUTOSAVE_INDEX_FILE);
	if(PathFileExistsW(szTemp)){
		GetPrivateProfileSectionNamesW(szBuffer, 10 * 1024, szTemp);
		pw = szBuffer;
		while(*pw){
			wcscpy(szTempNote, g_NotePaths.DataDir);
			wcscat(szTempNote, L"~");
			wcscat(szTempNote, pw);
			wcscat(szTempNote, L".no~");
			wcscpy(szPathNote, g_NotePaths.DataDir);
			wcscat(szPathNote, pw);
			wcscat(szPathNote, NOTE_EXTENTION);
			CopyFileW(szTempNote, szPathNote, false);
			SetFileAttributesW(szPathNote, FILE_ATTRIBUTE_NORMAL);
			GetPrivateProfileSectionW(pw, szSection, 10 * 1024, szTemp);
			WritePrivateProfileSectionW(pw, szSection, g_NotePaths.DataFile);
			pw += wcslen(pw) + 1;
		}
	}
}

static void CheckGroup(int id){
	int					count = PNGroupsCount(g_PGroups) + 1;

	for(int i =0; i < count; i++){
		if(m_GChecks[i].id == id){
			m_GChecks[i].checked = true;
		}
	}
}

static BOOL IsGroupChecked(int id){
	int					count = PNGroupsCount(g_PGroups) + 1;

	for(int i =0; i < count; i++){
		if(m_GChecks[i].id == id){
			return m_GChecks[i].checked;
		}
	}
	return false;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetAllNotes
 Created  : Thu May 10 16:59:11 2007
 Modified : Mon May 14 12:27:31 2007

 Synopsys : Loads all saved notes on proghram start
 Input    : Main window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void GetAllNotes(BOOL checkPassword){
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	wchar_t				szPath[MAX_PATH], szBuffer[4096], szFile[128];
	BOOL				fFound = true, fLoadNew;
	PMEMNOTE			pNote;
	LPPNGROUP			pg;

	GetGroupChecks();
	wcscpy(szPath, g_NotePaths.DataDir);
	wcscat(szPath, L"*");
	wcscat(szPath, NOTE_EXTENTION);
	handle = FindFirstFileW(szPath, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(fFound){
			wcscpy(szFile, fd.cFileName);
			PathRemoveExtensionW(szFile);
			pNote = AddMemNote();
			if(PathFileExistsW(g_NotePaths.DataFile)){
				if(GetPrivateProfileSectionW(szFile, szBuffer, 4096, g_NotePaths.DataFile)){
					fLoadNew = false;
					LoadNoteProperties(pNote, &g_RTHandles, szFile, false);
				}
				else{
					fLoadNew = true;
					LoadNoteProperties(pNote, &g_RTHandles, szFile, true);
				}
			}
			else{
				fLoadNew = true;
				LoadNoteProperties(pNote, &g_RTHandles, szFile, true);
			}
			if(pNote->pData->idGroup > GROUP_RECYCLE && pNote->pData->idGroup != GROUP_DIARY){
				//check note's group existance
				pg = PNGroupsGroupById(g_PGroups, pNote->pData->idGroup);
				if(!pg){
					//set note's group to General, if its group's not found
					pNote->pData->idGroup = 0;
				}
			}
			if(pNote->pData->visible){
				if(fLoadNew){
					if(!CreateNote(pNote, g_hInstance, true, szFile)){
						MessageBoxW(g_hMain, m_sNoWindowMessage, NULL, MB_OK);
					}
				}
				else{
					if(!checkPassword || IsGroupChecked(pNote->pData->idGroup)){
						if(!checkPassword || UnlockNote(pNote)){
							if(!CreateNote(pNote, g_hInstance, false, NULL)){
								MessageBoxW(g_hMain, m_sNoWindowMessage, NULL, MB_OK);
							}
						}
						else{
							pNote->pData->visible = false;
						}
					}
					else{
						if(!checkPassword || UnlockGroup(pNote->pData->idGroup)){
							CheckGroup(pNote->pData->idGroup);
							if(!checkPassword || UnlockNote(pNote)){
								if(!CreateNote(pNote, g_hInstance, false, NULL)){
									MessageBoxW(g_hMain, m_sNoWindowMessage, NULL, MB_OK);
								}
							}
							else{
								pNote->pData->visible = false;
							}
						}
						else{
							pNote->pData->visible = false;
						}
					}
				}
			}
			fFound = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
	FreeGroupChecks();
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateDefaultFont
 Created  : Thu May 10 17:00:50 2007
 Modified : Thu May 10 17:00:50 2007

 Synopsys : Creates default notes font, if no special font specified
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreateDefaultFont(PLOGFONTW plfFont, BOOL fBold){
	
	NONCLIENTMETRICSW	nc;
	
	ZeroMemory(&nc, sizeof(nc));
	nc.cbSize = sizeof(nc);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(nc), &nc, 0);
	memcpy(plfFont, &nc.lfMessageFont, sizeof(LOGFONTW));
	if(fBold)
		plfFont->lfWeight = FW_HEAVY;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateMenuFont
 Created  : Thu Aug  9 17:38:24 2007
 Modified : Thu Aug  9 17:38:24 2007

 Synopsys : Creates menu font
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreateMenuFont(void){

	NONCLIENTMETRICSW	nc;

	ZeroMemory(&nc, sizeof(nc));
	nc.cbSize = sizeof(nc);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(nc), &nc, 0);
	g_hMenuFont = CreateFontIndirectW(&nc.lfMenuFont);
}

static void Main_OnDestroy(HWND hwnd)
{
	if(m_TrackLogon)
		UnregisterLogonMessage(hwnd);
	PostQuitMessage(0);
}

static void DrawListItem(const DRAWITEMSTRUCT * lpd, int index){

	wchar_t			szBuffer[SendMessageW(lpd->hwndItem, LB_GETTEXTLEN, lpd->itemID, 0)];
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
		SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
		FillRect(lpd->hDC, &lpd->rcItem, GetSysColorBrush(COLOR_WINDOW));
	}
	SendMessageW(lpd->hwndItem, LB_GETTEXT, lpd->itemID, (LPARAM)szBuffer);
	//offset rectangle - make room for bitmap
	OffsetRect(&rc, 18, 0);
	//draw bitmap
	DrawTransparentBitmap(g_hBmpMisc, lpd->hDC, 1, rc.top + 1, 16, 16, 16 * index, 0, CLR_MASK);
	DrawTextExW(lpd->hDC, szBuffer, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL);
}

static void ShowHideOptionsDialogs(HWND hwnd){
	HWND		*lpDlg;
	int			size;

	lpDlg = (HWND *)&m_Dialogs;
	size = sizeof(m_Dialogs) / sizeof(HWND);
	for(int i = 0; i < size; i++){
		if(*lpDlg == hwnd)
			ShowWindow(*lpDlg, SW_SHOW);
		else
			ShowWindow(*lpDlg, SW_HIDE);
		lpDlg++;
	}
}

static LRESULT CALLBACK OptionsDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NMHDR		* nm;

    switch (uMsg)
    {
		HANDLE_MSG (hwnd, WM_CLOSE, Options_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Options_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Options_OnInitDialog);
		
		case WM_NOTIFY:
			nm = (NMHDR *)lParam;
			if(nm->idFrom == IDC_LEFT_PANE && nm->code == GTBN_BUTTONCLICKED){
				LPGTBNMHEADER	lpgtnm = (LPGTBNMHEADER)lParam;
				ShowHideOptionsDialogs(GetNavButton(lpgtnm->idButton)->hwnd);
			}
			return true;
		default:
			return false;
    }
}

static void Options_OnClose(HWND hwnd)
{
	wchar_t			szValue[16];

	// if(g_hAbout)
		// SendMessageW(g_hAbout, WM_CLOSE, 0, 0);
	m_CurrentOption = SendMessageW(m_hTopPanel, GTBM_GETSELECTED, 0, 0);
	if(m_CurrentOption == -1)
		m_CurrentOption = IDR_ICO_APPEARANCE;
	_itow(m_CurrentOption, szValue, 10);
	WritePrivateProfileStringW(S_NOTE, IK_LAST_OPTION, szValue, g_NotePaths.INIFile);
	ClearOnOptionsClose();
	if(m_hPrefsMenu){
		DestroyMenu(m_hPrefsMenu);
		m_hPrefsMenu = NULL;
	}
	g_hOptionsDlg = 0;
	DestroyWindow(hwnd);
}

static void Options_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	BOOL		result = false;

	switch (id){
		case IDM_CANCEL:
		case IDCANCEL:		//close options dialog
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDM_OK:
		case IDOK:			//close options dialog while applying changes
			result = ApplyChanges(hwnd);
			if(result){
				ReloadNotes(true);
			}
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDM_APPLY:
		case IDAPPLY:		//apply changes
			result = ApplyChanges(hwnd);
			if(result){
				ReloadNotes(true);
			}
			break;
		case IDC_CMD_DEF:
			if(MessageBoxW(hwnd, g_Strings.DefaultSettingsWarning, m_sSound2, MB_YESNO | MB_ICONQUESTION) == IDYES)
				RestoreDefaultSettings();
			break;
	}
}

static BOOL Options_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT			rcList, rcOptions;
	HBITMAP			hBmp;
	HMODULE			hIcons;
	int				scHeight = 0, dlHeight = 0;

	m_SettingsDialogLoaded = false;
	g_hOptionsDlg = hwnd;
	//set icon
	SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN)));
	//get screen height
	scHeight = GetSystemMetrics(SM_CYSCREEN);
	//get dialog size
	GetWindowRect(hwnd, &rcOptions);
	dlHeight = rcOptions.bottom - rcOptions.top;
	if(scHeight <= 600 || scHeight <= dlHeight){
		//add menu
		m_hPrefsMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_OPTIONS_MENU));
		SetMenu(hwnd, m_hPrefsMenu);
	}

	CenterWindow(hwnd, false);
	
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		//prepare image list for small bars
		if(m_hImlSmallBars == NULL && m_hImlSmallBarsGray == NULL){
			CreateImageListsW(&m_hImlSmallBars, &m_hImlSmallBarsGray, hIcons, IDB_SMALL_BARS, IDB_SMALL_BARS_GRAY, CLR_MASK, 16, 16, 4);
		}
		//prepare image list for check image
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_DEF_CHECK));
		m_hImlDefCheck = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(m_hImlDefCheck, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		//prepare image list for top pane
		m_hImlLeft = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 0, 1);
		HICON		hIcon;
		for(int i = 0; i < NELEMS(m_NavButtons); i++){
			hIcon = LoadIconW(hIcons, MAKEINTRESOURCEW(m_NavButtons[i].id));
			ImageList_AddIcon(m_hImlLeft, hIcon);
		}
		FreeLibrary(hIcons);
	}
	
	m_hTopPanel = CreateGTB(g_hInstance, hwnd, IDC_LEFT_PANE, 56);
	SendMessageW(m_hTopPanel, GTBM_SETIMAGELIST, 0, (LPARAM)m_hImlLeft);

	//show skin
	m_TempRHandles.hbSkin = g_RTHandles.hbSkin;
	//copy data
	// memcpy(&m_TempRHandles, &g_RTHandles, sizeof(NOTE_RTHANDLES));
	m_TempRHandles = g_RTHandles;
	// memcpy(&m_TempAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));
	m_TempAppearance = g_Appearance;
	// memcpy(&m_TempSmallValues, &g_SmallValues, sizeof(SMALLVALUES));
	m_TempSmallValues = g_SmallValues;
	// memcpy(&m_TempDiaryAppearance, &g_DiaryAppearance, sizeof(NOTE_APPEARANCE));
	m_TempDiaryAppearance = g_DiaryAppearance;
	// memcpy(&m_TempDiaryRTHandles, &g_DiaryRTHandles, sizeof(NOTE_RTHANDLES));
	m_TempDiaryRTHandles = g_DiaryRTHandles;

	GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
	m_TempDiaryRTHandles.hFCaption = CreateFontIndirectW(&m_TempDiaryAppearance.lfCaption);
	m_TempDiaryRTHandles.hFont = CreateFontIndirectW(&m_TempDiaryAppearance.lfFont);
	wcscpy(m_sTempSkinFile, g_Appearance.szSkin);
	//copy date/time formats
	wcscpy(m_TempFormats.DateFormat, g_DTFormats.DateFormat);
	wcscpy(m_TempFormats.TimeFormat, g_DTFormats.TimeFormat);
	//apply note settings
	memcpy(&m_TempNoteSettings, &g_NoteSettings, sizeof(NOTE_SETTINGS));

	//docking settings
	// memcpy(&m_TempDockingSettings, &g_DockSettings, sizeof(PNDOCK));
	m_TempDockingSettings = g_DockSettings;
	// memcpy(&m_TempDRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
	m_TempDRTHandles = g_DRTHandles;

	//save default browser
	wcscpy(m_sTempdefBrowser, g_DefBrowser);
	//diary format index
	m_TempDiaryFormatIndex = g_DiaryFormatIndex;
	//predefined tags
	m_TempPTagsPredefined = TagsCopy(m_TempPTagsPredefined, g_PTagsPredefined);
	//search engines
	m_TempSengs = SEngsCopy(m_TempSengs, g_PSengs);
	//FTP data
	m_TempFTPData = m_FTPData;
	//externals
	m_TempExternals = SEngsCopy(m_TempExternals, g_PExternalls);
	//default voice
	wcscpy(m_TempVoice, g_DefVoice);
	//next settings
	m_TempNextSettings = g_NextSettings;
	//contacts
	g_TempContacts = PContactsCopy(g_TempContacts, g_PContacts);
	//groups of contacts
	PContGroupsCopy(&g_TempContGroups, g_PContGroups);
	//local syncs
	PLocalSyncCopy(&g_TempLocalSyncs, g_PLocalSyncs);
	m_TempWSPort = g_wsDefPort;
	//spell underlining color
	m_TempSpellColor = g_Spell.color;
	//voices
	memcpy(g_TempPVoices, g_PVoices, sizeof(PNVOICE) * 32);

	m_Dialogs.hAppearance = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_APPEARANCE), hwnd, Appearance_DlgProc, 0);
	GetNavButton(IDR_ICO_APPEARANCE)->hwnd = m_Dialogs.hAppearance;
	m_Dialogs.hSkins = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SKINS), hwnd, Skins_DlgProc, 0);
	GetNavButton(IDR_ICO_SKINS)->hwnd = m_Dialogs.hSkins;
	m_Dialogs.hSchedule = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SCHEDULE), hwnd, Schedule_DlgProc, 0);
	GetNavButton(IDR_ICO_SCHEDULE)->hwnd = m_Dialogs.hSchedule;
	m_Dialogs.hBehavior = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_BEHAVIOR), hwnd, Behavior_DlgProc, 0);
	GetNavButton(IDR_ICO_BEHAVIOR)->hwnd = m_Dialogs.hBehavior;
	m_Dialogs.hMisc = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_MISC), hwnd, Misc_DlgProc, 0);
	GetNavButton(IDR_ICO_MISC)->hwnd = m_Dialogs.hMisc;
	m_Dialogs.hDocks = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_DOCKING), hwnd, Docks_DlgProc, 0);
	GetNavButton(IDR_ICO_DOCK)->hwnd = m_Dialogs.hDocks;
	m_Dialogs.hDiary = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_DIARY), hwnd, Diary_DlgProc, 0);
	GetNavButton(IDR_ICO_DIARY)->hwnd = m_Dialogs.hDiary;
	m_Dialogs.hProtection = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_PROTECTION), hwnd, Protection_DlgProc, 0);
	GetNavButton(IDR_ICO_PROTECTION)->hwnd = m_Dialogs.hProtection;
	m_Dialogs.hNetwork = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NETWORK), hwnd, Network_DlgProc, 0);
	GetNavButton(IDR_ICO_NETWORK)->hwnd = m_Dialogs.hNetwork;

	PrepareSmallToolbars();

	InitDlgAppearance(m_Dialogs.hAppearance);
	InitDlgSkins(m_Dialogs.hSkins);
	InitDlgSchedule(m_Dialogs.hSchedule);
	InitDlgBehavior(m_Dialogs.hBehavior);
	InitDlgMisc(m_Dialogs.hMisc);
	InitDlgDocks(m_Dialogs.hDocks);
	InitDlgProtection(m_Dialogs.hProtection);
	InitDlgDiary(m_Dialogs.hDiary);
	InitDlgNetwork(m_Dialogs.hNetwork);

	AddChecksBehavior();
	AddChecksSkins();
	AddChecksMisc();
	AddChecksAppearance();
	AddChecksSchedule();
	AddChecksProtection();
	AddChecksDiary();
	AddChecksDock();
	AddChecksNetwork();

	ApplyMainDlgLanguage(hwnd);
	GetWindowRect(m_hTopPanel, &rcList);
	MoveOptionsDialogs(0, rcList.bottom - rcList.top);

	SendMessageW(m_hTopPanel, GTBM_SELECTBUTTON, 0, m_CurrentOption);	
	SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	m_SettingsDialogLoaded = true;

	if(g_DisableInput)
		EnableWindow(hwnd, false);

	return false;
}

static void FillNavigationBar(wchar_t * lpFile){
	wchar_t		szBuffer[256], szKey[16];
	int			selected = 0;
	GTBBUTTON	gtb;

	selected = SendMessageW(m_hTopPanel, GTBM_GETSELECTED, 0, 0);
	if(selected == -1)
		selected = IDR_ICO_APPEARANCE;
	SendMessageW(m_hTopPanel, GTBM_CLEAR, 0, 0);
	for(int i = 0; i < NELEMS(m_NavButtons); i++){
		gtb.id = m_NavButtons[i].id;
		gtb.index = i;
		gtb.lpText = szBuffer;
		_itow(m_NavButtons[i].id, szKey, 10);
		GetPrivateProfileStringW(S_LEFT_PANEL, szKey, m_PanelDefs[i], szBuffer, 256, lpFile);
		SendMessageW(m_hTopPanel, GTBM_ADDBUTTON, (WPARAM)g_hInstance, (LPARAM)&gtb);
	}
	SendMessageW(m_hTopPanel, GTBM_SELECTBUTTON, 0, selected);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetShowDateTime
 Created  : Thu Aug  9 17:51:51 2007
 Modified : Thu Aug  9 17:51:51 2007

 Synopsys : Shows dates and times on preferences dialog according to 
            current formats
 Input    : hwnd - preferences dialog handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetShowDateTime(HWND hwnd){

	SYSTEMTIME		st;
	wchar_t			szDate[128], szTime[128], szDTimePart[128];
	wchar_t			szDFormat[128], szTFormat[128], szTemp[128];
	int				result;

	szDTimePart[0] = '\0';
	result = ParseDateFormat(m_TempFormats.DateFormat, szDFormat, szTFormat);
		
	ZeroMemory(&st, sizeof(st));
	GetLocalTime(&st);
	GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szDFormat, szDate, 128);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, szTFormat, szDTimePart, 128);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, m_TempFormats.TimeFormat, szTime, 128);
	if(result == 1)
		wcscat(szDate, szDTimePart);
	else{
		wcscpy(szTemp, szDate);
		wcscpy(szDate, szDTimePart);
		wcscat(szDate, szTemp);
	}
	SetDlgItemTextW(hwnd, IDC_DATE_SAMPLE, szDate);
	SetDlgItemTextW(hwnd, IDC_TIME_SAMPLE, szTime);
}

static BOOL CheckDblClickChanges(void){
	if(GetBit(g_NoteSettings.reserved2, SB2_NEW_NOTE) != GetBit(m_TempNoteSettings.reserved2, SB2_NEW_NOTE))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_CONTROL_PANEL) != GetBit(m_TempNoteSettings.reserved2, SB2_CONTROL_PANEL))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_PREFS) != GetBit(m_TempNoteSettings.reserved2, SB2_PREFS))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_SEARCH_NOTES) != GetBit(m_TempNoteSettings.reserved2, SB2_SEARCH_NOTES))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_LOAD_NOTE) != GetBit(m_TempNoteSettings.reserved2, SB2_LOAD_NOTE))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_FROM_CLIPBOARD) != GetBit(m_TempNoteSettings.reserved2, SB2_FROM_CLIPBOARD))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_ALL_TO_FRONT) != GetBit(m_TempNoteSettings.reserved2, SB2_ALL_TO_FRONT))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_SAVE_ALL) != GetBit(m_TempNoteSettings.reserved2, SB2_SAVE_ALL))
		return true;
	else if(GetBit(g_NoteSettings.reserved2, SB2_SEARCH_TAGS) != GetBit(m_TempNoteSettings.reserved2, SB2_SEARCH_TAGS))
		return true;
	else
		return false;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CheckHotKeysChanges
 Created  : Thu Aug  9 17:52:41 2007
 Modified : Thu Aug  9 17:52:41 2007

 Synopsys : Checks whether hot keys are changed
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL CheckHotKeysChanges(P_HK_TYPE lpKeys, P_HK_TYPE lpTempKeys, int count, BOOL reg){

	BOOL			result = false;

	for(int i = 0; i < count; i++){
		if(lpKeys[i].fsModifiers != lpTempKeys[i].fsModifiers || lpKeys[i].vk != lpTempKeys[i].vk){
			result = true;
			if(reg){
				if(lpKeys[i].fsModifiers != 0 || lpKeys[i].vk != 0){
					//if the hot key was registered before - unregister it
					UnregisterHotKey(g_hMain, lpKeys[i].identifier);
				}
			}
			memcpy(&lpKeys[i], &lpTempKeys[i], sizeof(HK_TYPE));
			if(reg){
				if(lpKeys[i].fsModifiers != 0 || lpKeys[i].vk != 0){
					//if the new hot key is not empty - register it
					RegisterHotKey(g_hMain, lpKeys[i].identifier, lpKeys[i].fsModifiers, lpKeys[i].vk);
				}
			}
		}
	}
	return result;
}

static BOOL CheckGroupsHotkeysChanges(LPPNGROUP pgOld, LPPNGROUP pgNew){
	LPPNGROUP	pOld, pNew;
	BOOL		result = false;

	pOld = pgOld;
	pNew = pgNew;
	while(pOld && pNew){
		if(pOld->hotKeyHide.fsModifiers != pNew->hotKeyHide.fsModifiers || pOld->hotKeyHide.vk != pNew->hotKeyHide.vk){
			if(pOld->hotKeyHide.fsModifiers != 0 || pOld->hotKeyHide.vk != 0){
				//if the hot key was registered before - unregister it
				UnregisterHotKey(g_hMain, pOld->hotKeyHide.id);
			}
			memcpy(&pOld->hotKeyHide, &pNew->hotKeyHide, sizeof(HK_TYPE));
			if(pOld->hotKeyHide.fsModifiers != 0 || pOld->hotKeyHide.vk != 0){
				//if the new hot key is not empty - register it
				pOld->hotKeyHide.id = pOld->id + HIDE_GROUP_ADDITION;
				RegisterHotKey(g_hMain, pOld->hotKeyHide.id, pOld->hotKeyHide.fsModifiers, pOld->hotKeyHide.vk);
			}
			SaveGroup(pOld);
			result = true;
		}
		if(pOld->hotKeyShow.fsModifiers != pNew->hotKeyShow.fsModifiers	|| pOld->hotKeyShow.vk != pNew->hotKeyShow.vk){
			if(pOld->hotKeyShow.fsModifiers != 0 || pOld->hotKeyShow.vk != 0){
				//if the hot key was registered before - unregister it
				UnregisterHotKey(g_hMain, pOld->hotKeyShow.id);
			}
			memcpy(&pOld->hotKeyShow, &pNew->hotKeyShow, sizeof(HK_TYPE));
			if(pOld->hotKeyShow.fsModifiers != 0 || pOld->hotKeyShow.vk != 0){
				//if the new hot key is not empty - register it
				pOld->hotKeyShow.id = pOld->id + SHOW_GROUP_ADDITION;
				RegisterHotKey(g_hMain, pOld->hotKeyShow.id, pOld->hotKeyShow.fsModifiers, pOld->hotKeyShow.vk);
			}
			SaveGroup(pOld);
			result = true;
		}
		pOld = pOld->next;
		pNew = pNew->next;
	}
	return result;
}

static void SavePredefinedTags(void){
	wchar_t		*pText;
	int			count = 0;

	for(LPPTAG pTag = g_PTagsPredefined; pTag; pTag = pTag->next)
		count++;
	if(count == 0){
		WritePrivateProfileStringW(S_PRE_TAGS, IK_TAGS, NULL, g_NotePaths.INIFile);
		return;
	}
	pText = calloc(count * 128 + 1, sizeof(wchar_t));
	if(pText){
		for(LPPTAG pTag = g_PTagsPredefined; pTag; pTag = pTag->next){
			wcscat(pText, pTag->text);
			wcscat(pText, L",");
		}
		if(*pText){
			pText[wcslen(pText) - 1] = '\0';
			WritePrivateProfileStringW(S_PRE_TAGS, IK_TAGS, pText, g_NotePaths.INIFile);
		}
		free(pText);
	}
}

static void CheckAllDeletedTags(void){
	for(LPPTAG p1 = m_TagsDeleted; p1; p1 = p1->next){	
		for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
			if(TagsContains(pNote->pTags, p1->text)){
				pNote->pTags = TagsRemove(pNote->pTags, p1->text);
				SaveTags(pNote);
				if(g_hCPDialog)
					SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
		}
	}
}

static void CheckAllModifiedTags(void){
	for(LPPTAG p1 = m_TagsModified; p1; p1 = p1->next){
		for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
			if(TagsContains(pNote->pTags, p1->oldText)){
				TagsReplace(pNote->pTags, p1->text, p1->oldText);
				SaveTags(pNote);
				if(g_hCPDialog)
					SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
		}
	}
}

static void SaveExternals(void){
	wchar_t			szBuffer[1024 + MAX_PATH];

	WritePrivateProfileSectionW(S_EXTERNAL_PROGRAMS, NULL, g_NotePaths.INIFile);
	for(LPPSENG	pNext = g_PExternalls; pNext; pNext = pNext->next){
		wcscpy(szBuffer, pNext->query);
		if(wcslen(pNext->commandline) > 0){
			wcscat(szBuffer, L"\1");
			wcscat(szBuffer, pNext->commandline);
		}
		WritePrivateProfileStringW(S_EXTERNAL_PROGRAMS, pNext->name, szBuffer, g_NotePaths.INIFile);
	}
	//build external programs menu
	BuildExternalMenu(m_hPopUp);
}

static void SaveSearchEngines(void){
	WritePrivateProfileSectionW(S_SEARCH_ENGINES, NULL, g_NotePaths.INIFile);
	for(LPPSENG pNext = g_PSengs; pNext; pNext = pNext->next){
		WritePrivateProfileStringW(S_SEARCH_ENGINES, pNext->name, pNext->query, g_NotePaths.INIFile);
	}
}

static void SaveLocalSyncs(void){
	wchar_t			szBuffer[MAX_PATH * 2 + 2];

	WritePrivateProfileSectionW(S_LOCAL_SYNCS, NULL, g_NotePaths.INIFile);
	for(LPPLOCALSYNC pSync = g_PLocalSyncs; pSync; pSync = pSync->next){
		wcscpy(szBuffer, pSync->pathNotes);
		wcscat(szBuffer, L"|");
		wcscat(szBuffer, pSync->pathId);
		WritePrivateProfileStringW(S_LOCAL_SYNCS, pSync->name, szBuffer, g_NotePaths.INIFile);
	}
}

static void SaveVoices(void){
	wchar_t			szBuffer[32];

	for(int i = 0; i < NELEMS(g_TempPVoices); i++){
		if(g_PVoices[i].volume != g_TempPVoices[i].volume || g_PVoices[i].rate != g_TempPVoices[i].rate || g_PVoices[i].pitch != g_TempPVoices[i].pitch){
			g_PVoices[i].volume = g_TempPVoices[i].volume;
			g_PVoices[i].rate = g_TempPVoices[i].rate;
			g_PVoices[i].pitch = g_TempPVoices[i].pitch;
			swprintf(szBuffer, 32, L"%d|%d|%d", g_PVoices[i].rate, g_PVoices[i].volume, g_PVoices[i].pitch);
			WritePrivateProfileStringW(S_VOICES, g_PVoices[i].name, szBuffer, g_NotePaths.INIFile);
		}
	}
}

static void RestoreDefaultVoices(void){
	for(int i = 0; i < NELEMS(g_TempPVoices); i++){
		g_TempPVoices[i].volume = 100;
		g_TempPVoices[i].rate = 0;
		g_TempPVoices[i].pitch = 0;
	}
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_TRACK_VOLUME, TBM_SETPOS, true, 100);
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_TRACK_RATE, TBM_SETPOS, true, 10);
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_TRACK_PITCH, TBM_SETPOS, true, 10);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ApplyChanges
 Created  : Mon May 14 12:30:26 2007
 Modified : Mon May 14 12:30:26 2007

 Synopsys : Applies new settings
 Input    : Options dialog handle, skin file name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL ApplyChanges(HWND hwnd){
	wchar_t 		szColor[32], szKey[16], szMessage[280], szBuffer[256];
	BOOL			fChanged = false, fSkinBefore, fReload = false, fAutoSVChanged = false;
	BOOL			fW = false, fH = false, fSetDefSize = true, fDockSizeChanged = false, fDockSkinChanged = false;
	BOOL			fDockBGColorChanged = false, fDockCaptionFontChanged = false;
	BOOL			fMarginChanged = false, fDiaryChanged = false;
	BOOL			fDBackColorChanged = false, fDCaptionColorChanged = false, fDCaptionFontChanged = false, fDSkinChanged = false;
	BOOL			fDiaryFormatChanged = false, fNextSettingsChanged = false, fReloadCP = false;
	BOOL			fStartWinsock = false, fStopWinsock = false, fUpdateCP = false;
	char			szLink[MAX_PATH];

	//default voice
	if(wcscmp(g_DefVoice, m_TempVoice) != 0){
		wcscpy(g_DefVoice, m_TempVoice);
		WritePrivateProfileStringW(S_NOTE, IK_VOICE_DEFAULT, g_DefVoice, g_NotePaths.INIFile);
	}
	//voices properties
	SaveVoices();
	//FTP data
	if(wcscmp(m_TempFTPData.server, m_FTPData.server) != 0 
		|| wcscmp(m_TempFTPData.directory, m_FTPData.directory) != 0 
		|| wcscmp(m_TempFTPData.user, m_FTPData.user) != 0 
		|| wcscmp(m_TempFTPData.password, m_FTPData.password) != 0 
		|| m_TempFTPData.port != m_FTPData.port){

		m_FTPData = m_TempFTPData;
		WritePrivateProfileStructW(S_NOTE, IK_FTP_DATA, &m_FTPData, sizeof(FTP_DATA), g_NotePaths.INIFile);
	}
	//local syncs
	if(PLocalSyncDifference(g_PLocalSyncs, g_TempLocalSyncs)){
		PLocalSyncCopy(&g_PLocalSyncs, g_TempLocalSyncs);
		SaveLocalSyncs();
	}
	//contacts
	if(PContactsDifference(g_PContacts, g_TempContacts)){
		g_PContacts = PContactsCopy(g_PContacts, g_TempContacts);
		SaveContacts();
	}
	//contacts group
	if(PContGroupsDifference(g_PContGroups, g_TempContGroups)){
		PContGroupsCopy(&g_PContGroups, g_TempContGroups);
		SaveContactsGroups();
	}
	//external programs
	if(SEngsDifference(g_PExternalls,m_TempExternals)){
		g_PExternalls = SEngsCopy(g_PExternalls, m_TempExternals);
		SaveExternals();
	}
	//search engines
	if(SEngsDifference(g_PSengs, m_TempSengs)){
		g_PSengs = SEngsCopy(g_PSengs, m_TempSengs);
		SaveSearchEngines();
	}
	//predefined tags
	if(TagsCompare(g_PTagsPredefined, m_TempPTagsPredefined) != 0){
		g_PTagsPredefined = TagsCopy(g_PTagsPredefined, m_TempPTagsPredefined);
		SavePredefinedTags();
		if(TagsCount(m_TagsDeleted) > 0){
			CheckAllDeletedTags();
			TagsFree(m_TagsDeleted);
			m_TagsDeleted = NULL;
		}
		if(TagsCount(m_TagsModified) > 0){
			CheckAllModifiedTags();
			TagsFree(m_TagsModified);
			m_TagsModified = NULL;
		}
	}
	//spell underlining color
	if(g_Spell.color != m_TempSpellColor){
		g_Spell.color = m_TempSpellColor;
		if(g_Spell.color == 0){
			g_Spell.color = 1;
		}
		_itow(g_Spell.color, szColor, 10);
		WritePrivateProfileStringW(S_NOTE, IK_SPELL_COLOR, szColor, g_NotePaths.INIFile);
		SetNewSpellColor(g_Spell.color);
		if(g_Spell.spellAuto){
			ApplyRedrawEdit();
		}
	}
	//diary format index
	if(g_DiaryFormatIndex != m_TempDiaryFormatIndex){
		g_DiaryFormatIndex = m_TempDiaryFormatIndex;
		_itow(g_DiaryFormatIndex, szKey, 10);
		WritePrivateProfileStringW(S_FORMAT, IK_DF_INDEX, szKey, g_NotePaths.INIFile);
		fDiaryFormatChanged = true;
	}
	//def browser
	if(wcscmp(g_DefBrowser, m_sTempdefBrowser)){
		wcscpy(g_DefBrowser, m_sTempdefBrowser);
		WritePrivateProfileStringW(L"def_browser", L"def_browser", g_DefBrowser, g_NotePaths.INIFile);
	}
	//sounds
	if(m_TempSound.allowSound != g_Sound.allowSound){// || m_TempSound.defSound != g_Sound.defSound	|| wcscmp(m_TempSound.custSound, g_Sound.custSound) != 0){
		g_Sound.allowSound = m_TempSound.allowSound;
		// memcpy(&m_PrevSound, &g_Sound, sizeof(SOUND_TYPE));
		m_PrevSound = g_Sound;
		if(m_TempSound.allowSound)
			WritePrivateProfileStringW(S_SOUND, DSK_ALLOW, L"1", g_NotePaths.INIFile);
		else
			WritePrivateProfileStringW(S_SOUND, DSK_ALLOW, L"0", g_NotePaths.INIFile);
	}
	if(_wcsicmp(m_TempSound.custSound, g_Sound.custSound) != 0){
		wcscpy(g_Sound.custSound, m_TempSound.custSound);
		WritePrivateProfileStringW(S_SOUND, DSK_SOUND_CUST, m_TempSound.custSound, g_NotePaths.INIFile);
	}
	//formats
	if(wcscmp(m_TempFormats.DateFormat, g_DTFormats.DateFormat) != 0){
		if(!wcschr(m_TempFormats.DateFormat, L'd') || !wcschr(m_TempFormats.DateFormat, L'M') || !wcschr(m_TempFormats.DateFormat, L'y') 
		|| !_wcsichr(m_TempFormats.DateFormat, L'h') || !wcschr(m_TempFormats.DateFormat, L'm')){
			MessageBoxW(hwnd, m_sInvalidDate, L"Invalid format", MB_OK);
			return false;
		}
		wcscpy(g_DTFormats.DateFormat, m_TempFormats.DateFormat);
		WritePrivateProfileStringW(S_FORMAT, L"date_format", m_TempFormats.DateFormat, g_NotePaths.INIFile);
	}
	if(wcscmp(m_TempFormats.TimeFormat, g_DTFormats.TimeFormat) != 0){
		if(!_wcsichr(m_TempFormats.TimeFormat, L'h') || !wcschr(m_TempFormats.TimeFormat, L'm')){
			MessageBoxW(hwnd, m_sInvalidTime, L"Invalid format", MB_OK);
			return false;
		}
		wcscpy(g_DTFormats.TimeFormat, m_TempFormats.TimeFormat);
		WritePrivateProfileStringW(S_FORMAT, L"time_format", m_TempFormats.TimeFormat, g_NotePaths.INIFile);
	}
	SetShowDateTime(m_Dialogs.hSchedule);

	//default skinless size
	if(g_RTHandles.szDef.cx != m_TempRHandles.szDef.cx)
		fW = true;
	if(g_RTHandles.szDef.cy != m_TempRHandles.szDef.cy)
		fH = true;
	if(fW || fH){
		if(m_TempRHandles.szDef.cx < DEF_SIZE_MIN || m_TempRHandles.szDef.cx > DEF_SIZE_MAX 
		|| m_TempRHandles.szDef.cy < DEF_SIZE_MIN || m_TempRHandles.szDef.cy > DEF_SIZE_MAX){
			wcscpy(szMessage, m_sDefSize);
			wcscat(szMessage, L" ");
			_itow(m_TempRHandles.szDef.cx, szKey, 10);
			wcscat(szMessage, szKey);
			wcscat(szMessage, L"x");
			_itow(m_TempRHandles.szDef.cy, szKey, 10);
			wcscat(szMessage, szKey);
			wcscat(szMessage, L"?");
			if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDNO)
				fSetDefSize = false;
		}
	}
	if(fSetDefSize){
		if(fW){
			g_RTHandles.szDef.cx = m_TempRHandles.szDef.cx;
			_itow(g_RTHandles.szDef.cx, szKey, 10);
			WritePrivateProfileStringW(S_DEFSIZE, L"w", szKey, g_NotePaths.INIFile);
			//change diary size accordingly
			g_DiaryRTHandles.szDef.cx = g_RTHandles.szDef.cx;
		}
		if(fH){
			g_RTHandles.szDef.cy = m_TempRHandles.szDef.cy;
			_itow(g_RTHandles.szDef.cy, szKey, 10);
			WritePrivateProfileStringW(S_DEFSIZE, L"h", szKey, g_NotePaths.INIFile);
			//change diary size accordingly
			g_DiaryRTHandles.szDef.cy = g_RTHandles.szDef.cy;
		}
		if(g_DockSettings.fCustSize == false && m_TempDockingSettings.fCustSize == false){
			fDockSizeChanged = true;
		}
	}
	
	//skin changed
	fSkinBefore = g_RTHandles.hbSkin ? true : false;
	if(g_RTHandles.hbSkin != m_TempRHandles.hbSkin){	
		if(m_TempRHandles.hbSkin){
			fReload = g_RTHandles.crMask == m_TempRHandles.crMask ? false : true;
			wcscpy(g_Appearance.szSkin, m_sTempSkinFile);
			GetSkinProperties(hwnd, &g_RTHandles, g_Appearance.szSkin, true);
			WritePrivateProfileStringW(S_APPEARANCE, S_SKIN, g_Appearance.szSkin, g_NotePaths.INIFile);
			if(fSkinBefore){
				if(!fReload){
					ApplyNewCommonSkin();
				}
			}
			else{
				fReload = true;
			}
		}
		else{
			g_Appearance.szSkin[0] = '\0';
			g_RTHandles.hbSkin = NULL;
			WritePrivateProfileStringW(S_APPEARANCE, S_SKIN, NULL, g_NotePaths.INIFile);

			for(LPPNGROUP ppg = g_PGroups; ppg; ppg = ppg->next){
				if(!ppg->customSkin){
					ppg->szSkin[0] = '\0';
				}
			}

			fReload = true;
		}
	}

	//font changed
	if(!IsFontsEqual(&g_Appearance.lfFont, &m_TempAppearance.lfFont)){	
		// memcpy(&g_Appearance.lfFont, &m_TempAppearance.lfFont, sizeof(LOGFONTW));
		g_Appearance.lfFont = m_TempAppearance.lfFont;
		DeleteFont(g_RTHandles.hFont);
		// memcpy(&g_RTHandles, &m_TempRHandles, sizeof(NOTE_RTHANDLES));
		g_RTHandles = m_TempRHandles;
		WritePrivateProfileStructW(S_APPEARANCE, S_FONT, &g_Appearance.lfFont, sizeof(g_Appearance.lfFont), g_NotePaths.INIFile);
		ApplyNewFont();	
	}

	//font color changed
	if(g_Appearance.crFont != m_TempAppearance.crFont){
		g_Appearance.crFont = m_TempAppearance.crFont;
		_ltow(g_Appearance.crFont, szColor, 10);
		WritePrivateProfileStringW(S_APPEARANCE, S_COLOR, szColor, g_NotePaths.INIFile);
		ApplyNewTextColor();
	}

	//window caption font changed
	if(!IsFontsEqual(&g_Appearance.lfCaption, &m_TempAppearance.lfCaption)){

		// memcpy(&g_Appearance.lfCaption, &m_TempAppearance.lfCaption, sizeof(LOGFONTW));
		g_Appearance.lfCaption = m_TempAppearance.lfCaption;
		DeleteFont(g_RTHandles.hFCaption);
		// memcpy(&g_RTHandles, &m_TempRHandles, sizeof(NOTE_RTHANDLES));
		g_RTHandles = m_TempRHandles;
		WritePrivateProfileStructW(S_APPEARANCE, S_CAPTION_FONT, &g_Appearance.lfCaption, sizeof(g_Appearance.lfCaption), g_NotePaths.INIFile);
		ApplyNewCaptionFont();
	}
	
	//window caption font color changed
	if(g_Appearance.crCaption != m_TempAppearance.crCaption){
		g_Appearance.crCaption = m_TempAppearance.crCaption;
		_ltow(g_Appearance.crCaption, szColor, 10);
		WritePrivateProfileStringW(S_APPEARANCE, S_CAPTION_COLOR, szColor, g_NotePaths.INIFile);
		ApplyNewCaptionColor();
	}

	//window background color changed
	if(g_Appearance.crWindow != m_TempAppearance.crWindow){
		g_Appearance.crWindow = m_TempAppearance.crWindow;
		_ltow(g_Appearance.crWindow, szColor, 10);
		WritePrivateProfileStringW(S_APPEARANCE, S_BCKG_COLOR, szColor, g_NotePaths.INIFile);
		ApplyNewBackgroundColor();
	}

	//dock skin settings changed
	if(g_DockSettings.fCustSkin != m_TempDockingSettings.fCustSkin){
		fDockSkinChanged = true;
		g_DockSettings.fCustSkin = m_TempDockingSettings.fCustSkin;
		if(g_DockSettings.fCustSkin){
			wcscpy(g_DockSettings.szCustSkin, m_TempDockingSettings.szCustSkin);
			GetDockSkinProperties(m_Dialogs.hDocks, &g_DRTHandles, m_TempDockingSettings.szCustSkin, false);
		}
		//TODO
	}
	else{
		if(g_DockSettings.fCustSkin){
			if(wcscmp(g_DockSettings.szCustSkin, m_TempDockingSettings.szCustSkin) != 0){
				fDockSkinChanged = true;
				wcscpy(g_DockSettings.szCustSkin, m_TempDockingSettings.szCustSkin);
				GetDockSkinProperties(m_Dialogs.hDocks, &g_DRTHandles, m_TempDockingSettings.szCustSkin, true);
			}
		}
	}
	if(fDockSkinChanged){
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
		ApplyNewDockSkin();
	}
	//dock skinless size changed
	if(g_DockSettings.fCustSize == false && m_TempDockingSettings.fCustSize == true){
		g_DockSettings.fCustSize = m_TempDockingSettings.fCustSize;
		// memcpy(&g_DockSettings.custSize, &m_TempDockingSettings.custSize, sizeof(SIZE));
		g_DockSettings.custSize = m_TempDockingSettings.custSize;
		fDockSizeChanged = true;	
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
	}
	else if(g_DockSettings.fCustSize == true && m_TempDockingSettings.fCustSize == true){
		if(g_DockSettings.custSize.cx != m_TempDockingSettings.custSize.cx || 
			g_DockSettings.custSize.cy != m_TempDockingSettings.custSize.cy){
			// memcpy(&g_DockSettings.custSize, &m_TempDockingSettings.custSize, sizeof(SIZE));;
			g_DockSettings.custSize = m_TempDockingSettings.custSize;
			fDockSizeChanged = true;
			WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
		}
	}
	else if(g_DockSettings.fCustSize == true && m_TempDockingSettings.fCustSize == false){
		g_DockSettings.fCustSize = m_TempDockingSettings.fCustSize;
		fDockSizeChanged = true;
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
	}
	//dock distance changed
	if(g_DockSettings.dist != m_TempDockingSettings.dist){
		g_DockSettings.dist = m_TempDockingSettings.dist;
		fDockSizeChanged = true;
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
	}
	if(fDockSizeChanged){
		ApplyDockSizeOrderChanged();
	}
	//dock order changed
	if(g_DockSettings.fInvOrder != m_TempDockingSettings.fInvOrder){
		g_DockSettings.fInvOrder = m_TempDockingSettings.fInvOrder;
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
		ApplyDockSizeOrderChanged();
	}
	//docking caption font
	if(!g_DockSettings.fCustCaption && m_TempDockingSettings.fCustCaption){
		//no cust caption before and cust caption after
		g_DRTHandles.hFCaption = m_TempDRTHandles.hFCaption;
		g_DockSettings.crCaption = m_TempDockingSettings.crCaption;
		g_DockSettings.lfCaption = m_TempDockingSettings.lfCaption;
		fDockCaptionFontChanged = true;
	}
	else if(g_DockSettings.fCustCaption && m_TempDockingSettings.fCustCaption){
		//cust caption before and cust caption after
		if(g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption){
			DeleteFont(g_DRTHandles.hFCaption);
			g_DRTHandles.hFCaption = m_TempDRTHandles.hFCaption;
		}
		g_DockSettings.crCaption = m_TempDockingSettings.crCaption;
		g_DockSettings.lfCaption = m_TempDockingSettings.lfCaption;
		fDockCaptionFontChanged = true;
	}
	if(g_DockSettings.crCaption != m_TempDockingSettings.crCaption){
		g_DockSettings.crCaption = m_TempDockingSettings.crCaption;
		fDockCaptionFontChanged = true;
	}
	if(g_DockSettings.fCustCaption != m_TempDockingSettings.fCustCaption){
		g_DockSettings.fCustCaption = m_TempDockingSettings.fCustCaption;
		fDockCaptionFontChanged = true;
	}
	if(fDockCaptionFontChanged){
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
		ApplyDockingColors();
	}
	//docking bg color
	if(g_DockSettings.fCustColor != m_TempDockingSettings.fCustColor){
		g_DockSettings.fCustColor = m_TempDockingSettings.fCustColor;
		fDockBGColorChanged = true;
	}
	if(g_DockSettings.crWindow != m_TempDockingSettings.crWindow){
		g_DockSettings.crWindow = m_TempDockingSettings.crWindow;
		fDockBGColorChanged = true;
	}
	if(fDockBGColorChanged){
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), g_NotePaths.INIFile);
		ApplyDockingColors();
	}

	fChanged = false;
	if(g_NoteSettings.onStartup != m_TempNoteSettings.onStartup){
		g_NoteSettings.onStartup = m_TempNoteSettings.onStartup;			//store startup settings
		fChanged = true;
		if(m_TempNoteSettings.onStartup){
			//create shortcut in Startup directory
			CreatePNotesShortcut();
		}
		else{
			if(SHGetSpecialFolderPath(NULL, szLink, CSIDL_STARTUP, false)){
				//remove shortcut from Startup directory
				strcat(szLink, LINK_NAME);
				remove(szLink);
			}
		}
	}
	//next settings
	if(g_NextSettings.cleanDays != m_TempNextSettings.cleanDays){
		g_NextSettings.cleanDays = m_TempNextSettings.cleanDays;
		fNextSettingsChanged = true;
	}
	if(g_NextSettings.flags2 != m_TempNextSettings.flags2){
		g_NextSettings.flags2 = m_TempNextSettings.flags2;
		fNextSettingsChanged = true;
	}
	if(g_NextSettings.flags1 != m_TempNextSettings.flags1){
		if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK) && !IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK))
			fStopWinsock = true;
		else if(!IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK) && IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK))
			fStartWinsock = true;
		if((IsBitOn(g_NextSettings.flags1, SB3_BIG_ICONS) && !IsBitOn(m_TempNextSettings.flags1, SB3_BIG_ICONS)) 
		|| (!IsBitOn(g_NextSettings.flags1, SB3_BIG_ICONS) && IsBitOn(m_TempNextSettings.flags1, SB3_BIG_ICONS))){
			fReloadCP = true;			
		}
		if((IsBitOn(g_NextSettings.flags1, SB3_HIDE_HIDE) && !IsBitOn(m_TempNextSettings.flags1, SB3_HIDE_HIDE)) 
		|| (!IsBitOn(g_NextSettings.flags1, SB3_HIDE_HIDE) && IsBitOn(m_TempNextSettings.flags1, SB3_HIDE_HIDE))){
			fReload = true;			
		}
		if((IsBitOn(g_NextSettings.flags1, SB3_DONOT_SHOW_CONTENT) && !IsBitOn(m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONTENT)) 
		|| (!IsBitOn(g_NextSettings.flags1, SB3_DONOT_SHOW_CONTENT) && IsBitOn(m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONTENT))){
			fUpdateCP = true;			
		}
		g_NextSettings.flags1 = m_TempNextSettings.flags1;
		fNextSettingsChanged = true;
	}
	if(g_NextSettings.cleanDays > 0 && m_TempNextSettings.cleanDays == 0){
		KillTimer(g_hMain, TIMER_CLEAN_BIN_ID);
		m_TimerCleanBin = 0;
	}
	else if(g_NextSettings.cleanDays == 0 && m_TempNextSettings.cleanDays > 0){
		m_TimerCleanBin = SetTimer(hwnd, TIMER_CLEAN_BIN_ID, CLEAN_BIN_INTERVAL, CleanBinTimerProc);
	}
	if(fNextSettingsChanged){
		WritePrivateProfileStructW(S_NOTE, IK_NEXT_SETTINGS, &g_NextSettings, sizeof(NEXT_SETTINGS), g_NotePaths.INIFile);
		if(g_hCPDialog && fReloadCP){
			SendMessageW(g_hCPDialog, WM_CLOSE, 0, 0);
			ShowControlPanel();
		}
		if(fStartWinsock){
			//initialize Winsock
			InitializeWinsock();
			//start listening
			StartWSServer(hwnd);
			GetHostProperties(g_wsLocalHost);
		}
		else if(fStopWinsock){
			//stop listening
			DeInitializeWinsock();
			while(g_wsServerUp){
				;
			}
		}
	}
	//socket port
	if(g_wsDefPort != m_TempWSPort){
		g_wsDefPort = m_TempWSPort;
		_itow(g_wsDefPort, szKey, 10);
		WritePrivateProfileStringW(S_NOTE, IK_DEF_SOCKET_PORT, szKey, g_NotePaths.INIFile);
		if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
			//stop listening
			DeInitializeWinsock();
			while(g_wsServerUp){
				;
			}
			InitializeWinsock();
			StartWSServer(g_hMain);
		}
	}

	//small values changed
	if(SmallValuesChanged()){
		if(GetSmallValue(g_SmallValues, SMS_MARGIN_SIZE) != GetSmallValue(m_TempSmallValues, SMS_MARGIN_SIZE))
			fMarginChanged = true;
		// memcpy(&g_SmallValues, &m_TempSmallValues, sizeof(SMALLVALUES));
		g_SmallValues = m_TempSmallValues;
		if(fMarginChanged && !g_RTHandles.hbSkin)
			ApplyNewMargins();
		WritePrivateProfileStructW(S_NOTE, IK_SMALL_VALUES, &g_SmallValues, sizeof(g_SmallValues), g_NotePaths.INIFile);
	}	
	//reserved changed
	if(g_NoteSettings.reserved2 != m_TempNoteSettings.reserved2){
		//default double click action
		BOOL	fDblClickChanged = CheckDblClickChanges();
		g_NoteSettings.reserved2 = m_TempNoteSettings.reserved2;
		if(fDblClickChanged){
			SetDefaultItemForDblClick();
		}
		fChanged = true;
	}

	if(g_NoteSettings.reserved1 != m_TempNoteSettings.reserved1){
		if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_DELETE) != IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_DELETE)){
			fReload = true;
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CROSS_INST_TRNGL) != IsBitOn(m_TempNoteSettings.reserved1, SB1_CROSS_INST_TRNGL)){
			fReload = true;
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_ADD_WEEKDAY_DIARY) != IsBitOn(m_TempNoteSettings.reserved1, SB1_ADD_WEEKDAY_DIARY) 
		|| IsBitOn(g_NoteSettings.reserved1, SB1_FULL_WEEKDAY_NAME) != IsBitOn(m_TempNoteSettings.reserved1, SB1_FULL_WEEKDAY_NAME) 
		|| IsBitOn(g_NoteSettings.reserved1, SB1_WEEKDAY_DIARY_END) != IsBitOn(m_TempNoteSettings.reserved1, SB1_WEEKDAY_DIARY_END)){
			fDiaryFormatChanged = true;
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED) != IsBitOn(m_TempNoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
			GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szBuffer, 256, g_NotePaths.INIFile);
			//no encryption before and encryption after
			if(IsBitOn(m_TempNoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
				if(wcscmp(szBuffer, L"N/A") != 0)
					EncryptAll(szBuffer);
			}
			//encryption before and no ecnryption after
			else{
				if(wcscmp(szBuffer, L"N/A") != 0)
					DecryptAll(szBuffer);
			}
		}
		//custome fonts
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_FONTS) == true && IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_FONTS) == false){
			ClearCustomFonts();
			ApplyJustRedraw();
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_FONTS) == false && IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_FONTS) == true){
			RegisterCustomFonts();
			ApplyJustRedraw();
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_NO_ALT_TAB) != IsBitOn(m_TempNoteSettings.reserved1, SB1_NO_ALT_TAB))
			fReload = true;
		//if hiding tray icon is set when program is locked
		if(IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_TRAY) && !IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_TRAY)){
			//check whether hot key is set for lock/unlock menu item
			for(int i = 0; i < NELEMS(m_MainMenus); i++){
				if(m_Hotkeys[i].id == IDM_LOCK_PROGRAM){
					if(wcslen(m_Hotkeys[i].szKey) == 0){
						GetPrivateProfileStringW(S_MESSAGES, L"allow_hide_tray", L"In order to allow the tray icon to be hidden when program is locked, you have to set a hot key for 'Lock Program' menu item.", szMessage, 279, g_NotePaths.CurrLanguagePath);
						//if no hot key is set - prohibit hiding tary icon
						MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK | MB_ICONERROR);
						BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_TRAY);
					}
					break;
				}
			}
		}
		fChanged = true;
	}
	if(g_NoteSettings.checkOnStart != m_TempNoteSettings.checkOnStart){
		g_NoteSettings.checkOnStart = m_TempNoteSettings.checkOnStart;			//store startup settings
		fChanged = true;
	}
	// if(g_NoteSettings.showOnStart != m_TempNoteSettings.showOnStart){
		// g_NoteSettings.showOnStart = m_TempNoteSettings.showOnStart;			//store startup settings
		// fChanged = true;
	// }
	if(g_NoteSettings.saveOnExit != m_TempNoteSettings.saveOnExit){
		g_NoteSettings.saveOnExit = m_TempNoteSettings.saveOnExit;			//store startup settings
		fChanged = true;
	}
	if(g_NoteSettings.confirmSave != m_TempNoteSettings.confirmSave){
		g_NoteSettings.confirmSave = m_TempNoteSettings.confirmSave;			//store startup settings
		fChanged = true;
	}
	if(g_NoteSettings.confirmDelete != m_TempNoteSettings.confirmDelete){
		g_NoteSettings.confirmDelete = m_TempNoteSettings.confirmDelete;			//store deletion settings
		fChanged = true;
	}
	if(g_NoteSettings.newOnTop != m_TempNoteSettings.newOnTop){
		g_NoteSettings.newOnTop = m_TempNoteSettings.newOnTop;			//store new note settings
		fChanged = true;
	}
	if(g_NoteSettings.autoSValue != m_TempNoteSettings.autoSValue){
		fAutoSVChanged = true;
		g_NoteSettings.autoSValue = m_TempNoteSettings.autoSValue;
		fChanged = true;
	}
	if(g_NoteSettings.autoSave != m_TempNoteSettings.autoSave){
		g_NoteSettings.autoSave = m_TempNoteSettings.autoSave;
		if(g_NoteSettings.autoSave){
			m_TimerAutosave = SetTimer(g_hMain, TIMER_AUTOSAVE_ID, g_NoteSettings.autoSValue * 1000 * 60, AutosaveTimerProc);
		}
		else{
			//kill autosave timer
			if(m_TimerAutosave){
				KillTimer(g_hMain, m_TimerAutosave);
				m_TimerAutosave = 0;
			}
		}
		fChanged = true;
	}
	else{
		if(fAutoSVChanged && g_NoteSettings.autoSave){
			//kill autosave timer
			if(m_TimerAutosave){
				KillTimer(g_hMain, TIMER_AUTOSAVE_ID);
				m_TimerAutosave = 0;
			}
			m_TimerAutosave = SetTimer(g_hMain, TIMER_AUTOSAVE_ID, g_NoteSettings.autoSValue * 1000 * 60, AutosaveTimerProc);
		}
	}
	//visual alert
	if(g_NoteSettings.visualAlert != m_TempNoteSettings.visualAlert){
		g_NoteSettings.visualAlert = m_TempNoteSettings.visualAlert;
		fChanged = true;
	}
	//roll on double click
	if(g_NoteSettings.rollOnDblClick != m_TempNoteSettings.rollOnDblClick){
		g_NoteSettings.rollOnDblClick = m_TempNoteSettings.rollOnDblClick;
		fChanged = true;
	}
	//scrollbar
	if(g_NoteSettings.showScrollbar != m_TempNoteSettings.showScrollbar){
		g_NoteSettings.showScrollbar = m_TempNoteSettings.showScrollbar;
		fReload = true;
		fChanged = true;
	}
	
	//diary settings
	if(!IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS) && IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
		//no custom diary settings before, custom diary settings after
		fDiaryChanged = true;
		if(g_DiaryAppearance.crCaption != m_TempDiaryAppearance.crCaption)
			fDCaptionColorChanged = true;
		if(g_DiaryAppearance.crWindow != m_TempDiaryAppearance.crWindow)
			fDBackColorChanged = true;
		if(!IsFontsEqual(&g_DiaryAppearance.lfCaption, &m_TempDiaryAppearance.lfCaption))
			fDCaptionFontChanged = true;
		if(wcscmp(g_DiaryAppearance.szSkin, m_TempDiaryAppearance.szSkin) != 0)
			fDSkinChanged = true;
		// memcpy(&g_DiaryAppearance, &m_TempDiaryAppearance, sizeof(NOTE_APPEARANCE));
		g_DiaryAppearance = m_TempDiaryAppearance;
		ClearRTHandles(&g_DiaryRTHandles);
		GetDiarySkin(g_DiaryAppearance, &g_DiaryRTHandles);
		g_DiaryRTHandles.hFCaption = CreateFontIndirectW(&g_DiaryAppearance.lfCaption);
		g_DiaryRTHandles.hFont = CreateFontIndirectW(&g_DiaryAppearance.lfFont);
	}
	if(!fDiaryChanged){
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS) && !IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
			//custom diary settings before, no custom diary settings after
			fDiaryChanged = true;
			if(g_DiaryAppearance.crCaption != m_TempDiaryAppearance.crCaption)
				fDCaptionColorChanged = true;
			if(g_DiaryAppearance.crWindow != m_TempDiaryAppearance.crWindow)
				fDBackColorChanged = true;
			if(!IsFontsEqual(&g_DiaryAppearance.lfCaption, &m_TempDiaryAppearance.lfCaption))
				fDCaptionFontChanged = true;
			if(wcscmp(g_DiaryAppearance.szSkin, m_TempDiaryAppearance.szSkin) != 0)
				fDSkinChanged = true;
			// memcpy(&g_DiaryAppearance, &m_TempDiaryAppearance, sizeof(NOTE_APPEARANCE));
			g_DiaryAppearance = m_TempDiaryAppearance;
			ClearRTHandles(&g_DiaryRTHandles);
			GetDiarySkin(g_DiaryAppearance, &g_DiaryRTHandles);
			g_DiaryRTHandles.hFCaption = CreateFontIndirectW(&g_DiaryAppearance.lfCaption);
			g_DiaryRTHandles.hFont = CreateFontIndirectW(&g_DiaryAppearance.lfFont);
		}
		else{
			//same custom settings before and after
			if(g_DiaryAppearance.crCaption != m_TempDiaryAppearance.crCaption){
				fDCaptionColorChanged = true;
				fDiaryChanged = true;
			}
			if(g_DiaryAppearance.crWindow != m_TempDiaryAppearance.crWindow){
				fDBackColorChanged = true;
				fDiaryChanged = true;
			}
			if(!IsFontsEqual(&g_DiaryAppearance.lfCaption, &m_TempDiaryAppearance.lfCaption)){
				fDCaptionFontChanged = true;
				fDiaryChanged = true;
			}
			if(wcscmp(g_DiaryAppearance.szSkin, m_TempDiaryAppearance.szSkin) != 0){
				fDSkinChanged = true;
				fDiaryChanged = true;
			}
			if(fDiaryChanged){
				// memcpy(&g_DiaryAppearance, &m_TempDiaryAppearance, sizeof(NOTE_APPEARANCE));
				g_DiaryAppearance = m_TempDiaryAppearance;
				ClearRTHandles(&g_DiaryRTHandles);
				GetDiarySkin(g_DiaryAppearance, &g_DiaryRTHandles);
				g_DiaryRTHandles.hFCaption = CreateFontIndirectW(&g_DiaryAppearance.lfCaption);
				g_DiaryRTHandles.hFont = CreateFontIndirectW(&g_DiaryAppearance.lfFont);
			}
		}
	}
	
	if(!IsBitOn(g_NoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK) && IsBitOn(m_TempNoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK)){
		ToggleAeroPeek(true);
	}
	else if(IsBitOn(g_NoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK) && !IsBitOn(m_TempNoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK)){
		ToggleAeroPeek(false);
	}

	g_NoteSettings.reserved1 = m_TempNoteSettings.reserved1;			//store reserved1 settings

	if(g_NoteSettings.transAllow != m_TempNoteSettings.transAllow){		//allow transparency changed
		g_NoteSettings.transAllow = m_TempNoteSettings.transAllow;
		g_NoteSettings.transValue = m_TempNoteSettings.transValue;
		if(!g_NoteSettings.transAllow)		//becomes not allowed
			ApplyRestrictTransparency();
		else							//becomes allowed
			ApplyTransparencyValue();
		WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), g_NotePaths.INIFile);
	}
	else if(g_NoteSettings.transValue != m_TempNoteSettings.transValue){
		g_NoteSettings.transValue = m_TempNoteSettings.transValue;
		if(g_NoteSettings.transAllow){
			ApplyTransparencyValue();
		}
		WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), g_NotePaths.INIFile);
	}
	else if(fChanged)		//if only startup settings changed
		WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), g_NotePaths.INIFile);

	if(fReload)
		GetSkinProperties(hwnd, &g_RTHandles, g_Appearance.szSkin, true);

	if(fDiaryFormatChanged){
		//change names of all diary notes
		ChangeAllDiaryNames(m_DiaryFormats);		
		if(g_hCPDialog){
			//apply new language
			SendMessageW(g_hCPDialog, PNM_CTRL_UPD_LANG, 0, 0);
			SendMessageW(g_hCPDialog, PNM_CTRL_DIARY_CHANGE_LANG, 0, 0);
		}
	}

	if(fDiaryChanged){
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS))
			WritePrivateProfileStructW(S_DIARY, IK_DIARY_APPEARANCE, &g_DiaryAppearance, sizeof(NOTE_APPEARANCE), g_NotePaths.INIFile);
		if(!fReload){
			//apply new diary style
			if(wcslen(g_Appearance.szSkin) == 0 || wcscmp(g_Appearance.szSkin, DS_NO_SKIN) == 0){
				if(fDBackColorChanged){
					ApplyNewDiaryBackgroundColor();
				}
				if(fDCaptionColorChanged){
					ApplyNewDiaryCaptionColor();
				}
				if(fDCaptionFontChanged){
					ApplyNewDiaryCaptionFont();
				}
			}
			else{
				if(fDSkinChanged){
					ApplyNewDiarySkin();
				}
			}
		}
	}
	EnableMainMenus();
	RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
	if(fUpdateCP && g_hCPDialog)
		SendMessageW(g_hCPDialog, PNM_RELOAD, 0, 0);
	return fReload;
}

static BOOL SmallValuesChanged(void){
	LPBYTE				pS1 = (LPBYTE)&g_SmallValues, pS2 = (LPBYTE)&m_TempSmallValues;

	for(int i = 0; i < sizeof(SMALLVALUES); i++, pS1++, pS2++){
		if(*pS1 != *pS2)
			return true;
	}
	return false;
}

static BOOL CALLBACK DockAllProc(HWND hwnd, LPARAM lParam){
	wchar_t				szClass[256];
	P_NOTE_DATA			pD;
	dock_type			dockType;

	GetClassNameW(hwnd, szClass, 256);
	if(wcscmp(szClass, NWC_SKINNABLE_CLASS) == 0){
		if(IsWindowVisible(hwnd)){
			pD = NoteData(hwnd);
			dockType = HIWORD(lParam);
			if(DockType(pD->dockData) != dockType){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(LOWORD(lParam), 0), 0);
			}
		}
	}

	return true;
}

static BOOL CALLBACK HideShowWhenLockedProc(HWND hwnd, LPARAM lParam){
	wchar_t				szClass[256];

	GetClassNameW(hwnd, szClass, 256);
	if(wcscmp(szClass, NWC_SKINNABLE_CLASS) == 0){
		ShowWindow(hwnd, lParam);
	}

	return true;
}

static void ShowAllNotes(void){
	for(LPPNGROUP pGroup = g_PGroups; pGroup; pGroup = pGroup->next){
		if((pGroup->id > GROUP_RECYCLE || pGroup->id == GROUP_INCOMING) && IsAnyHiddenInGroup(pGroup->id)){
			if(UnlockGroup(pGroup->id)){
				for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
					if(pNote->pData->idGroup == pGroup->id){
						if(IsBitOn(g_NextSettings.flags1, SB3_HIDE_COMPLETED)){
							if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
								continue;
							}
						}
						if(UnlockNote(pNote))
							ShowNote(pNote);
					}
				}
			}
		}
	}
	if(IsAnyHiddenInGroup(GROUP_DIARY)){
		if(UnlockGroup(GROUP_DIARY)){
			for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
				if(pNote->pData->idGroup == GROUP_DIARY){
					if(IsBitOn(g_NextSettings.flags1, SB3_HIDE_COMPLETED)){
						if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
							continue;
						}
					}
					if(UnlockNote(pNote))
						ShowNote(pNote);
				}
			}
		}
	}
}

static void AddNotifyIcon(void){
	//add tray notify icon and set it up

	m_InTray = true;
	
	// if(m_ShellVersion == WV_VISTA_AND_MORE){
		// m_nData.cbSize = sizeof(PN_NOTIFYICONDATAW);
	// }
	// else if(m_ShellVersion == WV_XP){
		// m_nData.cbSize = PNNOTIFYICONDATAW_V3_SIZE;
	// }
	// else if(m_ShellVersion == WV_PRE_XP){
		// m_nData.cbSize = PNNOTIFYICONDATAW_V2_SIZE;
	// }
	m_nData.cbSize = (m_NewDLLVersion) ? sizeof(NOTIFYICONDATAW) : NOTIFYICONDATAW_V1_SIZE;
	
	// m_nData.uVersion = 4; //NOTIFYICON_VERSION_4;
	// Shell_NotifyIconW(NIM_SETVERSION, &m_nData);

	m_nData.hWnd = g_hMain;
	m_nData.uID = IDI_TRAY;
	m_nData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nData.uCallbackMessage = WM_SHELLNOTIFY;
	
	m_nData.hIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
	wcscpy(m_nData.szTip, g_NotePaths.ProgName);
	Shell_NotifyIconW(NIM_ADD, &m_nData);
}


static void MoveOptionsDialogs(int x, int y){
	HWND		*lpDlg;
	int			size;

	size = sizeof(m_Dialogs) / sizeof(HWND);
	lpDlg = (HWND *)&m_Dialogs;
	for(int i = 0; i < size ; i++){
		SetWindowPos(*lpDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
		lpDlg++;
	}
}

static void AddChecksNetwork(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 16;
	tvs.item.iSelectedImage = 16;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_SYNC);
	m_ChecksTNetwork[ntSyncRoot] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTNetwork[ntSyncRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_INC_DEL_SYNC);
	m_ChecksTNetwork[ntIncDelSync] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntIncDelSync], IsBitOn(m_TempNoteSettings.reserved1, SB1_INC_DEL_IN_SYNC));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_SYNC_ON_START);
	m_ChecksTNetwork[ntSyncStart] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntSyncStart], IsBitOn(m_TempNoteSettings.reserved1, SB1_SYNC_ON_START));
	
	tvs.item.lParam = MAKELONG(1, IDC_CHK_SAVE_BFR_SYNC);
	m_ChecksTNetwork[ntSaveBeforeSync] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSync], IsBitOn(m_TempNoteSettings.reserved1, SB1_SAVE_BEFORE_SYNC));

	tvs.hParent = TVI_ROOT;
	tvs.item.iImage = 17;
	tvs.item.iSelectedImage = 17;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_EXCHANGE);
	m_ChecksTNetwork[ntExchRoot] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTNetwork[ntExchRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_ENABLE_NETWORK);
	m_ChecksTNetwork[ntEnableNetwork] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntEnableNetwork], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_SAVE_EXCHANGE);
	m_ChecksTNetwork[ntSaveBeforeSending] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSending], IsBitOn(m_TempNextSettings.flags1, SB3_SAVE_BEFORE_SEND));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSending], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_NOT_EXC_NOTIFY);
	m_ChecksTNetwork[ntNotShowNotifyNewMessage] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage], IsBitOn(m_TempNextSettings.flags1, SB3_RECEIVE_HIDE_BALOON));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_NOT_EXC_SOUND);
	m_ChecksTNetwork[ntNotPlaySoundNewMessage] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotPlaySoundNewMessage], IsBitOn(m_TempNextSettings.flags1, SB3_RECEIVE_NOT_PLAY_SOUND));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotPlaySoundNewMessage], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_NOT_SEND_BALOON);
	m_ChecksTNetwork[ntNotShowSendMessage] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowSendMessage], IsBitOn(m_TempNextSettings.flags1, SB3_SEND_HIDE_BALOON));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowSendMessage], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_SHOW_IMMED);
	m_ChecksTNetwork[ntShowAfterReceiving] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntShowAfterReceiving], IsBitOn(m_TempNextSettings.flags1, SB3_SHOW_AFTER_RECEIVING));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowAfterReceiving], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_HIDE_AFTER_SEND);
	m_ChecksTNetwork[ntHideAfterSend] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntHideAfterSend], IsBitOn(m_TempNextSettings.flags1, SB3_HIDE_AFTER_SEND));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntHideAfterSend], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_HIDE_CONT_MENU);
	m_ChecksTNetwork[ntNotShowContactsInMenu] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowContactsInMenu], IsBitOn(m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONT_MENU));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowContactsInMenu], IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.hParent = m_ChecksTNetwork[ntNotShowNotifyNewMessage];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_BALOON_SHOW_N);
	m_ChecksTNetwork[ntShowNoteClickNotify] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntShowNoteClickNotify], IsBitOn(m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_NOTE));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowNoteClickNotify], !CTreeView_GetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage]) && IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_BALOON_SHOW_CP);
	m_ChecksTNetwork[ntShowCPClickNotify] = (HTREEITEM)SendMessageW(m_hTreeNetwork, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntShowCPClickNotify], IsBitOn(m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_CP));
	CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowCPClickNotify], !CTreeView_GetCheckBoxState(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage]) && IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK));

	TreeView_Expand(m_hTreeNetwork, m_ChecksTNetwork[ntSyncRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeNetwork, m_ChecksTNetwork[ntExchRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage], TVE_EXPAND);

	SendMessageW(m_hTreeNetwork, TVM_ENSUREVISIBLE, 0, (LPARAM)m_ChecksTBehavior[ntSyncRoot]);
}

static void AddChecksProtection(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 11;
	tvs.item.iSelectedImage = 11;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_ENCRYPTION);
	m_ChecksTProtection[prEncryptRoot] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	
	tvs.item.iImage = 12;
	tvs.item.iSelectedImage = 12;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_LOCK);
	m_ChecksTProtection[prLocked] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.item.iImage = 13;
	tvs.item.iSelectedImage = 13;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_BACKUP);
	m_ChecksTProtection[prBackup] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.item.iImage = 5;
	tvs.item.iSelectedImage = 5;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_BEH_MISC);
	m_ChecksTProtection[prProtectMisc] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTProtection[prEncryptRoot];
	if(!IsPasswordSet())
		tvs.item.lParam = MAKELONG(0, IDC_CHK_DECRYPT_NOTES);
	else
		tvs.item.lParam = MAKELONG(1, IDC_CHK_DECRYPT_NOTES);
	m_ChecksTProtection[prEncryption] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prEncryption], IsBitOn(m_TempNoteSettings.reserved1, SB1_STORE_ENCRYPTED));

	tvs.hParent = m_ChecksTProtection[prLocked];
	if(!IsPasswordSet())
		tvs.item.lParam = MAKELONG(0, IDC_CHK_HIDE_TRAY);
	else
		tvs.item.lParam = MAKELONG(1, IDC_CHK_HIDE_TRAY);
	m_ChecksTProtection[prHideFromTray] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prHideFromTray], IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_TRAY));

	tvs.hParent = m_ChecksTProtection[prBackup];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_BACKUP);
	m_ChecksTProtection[prBackupNotes] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prBackupNotes], IsBitOn(m_TempNoteSettings.reserved1, SB1_USE_BACKUP));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_SILENT_BACKUP);
	m_ChecksTProtection[prSilentBackup] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prSilentBackup], IsBitOn(m_TempNoteSettings.reserved1, SB1_SILENT_BACKUP));
	
	tvs.hParent = m_ChecksTProtection[prProtectMisc];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_DN_SHOW_CONT);
	m_ChecksTProtection[prDontShowContent] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prDontShowContent], IsBitOn(m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONTENT));

	tvs.hParent = TVI_ROOT;
	tvs.item.iImage = 16;
	tvs.item.iSelectedImage = 16;
	tvs.item.lParam = MAKELONG(1, IDC_CHK_LOCAL_SYNC_ROOT);
	m_ChecksTProtection[prSyncRoot] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTProtection[prSyncRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_LOCAL_SYNC_REC);
	m_ChecksTProtection[prSyncLocalBin] = (HTREEITEM)SendMessageW(m_hTreeProtection, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeProtection, m_ChecksTProtection[prSyncLocalBin], IsBitOn(m_TempNextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE));

	TreeView_Expand(m_hTreeProtection, m_ChecksTProtection[prEncryptRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeProtection, m_ChecksTProtection[prLocked], TVE_EXPAND);
	TreeView_Expand(m_hTreeProtection, m_ChecksTProtection[prBackup], TVE_EXPAND);
	TreeView_Expand(m_hTreeProtection, m_ChecksTProtection[prProtectMisc], TVE_EXPAND);
	TreeView_Expand(m_hTreeProtection, m_ChecksTProtection[prSyncRoot], TVE_EXPAND);
}

static void AddChecksSchedule(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 10;
	tvs.item.iSelectedImage = 10;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_SCHEDULE_BHV);
	m_ChecksTSchedule[scScheduleRoot] = (HTREEITEM)SendMessageW(m_hTreeSchedule, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTSchedule[scScheduleRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_ALLOW_SOUND);
	m_ChecksTSchedule[scAllowSound] = (HTREEITEM)SendMessageW(m_hTreeSchedule, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scAllowSound], m_TempSound.allowSound);

	tvs.item.lParam = MAKELONG(1, IDC_VISUAL_NOTIFY);
	m_ChecksTSchedule[scVisualAlert] = (HTREEITEM)SendMessageW(m_hTreeSchedule, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scVisualAlert], m_TempNoteSettings.visualAlert);

	tvs.item.lParam = MAKELONG(1, IDC_CHK_OVERDUE);
	m_ChecksTSchedule[scTrackOverdue] = (HTREEITEM)SendMessageW(m_hTreeSchedule, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scTrackOverdue], IsBitOn(m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_DONOT_CENTER);
	m_ChecksTSchedule[scDontMove] = (HTREEITEM)SendMessageW(m_hTreeSchedule, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSchedule, m_ChecksTSchedule[scDontMove], IsBitOn(m_TempNoteSettings.reserved1, SB1_DONOT_CENTER));

	TreeView_Expand(m_hTreeSchedule, m_ChecksTSchedule[scScheduleRoot], TVE_EXPAND);
}

static void AddChecksAppearance(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 9;
	tvs.item.iSelectedImage = 9;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_APPEARANCE);
	m_ChecksTApp[apAppRoot] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTApp[apAppRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_TRANS);
	m_ChecksTApp[apTransparency] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apTransparency], m_TempNoteSettings.transAllow);

	tvs.item.lParam = MAKELONG(1, IDC_HIDE_TOOLBAR);
	m_ChecksTApp[apHideToolbar] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apHideToolbar], IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR));
	
	tvs.item.lParam = MAKELONG(1, IDC_USE_CUST_FONTS);
	m_ChecksTApp[apCustomFonts] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apCustomFonts], IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_FONTS));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_SCROLLBAR);
	m_ChecksTApp[apShowScroll] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apShowScroll], m_TempNoteSettings.showScrollbar);

	tvs.item.lParam = MAKELONG(1, IDC_HIDE_DELETE);
	m_ChecksTApp[apHideDelete] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apHideDelete], IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_DELETE));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_HIDE_HIDE);
	m_ChecksTApp[apHideHide] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apHideHide], IsBitOn(m_TempNextSettings.flags1, SB3_HIDE_HIDE));

	tvs.hParent = m_ChecksTApp[apHideDelete];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_CROSS_INST_TR);
	m_ChecksTApp[apCrossInstTrg] = (HTREEITEM)SendMessageW(m_hTreeApp, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], IsBitOn(m_TempNoteSettings.reserved1, SB1_CROSS_INST_TRNGL));
	CTreeView_SetEnable(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_DELETE));

	TreeView_Expand(m_hTreeApp, m_ChecksTApp[apAppRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeApp, m_ChecksTApp[apHideDelete], TVE_EXPAND);
}

static void AddChecksDiary(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 14;
	tvs.item.iSelectedImage = 14;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_CUST_DIARY);
	m_ChecksTDiary[drDiaryRoot] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTDiary[drDiaryRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_CUST_DIARY);
	m_ChecksTDiary[drDiaryCust] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drDiaryCust], IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_DIARY_WEEKDAY);
	m_ChecksTDiary[drAddWeekday] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drAddWeekday], IsBitOn(m_TempNoteSettings.reserved1, SB1_ADD_WEEKDAY_DIARY));

	tvs.hParent = m_ChecksTDiary[drAddWeekday];
	tvs.item.lParam = MAKELONG(CTreeView_GetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drAddWeekday]), IDC_CHK_FULL_WD_NAME);
	m_ChecksTDiary[drFullWeekday] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drFullWeekday], IsBitOn(m_TempNoteSettings.reserved1, SB1_FULL_WEEKDAY_NAME));

	tvs.item.lParam = MAKELONG(CTreeView_GetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drAddWeekday]), IDC_CHK_DR_WEEKDAY_END);
	m_ChecksTDiary[drWeekdayAtEnd] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drWeekdayAtEnd], IsBitOn(m_TempNoteSettings.reserved1, SB1_WEEKDAY_DIARY_END));

	tvs.hParent = m_ChecksTDiary[drDiaryRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_NO_DIARY_PAGES);
	m_ChecksTDiary[drNoPages] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drNoPages], IsBitOn(m_TempNoteSettings.reserved1, SB1_NO_DIARY_PAGES));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_DIARY_SORT_ASC);
	m_ChecksTDiary[drAscSort] = (HTREEITEM)SendMessageW(m_hTreeDiary, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDiary, m_ChecksTDiary[drAscSort], IsBitOn(m_TempNoteSettings.reserved1, SB1_DIARY_SORT_ASC));

	TreeView_Expand(m_hTreeDiary, m_ChecksTDiary[drDiaryRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeDiary, m_ChecksTDiary[drAddWeekday], TVE_EXPAND);
}

static void AddChecksDock(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 15;
	tvs.item.iSelectedImage = 15;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_DOCK_SKINS);
	m_ChecksTDock[dcSkinsRoot] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.item.iImage = 6;
	tvs.item.iSelectedImage = 6;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_DOCK_SKINLESS);
	m_ChecksTDock[dcSkinlessRoot] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.item.iImage = 5;
	tvs.item.iSelectedImage = 5;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_DOCK_MISC);
	m_ChecksTDock[dcMiscRoot] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTDock[dcSkinsRoot];
	tvs.item.lParam = MAKELONG(1, IDC_OPT_DOCK_RND_SKIN);
	m_ChecksTDock[dcCustomSkins] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustomSkins], m_TempDockingSettings.fCustSkin);

	tvs.hParent = m_ChecksTDock[dcSkinlessRoot];
	tvs.item.lParam = MAKELONG(1, IDC_OPT_DOCK_RND_SIZE);
	m_ChecksTDock[dcCustSize] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustSize], m_TempDockingSettings.fCustSize);

	tvs.item.lParam = MAKELONG(1, IDC_CHK_DOCK_C);
	m_ChecksTDock[dcCustColor] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustColor], m_TempDockingSettings.fCustColor);

	tvs.item.lParam = MAKELONG(1, IDC_CHK_DOCK_F);
	m_ChecksTDock[dcCustFont] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcCustFont], m_TempDockingSettings.fCustCaption);

	tvs.hParent = m_ChecksTDock[dcMiscRoot];
	tvs.item.lParam = MAKELONG(1, IDC_INV_DOCK_ORDER);
	m_ChecksTDock[dcInverseOrder] = (HTREEITEM)SendMessageW(m_hTreeDock, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeDock, m_ChecksTDock[dcInverseOrder], m_TempDockingSettings.fInvOrder);

	TreeView_Expand(m_hTreeDock, m_ChecksTDock[dcSkinsRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeDock, m_ChecksTDock[dcSkinlessRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeDock, m_ChecksTDock[dcMiscRoot], TVE_EXPAND);
}

static void AddChecksMisc(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 7;
	tvs.item.iSelectedImage = 7;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_START_OPT);
	m_ChecksTMisc[msStartupRoot] = (HTREEITEM)SendMessageW(m_hTreeMisc, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	tvs.item.iImage = 8;
	tvs.item.iSelectedImage = 8;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_EFFECTS);
	m_ChecksTMisc[msEffectsRoot] = (HTREEITEM)SendMessageW(m_hTreeMisc, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTMisc[msStartupRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_AUTORUN);
	m_ChecksTMisc[msStartWithWindows] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msStartWithWindows], m_TempNoteSettings.onStartup);

	tvs.item.lParam = MAKELONG(1, IDC_CHK_SHOW_CP);
	m_ChecksTMisc[msShowCPOnStart] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msShowCPOnStart], IsBitOn(m_TempNoteSettings.reserved1, SB1_SHOW_CP));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_NEW_V);
	m_ChecksTMisc[msCheckNewVOnStart] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msCheckNewVOnStart], m_TempNoteSettings.checkOnStart);

	tvs.hParent = m_ChecksTMisc[msEffectsRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_HIDE_FLUENTLY);
	m_ChecksTMisc[msHideFluently] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msHideFluently], IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_FLUENTLY));

	tvs.item.lParam = MAKELONG(1, IDC_CHK_PLAY_HIDE_SOUND);
	m_ChecksTMisc[msPlaySoundOnHide] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeMisc, m_ChecksTMisc[msPlaySoundOnHide], IsBitOn(m_TempNoteSettings.reserved1, SB1_PLAY_HIDE_SOUND));

	TreeView_Expand(m_hTreeMisc, m_ChecksTMisc[msStartupRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeMisc, m_ChecksTMisc[msEffectsRoot], TVE_EXPAND);
}

static void AddChecksSkins(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 6;
	tvs.item.iSelectedImage = 6;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_COMMON_SKNLESS);
	m_ChecksTSkins[skSkinlessRoot] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
		
	tvs.hParent = m_ChecksTSkins[skSkinlessRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_RANDOM_COLOR);
	m_ChecksTSkins[skRandomColor] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skRandomColor], IsBitOn(m_TempNoteSettings.reserved1, SB1_RANDCOLOR));

	tvs.hParent = m_ChecksTSkins[skRandomColor];
	tvs.item.lParam = MAKELONG(CTreeView_GetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skRandomColor]), IDC_CHK_INVERT_COLOR);
	m_ChecksTSkins[skInvertColor] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skInvertColor], IsBitOn(m_TempNoteSettings.reserved1, SB1_INVERT_TEXT));
	
	tvs.hParent = m_ChecksTSkins[skSkinlessRoot];
	tvs.item.lParam = MAKELONG(1, IDC_REDUCE_TO_CAPTION);
	m_ChecksTSkins[skRollUnroll] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skRollUnroll], m_TempNoteSettings.rollOnDblClick);

	tvs.hParent = m_ChecksTSkins[skRollUnroll];
	tvs.item.lParam = MAKELONG(CTreeView_GetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skRollUnroll]), IDC_CHK_FIT_TO_CAPTION);
	m_ChecksTSkins[skFittToCaption] = (HTREEITEM)SendMessageW(m_hTreeSkins, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeSkins, m_ChecksTSkins[skFittToCaption], IsBitOn(m_TempNoteSettings.reserved1, SB1_FIT_TO_CAPTION));

	TreeView_Expand(m_hTreeSkins, m_ChecksTSkins[skSkinlessRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeSkins, m_ChecksTSkins[skRandomColor], TVE_EXPAND);
	TreeView_Expand(m_hTreeSkins, m_ChecksTSkins[skRollUnroll], TVE_EXPAND);
}

static void AddChecksBehavior(void){
	TVINSERTSTRUCTW		tvs = {0};

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.iImage = 4;
	tvs.item.iSelectedImage = 4;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_SAVE_OPT);
	m_ChecksTBehavior[bhSaveRoot] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	tvs.item.iImage = 5;
	tvs.item.iSelectedImage = 5;
	tvs.item.lParam = MAKELONG(1, IDC_GRP_BEH_MISC);
	m_ChecksTBehavior[bhPositionRoot] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hParent = m_ChecksTBehavior[bhSaveRoot];
	tvs.item.lParam = MAKELONG(1, IDC_ALWAYS_SAVE);
	m_ChecksTBehavior[bhSaveAlways] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhSaveAlways], m_TempNoteSettings.saveOnExit);
	tvs.item.lParam = MAKELONG(1, IDC_ASK_B_SAVE);
	m_ChecksTBehavior[bhAskBefore] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhAskBefore], m_TempNoteSettings.confirmSave);
	tvs.item.lParam = MAKELONG(1, IDC_CHK_DEL_CONFIRM);
	m_ChecksTBehavior[bhConfirmDel] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhConfirmDel], m_TempNoteSettings.confirmDelete);
	tvs.item.lParam = MAKELONG(1, IDC_CHK_NO_PRMPT_HIDING);
	m_ChecksTBehavior[bhHideWithout] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhHideWithout], IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDE_WO_PROMPT));
	tvs.item.lParam = MAKELONG(1, IDC_CHK_SILENT_CLEAN);
	m_ChecksTBehavior[bhCleanBin] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhCleanBin], IsBitOn(m_TempNextSettings.flags1, SB3_CLEAN_BIN_WITH_WARNING));
	tvs.hParent = m_ChecksTBehavior[bhPositionRoot];
	tvs.item.lParam = MAKELONG(1, IDC_CHK_NEW_ONTOP);
	m_ChecksTBehavior[bhAlwaysOnTop] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhAlwaysOnTop], m_TempNoteSettings.newOnTop);
	tvs.item.lParam = MAKELONG(1, IDC_CHK_REL_POSITION);
	m_ChecksTBehavior[bhRelational] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhRelational], IsBitOn(m_TempNoteSettings.reserved1, SB1_RELPOSITION));
	tvs.item.lParam = MAKELONG(1, IDC_CHK_HIDE_COMPLETED);
	m_ChecksTBehavior[bhHideCompleted] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhHideCompleted], IsBitOn(m_TempNextSettings.flags1, SB3_HIDE_COMPLETED));
	tvs.item.lParam = MAKELONG(1, IDC_CHK_BIG_ICONS);
	m_ChecksTBehavior[bhBigIcons] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhBigIcons], IsBitOn(m_TempNextSettings.flags1, SB3_BIG_ICONS));
	tvs.item.lParam = MAKELONG(1, IDC_CHK_NO_ALT_TAB);
	m_ChecksTBehavior[bhNoWindowList] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhNoWindowList], IsBitOn(m_TempNoteSettings.reserved1, SB1_NO_ALT_TAB));
	tvs.item.lParam = MAKELONG(1, IDC_CHK_DWM_EXCLUDE);
	m_ChecksTBehavior[bhExcAeroPeek] = (HTREEITEM)SendMessageW(m_hTreeBehavior, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	CTreeView_SetCheckBoxState(m_hTreeBehavior, m_ChecksTBehavior[bhExcAeroPeek], IsBitOn(m_TempNoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK));
	CTreeView_SetEnable(m_hTreeBehavior, m_ChecksTBehavior[bhExcAeroPeek], (g_WinVer == WV_VISTA_AND_MORE) ? true : false);

	TreeView_Expand(m_hTreeBehavior, m_ChecksTBehavior[bhSaveRoot], TVE_EXPAND);
	TreeView_Expand(m_hTreeBehavior, m_ChecksTBehavior[bhPositionRoot], TVE_EXPAND);

	SendMessageW(m_hTreeBehavior, TVM_ENSUREVISIBLE, 0, (LPARAM)m_ChecksTBehavior[bhSaveRoot]);
}

static void ApplyChecksLanguage(wchar_t *lpFile){
	wchar_t 	szBuffer[256], szKey[16];
	TVITEMW		tvi = {0};
	int			count;

	tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	tvi.pszText = szBuffer;
	tvi.cchTextMax = 256;
	count = NELEMS(m_ChecksTBehavior);
	for(int i = 0; i < count; i++){
		if(m_ChecksTBehavior[i]){
			tvi.hItem = m_ChecksTBehavior[i];
			SendMessageW(m_hTreeBehavior, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTBehavior[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeBehavior, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTSkins);
	for(int i = 0; i < count; i++){
		if(m_ChecksTSkins[i]){
			tvi.hItem = m_ChecksTSkins[i];
			SendMessageW(m_hTreeSkins, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTSkins[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeSkins, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTMisc);
	for(int i = 0; i < count; i++){
		if(m_ChecksTMisc[i]){
			tvi.hItem = m_ChecksTMisc[i];
			SendMessageW(m_hTreeMisc, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTMisc[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeMisc, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTApp);
	for(int i = 0; i < count; i++){
		if(m_ChecksTApp[i]){
			tvi.hItem = m_ChecksTApp[i];
			SendMessageW(m_hTreeApp, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTApp[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeApp, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTSchedule);
	for(int i = 0; i < count; i++){
		if(m_ChecksTSchedule[i]){
			tvi.hItem = m_ChecksTSchedule[i];
			SendMessageW(m_hTreeSchedule, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTSchedule[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeSchedule, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTProtection);
	for(int i = 0; i < count; i++){
		if(m_ChecksTProtection[i]){
			tvi.hItem = m_ChecksTProtection[i];
			SendMessageW(m_hTreeProtection, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTProtection[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeProtection, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTDiary);
	for(int i = 0; i < count; i++){
		if(m_ChecksTDiary[i]){
			tvi.hItem = m_ChecksTDiary[i];
			SendMessageW(m_hTreeDiary, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTDiary[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeDiary, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTDock);
	for(int i = 0; i < count; i++){
		if(m_ChecksTDock[i]){
			tvi.hItem = m_ChecksTDock[i];
			SendMessageW(m_hTreeDock, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTDock[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeDock, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
	count = NELEMS(m_ChecksTNetwork);
	for(int i = 0; i < count; i++){
		if(m_ChecksTNetwork[i]){
			tvi.hItem = m_ChecksTNetwork[i];
			SendMessageW(m_hTreeNetwork, TVM_GETITEMW, 0, (LPARAM)&tvi);
			_itow(HIWORD(tvi.lParam), szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, m_DefTNetwork[i], szBuffer, 256, lpFile);
			SendMessageW(m_hTreeNetwork, TVM_SETITEMW, 0, (LPARAM)&tvi);
		}
	}
}

static void ApplyMainDlgLanguage(HWND hwnd){
	
	wchar_t 		szBuffer[256], szKey[16];
	int				index;
	LVCOLUMNW		lvc = {0};

	LockWindowUpdate(hwnd);

	FillNavigationBar(g_NotePaths.CurrLanguagePath);
	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1001", DS_CAPTION, szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);

	//main dialog
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDAPPLY, g_NotePaths.CurrLanguagePath, DS_APPLY);
	SetDlgCtlText(hwnd, IDC_CMD_DEF, g_NotePaths.CurrLanguagePath, DS_O_DEFAULT);
	SetDlgCtlText(hwnd, IDC_ST_RELOAD_ALL, g_NotePaths.CurrLanguagePath, L"( * ) - All notes will be reloaded");
	SetDlgCtlText(hwnd, IDC_ST_NEW_NOTES_ONLY, g_NotePaths.CurrLanguagePath, L"( ** ) - New value will be used in newly created notes only");
	SetDlgCtlText(hwnd, IDC_ST_ABOVE_VISTA, g_NotePaths.CurrLanguagePath, L"( *** ) - Requires Windows Vista or above");

	//appearance dialog
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_TRANS_WARNING, g_NotePaths.CurrLanguagePath, DS_TR_WARNING);
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_GRP_TRANS, g_NotePaths.CurrLanguagePath, L"Transparency");
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_CMD_FONT, g_NotePaths.CurrLanguagePath, L"Change font");
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_GRP_TEXT_SETTINGS, g_NotePaths.CurrLanguagePath, L"Edit area");
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_ST_BINDENT_TEXT, g_NotePaths.CurrLanguagePath, L"Bullets indent (in twips) ( ** )");
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_ST_MARGIN_TEXT, g_NotePaths.CurrLanguagePath, L"Left and right margins width - for skinless notes");
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_ST_SPELL_COLOR, g_NotePaths.CurrLanguagePath, L"Underlining color");
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_GRP_SPELL, g_NotePaths.CurrLanguagePath, L"Spell checking");
	SetDlgItemTextW(m_Dialogs.hAppearance, IDC_FONT_SAMPLE, g_Strings.FontSample);

	//skins dialog
	SetDlgCtlText(m_Dialogs.hSkins, IDC_GRP_DEF_SIZE, g_NotePaths.CurrLanguagePath, L"Default size");
	SetDlgCtlText(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_W, g_NotePaths.CurrLanguagePath, L"Width");
	SetDlgCtlText(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_H, g_NotePaths.CurrLanguagePath, L"Height");
	SetDlgCtlText(m_Dialogs.hSkins, IDC_LBL_MORE_SKINS, g_NotePaths.CurrLanguagePath, L"More skins");
	SetDlgCtlText(m_Dialogs.hSkins, IDC_GRP_SKINLESS_PROPS, g_NotePaths.CurrLanguagePath, L"Skinless notes");
	SetDlgCtlText(m_Dialogs.hSkins, IDC_CMD_SKNLESS_CFONT, g_NotePaths.CurrLanguagePath, L"Caption font");
	SetDlgCtlText(m_Dialogs.hSkins, IDC_ST_BCKG_COLOR, g_NotePaths.CurrLanguagePath, L"Background");

	//schedule dialog
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_GRP_SOUND, g_NotePaths.CurrLanguagePath, L"Sound alert");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_DATE_FMTS, g_NotePaths.CurrLanguagePath, L"Date sample");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_TIME_FMTS, g_NotePaths.CurrLanguagePath, L"Time sample");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_GRP_FORMATS, g_NotePaths.CurrLanguagePath, L"Date/time formats");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_SINGLE_ALARM, g_NotePaths.CurrLanguagePath, L"Date/time format");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_REPEATED_ALARM, g_NotePaths.CurrLanguagePath, L"Time format");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_GRP_TTS_SETTINGS, g_NotePaths.CurrLanguagePath, L"Text-to-speech settings");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_GRP_VOICES, g_NotePaths.CurrLanguagePath, L"Available voices");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CMD_SET_DEF_VOICE, g_NotePaths.CurrLanguagePath, L"Default voice");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CMD_LISTEN_VOICE, g_NotePaths.CurrLanguagePath, L"Listen sample");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_SAMPLE_VOICE, g_NotePaths.CurrLanguagePath, L"Type sample text:");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_VOLUME, g_NotePaths.CurrLanguagePath, L"Volume");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_RATE, g_NotePaths.CurrLanguagePath, L"Speed");
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_PITCH, g_NotePaths.CurrLanguagePath, L"Pitch");

	//docking dialog
	SetDlgCtlText(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_W, g_NotePaths.CurrLanguagePath, L"Width");
	SetDlgCtlText(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_H, g_NotePaths.CurrLanguagePath, L"Height");
	SetDlgCtlText(m_Dialogs.hDocks, IDC_DOCK_ST_DISTANCE, g_NotePaths.CurrLanguagePath, L"Distance between notes");
	SetDlgCtlText(m_Dialogs.hDocks, IDC_CMD_SKNLESS_CFONT, g_NotePaths.CurrLanguagePath, L"Caption font");
	SetDlgCtlText(m_Dialogs.hDocks, IDC_GRP_DOCK_MISC, g_NotePaths.CurrLanguagePath, L"Miscellaneous");
	SetDlgCtlText(m_Dialogs.hDocks, IDC_ST_BCKG_COLOR, g_NotePaths.CurrLanguagePath, L"Background");

	//diary dialog
	SetDlgCtlText(m_Dialogs.hDiary, IDC_ST_DIARY_FORMATS, g_NotePaths.CurrLanguagePath, L"Date format using for naming of notes belong to \"Dairy\" group");
	SetDlgCtlText(m_Dialogs.hDiary, IDC_ST_BCKG_COLOR, g_NotePaths.CurrLanguagePath, L"Background");
	SetDlgCtlText(m_Dialogs.hDiary, IDC_CMD_SKNLESS_CFONT, g_NotePaths.CurrLanguagePath, L"Caption font");
	SetDlgCtlText(m_Dialogs.hDiary, IDC_ST_DIARY_COUNT, g_NotePaths.CurrLanguagePath, L"Number of diary \"pages\" shown in \"Diary\" menu");

	//misc dialog
	SetDlgCtlText(m_Dialogs.hMisc, IDC_GRP_UPDATE, g_NotePaths.CurrLanguagePath, L"New version");
	SetDlgCtlText(m_Dialogs.hMisc, IDC_CMD_NEW_V, g_NotePaths.CurrLanguagePath, L"Check now");
	SetDlgCtlText(m_Dialogs.hMisc, IDC_ST_DEF_BROWSER, g_NotePaths.CurrLanguagePath, L"Specify your preferred browser (leave the field blank in order to use default system browser).");
	SetDlgCtlText(m_Dialogs.hMisc, IDC_GRP_TAGS, g_NotePaths.CurrLanguagePath, L"Predefined tags");
	SetDlgCtlText(m_Dialogs.hMisc, IDC_ST_NEW_TAG, g_NotePaths.CurrLanguagePath, L"Tag");
	SetDlgCtlText(m_Dialogs.hMisc, IDC_GRP_SEARCH_WEB, g_NotePaths.CurrLanguagePath, L"Search providers");
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	for(int i = 0; i < NELEMS(m_EngCols); i++){
		_itow(i, szKey, 10);
		lvc.pszText = szBuffer;
		SendDlgItemMessageW(m_Dialogs.hMisc, IDC_LST_SEARCH_WEB, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
		GetPrivateProfileStringW(S_SEARCH_ENG_COLUMNS, szKey, m_EngCols[i], szBuffer, 256, g_NotePaths.CurrLanguagePath);
		SendDlgItemMessageW(m_Dialogs.hMisc, IDC_LST_SEARCH_WEB, LVM_SETCOLUMNW, i, (LPARAM)&lvc);
	}
	SetDlgCtlText(m_Dialogs.hMisc, IDC_GRP_EXT_PROGS, g_NotePaths.CurrLanguagePath, L"External programs");
	for(int i = 0; i < NELEMS(m_ExtProgsColumns); i++){
		_itow(i, szKey, 10);
		lvc.pszText = szBuffer;
		SendDlgItemMessageW(m_Dialogs.hMisc, IDC_LST_EXT_PROGS, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
		GetPrivateProfileStringW(S_EXT_PROGS_COLUMNS, szKey, m_ExtProgsColumns[i], szBuffer, 256, g_NotePaths.CurrLanguagePath);
		SendDlgItemMessageW(m_Dialogs.hMisc, IDC_LST_EXT_PROGS, LVM_SETCOLUMNW, i, (LPARAM)&lvc);
	}

	//Behavior dialog
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CHK_AUTOSAVE, g_NotePaths.CurrLanguagePath, L"Autosave every:");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_AUTOSAVE, g_NotePaths.CurrLanguagePath, L"minute(s)");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_DBL_CLICK, g_NotePaths.CurrLanguagePath, L"Default action after double click on system tray icon");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_SINGLE_CLICK, g_NotePaths.CurrLanguagePath, L"Default action after single click on system tray icon");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_DEF_SAVE_CHARS, g_NotePaths.CurrLanguagePath, L"Length of note's text used as default name for new note");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_CONTENT_LENGHT, g_NotePaths.CurrLanguagePath, L"Length of note's text shown in \"Content\" column at PNotes Control Panel");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_DEF_NAME_OPT, g_NotePaths.CurrLanguagePath, L"Use following as default name of new note");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_CLEAN_BIN, g_NotePaths.CurrLanguagePath, L"Delete note from Recycle Bin after selected period (days)");
	index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DEF_NAME_OPT, CB_GETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DEF_NAME_OPT, CB_RESETCONTENT, 0, 0);
	for(int i = 0; i < NELEMS(m_DefNaming); i++){
		_itow(i, szKey, 10);
		GetPrivateProfileStringW(S_DEF_SAVE_NAME, szKey, m_DefNaming[i], szBuffer, 255, g_NotePaths.CurrLanguagePath);
		SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DEF_NAME_OPT, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	}
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DEF_NAME_OPT, CB_SETCURSEL, index, 0);

	index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_GETCURSEL, 0, 0);
	GetPrivateProfileStringW(S_CAPTIONS, L"never", L"Never", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_DELETESTRING, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETITEMDATA, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETCURSEL, index, 0);

	//Protection dialog
	SetDlgCtlText(m_Dialogs.hProtection, IDC_ST_BACKUP_LEVELS, g_NotePaths.CurrLanguagePath, L"Backup deepness (amount of copies for each note)");
	SetDlgCtlText(m_Dialogs.hProtection, IDC_GRP_LOCAL_SYNC, g_NotePaths.CurrLanguagePath, L"Automatically synchronize with following computers when started from USB drive");
	//set local sync columns text
	for(int i = 0; i < NELEMS(m_LocalSyncCols); i++){
		_itow(i, szKey, 10);
		lvc.pszText = szBuffer;
		SendDlgItemMessageW(m_Dialogs.hProtection, IDC_LST_LOCAL_SYNC, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
		GetPrivateProfileStringW(S_LOCAL_SYNC_COLUMNS, szKey, m_LocalSyncCols[i], szBuffer, 256, g_NotePaths.CurrLanguagePath);
		SendDlgItemMessageW(m_Dialogs.hProtection, IDC_LST_LOCAL_SYNC, LVM_SETCOLUMNW, i, (LPARAM)&lvc);
	}

	//Network dialog
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_GRP_SYNC, g_NotePaths.CurrLanguagePath, L"Synchronization");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_FTP_ST_SERVER, g_NotePaths.CurrLanguagePath, L"FTP server");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_FTP_ST_DIRECTORY, g_NotePaths.CurrLanguagePath, L"FTP directory");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_FTP_ST_USER, g_NotePaths.CurrLanguagePath, L"User name");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_FTP_ST_PASSWORD, g_NotePaths.CurrLanguagePath, L"Password");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_FTP_ST_PORT, g_NotePaths.CurrLanguagePath, L"Port");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_FTP_CMD_SYNC, g_NotePaths.CurrLanguagePath, L"Synchronize now");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_GRP_EXCHANGE, g_NotePaths.CurrLanguagePath, L"Notes exchange");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_ST_CONTACTS, g_NotePaths.CurrLanguagePath, L"Contacts");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_ST_SOCKET_PORT, g_NotePaths.CurrLanguagePath, L"Port");
	SetDlgCtlText(m_Dialogs.hNetwork, IDC_ST_CONT_GROUPS, g_NotePaths.CurrLanguagePath, L"Groups");

	//set contacts columns text
	for(int i = 0; i < NELEMS(m_ContactCols); i++){
		_itow(i, szKey, 10);
		lvc.pszText = szBuffer;
		SendDlgItemMessageW(m_Dialogs.hNetwork, IDC_LST_CONTACTS, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
		GetPrivateProfileStringW(S_CONTACTS_COLUMNS, szKey, m_ContactCols[i], szBuffer, 256, g_NotePaths.CurrLanguagePath);
		SendDlgItemMessageW(m_Dialogs.hNetwork, IDC_LST_CONTACTS, LVM_SETCOLUMNW, i, (LPARAM)&lvc);
	}

	//add menu commands for double click action
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_RESETCONTENT, 0, 0);
	for(int i = 0; i < DBL_CLICK_COUNT; i++){
		index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_ADDSTRING, 0, (LPARAM)m_DblCommands[i].szCommand);
		SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETITEMDATA, index, index);
	}
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETCURSEL, GetIndexOfDblClick(), 0);

	//add menu commands for single click action
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_RESETCONTENT, 0, 0);
	GetPrivateProfileStringW(S_CAPTIONS, L"no_single_action", L"(No action)", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	for(int i = 0; i < DBL_CLICK_COUNT; i++){
		index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_ADDSTRING, 0, (LPARAM)m_DblCommands[i].szCommand);
		SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_SETITEMDATA, index, index);
	}
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_SETCURSEL, m_TempNextSettings.flags2, 0);

	//get all tooltips
	PTBRTOOLTIP		ptt = m_Tooltips;
	for(int i = 0; i < NELEMS(m_Tooltips); i++){
		_itow(ptt->id, szKey, 10);
		GetPrivateProfileStringW(S_OPTIONS, szKey, ptt->szTip, ptt->szTip, 128, g_NotePaths.CurrLanguagePath);
		ptt++;
	}
	
	ApplyChecksLanguage(g_NotePaths.CurrLanguagePath);

	LockWindowUpdate(NULL);
}

static int ExceptionFilter(unsigned long code);
static int ExceptionFilter(unsigned long code){
	if(code == EXCEPTION_ACCESS_VIOLATION) {
 		return EXCEPTION_EXECUTE_HANDLER;
  	}
	else{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

static void CleanUp(BOOL freeMemNotes){
	__try{
		//write exit state parameter ("1" on start, "0" on succesfull end)
		WritePrivateProfileStringW(S_EXITFLAG, DSK_CURRENT_STATE, L"0", g_NotePaths.INIFile);
		//stop listening
		if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK))
			DeInitializeWinsock();
		//clear data array
		if(g_wsPData){
			free(g_wsPData);
			g_wsPData = NULL;
		}
		//clear voices
		g_VoicesCount = 0;
		//release voice
		ReleaseVoice(m_pVoice);
		//free speller
		DestroySpeller();
		//free local syncs
		if(g_PLocalSyncs){
			PLocalSyncFree(&g_PLocalSyncs);
		}
		//free global contacts
		if(g_PContacts){
			PContactsFree(g_PContacts);
			g_PContacts = NULL;
		}
		//free global contacts groups
		if(g_PContGroups)
			PContGroupsFree(&g_PContGroups);
		//free global external programs
		if(g_PExternalls){
			SEngsFree(g_PExternalls);
			g_PExternalls = NULL;
		}
		//free global search engines
		if(g_PSengs){
			SEngsFree(g_PSengs);
			g_PSengs = NULL;
		}
		//free global tags
		if(g_PTagsPredefined){
			TagsFree(g_PTagsPredefined);
			g_PTagsPredefined = NULL;
		}
		//free smilies DLL
		if(g_hSmilies)
			FreeLibrary(g_hSmilies);
		//delete autosaved files
		DeleteAutosaved();
		//free sounds list
		FreeAllSounds();
		//destroy temp skin bitmap
		if(m_TempRHandles.hbSkin && m_TempRHandles.hbSkin != g_RTHandles.hbSkin)
			DeleteBitmap(m_TempRHandles.hbSkin);
		//destroy diary components
		ClearRTHandles(&g_DiaryRTHandles);
		//destroy dock windows bitmap
		if(wcscmp(g_Appearance.szSkin, g_DockSettings.szCustSkin) != 0){
			DeleteBitmap(g_DRTHandles.hbSkin);
			DeleteBitmap(g_DRTHandles.hbSys);
			DeleteBitmap(g_DRTHandles.hbBack);
			DeleteBitmap(g_DRTHandles.hbDelHide);
			DeleteBitmap(g_DRTHandles.hbCommand);
		}
		if(g_DockSettings.fCustCaption){
			DeleteFont(g_DRTHandles.hFCaption);
		}
		//free repeat parts
		FreeRepParts();
		//free weekdays
		FreeWeekdays();
		//remove custom fonts
		ClearCustomFonts();
		//destroy main run-time handles
		ClearRTHandles(&g_RTHandles);
		//free favorites menu
		FreeFavoritesMenu();
		//save edit hot keys
		SaveHotKeys(S_EDIT_HOTKEYS, g_EditHotkeys, COUNT_EDIT_HOTKEYS);
		//free edit hotkeys
		//free hotkeys
		if(g_EditHotkeys)
			free(g_EditHotkeys);
		if(m_TempEditHotkeys)
			free(m_TempEditHotkeys);
		//save note hot keys
		SaveHotKeys(S_NOTE_HOTKEYS, g_NoteHotkeys, NoteMenuCount());
		//free hotkeys
		if(g_NoteHotkeys)
			free(g_NoteHotkeys);
		if(m_TempNoteHotkeys)
			free(m_TempNoteHotkeys);
		//destroy popup menu
		if(m_hMenu){
			FreeMenus(m_hPopUp);
			DestroyMenu(m_hMenu);
		}
		if(g_hNoteMainMenu){
			FreeMenus(g_hNotePopUp);
			DestroyMenu(g_hNoteMainMenu);
		}
		if(g_hEditMainMenu){
			FreeMenus(g_hEditPopUp);
			DestroyMenu(g_hEditMainMenu);
		}
		if(g_hDropMenu){
			FreeMenus(g_hDropPopUp);
			DestroyMenu(g_hDropMenu);
		}
		//destroy menu bitmaps
		if(g_hBmpMisc)
			DeleteBitmap(g_hBmpMisc);
		if(g_hBmpMenuNormal)
			DeleteBitmap(g_hBmpMenuNormal);
		if(g_hBmpMenuGray)
			DeleteBitmap(g_hBmpMenuGray);
		if(g_hBmpEditNormal)
			DeleteBitmap(g_hBmpEditNormal);
		if(g_hBmpEditGray)
			DeleteBitmap(g_hBmpEditGray);
		if(g_hMenuFont)
			DeleteFont(g_hMenuFont);
		//delete tray icon
		if(m_InTray){
			m_InTray = false;
			Shell_NotifyIconW(NIM_DELETE, &m_nData);
		}
		//kill timers
		if(m_TimerAutosave){
			KillTimer(g_hMain, TIMER_AUTOSAVE_ID);
			m_TimerAutosave = 0;
		}
		if(m_TimerCleanBin){
			KillTimer(g_hMain, TIMER_CLEAN_BIN_ID);
			m_TimerCleanBin = 0;
		}
		//free empty notes memory
		if(g_Empties.pPoint)
			free(g_Empties.pPoint);
		if(g_Empties.pDockData)
			free(g_Empties.pDockData);
		//remove groups
		PNGroupsTotalFree(&g_PGroups);
		//clean docking lists
		FreeDockHeaders();
		//free memory notes list
		if(freeMemNotes)
			FreeMemNotes();
		//delete cursors
		if(g_CurV)
			DestroyCursor(g_CurV);
		if(g_CurH)
			DestroyCursor(g_CurH);
		//destroy image lists
		if(m_hImlTreeCheck)
			ImageList_Destroy(m_hImlTreeCheck);
		if(m_hImlSmallBars)
			ImageList_Destroy(m_hImlSmallBars);
		if(m_hImlSmallBarsGray)
			ImageList_Destroy(m_hImlSmallBarsGray);
		if(g_hImlPvwNormal)
			ImageList_Destroy(g_hImlPvwNormal);
		if(g_hImlPvwGray)
			ImageList_Destroy(g_hImlPvwGray);
		if(m_hImlHotKeys)
			ImageList_Destroy(m_hImlHotKeys);
		//clean registry
		CleanRegMUICache(PROG_EXE_NAME);
		CleanRegMRU(PROG_EXE_NAME);
		CleanRegOpenWithList(PROG_EXE_NAME);
		CleanRegOpenSaveMRU(PROG_EXE_NAME);
		//close optional help window
		EnumWindows(EnumHelpWindows, 0);		
		// CleanPrefetch(L"PNotes.exe");
	}
	__except(ExceptionFilter(exception_code())){
		;
	}
}

static void PrepareMessages(void){
	//prepare messages
	GetPNotesMessage(L"no_note_created", L"Unable to create note window", m_sNoWindowMessage);
	GetPNotesMessage(L"delete_confirm", L"Do you really want to delete this note?", g_Strings.DMessage);
	GetPNotesMessage(L"delete_confirm_multiple", L"Do you really want to delete selected item(s)?", g_Strings.DMessageMultiple);
	GetPNotesMessage(L"delete_bin", L"Do you want to send this note to Recycle Bin?", g_Strings.DMessageBin);
	GetPNotesMessage(L"delete_complete", L"Do you want to completely delete this note?", g_Strings.DMessageComplete);
	GetPNotesMessage(L"delete_backup", L"Do you really want to delete selected backup file(s)?", g_Strings.DMessageBackup);
	GetPNotesMessage(L"empty_bin", L"Empty recycle bin (the operation cannot be undone)?", g_Strings.EmptyBinMessage);
	GetPNotesMessage(L"delete_caption", L"Deletion", g_Strings.DCaption);
	GetPNotesMessage(L"too_many_files", L"Only one file can be dropped onto the note.", g_Strings.TooManyFiles);
	GetPNotesMessage(L"save_caption", L"Saving", g_Strings.SCaption);
	GetPNotesMessage(L"save_note", L"Note has been changed", g_Strings.SNote);
	GetPNotesMessage(L"save_question", L"Do you want to save it?", g_Strings.SQuestion);
	GetPNotesMessage(L"delete_previous", L"Would you like to delete the database file of previous version?", g_Strings.DPrevQuestion);
	GetPNotesMessage(L"invalid_date", L"The date format is invalid. It should include day, month, year, hour and minute.", m_sInvalidDate);
	GetPNotesMessage(L"invalid_time", L"The time format is invalid. It should include hour and minute.", m_sInvalidTime);
	GetPNotesMessage(L"hk_1", L"The hot keys combination", g_Strings.HK1);
	GetPNotesMessage(L"hk_2", L"is already registered. Please, choose another one.", g_Strings.HK2);
	GetPNotesMessage(L"hk_3", L"Delete current hot keys?", g_Strings.HK3);
	GetPNotesMessage(L"no_success", L"Program did not finish correctly last time. Would you like to load autosaved notes?", m_sUnsuccessfull);
	GetPNotesMessage(L"sound_1", L"Add sound", m_sSound1);
	GetPNotesMessage(L"sound_2", L"Confirmation", m_sSound2);
	GetPNotesMessage(L"sound_3", L"The file already exists in your 'sound' directory. Copy anyway?", m_sSound3);
	GetPNotesMessage(L"sound_4", L"The sound file not found. Reset to default.", m_sSound4);
	GetPNotesMessage(L"sound_5", L"Delete selected sound?", m_sSound5);
	GetPNotesMessage(L"def_size", L"Are you sure you want to set the default size of note to", m_sDefSize);
	GetPNotesMessage(L"delete_group_0", L"Group deletion", g_Strings.DeleteGroup0);
	GetPNotesMessage(L"delete_group_1", L"CAUTION! The selected group, including all its notes, is about to be deleted.", g_Strings.DeleteGroup1);
	GetPNotesMessage(L"delete_group_2", L"All its subgroups and their notes will be deleted as well.", g_Strings.DeleteGroup2);
	GetPNotesMessage(L"delete_group_3", L"The operation cannot be rolled back. Continue?", g_Strings.DeleteGroup3);
	GetPNotesMessage(L"search_complete", L"Search complete", g_Strings.SearchComplete);
	GetPNotesMessage(L"replace_complete", L"Replace complete", g_Strings.ReplaceComplete);
	GetPNotesMessage(L"matches_found", L"Matches found:", g_Strings.MatchesFound);
	GetPNotesMessage(L"no_matches", L"No matches found:", g_Strings.NoMatchesFound);
	GetPNotesMessage(L"check_for_update", L"Checking for update", g_Strings.CheckUpdate);
	GetPNotesMessage(L"new_version_1", L"The new version of PNotes is available", g_Strings.NewVersion1);
	GetPNotesMessage(L"new_version_2", L"Click 'OK' in order to proceed to download page.", g_Strings.NewVersion2);
	GetPNotesMessage(L"same_version", L"You are using the latest version of PNotes.", g_Strings.SameVersion);
	GetPNotesMessage(L"font_sample", L"The quick brown fox jumps over the lazy dog", g_Strings.FontSample);
	GetPNotesMessage(L"save_before_send", L"The note has to be saved before sending as attachment. Save now?", g_Strings.SaveBeforeSend);
	GetPNotesMessage(L"default_warning", L"You are about to reset ALL program settings to their default values. Continue?", g_Strings.DefaultSettingsWarning);
	GetPNotesMessage(L"first_baloon_caption", L"Thank you for using PNotes!", g_Strings.FirstBaloonCaption);
	GetPNotesMessage(L"first_baloon_message", L"Right click on system tray icon to begin the work.", g_Strings.FirstBaloonMessage);
	GetPNotesMessage(L"seng_delete", L"Delete selected search provider?", g_Strings.DeleteSEngine);
	GetPNotesMessage(L"downloading", L"Downloading...", g_Strings.Downloading);
	GetPNotesMessage(L"replace_warning_1", L"In order to perform replacement operation the program has to switch to search condition", g_Strings.ReplaceWarning1);
	GetPNotesMessage(L"replace_warning_2", L"while your current search condition is", g_Strings.ReplaceWarning2);
	GetPNotesMessage(L"continue_question", L"Continue?", g_Strings.ContinueQuestion);
	GetPNotesMessage(L"internet_unavailable", L"Internet connection is unavailable", g_Strings.InternetUnavailable);
	GetPNotesMessage(L"restore_full_warning", L"ATTENTION! All existing notes will be removed and replaced by notes from selected backup.", g_Strings.FullBackupWarnung);
	GetPNotesMessage(L"external_delete", L"Delete selected external program from the list?", g_Strings.DeleteExternal);
	GetPNotesMessage(L"clean_bin_1", L"The following notes will be permanently deleted from Recycle Bin:", g_Strings.CleanBin1);
	GetPNotesMessage(L"clean_bin_2", L"Choose \"Yes\" to continue or \"No\" to postpone the deletion to a future time.", g_Strings.CleanBin2);
	GetPNotesMessage(L"delete_bin_multi", L"Do you want to completely delete selected item(s) (the operation cannot be undone)?", g_Strings.DMessageMultiFromBin);
	GetPNotesMessage(L"received_caption", L"New note received", g_Strings.ReceivedCaption);
	GetPNotesMessage(L"received_1", L"Note name", g_Strings.Received1);
	GetPNotesMessage(L"received_2", L"Sender", g_Strings.Received2);
	GetPNotesMessage(L"received_3", L"Receiving date/time", g_Strings.Received3);
	GetPNotesMessage(L"sent_caption", L"Note sent", g_Strings.SentCaption);
	GetPNotesMessage(L"sent_1", L"Note", g_Strings.Sent1);
	GetPNotesMessage(L"sent_2", L"has been sent successfully", g_Strings.Sent2);
	GetPNotesMessage(L"sent_3", L"Recipient:", g_Strings.Sent3);
	GetPNotesMessage(L"sent_4", L"Sending date/time:", g_Strings.Sent4);
	GetPNotesMessage(L"no_suggestion", L"No Suggestions", g_Strings.NoSuggestion);
	GetPNotesMessage(L"add_to_dict", L"Add To Dictionary", g_Strings.AddToDictionary);
	GetPNotesMessage(L"spell_check_finished", L"The spelling check is complete", g_Strings.SpellCheckComplete);

	PrepareDateFormatsMessages();
}

static void PrepareDateFormatsMessages(void){
	wchar_t			szBuffer[256];
	wchar_t			szKeys[4096], *pw;

	//get additional captions (not related to date/time formats)
	GetPrivateProfileStringW(S_CAPTIONS, L"insert_picture", L"Insert Picture", g_Strings.InsPictureCaption, 128, g_NotePaths.CurrLanguagePath);

	g_Strings.DFormats[0] = '\0';
	g_Strings.TFormats[0] = '\0';
	GetPrivateProfileStringW(S_CAPTIONS, L"date_formats", L"Possible date formats", g_Strings.DFCaption, 128, g_NotePaths.CurrLanguagePath);
	GetPrivateProfileStringW(S_CAPTIONS, L"time_formats", L"Possible time formats", g_Strings.TFCaption, 128, g_NotePaths.CurrLanguagePath);
	//date formats
	GetPrivateProfileStringW(S_DATE_FORMAT_CHARS, NULL, NULL, szKeys, 4096, g_NotePaths.CurrLanguagePath);
	if(wcslen(szKeys) == 0) {
		wcscpy(g_Strings.DFormats, DATE_FORMAT_MASKS);
	}
	else{
		pw = szKeys;
		while(*pw){
			GetPrivateProfileStringW(S_DATE_FORMAT_CHARS, pw, L"\n", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			wcscat(g_Strings.DFormats, pw);
			wcscat(g_Strings.DFormats, L"\t");
			wcscat(g_Strings.DFormats, szBuffer);
			wcscat(g_Strings.DFormats, L"\n");
			pw += wcslen(pw) + 1;
		}
	}
	//time formats
	GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H12, NULL, NULL, szKeys, 4096, g_NotePaths.CurrLanguagePath);
	if(wcslen(szKeys) == 0) {
		wcscpy(g_Strings.TFormats, TIME_FORMAT_MASKS);
		return;
	}
	else{
		pw = szKeys;
		while(*pw){
			GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H12, pw, L"\n", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			wcscat(g_Strings.TFormats, pw);
			wcscat(g_Strings.TFormats, L"\t");
			wcscat(g_Strings.TFormats, szBuffer);
			wcscat(g_Strings.TFormats, L"\n");
			pw += wcslen(pw) + 1;
		}
	}
	GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H24, NULL, NULL, szKeys, 4096, g_NotePaths.CurrLanguagePath);
	if(wcslen(szKeys) == 0) {
		return;
	}
	else{
		pw = szKeys;
		while(*pw){
			GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H24, pw, L"\n", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			wcscat(g_Strings.TFormats, pw);
			wcscat(g_Strings.TFormats, L"\t");
			wcscat(g_Strings.TFormats, szBuffer);
			wcscat(g_Strings.TFormats, L"\n");
			pw += wcslen(pw) + 1;
		}
	}
	GetPrivateProfileStringW(S_TIME_FORMAT_CHARS, NULL, NULL, szKeys, 4096, g_NotePaths.CurrLanguagePath);
	if(wcslen(szKeys) == 0) {
		return;
	}
	else{
		pw = szKeys;
		while(*pw){
			GetPrivateProfileStringW(S_TIME_FORMAT_CHARS, pw, L"\n", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			wcscat(g_Strings.TFormats, pw);
			wcscat(g_Strings.TFormats, L"\t");
			wcscat(g_Strings.TFormats, szBuffer);
			wcscat(g_Strings.TFormats, L"\n");
			pw += wcslen(pw) + 1;
		}
	}
}

static void AddDblCommands(wchar_t * lpFile){
	wchar_t 		szBuffer[256];

	GetPrivateProfileStringW(S_MENU, L"10001", L"New Note", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[0].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10008", L"Control Panel", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[1].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10002", L"Preferences", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[2].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10034", L"Search In Notes", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[3].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10056", L"Load Note", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[4].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10065", L"New Note From Clipboard", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[5].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10066", L"Bring All To Front", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[6].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10025", L"Save All", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[7].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"9118", L"Show All/Hide All", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[8].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"9120", L"Search By Tags", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[9].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"9142", L"Search By Dates", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[10].szCommand, szBuffer);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreatePopUp
 Created  : Thu Aug  9 18:00:32 2007
 Modified : Thu Aug  9 18:00:32 2007

 Synopsys : Prepares menus and menu commands array
 Input    : lpFile - language file name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreatePopUp(void){
	int				size;

	if(m_hPopUp){
		FreeMenus(m_hPopUp);
	}
	if(m_hMenu){
		DestroyMenu(m_hMenu);
	}
	m_hMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_MAIN));
	m_hPopUp = GetSubMenu(m_hMenu, 0);

	if(g_hNoteMainMenu){
		FreeMenus(g_hNotePopUp);
		DestroyMenu(g_hNoteMainMenu);
	}
	g_hNoteMainMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_NOTE));
	g_hNotePopUp = GetSubMenu(g_hNoteMainMenu, 0);
	if(g_hEditMainMenu){
		FreeMenus(g_hEditPopUp);
		DestroyMenu(g_hEditMainMenu);
	}
	g_hEditMainMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_NOTE_POPUP));
	g_hEditPopUp = GetSubMenu(g_hEditMainMenu, 0);
	if(g_hDropMenu){
		FreeMenus(g_hDropPopUp);
		DestroyMenu(g_hDropMenu);
	}
	g_hDropMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_DROP_MENU));
	g_hDropPopUp = GetSubMenu(g_hDropMenu, 0);

	//get default new note name
	GetPrivateProfileStringW(S_CAPTIONS, L"new_note", L"Untitled", g_NewNoteName, 128, g_NotePaths.CurrLanguagePath);
	//get langID and SubLangID
	g_LangID = (short)GetPrivateProfileIntW(L"language", L"langID", 0, g_NotePaths.CurrLanguagePath);
	g_SubLangID = (short)GetPrivateProfileIntW(L"language", L"sublangID", 0, g_NotePaths.CurrLanguagePath);
	size = NELEMS(m_MainMenus);

	//-----------	main popup menu	------------
	PrepareMenuRecursiveWithAccelerators(m_MainMenus, size, m_hPopUp, g_NotePaths.CurrLanguagePath, S_MENU, m_Hotkeys, NELEMS(m_Hotkeys));

	//-----------	note popup menu	-----------
	PrepareMenuRecursiveWithAccelerators(NoteMenu(), NoteMenuCount(), g_hNotePopUp, g_NotePaths.CurrLanguagePath, S_MENU, g_NoteHotkeys, NoteMenuCount());
	//PrepareMenuRecursive(NoteMenu(), NoteMenuCount(), g_hNotePopUp, szFile, S_MENU);
	
	//-----------	edit popup menu	-----------
	PrepareMenuRecursive(EditMenu(), EditMenuCount(), g_hEditPopUp, g_NotePaths.CurrLanguagePath, S_MENU);
	PrepareMenuRecursiveWithAccelerators(EditMenu(), EditMenuCount(), GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_INSERT)), g_NotePaths.CurrLanguagePath, S_MENU, g_EditHotkeys, COUNT_EDIT_HOTKEYS);
	PrepareSingleMenuWithAccelerator(EditMenu(), EditMenuCount(), IDM_COPY_AS_TEXT, g_hEditPopUp, g_NotePaths.CurrLanguagePath, S_MENU, g_EditHotkeys, COUNT_EDIT_HOTKEYS);
	PrepareSingleMenuWithAccelerator(EditMenu(), EditMenuCount(), IDM_PASTE_AS_TEXT, g_hEditPopUp, g_NotePaths.CurrLanguagePath, S_MENU, g_EditHotkeys, COUNT_EDIT_HOTKEYS);

	//formatting menu
	PrepareMenuRecursive(EditMenu(), EditMenuCount(), GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_FORMAT_NOTE_TEXT)), g_NotePaths.CurrLanguagePath, S_COMMANDS);
	
	//drop popup menu
	PrepareMenuRecursive(DropMenu(), DropMenuCount(), g_hDropPopUp, g_NotePaths.CurrLanguagePath, S_MENU);

	LoadMenuLanguages(m_hPopUp);

	SetMenuOwnerDraw(m_hPopUp);

	SetMenuOwnerDraw(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)));

	AddColorsMenu();

	LoadWeekdays(g_NotePaths.CurrLanguagePath);

	AddDblCommands(g_NotePaths.CurrLanguagePath);

	SetDefaultItemForDblClick();

	PrepareRepParts(g_NotePaths.CurrLanguagePath);

	//prepare bullets menu
	PrepareBulletsMenu();

	BuildSpellMenu();
}

static void PrepareRepParts(wchar_t * lpFile){
	wchar_t		szKey[16];

	for(int i = 0; i < NELEMS(g_RepParts); i++){
		if(!g_RepParts[i])
			g_RepParts[i] = calloc(128, sizeof(wchar_t));
	}
	_itow(IDC_ST_YEARS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Years", g_RepParts[0], 128, lpFile);
	_itow(IDC_ST_MONTHS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Months", g_RepParts[1], 128, lpFile);
	_itow(IDC_ST_WEEKS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Weeks", g_RepParts[2], 128, lpFile);
	_itow(IDC_ST_DAYS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Days", g_RepParts[3], 128, lpFile);
	_itow(IDC_ST_HOURS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Hours", g_RepParts[4], 128, lpFile);
	_itow(IDC_ST_MINS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Minutes", g_RepParts[5], 128, lpFile);
	_itow(IDC_ST_SECS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Seconds", g_RepParts[6], 128, lpFile);
}

static BOOL IsNeededEditID(int id){
	switch(id){
		case IDM_INSERT_PICTURE:
		case IDM_INSERT_SMILIE:
		case IDM_INSERT_DATETIME:
		case IDM_COPY_AS_TEXT:
		case IDM_PASTE_AS_TEXT:
			return true;
		default:
			return false;
	}
}

static void SetDefaultItemForDblClick(void){
	SetMenuDefaultItem(m_hPopUp, -1, false);
	SetMenuDefaultItem(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)), -1, false);
	switch(GetIndexOfDblClick()){
		case SB2_NEW_NOTE:
			SetMenuDefaultItem(m_hPopUp, IDM_NEW, false);
			break;
		case SB2_CONTROL_PANEL:
			SetMenuDefaultItem(m_hPopUp, IDM_CTRL, false);
			break;
		case SB2_PREFS:
			SetMenuDefaultItem(m_hPopUp, IDM_OPTIONS, false);
			break;
		case SB2_SEARCH_NOTES:
			SetMenuDefaultItem(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)), IDM_SEARCH_IN_NOTES, false);
			break;
		case SB2_LOAD_NOTE:
			SetMenuDefaultItem(m_hPopUp, IDM_LOAD_NOTE, false);
			break;
		case SB2_FROM_CLIPBOARD:
			SetMenuDefaultItem(m_hPopUp, IDM_NOTE_FROM_CLIPBOARD, false);
			break;
		case SB2_ALL_TO_FRONT:
			SetMenuDefaultItem(m_hPopUp, IDM_ALL_TO_FRONT, false);
			break;
		case SB2_SAVE_ALL:
			SetMenuDefaultItem(m_hPopUp, IDM_SAVE_ALL, false);
			break;
		case SB2_SHOW_HIDE:
			//nothing
			break;
		case SB2_SEARCH_TAGS:
			SetMenuDefaultItem(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)), IDM_SEARCH_BY_TAGS, false);
			break;
		case SB2_SEARCH_BY_DATES:
			SetMenuDefaultItem(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)), IDM_SEARCH_BY_DATE, false);
			break;
		default:
			SetMenuDefaultItem(m_hPopUp, IDM_NEW, false);
			break;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FileExistsByFullPath
 Created  : Mon May 14 12:34:40 2007
 Modified : Mon May 14 12:34:40 2007

 Synopsys : Checks file existance by full file path
 Input    : Full file path
 Output   : true if exists, otherwise - false
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL FileExistsByFullPath(wchar_t * lpFile){
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	handle = FindFirstFileW(lpFile, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		FindClose(handle);
		return true;
	}
	return false;
}

static void RegisterCustomFonts(void){
	WIN32_FIND_DATAW 			fd;
	wchar_t						filespec[MAX_PATH], szTemp[MAX_PATH];
	HANDLE 						handle = INVALID_HANDLE_VALUE;
	BOOL 						result = true;
	const wchar_t				*suffixes[] = {L"fon", L"fnt", L"ttf", L"ttc", L"fot", L"otf"};
	P_CUST_FONT					pTemp, pNext;

	wcscpy(filespec, g_NotePaths.FontsPath);
	wcscat(filespec, L"*.*");
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szTemp, g_NotePaths.FontsPath);
			wcscat(szTemp, fd.cFileName);
			if(PathFileExistsW(szTemp)){
				if(PathFindSuffixArrayW(szTemp, suffixes, NELEMS(suffixes))){
					if(AddFontResourceExW(szTemp, FR_PRIVATE, 0) > 0){
						pTemp = calloc(1, sizeof(CUST_FONT));
						pTemp->lpName = calloc(wcslen(szTemp) + 1, sizeof(wchar_t));
						wcscpy(pTemp->lpName, szTemp);
						if(m_pFonts == NULL){
							m_pFonts = pTemp;
						}
						else{
							pNext = m_pFonts;
							while(pNext->next){
								pNext = pNext->next;
							}
							pNext->next = pTemp;
						}
					}
				}
			}
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
}

static void ClearCustomFonts(void){
	P_CUST_FONT					pTemp, pNext;
	
	pNext = m_pFonts;
	while(pNext){
		pTemp = pNext->next;
		RemoveFontResourceExW(pNext->lpName, FR_PRIVATE, 0);
		free(pNext->lpName);
		free(pNext);
		pNext = pTemp;
	}
	m_pFonts = NULL;
}

static void CreateShowHideGroupsMenus(HMENU hMenu){
	HMENU					h1, h2;
	int						count, j = 0, groupsCount;
	LPPNGROUP				ppg;
	wchar_t					szB1[128], szB2[128], szKey[12];
	MITEM					mit;
	P_HK_TYPE				pHK;
	P_GROUP_MENU_STRUCT		pGroups, pTemp;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = 49;

	_itow(IDM_SHOW_ALL, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Show All", szB1, 128, g_NotePaths.CurrLanguagePath);
	pHK =  GetHotKeyByID(IDM_SHOW_ALL, m_Hotkeys, NELEMS(m_Hotkeys));
	if(pHK){
		wcscat(szB1, L"\t");
		wcscat(szB1, pHK->szKey);
	}
	_itow(IDM_HIDE_ALL, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Hide All", szB2, 128, g_NotePaths.CurrLanguagePath);
	pHK =  GetHotKeyByID(IDM_HIDE_ALL, m_Hotkeys, NELEMS(m_Hotkeys));
	if(pHK){
		wcscat(szB2, L"\t");
		wcscat(szB2, pHK->szKey);
	}
	h1 = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_SHOW_GROUPS));
	h2 = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_HIDE_GROUPS));
	count = GetMenuItemCount(h1);
	for(int i = count - 1; i >=0; i--){
		FreeSingleMenu(h1, i);
		FreeSingleMenu(h2, i);
		DeleteMenu(h1, i, MF_BYPOSITION);
		DeleteMenu(h2, i, MF_BYPOSITION);
	}
	mit.id = IDM_SHOW_ALL;
	wcscpy(mit.szText, szB1);
	AppendMenuW(h1, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, h1, 0, true);
	mit.id = IDM_HIDE_ALL;
	wcscpy(mit.szText, szB2);
	AppendMenuW(h2, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, h2, 0, true);
	j++;
	AppendMenuW(h1, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	AppendMenuW(h2, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	j++;
	ppg = g_PGroups;
	groupsCount = PNGroupsCount(g_PGroups);
	if(ppg){
		pGroups = calloc(groupsCount, sizeof(GROUP_MENU_STRUCT));
		if(pGroups){
			pTemp = pGroups;
			for(; ppg; ppg = ppg->next){
				pTemp->id = ppg->id;
				pTemp->pName = ppg->szName;
				pTemp->pKeyShow = &ppg->hotKeyShow;
				pTemp->pKeyHide = &ppg->hotKeyHide;
				pTemp++;
			}
			pTemp = pGroups;
			if(groupsCount > 1){
				qsort(pTemp, groupsCount, sizeof(GROUP_MENU_STRUCT), GroupMenusCompare);
			}
			for(int i = 0; i < groupsCount; i++){
				wcscpy(mit.szText, pTemp->pName);
				if(pTemp->pKeyShow->fsModifiers != 0 || pTemp->pKeyShow->vk != 0){
					wcscat(mit.szText, L"\t");
					wcscat(mit.szText, pTemp->pKeyShow->szKey);
				}
				mit.id = pTemp->id + SHOW_GROUP_ADDITION;
				//insert menu item
				AppendMenuW(h1, MF_STRING, mit.id, mit.szText);
				SetMenuItemProperties(&mit, h1, j, true);
				wcscpy(mit.szText, pTemp->pName);
				if(pTemp->pKeyHide->fsModifiers != 0 || pTemp->pKeyHide->vk != 0){
					wcscat(mit.szText, L"\t");
					wcscat(mit.szText, pTemp->pKeyHide->szKey);
				}
				mit.id = pTemp->id + HIDE_GROUP_ADDITION;
				//insert menu item
				AppendMenuW(h2, MF_STRING, mit.id, mit.szText);
				SetMenuItemProperties(&mit, h2, j, true);
				//move next
				j++;
				pTemp++;
			}
			free(pGroups);
		}
	}
	j++;
	AppendMenuW(h1, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	AppendMenuW(h2, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	GetPrivateProfileStringW(S_CAPTIONS, IK_INCOMING, L"Incoming", szB1, 128, g_NotePaths.CurrLanguagePath);
	mit.id = IDM_SHOW_INCOMING;
	wcscpy(mit.szText, szB1);
	AppendMenuW(h1, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, h1, j, true);
	mit.id = IDM_HIDE_INCOMING;
	wcscpy(mit.szText, szB1);
	AppendMenuW(h2, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, h2, j, true);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadMenuLanguages
 Created  : Thu Aug  9 18:01:54 2007
 Modified : Thu Aug  9 18:01:54 2007

 Synopsys : Loads languages names for language menu
 Input    : hMenu - main popup menu
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void LoadMenuLanguages(HMENU	hMenu){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH], szTemp[MAX_PATH];
	wchar_t				szLang[256];
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL 				result = true;
	int 				index = 0, position = -1, i = 0;
	HMENU				hLang;
	MITEM				mit;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = 49;

	hLang = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_LANGUAGES));

	wcscpy(filespec, g_NotePaths.LangDir);
	wcscat(filespec, L"*.lng");

	handle = FindFirstFileW(filespec, &fd);
	//find language file
	if(handle != INVALID_HANDLE_VALUE){
		//remove "empty" menu
		FreeSingleMenu(hLang, 0);
		RemoveMenu(hLang, IDM_L_EMPTY, MF_BYCOMMAND);
		while(result){
			//get language name
			wcscpy(szTemp, g_NotePaths.LangDir);
			wcscat(szTemp, fd.cFileName);
			GetPrivateProfileStringW(L"language", L"name", NULL, szLang, 256, szTemp);
			index = GetPrivateProfileIntW(L"language", L"id", 0, szTemp);
			//set up menu item
			mit.id = index + LANG_ADDITION;
			wcscpy(mit.szText, szLang);
			wcscpy(mit.szReserved, szTemp);
			//insert menu item
			AppendMenuW(hLang, MF_STRING, mit.id, szLang);
			SetMenuItemProperties(&mit, hLang, i, true);
			if(wcscmp(fd.cFileName, g_NotePaths.LangFile) == 0)
				position = i;
			i++;
			result = FindNextFileW(handle, &fd);
		}
		if(position >= 0){
			//check currently selected item
			CheckMenuItem(hLang, position, MF_BYPOSITION | MF_CHECKED);
		}
		FindClose(handle);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreatePNotesShortcut
 Created  : Thu Aug  9 18:04:50 2007
 Modified : Thu Aug  9 18:04:50 2007

 Synopsys : Creates program shortcut in StartUp directory
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static HRESULT CreatePNotesShortcut(void){

	char			szWorkingDir[MAX_PATH], szLink[MAX_PATH], szProgDir[MAX_PATH];
	
	if(SHGetSpecialFolderPath(NULL, szLink, CSIDL_STARTUP, false)){
		strcat(szLink, LINK_NAME);
		WideCharToMultiByte(CP_ACP, 0, g_NotePaths.ProgFullPath, -1, szProgDir, MAX_PATH, NULL, NULL);
		strcpy(szWorkingDir, szProgDir);
		PathRemoveFileSpec(szWorkingDir);
		return CreateShortcut(szLink, szProgDir, szWorkingDir, NULL, SW_SHOWNORMAL, szProgDir, 0, NULL, 0);
	}
	else
		return -1;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetHKId
 Created  : Sun Aug 12 16:02:24 2007
 Modified : Sun Aug 12 16:02:24 2007

 Synopsys : Returns hot keys ID by identifier
 Input    : identifier - hot keys identifier
 Output   : Hot keys id
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int GetHKId(P_HK_TYPE lpKeys, int count, int identifier){
	for(int i = 0; i < count; i++){
		if(lpKeys[i].identifier == identifier)
			return lpKeys[i].id;
	}
	return 0;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RegisterHKeys
 Created  : Sun Aug 12 15:59:22 2007
 Modified : Sun Aug 12 15:59:22 2007

 Synopsys : Registers hot keys
 Input    : hwnd - main window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RegisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count){
	P_HK_TYPE	pKeys = lpKeys;
	for(int i = 0; i < count; i++){
		if(pKeys->fsModifiers != 0 || pKeys->vk != 0){
			if(RestrictedHotKey(pKeys->szKey)){
				HKeysAlreadyRegistered(pKeys->szKey);
			}
			else if(!RegisterHotKey(hwnd, pKeys->identifier, pKeys->fsModifiers, pKeys->vk)){
				HKeysAlreadyRegistered(pKeys->szKey);
			}
		}
		pKeys++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: UnregisterHKeys
 Created  : Sun Aug 12 15:58:51 2007
 Modified : Sun Aug 12 15:58:51 2007

 Synopsys : Unregisters hot keys
 Input    : hwnd - main window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void UnregisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count){
	P_HK_TYPE	pKeys = lpKeys;
	for(int i = 0; i < count; i++){
		if(pKeys->fsModifiers != 0 || pKeys->vk != 0){
			UnregisterHotKey(hwnd, pKeys->identifier);
		}
		pKeys++;
	}
}

static void PrepareEditHotKeys(void){
	wchar_t		szKey[12];
	HK_TYPE		tKey, *pKeys = g_EditHotkeys;
	PMITEM		menus = EditMenu();
	int			j = 0;

	for(int i = 0; i < EditMenuCount(); i++){
		if(IsNeededEditID(menus[i].id)){
			pKeys->id = menus[i].id;
			_itow(pKeys->id, szKey, 10);
			pKeys->identifier = EDIT_HOTKEYS_IND + j++;
			//use temporary key instead of array member, 
			//because identifiers can change, when menu items are added or deleted
			if(!GetPrivateProfileStructW(S_EDIT_HOTKEYS, szKey, &tKey, sizeof(HK_TYPE), g_NotePaths.INIFile)){
				pKeys->fsModifiers = 0;
				pKeys->vk = 0;
				pKeys->szKey[0] = '\0';
			}
			else{
				pKeys->fsModifiers = tKey.fsModifiers;
				pKeys->vk = tKey.vk;
				wcscpy(pKeys->szKey, tKey.szKey);
			}
			pKeys++;
		}
	}
}

static void PrepareNoteHotKeys(void){
	wchar_t		szKey[12];
	HK_TYPE		tKey, *pKeys = g_NoteHotkeys;
	int			count = NoteMenuCount();
	PMITEM		menus = NoteMenu();

	for(int i = 0; i < count; i++){
		pKeys->id = menus[i].id;
		_itow(pKeys->id, szKey, 10);
		pKeys->identifier = NOTE_HOTKEYS_IND + i;
		//use temporary key instead of array member, 
		//because identifiers can change, when menu items are added or deleted
		if(!GetPrivateProfileStructW(S_NOTE_HOTKEYS, szKey, &tKey, sizeof(HK_TYPE), g_NotePaths.INIFile)){
			pKeys->fsModifiers = 0;
			pKeys->vk = 0;
			pKeys->szKey[0] = '\0';
		}
		else{
			pKeys->fsModifiers = tKey.fsModifiers;
			pKeys->vk = tKey.vk;
			wcscpy(pKeys->szKey, tKey.szKey);
		}
		pKeys++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PrepareHotKeys
 Created  : Sun Aug 12 15:57:27 2007
 Modified : Sun Aug 12 15:57:27 2007

 Synopsys : Prepares hot keys array, according to menu items
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PrepareHotKeys(void){
	wchar_t		szKey[12];
	HK_TYPE		tKey;

	for(int i = 0; i < NELEMS(m_Hotkeys); i++){
		m_Hotkeys[i].id = m_MainMenus[i].id;
		_itow(m_Hotkeys[i].id, szKey, 10);
		m_Hotkeys[i].identifier = MAIN_HOTKEYS_IND + i;
		//use temporary key instead of array member, 
		//because identifiers can change, when menu items are added or deleted
		if(!GetPrivateProfileStructW(S_HOTKEYS, szKey, &tKey, sizeof(HK_TYPE), g_NotePaths.INIFile)){
			m_Hotkeys[i].fsModifiers = 0;
			m_Hotkeys[i].vk = 0;
			m_Hotkeys[i].szKey[0] = '\0';
		}
		else{
			m_Hotkeys[i].fsModifiers = tKey.fsModifiers;
			m_Hotkeys[i].vk = tKey.vk;
			wcscpy(m_Hotkeys[i].szKey, tKey.szKey);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SaveHotKeys
 Created  : Sun Aug 12 15:54:06 2007
 Modified : Sun Aug 12 15:54:06 2007

 Synopsys : Saves/deletes hot keys in INI file
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SaveHotKeys(const wchar_t * lpSection, P_HK_TYPE lpKeys, int count){
	wchar_t		szKey[12];
	P_HK_TYPE	pKeys = lpKeys;

	for(int i = 0; i < count; i++){
		_itow(pKeys->id, szKey, 10);
		if(pKeys->fsModifiers != 0 || pKeys->vk != 0)
			//save hot key
			WritePrivateProfileStructW(lpSection, szKey, pKeys, sizeof(HK_TYPE), g_NotePaths.INIFile);
		else
			//delete hot key
			WritePrivateProfileStructW(lpSection, szKey, NULL, sizeof(HK_TYPE), g_NotePaths.INIFile);
		pKeys++;
	}
}

static BOOL CALLBACK Externals_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Externals_OnCommand);
		HANDLE_MSG (hwnd, WM_CLOSE, Externals_OnClose);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Externals_OnInitDialog);

		default: return false;
	}
}

static void Externals_OnClose(HWND hwnd)
{
	RemovePropW(hwnd, EXTERNAL_PROP);
	EndDialog(hwnd, IDCANCEL);
}

static void Externals_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t			szName[256], szQuery[1024], szCommand[MAX_PATH], szMessage[512];
	wchar_t			szBuffer[256], szFileName[MAX_PATH], szFileTitle[MAX_PATH];

	switch(id){
		case IDOK:
			GetDlgItemTextW(hwnd, IDC_EDT_EXT_NAME, szName, 256);
			if(GetPropW(hwnd, EXTERNAL_PROP) == 0){
				if(SEngsContains(m_TempExternals, szName)){
					GetPrivateProfileStringW(S_MESSAGES, L"external_exists", L"List of external programs already contains this name", szMessage, 256, g_NotePaths.CurrLanguagePath);
					wcscat(szMessage, L" - ");
					wcscat(szMessage, szName);
					MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
			}
			GetDlgItemTextW(hwnd, IDC_EDT_EXT_QUERY, szQuery, 1024);
			GetDlgItemTextW(hwnd, IDC_EDT_EXT_COMMAND, szCommand, MAX_PATH);
			wcscpy(m_External.name, szName);
			wcscpy(m_External.query, szQuery);
			_wcstrm(szCommand);
			wcscpy(m_External.commandline, szCommand);
			RemovePropW(hwnd, EXTERNAL_PROP);
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			RemovePropW(hwnd, EXTERNAL_PROP);
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDC_EDT_EXT_NAME:
		case IDC_EDT_EXT_QUERY:
			if(codeNotify == EN_UPDATE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_EXT_NAME)) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_EXT_QUERY)) > 0){
					EnableWindow(GetDlgItem(hwnd, IDOK), true);
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDOK), false);
				}
			}
			break;
		case IDC_CMD_EXT_QUERY:
			GetPrivateProfileStringW(L"captions", L"add_external", L"Choose executable file", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, EXECUTABLE_FILTER, szBuffer, NULL)){
				SetDlgItemTextW(hwnd, IDC_EDT_EXT_QUERY, szFileName);
			}
			break;
	}
}

static BOOL Externals_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256];

	SendDlgItemMessageW(hwnd, IDC_EDT_EXT_NAME, EM_SETLIMITTEXT, 127, 0);
	SendDlgItemMessageW(hwnd, IDC_EDT_EXT_QUERY, EM_SETLIMITTEXT, 1023, 0);
	SendDlgItemMessageW(hwnd, IDC_EDT_EXT_COMMAND, EM_SETLIMITTEXT, MAX_PATH  - 1, 0);
	if(lParam == 0){
		GetPrivateProfileStringW(S_CAPTIONS, L"external_new", L"New external program", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	}
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"external_modify", L"Edit external program", szBuffer, 256, g_NotePaths.CurrLanguagePath);
		SetDlgItemTextW(hwnd, IDC_EDT_EXT_NAME, m_External.name);
		SetDlgItemTextW(hwnd, IDC_EDT_EXT_QUERY, m_External.query);
		SetDlgItemTextW(hwnd, IDC_EDT_EXT_COMMAND, m_External.commandline);
	}
	SetPropW(hwnd, EXTERNAL_PROP, (HANDLE)lParam);
	SetWindowTextW(hwnd, szBuffer);
	GetPrivateProfileStringW(S_EXT_PROGS_COLUMNS, L"0", m_EngCols[0], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetDlgItemTextW(hwnd, IDC_ST_EXT_NAME, szBuffer);
	GetPrivateProfileStringW(S_EXT_PROGS_COLUMNS, L"1", m_EngCols[1], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetDlgItemTextW(hwnd, IDC_ST_EXT_QUERY, szBuffer);
	GetPrivateProfileStringW(S_EXT_PROGS_COLUMNS, L"2", m_EngCols[2], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetDlgItemTextW(hwnd, IDC_ST_EXT_COMMAND, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	return false;
}

static BOOL CALLBACK SEngs_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, SEngs_OnCommand);
		HANDLE_MSG (hwnd, WM_CLOSE, SEngs_OnClose);
		HANDLE_MSG (hwnd, WM_INITDIALOG, SEngs_OnInitDialog);

		default: return false;
	}
}

static void SEngs_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t			szName[256], szQuery[1024], szMessage[512];

	switch(id){
		case IDOK:
			GetDlgItemTextW(hwnd, IDC_EDT_SENG_NAME, szName, 256);
			if(GetPropW(hwnd, SENG_PROP) == 0){
				if(SEngsContains(m_TempSengs, szName)){
					GetPrivateProfileStringW(S_MESSAGES, L"seng_exists", L"List of search providers already contains this name", szMessage, 256, g_NotePaths.CurrLanguagePath);
					wcscat(szMessage, L" - ");
					wcscat(szMessage, szName);
					MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
			}
			GetDlgItemTextW(hwnd, IDC_EDT_SENG_QUERY, szQuery, 1024);
			wcscpy(m_Seng.name, szName);
			wcscpy(m_Seng.query, szQuery);
			RemovePropW(hwnd, SENG_PROP);
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			RemovePropW(hwnd, SENG_PROP);
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDC_EDT_SENG_NAME:
		case IDC_EDT_SENG_QUERY:
			if(codeNotify == EN_UPDATE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SENG_NAME)) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SENG_QUERY)) > 0){
					EnableWindow(GetDlgItem(hwnd, IDOK), true);
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDOK), false);
				}
			}
			break;
	}
}

static void SEngs_OnClose(HWND hwnd)
{
	RemovePropW(hwnd, SENG_PROP);
	EndDialog(hwnd, IDCANCEL);
}

static BOOL SEngs_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256];

	if(lParam == 0){
		GetPrivateProfileStringW(S_CAPTIONS, L"seng_new", L"New search provider", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	}
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"seng_modify", L"Edit search provider", szBuffer, 256, g_NotePaths.CurrLanguagePath);
		SetDlgItemTextW(hwnd, IDC_EDT_SENG_NAME, m_Seng.name);
		SetDlgItemTextW(hwnd, IDC_EDT_SENG_QUERY, m_Seng.query);
	}
	SetPropW(hwnd, SENG_PROP, (HANDLE)lParam);
	SetWindowTextW(hwnd, szBuffer);
	GetPrivateProfileStringW(S_SEARCH_ENG_COLUMNS, L"0", m_EngCols[0], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetDlgItemTextW(hwnd, IDC_ST_SENG_NAME, szBuffer);
	GetPrivateProfileStringW(S_SEARCH_ENG_COLUMNS, L"1", m_EngCols[1], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetDlgItemTextW(hwnd, IDC_ST_SENG_QUERY, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_SENG_Q_SAMPLE, g_NotePaths.CurrLanguagePath, L"Please, check the query line of search provider in browser before editing. It may looks like 'http://www.google.com/search?q=/'");
	return false;
}

static BOOL CALLBACK EditHotProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	wchar_t			szModifiers[24], szKey[8], szBuffer[24];
	int				idMod = 0, key = 0;
	BOOL			proceed = false;

	switch(msg){
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			//get modifiers
			idMod = GetModifiers(szModifiers);
			//get the very first other key pressed
			key = GetOtherKey(szKey);
			if(idMod == 0 && (key >= VK_F1 && key <= VK_F24)){
				if(m_HKCurrent.fsModifiers != idMod || m_HKCurrent.vk != key){
					proceed = true;
				}
			}
			else if(idMod && key){
				if(m_HKCurrent.fsModifiers != idMod || m_HKCurrent.vk != key){
					proceed = true;
				}
			}
			if(proceed){
				wcscpy(szBuffer, szModifiers);
				wcscat(szBuffer, szKey);
				if(RestrictedHotKey(szBuffer)){
					HKeysAlreadyRegistered(szBuffer);
					return 0;
				}
				//check whether hot keys are already registered
				if(!RegisterHotKey(g_hMain, 1001, idMod, key)){
					HKeysAlreadyRegistered(szBuffer);
				}
				else{
					UnregisterHotKey(g_hMain, 1001);
				}
				//apply to current hot key
				wcscpy(m_HKCurrent.szKey, szBuffer);
				m_HKCurrent.fsModifiers = idMod;
				m_HKCurrent.vk = key;
				SetWindowTextW(hwnd, szBuffer);
			}
			return 0;
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static BOOL CALLBACK Hot_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, Hot_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, Hot_OnCommand);
	HANDLE_MSG (hwnd, WM_CLOSE, Hot_OnClose);
	default: return false;
	}
}

static void Hot_OnClose(HWND hwnd)
{
	m_hHotDlg = 0;
	EndDialog(hwnd, 0);
}

static void Hot_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDOK:
			m_hHotDlg = 0;
			EndDialog(hwnd, IDOK);
			break;
	}
}

static BOOL Hot_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND			hEdit;

	m_hHotDlg = hwnd;
	hEdit = GetDlgItem(hwnd, IDC_EDT_HOT);
	//subclass edit box window
	SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)EditHotProc));
	//apply language
	ApplyHotDialogLanguage(hwnd);
	//display current hot keys
	SetDlgItemTextW(hwnd, IDC_EDT_HOT, m_HKCurrent.szKey);
	SetFocus(hEdit);
	return false;
}

static BOOL IsAnyAutosavedExists(void){
	wchar_t				filespec[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle;

	wcscpy(filespec, g_NotePaths.DataDir);
	wcscat(filespec, L"~*.no~");
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE){
		return false;
	}
	else{
		FindClose(handle);
		return true;
	}
}

static void DeleteAutosaved(void){
	//delete all autosaved notes
	wchar_t				filespec[MAX_PATH], szPath[MAX_PATH], szTemp[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL				result = true;

	if(!NotesDirExists(g_NotePaths.DataDir))
		return;
	wcscpy(szTemp, g_NotePaths.DataDir);
	wcscpy(filespec, g_NotePaths.DataDir);
	wcscat(filespec, L"~*.no~");
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return;
	while(result){
		wcscpy(szPath, szTemp);
		wcscat(szPath, fd.cFileName);
		DeleteFileW(szPath);
		result = FindNextFileW(handle, &fd);
	}
	FindClose(handle);
	wcscat(szTemp, AUTOSAVE_INDEX_FILE);
	DeleteFileW(szTemp);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadSounds
 Created  : Sun Aug 12 15:47:06 2007
 Modified : Sun Aug 12 15:47:06 2007

 Synopsys : Loads all sound files from 'sound' directory into listbox on 
            Preferences window
 Input    : hwnd - Preferences window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void LoadSounds(HWND hwnd){
	HWND				hList;

	hList = GetDlgItem(hwnd, IDC_LST_SOUND);
	for(int i = 1; i < 256; i++){
		if(!g_Sounds[i])
			break;
		SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)g_Sounds[i]);
	}
}

static void FreeAllSounds(void){
	for(int i = 1; i < 256; i++){
		if(g_Sounds[i])
			free(g_Sounds[i]);
	}
}

static void AddSound(wchar_t * lpSound){
	for(int i = 1; i < 256; i++){
		if(!g_Sounds[i]){
			g_Sounds[i] = (wchar_t *)calloc(wcslen(lpSound) + 1, sizeof(wchar_t));
			wcscpy(g_Sounds[i], lpSound);
			break;
		}
	}
}

static void RemoveSound(wchar_t * lpSound){
	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		if(wcscmp(pNote->pSchedule->szSound, lpSound) == 0)
			*pNote->pSchedule->szSound = '\0';
	}
	for(int i = 0; i < 256; i++){
		if(!g_Sounds[i])
			break;
		else if(wcscmp(g_Sounds[i], lpSound) == 0){
			free(g_Sounds[i]);
			g_Sounds[i] = 0;
			for(int j = i + 1; j < 255; j++){
				if(!g_Sounds[j])
					break;
				g_Sounds[j - 1] = (wchar_t *)calloc(wcslen(g_Sounds[j]) + 1, sizeof(wchar_t));
				wcscpy(g_Sounds[j - 1], g_Sounds[j]);
				free(g_Sounds[j]);
				g_Sounds[j] = 0;
			}
			break;
		}
	}
}

static void LoadAllSounds(void){
	wchar_t				filespec[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL				result = true;
	int					index = 1;

	g_Sounds[0] = (wchar_t *)calloc(wcslen(DS_DEF_SOUND) + 1, sizeof(wchar_t));
	wcscpy(g_Sounds[0], DS_DEF_SOUND);
	//if 'sound' directory does not exist - return
	if(!NotesDirExists(g_NotePaths.SoundDir))
		return;
	//search for all *.wav files
	wcscpy(filespec, g_NotePaths.SoundDir);
	wcscat(filespec, L"*.wav");
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return;
	while(result){
		g_Sounds[index] = (wchar_t *)calloc(wcslen(fd.cFileName) + 1, sizeof(wchar_t));
		wcscpy(g_Sounds[index], fd.cFileName);
		result = FindNextFileW(handle, &fd);
		index++;
	}
	FindClose(handle);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CopySoundFile
 Created  : Sun Aug 12 15:40:56 2007
 Modified : Sun Aug 12 15:40:56 2007

 Synopsys : Copies sound file into 'sound' directory
 Input    : lpSrcFull - full source path
            lpSrcFile - sound file short name
 Output   : true if succeeded, otherwise - false
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL CopySoundFile(wchar_t * lpSrcFull, wchar_t * lpSrcFile){
	wchar_t				szDest[MAX_PATH];

	wcscpy(szDest, g_NotePaths.SoundDir);
	wcscat(szDest, lpSrcFile);
	return CopyFileW(lpSrcFull, szDest, false);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DeleteSoundFile
 Created  : Sun Aug 12 15:38:55 2007
 Modified : Sun Aug 12 15:38:55 2007

 Synopsys : Deletes sound file
 Input    : lpFile - sound file short name
 Output   : true if succeeded, otherwise - false
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL DeleteSoundFile(wchar_t * lpFile){
	wchar_t				szDest[MAX_PATH];

	wcscpy(szDest, g_NotePaths.SoundDir);
	wcscat(szDest, lpFile);
	return DeleteFileW(szDest);
}

static int ParsePNCommandLine(wchar_t * pINIPath, wchar_t * pDataPath, wchar_t * pProgPath, wchar_t * pDBPath, wchar_t * pSkinsPath, wchar_t * pBackPath, wchar_t * pLangPath, wchar_t * pSoundPath, wchar_t * pFontsPath, wchar_t * pDictPath){
	LPWSTR 			* szArglist;
   	int 			nArgs;
	wchar_t			*szArg, *p, *ptr;
	int				result = 0, iConf = 0;

	pINIPath[0] = '\0';
	pDataPath[0] = '\0';
	pProgPath[0] = '\0';
	pDBPath[0] = '\0';
	pSkinsPath[0] = '\0';
	pBackPath[0] = '\0';
	pLangPath[0] = '\0';
	pSoundPath[0] = '\0';
	pFontsPath[0] = '\0';
	pDictPath[0] = '\0';
	m_BackupDirCL[0] = '\0';

	//parse command line into WCHAR array
   	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if(szArglist != 0){
		if(nArgs > 1){			//the first argument is a program name
			for(int i = 1; i < nArgs; i++){
				//first search for -conf or -config switch
				if((_wcsicmp(szArglist[i], L"-conf") == 0) || (_wcsicmp(szArglist[i], L"-config") == 0) || (_wcsicmp(szArglist[i], L"-confnonetwork") == 0) || (_wcsicmp(szArglist[i], L"-confignonetwork") == 0)){
					iConf = i;
					if((_wcsicmp(szArglist[i], L"-confnonetwork") == 0) || (_wcsicmp(szArglist[i], L"-confignonetwork") == 0))
						result |= ARG_NONETWORK;
					break;
				}
			}
			//if -conf or -config found - get supplied parameters and return
			if(iConf != 0){
				if(nArgs - iConf >= 2){
					if(wcslen(szArglist[iConf + 1]) > 0){
						wcscpy(pINIPath, szArglist[iConf + 1]);
						PathUnquoteSpacesW(pINIPath);
						result |= ARG_INI_PATH;
					}
				}
				if(nArgs - iConf >= 3){
					if(wcslen(szArglist[iConf + 2]) > 0){
						wcscpy(pDataPath, szArglist[iConf + 2]);
						PathUnquoteSpacesW(pDataPath);
						result |= ARG_DATA_PATH;
					}
				}
				if(nArgs - iConf >= 4){
					if(wcslen(szArglist[iConf + 3]) > 0){
						wcscpy(pProgPath, szArglist[iConf + 3]);
						PathUnquoteSpacesW(pProgPath);
						result |= ARG_PROG_PATH;
					}
				}
				if(nArgs - iConf >= 5){
					if(wcslen(szArglist[iConf + 4]) > 0){
						wcscpy(pDBPath, szArglist[iConf + 4]);
						PathUnquoteSpacesW(pDBPath);
						result |= ARG_DB_PATH;
					}
				}
				if(nArgs - iConf >= 6){
					if(wcslen(szArglist[iConf + 5]) > 0){
						wcscpy(pSkinsPath, szArglist[iConf + 5]);
						PathUnquoteSpacesW(pSkinsPath);
						result |= ARG_SKINS_PATH;
					}
				}
				if(nArgs - iConf >= 7){
					if(wcslen(szArglist[iConf + 6]) > 0){
						wcscpy(pBackPath, szArglist[iConf + 6]);
						PathUnquoteSpacesW(pBackPath);
						result |= ARG_BACKUP_PATH;
					}
				}
				if(nArgs - iConf >= 8){
					if(wcslen(szArglist[iConf + 7]) > 0){
						wcscpy(pLangPath, szArglist[iConf + 7]);
						PathUnquoteSpacesW(pLangPath);
						result |= ARG_LANG_PATH;
					}
				}
				if(nArgs - iConf >= 9){
					if(wcslen(szArglist[iConf + 8]) > 0){
						wcscpy(pSoundPath, szArglist[iConf + 8]);
						PathUnquoteSpacesW(pSoundPath);
						result |= ARG_SOUND_PATH;
					}
				}
				if(nArgs - iConf >= 10){
					if(wcslen(szArglist[iConf + 9]) > 0){
						wcscpy(pFontsPath, szArglist[iConf + 9]);
						PathUnquoteSpacesW(pFontsPath);
						result |= ARG_FONTS_PATH;
					}
				}
				if(nArgs - iConf >= 11){
					if(wcslen(szArglist[iConf + 10]) > 0){
						wcscpy(pDictPath, szArglist[iConf + 10]);
						PathUnquoteSpacesW(pDictPath);
						result |= ARG_DICT_PATH;
					}
				}
				//free arguments list
				LocalFree(szArglist);
				//return function result
				return result;
			}
			//if no -conf or -config switch found - check from the beginning for other parameters
			for(int i = 1; i < nArgs; i++){
				szArg = szArglist[i];
				p = wcstok(szArg, L"-", &ptr);
				if(p){			//arguments should start from '-'
					if(wcscmp(p, L"b") == 0){				//backup
						if(((result & ARG_EXIT) == ARG_EXIT) || ((result & ARG_CREATE_NEW) == ARG_CREATE_NEW))
							continue;
						result |= ARG_BACKUP;
						//try to get backup directory from command line
						if(i < nArgs - 1){
							wcscpy(m_BackupDirCL, szArglist[i + 1]);
							if(!IsLastBackslash(m_BackupDirCL))
								wcscat(m_BackupDirCL, L"\\");
						}
					}
					else if(wcscmp(p, L"x") == 0){			//close program
						if(((result & ARG_CREATE_NEW) == ARG_CREATE_NEW) || ((result & ARG_BACKUP) == ARG_BACKUP))
							continue;
						result |= ARG_EXIT;
					}
					else if(wcscmp(p, L"xs") == 0){			//silent close
						if(((result & ARG_CREATE_NEW) == ARG_CREATE_NEW) || ((result & ARG_BACKUP) == ARG_BACKUP))
							continue;
						result |= (ARG_EXIT | ARG_SILENT);
					}
					else if(wcscmp(p, L"xn") == 0){			//close without saving
						if(((result & ARG_CREATE_NEW) == ARG_CREATE_NEW) || ((result & ARG_BACKUP) == ARG_BACKUP))
							continue;
						result |= (ARG_EXIT | ARG_NO_SAVE);
					}
					else if(wcscmp(p, L"c") == 0){			//create new note
						if(((result & ARG_EXIT) == ARG_EXIT) || ((result & ARG_BACKUP) == ARG_BACKUP))
							continue;
						result |= ARG_CREATE_NEW;
					}
					else if(wcscmp(p, L"cr") == 0){			//create new note from clipboard
						if(((result & ARG_EXIT) == ARG_EXIT) || ((result & ARG_BACKUP) == ARG_BACKUP))
							continue;
						result |= (ARG_CREATE_NEW | ARG_NEW_FROM_CLIPBOARD);
					}
					else if(wcscmp(p, L"cd") == 0){			//create or open new Today diary page
						if(((result & ARG_EXIT) == ARG_EXIT) || ((result & ARG_BACKUP) == ARG_BACKUP))
							continue;
						result |= (ARG_CREATE_NEW | ARG_DIARY_TODAY);
					}
					else if(wcscmp(p, L"nonetwork") == 0){	//disable network
						result |= ARG_NONETWORK;
					}
				}
			}
		}
		LocalFree(szArglist);
	}
	return result;
}

static BOOL CALLBACK EnumHelpWindows(HWND hwnd, LPARAM lParam){
	wchar_t				szBuffer[MAX_PATH];

	GetWindowTextW(hwnd, szBuffer, MAX_PATH);
	if(_wcsistr(szBuffer, L"PNotes.pdf")){
		//close PDF help window
		SendMessageW(hwnd, WM_CLOSE, 0, 0);
		return false;
	}
	else if(_wcsistr(szBuffer, L"PNotes Help")){
		GetClassNameW(hwnd, szBuffer, MAX_PATH);
		if(_wcsistr(szBuffer, L"HH Parent")){
			//close CHM help window
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			return false;
		}
	}
	return true;
}

static BOOL CALLBACK EnumPNotes(HWND hwnd, LPARAM lParam){
	wchar_t				szClass[256];

	GetClassNameW(hwnd, szClass, 256);
	if(wcscmp(szClass, NOTES_PROG_MAIN_CLASS) == 0){
		//if main PNotes window found
		if((lParam & ARG_EXIT) == ARG_EXIT){
			//if command line argument = exit - close window
			SendMessageW(hwnd, PNM_MAIN_CLOSE, 0, lParam);
			return false;
		}
		else if((lParam & ARG_CREATE_NEW) == ARG_CREATE_NEW){
			if((lParam & ARG_NEW_FROM_CLIPBOARD) == ARG_NEW_FROM_CLIPBOARD)
				SendMessageW(hwnd, PNM_NEW_FROM_CLIPBOARD, 0, 0);
			else if((lParam & ARG_DIARY_TODAY) == ARG_DIARY_TODAY)
				SendMessageW(hwnd, PNM_DIARY_TODAY, 0, 0);
			else
				SendMessageW(hwnd, PNM_CREATE_NEW_NOTE, 0, 0);
			return false;
		}
	}
	return true;
}

static void ClearOnOptionsClose(void){
	if(m_hImlDefCheck)
		ImageList_Destroy(m_hImlDefCheck);
	if(g_TempLocalSyncs)
		PLocalSyncFree(&g_TempLocalSyncs);
	if(g_TempContGroups)
		PContGroupsFree(&g_TempContGroups);
	if(g_TempContacts){
		PContactsFree(g_TempContacts);
		g_TempContacts = NULL;
	}
	if(m_TempSengs){
		SEngsFree(m_TempSengs);
		m_TempSengs = NULL;
	}
	if(m_TempExternals){
		SEngsFree(m_TempExternals);
		m_TempExternals = NULL;
	}
	if(m_TempPTagsPredefined){
		TagsFree(m_TempPTagsPredefined);
		m_TempPTagsPredefined = NULL;
	}
	if(m_TagsDeleted){
		TagsFree(m_TagsDeleted);
		m_TagsDeleted = NULL;
	}
	if(m_TagsModified){
		TagsFree(m_TagsModified);
		m_TagsModified = NULL;
	}
	ClearRTHandles(&m_TempDiaryRTHandles);
	if(g_RTHandles.hFCaption != m_TempRHandles.hFCaption)
		DeleteFont(m_TempRHandles.hFCaption);
	if(g_RTHandles.hFont != m_TempRHandles.hFont)
		DeleteFont(m_TempRHandles.hFont);
	if(g_RTHandles.hbSkin != m_TempRHandles.hbSkin)
		DeleteBitmap(m_TempRHandles.hbSkin);
	if(g_RTHandles.hbDelHide != m_TempRHandles.hbDelHide)
		DeleteBitmap(m_TempRHandles.hbDelHide);
	if(g_RTHandles.hbSys != m_TempRHandles.hbSys)
		DeleteBitmap(m_TempRHandles.hbSys);
	if(g_RTHandles.hbCommand != m_TempRHandles.hbCommand)
		DeleteBitmap(m_TempRHandles.hbCommand);
	if(m_TempDockingSettings.fCustCaption && g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption)
		DeleteFont(m_TempDRTHandles.hFCaption);
	if(m_TempDockingSettings.fCustSkin){
		if(g_DRTHandles.hbSkin != m_TempDRTHandles.hbSkin)
			DeleteBitmap(m_TempDRTHandles.hbSkin);
		if(g_DRTHandles.hbDelHide != m_TempDRTHandles.hbDelHide)
			DeleteBitmap(m_TempDRTHandles.hbDelHide);
		if(g_DRTHandles.hbSys != m_TempDRTHandles.hbSys)
			DeleteBitmap(m_TempDRTHandles.hbSys);
		if(g_DRTHandles.hbCommand != m_TempDRTHandles.hbCommand)
			DeleteBitmap(m_TempDRTHandles.hbCommand);
	}
	//destoy image list
	if(m_hImlLeft)
		ImageList_Destroy(m_hImlLeft);
	ZeroMemory(&m_Dialogs, sizeof(DLG_WNDS));
}

static BOOL IsLastBackslash(wchar_t * src){
	wchar_t 	* temp;

	temp = src;
	while(*temp++)
		;
	*temp--;
	*temp--;
	if(*temp == '\\')
		return true;
	else
		return false;
}

static void CALLBACK DblClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	if(m_Elapsed < GetDoubleClickTime()){
		m_Elapsed += 100;
	}
	else{
		KillTimer(hwnd, TIMER_DBL_CLICK);
		m_Elapsed = 0;
		switch(g_NextSettings.flags2 - 1){
			case SB2_NEW_NOTE:
				SendMessageW(hwnd, WM_COMMAND, IDM_NEW, 0);
				break;
			case SB2_CONTROL_PANEL:
				SendMessageW(hwnd, WM_COMMAND, IDM_CTRL, 0);
				break;
			case SB2_PREFS:
				SendMessageW(hwnd, WM_COMMAND, IDM_OPTIONS, 0);
				break;
			case SB2_SEARCH_NOTES:
				SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_IN_NOTES, 0);
				break;
			case SB2_LOAD_NOTE:
				SendMessageW(hwnd, WM_COMMAND, IDM_LOAD_NOTE, 0);
				break;
			case SB2_FROM_CLIPBOARD:
				SendMessageW(hwnd, WM_COMMAND, IDM_NOTE_FROM_CLIPBOARD, 0);
				break;
			case SB2_ALL_TO_FRONT:
				SendMessageW(hwnd, WM_COMMAND, IDM_ALL_TO_FRONT, 0);
				break;
			case SB2_SAVE_ALL:
				SendMessageW(hwnd, WM_COMMAND, IDM_SAVE_ALL, 0);
				break;
			case SB2_SHOW_HIDE:
				if(m_ShowHideIndex == 0)
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SHOW_ALL, 0), 0);
				else
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIDE_ALL, 0), 0);
				break;
			case SB2_SEARCH_TAGS:
				SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_BY_TAGS, 0);
				break;
			case SB2_SEARCH_BY_DATES:
				SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_BY_DATE, 0);
				break;
			default:
				//do nothing
				break;
		}
	}
}

static void CALLBACK AutosaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	ApplyAutosaveNotes();
}

static void CALLBACK CleanBinTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	SYSTEMTIME		st = {0};
	PMEMNOTE		pNote = NULL;
	int				result = 0;
	wchar_t			szMessage[4096] = {0};
	BOOL			found = false;

	if(!m_CleanBinProgress && g_NextSettings.cleanDays > 0){
		m_CleanBinProgress = true;
		GetLocalTime(&st);
		wcscpy(szMessage, g_Strings.CleanBin1);
		pNote = MemoryNotes();
		while(pNote){
			if(pNote->pData->idGroup == GROUP_RECYCLE){
				result = DateDiff(st, *pNote->pDeleted, DAY);
				if(result >= g_NextSettings.cleanDays){
					pNote->pRTHandles->wipe = true;
					found = true;
					if(wcslen(szMessage) < 4096 - 128 - 28 - 2){
						wcscat(szMessage, L"\n");
						wcscat(szMessage, L"'");
						wcscat(szMessage, pNote->pData->szName);
						wcscat(szMessage, L"'");
					}
				}
			}
			pNote = pNote->next;
		}
		wcscat(szMessage, L"\n");
		wcscat(szMessage, g_Strings.CleanBin2);
		if(found){
			if(IsBitOn(g_NextSettings.flags1, SB3_CLEAN_BIN_WITH_WARNING)){
				if(MessageBoxW(g_hMain, szMessage, PROG_NAME, MB_YESNO | MB_ICONWARNING) == IDNO){
					pNote = MemoryNotes();
					while(pNote){
						if(pNote->pData->idGroup == GROUP_RECYCLE && pNote->pRTHandles->wipe){
							pNote->pRTHandles->wipe = false;
							memcpy(pNote->pDeleted, &st, sizeof(SYSTEMTIME));
							WritePrivateProfileStructW(pNote->pFlags->id, IK_DELETION_TIME, pNote->pDeleted, sizeof(SYSTEMTIME), g_NotePaths.DataFile);
						}
						pNote = pNote->next;
					}
					return;
				}
			}
			pNote = MemoryNotes();
			while(pNote){
				if(pNote->pData->idGroup == GROUP_RECYCLE && pNote->pRTHandles->wipe){
					DeleteNoteCompletely(pNote);
				}
				pNote = pNote->next;
			}
			if(g_hCPDialog)
				SendMessageW(g_hCPDialog, PNM_CLEAN_BIN, 0, 0);
		}
		m_CleanBinProgress = false;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ReplaceCharacters
 Created  : Thu May 10 14:07:22 2007
 Modified : Thu May 10 14:07:49 2007

 Synopsys : Replaces carriage return with aknowledge character, new line 
            with bell character and vice versa. It's need in order to write 
            single line in file instead of several lines.
 Input    : Destination and source strings
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

// static void ReplaceCharacters(wchar_t * dst, wchar_t * src){

	// for(; *src; src++, dst++){
		// switch(*src){
			// case '\r':
				// *dst = 6;		//from carriage return to aknowledge
				// break;
			// case 6:
				// *dst = '\r';	//from aknowledge to carriage return
				// break;
			// case '\n':
				// *dst = 7;		//from new line to bell
				// break;
			// case 7:
				// *dst = '\n';	//from bell to new line	
				// break;
			// default:
				// *dst = *src;
		// }
	// }
	// *dst = *src;			//last null character
// }

/*-@@+@@------------------------------------------------------------------
 Procedure: CleanPrefetch
 Created  : Mon Aug 20 11:27:22 2007
 Modified : Mon Aug 20 11:27:22 2007

 Synopsys : Cleans Prefetch folder on program exit
 Input    : lpcProgName - program executable file name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
// static void CleanPrefetch(const wchar_t * lpcProgName){
	// wchar_t				szPath[MAX_PATH], szTemp[MAX_PATH], szCopy[MAX_PATH];
	// HANDLE 				handle = INVALID_HANDLE_VALUE;
	// BOOL				result = true;
	// WIN32_FIND_DATAW 	fd;

	// GetSystemWindowsDirectoryW(szPath, MAX_PATH);
	// wcscat(szPath, L"\\Prefetch\\");
	// wcscpy(szCopy, szPath);
	// wcscat(szPath, L"*.*");
	// handle = FindFirstFileW(szPath, &fd);
	// if(handle == INVALID_HANDLE_VALUE)
		// return;
	// while(result){
		// if(_wcsistr(fd.cFileName, lpcProgName)){
			// wcscpy(szTemp, szCopy);
			// wcscat(szTemp, fd.cFileName);
			// DeleteFileW(szTemp);
		// }
		// result = FindNextFileW(handle, &fd);
	// }
	// FindClose(handle);
// }

static void ChangeDiarySkinProperties(void){
	if(m_TempRHandles.hbSkin){
		if(IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_LST_SKIN), true);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BCKG_COLOR), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CMD_SKNLESS_CFONT), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CHOOSE_COLOR), false);
			GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
		}
		else{
			wcscpy(m_TempDiaryAppearance.szSkin, m_TempAppearance.szSkin);
			GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
		}
	}
	else{
		*m_TempDiaryAppearance.szSkin = '\0';
		if(IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_LST_SKIN), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BCKG_COLOR), true);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CMD_SKNLESS_CFONT), true);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CHOOSE_COLOR), true);
		}
	}
	GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
	SendDlgItemMessageW(m_Dialogs.hDiary, IDC_LST_SKIN, LB_SETCURSEL, SendDlgItemMessageW(m_Dialogs.hDiary, IDC_LST_SKIN, LB_FINDSTRINGEXACT, -1, (LPARAM)m_TempDiaryAppearance.szSkin), 0);
}

static void ChangeDockingDefSkin(void){
	HWND		hList = GetDlgItem(m_Dialogs.hDocks, IDC_LST_SKIN);
	wchar_t		szTemp[64];
	int			index = -1;

	wcscpy(szTemp, m_sTempSkinFile);
	szTemp[wcslen(szTemp) - 4] = '\0';
	index = SendMessageW(hList, LB_FINDSTRINGEXACT, -1, (LPARAM)szTemp);
	if(index != CB_ERR){
		SendMessageW(hList, LB_SETCURSEL, index, 0);
	}
	// wcscpy(m_TempDockingSettings.szCustSkin, szTemp);
	GetDockSkinProperties(m_Dialogs.hDocks, &m_TempDRTHandles, szTemp, false);
}

static void ChangeDockingDefSize(int id, int val){
	// SetDlgItemInt(m_Dialogs.hDocks, id, val, false);
}

//-----------	Skins stuff	-----------
static BOOL CALLBACK Skins_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, Skins_OnInitDialog);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Skins_OnDrawItem);
		HANDLE_MSG (hwnd, WM_COMMAND, Skins_OnCommand);
		case WM_NOTIFY:{
			LPNMHDR lpnmh = (LPNMHDR)lParam;
			if(lpnmh->idFrom == IDC_LBL_MORE_SKINS && lpnmh->code == NM_CLICK){
				OpenPage(hwnd, SKIN_PAGE);
			}
			else if(lpnmh->idFrom == IDC_TVW_CHK_SKINLESS && lpnmh->code == CTVN_CHECKSTATECHANGED){
				LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
				CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
			}
			else if(lpnmh->idFrom == IDC_TVW_CHK_SKINLESS && lpnmh->code == NM_CUSTOMDRAW){
				LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
				return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
			}
			return true;
		}
		case GCN_COLOR_SELECTED:
			if(m_TempAppearance.crWindow != (COLORREF)lParam){
				m_TempAppearance.crWindow = (COLORREF)lParam;
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
				if(!IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
					m_TempDiaryAppearance.crWindow = (COLORREF)lParam;
				}
				if(!m_TempDockingSettings.fCustColor){
					m_TempDockingSettings.crWindow = (COLORREF)lParam;
				}
			}
			return true;
		// case PNM_DRAW_SN_PVW:
			// DrawSimpleNotePreview((HWND)wParam, (DRAWITEMSTRUCT *)lParam, &m_TempAppearance, &m_TempRHandles);
			// return true;
		default: return false;
	}
}

static void InitDlgSkins(HWND hwnd){
	//fill skins names
	FillSkins(hwnd, g_Appearance.szSkin, true, true);
	//set default skinless size
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_W, EM_LIMITTEXT, 3, 0);
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_H, EM_LIMITTEXT, 3, 0);
	SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx, false);
	SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy, false);
	SendMessageW(GetDlgItem(hwnd, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TempAppearance.crWindow);
	//prepare treeview
	m_hTreeSkins = GetDlgItem(hwnd, IDC_TVW_CHK_SKINLESS);
	CTreeView_Subclass(m_hTreeSkins);
	TreeView_SetImageList(m_hTreeSkins, m_hImlTreeCheck, TVSIL_NORMAL);
	//disable skinless properties when any skin is selected
	if(m_TempRHandles.hbSkin)
		EnableSkinlessProperties(false);
	else
		EnableSkinlessProperties(true);
}

static BOOL Skins_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc, rc3;
	HWND		hPicker;

	GetWindowRect(GetDlgItem(hwnd, IDC_PLACEHOLDER_1), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CreateLinkLableW(hwnd, g_hInstance, L"More skins", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, IDC_LBL_MORE_SKINS, true);
	GetWindowRect(GetDlgItem(hwnd, IDC_PL_1), &rc3);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc3, 2);
	hPicker = CreateGCPickerWindowW(rc3.left, rc3.top, (rc3.right - rc3.left), (rc3.bottom - rc3.top), hwnd, IDC_CHOOSE_COLOR, m_TempAppearance.crWindow);
	BringWindowToTop(hPicker);
	return true;
}

static void Skins_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_CMD_SKNLESS_CFONT:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessCaptionFont(hwnd, &m_TempAppearance.lfCaption, &m_TempAppearance.crCaption, &m_TempRHandles, IDC_ST_BMP, false);
				//dock settings
				if(!m_TempDockingSettings.fCustCaption){
					// memcpy(&m_TempDockingSettings.lfCaption, &m_TempAppearance.lfCaption, sizeof(LOGFONTW));
					m_TempDockingSettings.lfCaption = m_TempAppearance.lfCaption;
					m_TempDockingSettings.crCaption = m_TempAppearance.crCaption;
					m_TempDRTHandles.hFCaption = m_TempRHandles.hFCaption;										
				}
				//diary settings
				if(!IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS)){
					m_TempDiaryAppearance.crCaption = m_TempAppearance.crCaption;
					// memcpy(&m_TempDiaryAppearance.lfCaption, &m_TempAppearance.lfCaption, sizeof(LOGFONTW));
					m_TempDiaryAppearance.lfCaption = m_TempAppearance.lfCaption;
					if(m_TempDiaryRTHandles.hFCaption)
						DeleteFont(m_TempDiaryRTHandles.hFCaption);
					m_TempDiaryRTHandles.hFCaption = CreateFontIndirectW(&m_TempDiaryAppearance.lfCaption);
				}
			}
			break;
		case IDC_LST_SKIN:	//show skin preview
			if(codeNotify == LBN_SELCHANGE){
				ChangeSkinPreview(hwnd, IDC_LST_SKIN, IDC_ST_BMP, &m_TempRHandles, m_sTempSkinFile, (g_RTHandles.hbSkin == m_TempRHandles.hbSkin) ? false : true);
				SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETTEXT, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETCURSEL, 0, 0), (LPARAM)m_TempAppearance.szSkin);
				ChangeDiarySkinProperties();
				//disable skinless properties when any skin is selected
				if(m_TempRHandles.hbSkin){
					EnableSkinlessProperties(false);
					//change docking settings
					if(!m_TempDockingSettings.fCustSkin){
						ChangeDockingDefSkin();
					}
				}
				else
					EnableSkinlessProperties(true);
			}
			break;
		case IDC_EDT_DEF_SIZE_W:
			if(codeNotify == EN_CHANGE){
				m_TempRHandles.szDef.cx = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, NULL, false);			
				if(!m_TempDockingSettings.fCustSize){
					ChangeDockingDefSize(IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx);
				}
			}
			break;
		case IDC_EDT_DEF_SIZE_H:
			if(codeNotify == EN_CHANGE){
				m_TempRHandles.szDef.cy = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, NULL, false);
				if(!m_TempDockingSettings.fCustSize){
					ChangeDockingDefSize(IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy);
				}
			}
			break;
	}
}

static void Skins_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC && lpDrawItem->CtlID == IDC_ST_BMP)
		//draw skin preview
		DrawSkinPreview(GetDlgItem(hwnd, IDC_ST_BMP), &m_TempRHandles, &m_TempAppearance, m_TempRHandles.crMask, lpDrawItem, true);
}

//-----------	Appearance stuff	-----------

static void SetTransValue(HWND hwnd, int value, BYTE * transValue){
	wchar_t			szBuffer[32];

	_itow(value, szBuffer, 10);
	wcscat(szBuffer, L" %");
	SetDlgItemTextW(hwnd, IDC_TRANS_VALUE, szBuffer);
	*transValue = 255 - value * 2.55;
	// m_TempNoteSettings.transValue = 255 - value * 2.55;
}

static BOOL CALLBACK Appearance_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Appearance_OnCommand);
		HANDLE_MSG (hwnd, WM_CTLCOLORSTATIC, Appearance_OnCtlColorStatic);
		HANDLE_MSG (hwnd, WM_HSCROLL, Appearance_OnHScroll);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Appearance_OnDrawItem);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Appearance_OnInitDialog);

		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_APP && lpnmh->code == CTVN_CHECKSTATECHANGED){
				LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
				CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
			}
			else if(lpnmh->idFrom == IDC_TVW_CHK_APP && lpnmh->code == NM_CUSTOMDRAW){
				LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
				return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
			}
			else if(lpnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(lpnmh->idFrom, ttp->szText);
				return true;
			}
			return false;
		}
		case GCN_COLOR_SELECTED:
			if(m_TempSpellColor != (COLORREF)lParam){
				m_TempSpellColor = (COLORREF)lParam;
			}
			return true;
		default: return false;
	}
}

static void InitDlgAppearance(HWND hwnd){
	HWND		hTrack;
	int			transVal;
	wchar_t		szMargin[8];

	//set up track values
	hTrack = GetDlgItem(hwnd, IDC_TRANS_TRACK_BAR);
	SendMessageW(hTrack, TBM_SETRANGE, true, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 10);
	
	EnableWindow(GetDlgItem(hwnd, IDC_GRP_TRANS), (m_TempNoteSettings.transAllow ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_LEFT_ST), (m_TempNoteSettings.transAllow ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_RIGHT_ST), (m_TempNoteSettings.transAllow ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_TRACK_BAR), (m_TempNoteSettings.transAllow ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_VALUE), (m_TempNoteSettings.transAllow ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_WARNING), (m_TempNoteSettings.transAllow ? true : false));
	
	if(m_TempNoteSettings.transValue)
		transVal = 100 - m_TempNoteSettings.transValue / 2.55;
	else
		transVal = 0;
	SetTransValue(hwnd, transVal, &m_TempNoteSettings.transValue);
	SendMessageW(hTrack, TBM_SETPOS, true, transVal);

	SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_ADDSTRING, 0, (LPARAM)L"100");
	SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_ADDSTRING, 0, (LPARAM)L"200");
	SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_ADDSTRING, 0, (LPARAM)L"300");
	SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_ADDSTRING, 0, (LPARAM)L"400");
	SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_ADDSTRING, 0, (LPARAM)L"500");
	SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_SETCURSEL, GetSmallValue(m_TempSmallValues, SMS_BULLET_INDENT) - 1, 0);
	for(int i = 0; i <= 10; i++){
		_itow(i, szMargin, 10);
		SendDlgItemMessageW(hwnd, IDC_CBO_MARGIN, CB_ADDSTRING, 0, (LPARAM)szMargin);
	}
	SendDlgItemMessageW(hwnd, IDC_CBO_MARGIN, CB_SETCURSEL, GetSmallValue(m_TempSmallValues, SMS_MARGIN_SIZE), 0);
	// SetDlgItemInt(hwnd, IDC_EDT_TAB_SPACE, GetSmallValue(m_TempSmallValues, SMS_TAB_WIDTH), false);
	// SendDlgItemMessageW(hwnd, IDC_EDT_TAB_SPACE, EM_LIMITTEXT, 1, 0);
	// SendDlgItemMessageW(hwnd, IDC_UPD_TAB_SPACE, UDM_SETRANGE, 0, (LPARAM) MAKELONG(8, 2));
	
	SendMessageW(GetDlgItem(hwnd, IDC_FONT_SAMPLE), WM_SETFONT, (WPARAM)m_TempRHandles.hFont, true);
	
	//prepare treeview
	m_hTreeApp = GetDlgItem(hwnd, IDC_TVW_CHK_APP);
	CTreeView_Subclass(m_hTreeApp);
	TreeView_SetImageList(m_hTreeApp, m_hImlTreeCheck, TVSIL_NORMAL);
}

static BOOL Appearance_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT				rc;
	HWND				hPicker;

	GetWindowRect(GetDlgItem(hwnd, IDC_PL_1), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	hPicker = CreateGCPickerWindowW(rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), hwnd, IDC_CHOOSE_COLOR, m_TempSpellColor);
	BringWindowToTop(hPicker);

	return true;
}

static void Appearance_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(code == TB_THUMBPOSITION || code == TB_THUMBTRACK){
		SetTransValue(hwnd, pos, &m_TempNoteSettings.transValue);
	}
	else if(code == TB_LINEUP || code == TB_LINEDOWN || code == TB_PAGEUP || code == TB_PAGEDOWN || code == TB_BOTTOM || code == TB_TOP){
		SetTransValue(hwnd, SendDlgItemMessageW(hwnd, IDC_TRANS_TRACK_BAR, TBM_GETPOS, 0, 0), &m_TempNoteSettings.transValue);
	}
}

static void Appearance_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC){
		if(lpDrawItem->CtlID == IDC_FONT_SAMPLE){
			wchar_t			szText[256];
			COLORREF		crOld;
			RECT			rc;

			CopyRect(&rc, &lpDrawItem->rcItem);
			SendDlgItemMessageW(hwnd, IDC_FONT_SAMPLE, WM_GETTEXT, 256, (LPARAM)szText);
			crOld = SetTextColor(lpDrawItem->hDC, m_TempAppearance.crFont);
			SetBkMode(lpDrawItem->hDC, TRANSPARENT);
			FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, GetSysColorBrush(COLOR_BTNFACE));
			DrawTextW(lpDrawItem->hDC, szText, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			SetTextColor(lpDrawItem->hDC, crOld);
		}
	}
}

static void Appearance_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t 			szBuffer[256];
	CHOOSEFONTW			cf;
	LOGFONTW			lfw;

	switch(id){
		case IDC_CMD_FONT:	//choose font
			GetPrivateProfileStringW(L"captions", L"font", L"Choose font", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			// memcpy(&lfw, &m_TempAppearance.lfFont, sizeof(lfw));
			lfw = m_TempAppearance.lfFont;
			ZeroMemory(&cf, sizeof(cf));
			cf.lStructSize = sizeof(cf);
			cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
			cf.hwndOwner = hwnd;
			cf.lpLogFont = &lfw;
			cf.rgbColors = m_TempAppearance.crFont;
			cf.lpfnHook = CFHookProc;
			cf.lCustData = (int)szBuffer;
			if(ChooseFontW(&cf)){
				//common appearane
				// memcpy(&m_TempAppearance.lfFont, &lfw, sizeof(lfw));
				m_TempAppearance.lfFont = lfw;
				m_TempRHandles.hFont = CreateFontIndirectW(&m_TempAppearance.lfFont);
				m_TempAppearance.crFont = (cf.rgbColors == 0) ? DEF_TEXT_COLOR : ((cf.rgbColors == m_TempRHandles.crMask) ? CR_IN_MASK : cf.rgbColors);
				//dock appearance
				m_TempDRTHandles.hFont = m_TempRHandles.hFont;
				//diary appearance
				// memcpy(&m_TempDiaryAppearance.lfFont, &lfw, sizeof(lfw));
				m_TempDiaryAppearance.lfFont = lfw;
				DeleteFont(m_TempDiaryRTHandles.hFont);
				m_TempDiaryRTHandles.hFont = CreateFontIndirectW(&m_TempDiaryAppearance.lfFont);
				m_TempDiaryAppearance.crFont = (cf.rgbColors == 0) ? DEF_TEXT_COLOR : ((cf.rgbColors == CLR_MASK) ? CLR_MASK + 1 : cf.rgbColors);
				SendMessageW(GetDlgItem(hwnd, IDC_FONT_SAMPLE), WM_SETFONT, (WPARAM)m_TempRHandles.hFont, true);
			}
			break;
		case IDC_CBO_BINDENT:
			if(codeNotify == CBN_SELENDOK){
				SetSmallValue(&m_TempSmallValues, SMS_BULLET_INDENT, SendDlgItemMessageW(hwnd, IDC_CBO_BINDENT, CB_GETCURSEL, 0, 0) + 1);
			}
			break;
		case IDC_CBO_MARGIN:
			if(codeNotify == CBN_SELENDOK){
				SetSmallValue(&m_TempSmallValues, SMS_MARGIN_SIZE, SendDlgItemMessageW(hwnd, IDC_CBO_MARGIN, CB_GETCURSEL, 0, 0));
			}
			break;
		// case IDC_EDT_TAB_SPACE:
			// if(codeNotify == EN_UPDATE){
				// wchar_t		szValue[12];
				// int			value = 3;
				// GetWindowTextW(GetDlgItem(hwnd, IDC_EDT_TAB_SPACE), szValue, 12);
				// if(wcslen(szValue) == 0 || _wtoi(szValue) < 2 || _wtoi(szValue) > 8){
					// wcscpy(szValue, L"4");
					// SetWindowTextW(GetDlgItem(hwnd, IDC_EDT_TAB_SPACE), szValue);
				// }
				// value = _wtoi(szValue);
				// SetSmallValue(&m_TempSmallValues, SMS_TAB_WIDTH, (BYTE)value);
			// }
			// break;
	}
}

static HBRUSH Appearance_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	//draw "transparency warning" in red
	SetBkMode(hdc, TRANSPARENT);
	if(hwndChild == GetDlgItem(hwnd, IDC_TRANS_WARNING))// || hwndChild == GetDlgItem(hwnd, IDC_ST_RELOAD_WARNING))
		SetTextColor(hdc, RGB(255, 0, 0));
	return GetSysColorBrush(COLOR_BTNFACE);
}

//-----------	Schedule stuff	-----------
static BOOL CALLBACK Schedule_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Schedule_OnCommand);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Schedule_OnDrawItem);
		HANDLE_MSG (hwnd, WM_HSCROLL, Schedule_OnHScroll);

		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_SCHEDULE){
				if(lpnmh->code == CTVN_CHECKSTATECHANGED){
					LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
					CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
				}
				else if(lpnmh->code == NM_CUSTOMDRAW){
					LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
					return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
				}
			}
			else if(lpnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(lpnmh->idFrom, ttp->szText);
				return true;
			}
			return false;
		}
		default: return false;
	}
}

static void InitDlgSchedule(HWND hwnd){
	int			index;
	wchar_t		szBuffer[256];
	HWND		hTrack;

	//set up track value
	hTrack = GetDlgItem(hwnd, IDC_TRACK_VOLUME);
	SendMessageW(hTrack, TBM_SETRANGE, true, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 10);
	hTrack = GetDlgItem(hwnd, IDC_TRACK_RATE);
	SendMessageW(hTrack, TBM_SETRANGE, true, MAKELONG(0, 20));
	SendMessageW(hTrack, TBM_SETTICFREQ, 1, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 1);
	hTrack = GetDlgItem(hwnd, IDC_TRACK_PITCH);
	SendMessageW(hTrack, TBM_SETRANGE, true, MAKELONG(0, 20));
	SendMessageW(hTrack, TBM_SETTICFREQ, 1, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 1);

	//date/time formats
	SetDlgItemTextW(hwnd, IDC_EDT_DATE_FMTS, g_DTFormats.DateFormat);
	SetDlgItemTextW(hwnd, IDC_EDT_TIME_FMTS, g_DTFormats.TimeFormat);
	SetShowDateTime(hwnd);
	//sounds
	// memcpy(&m_TempSound, &g_Sound, sizeof(SOUND_TYPE));
	m_TempSound = g_Sound;
	// memcpy(&m_PrevSound, &g_Sound, sizeof(SOUND_TYPE));
	m_PrevSound = g_Sound;
	SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_ADDSTRING, 0, (LPARAM)DS_DEF_SOUND);
	LoadSounds(hwnd);
	index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_FINDSTRINGEXACT, -1, (LPARAM)m_TempSound.custSound);
	if(index == LB_ERR || index == 0){
		SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
		SendMessageW(GetDlgItem(hwnd, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, false);
	}
	else{
		SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, index, 0);
		SendMessageW(GetDlgItem(hwnd, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, true);
	}
	//fill available voices
	AddVoicesToControl(GetDlgItem(hwnd, IDC_LST_VOICES), LB_ADDSTRING, LB_SETITEMDATA, LB_SETCURSEL, g_DefVoice);
	index = SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETCURSEL, 0, 0);
	if(index != LB_ERR){
		SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETTEXT, index, (LPARAM)szBuffer);
		if(wcscmp(szBuffer, m_TempVoice) == 0)
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_SET_DEF_VOICE), false);
		else
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_SET_DEF_VOICE), true);
		PPNVOICE	pv = GetVoice(g_TempPVoices, szBuffer);
		if(pv){
			SendDlgItemMessageW(hwnd, IDC_TRACK_VOLUME, TBM_SETPOS, true, pv->volume);
			SendDlgItemMessageW(hwnd, IDC_TRACK_RATE, TBM_SETPOS, true, (pv->rate + 10));
			SendDlgItemMessageW(hwnd, IDC_TRACK_PITCH, TBM_SETPOS, true, (pv->pitch + 10));
		}
	}
	//prepare treeview
	m_hTreeSchedule = GetDlgItem(hwnd, IDC_TVW_CHK_SCHEDULE);
	CTreeView_Subclass(m_hTreeSchedule);
	TreeView_SetImageList(m_hTreeSchedule, m_hImlTreeCheck, TVSIL_NORMAL);
}

static void Schedule_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	int				index;
	DWORD			token = 0;
	PPNVOICE		voice = NULL;

	if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_VOLUME) || hwndCtl == GetDlgItem(hwnd, IDC_TRACK_RATE) || hwndCtl == GetDlgItem(hwnd, IDC_TRACK_PITCH)){
		index = SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR){
			token = (DWORD)SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETITEMDATA, index, 0);
			if(token){
				voice = GetVoiceByToken(g_TempPVoices, token);
			}
		}
		if(code == TB_THUMBPOSITION || code == TB_THUMBTRACK){
			if(voice){
				if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_VOLUME)){
					voice->volume = pos;
				}
				else if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_RATE)){
					voice->rate = pos - 10;
				}
				else if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_PITCH)){
					voice->pitch = pos - 10;
				}
			}
		}
		else if(code == TB_LINEUP || code == TB_LINEDOWN || code == TB_PAGEUP || code == TB_PAGEDOWN || code == TB_BOTTOM || code == TB_TOP){
			if(voice){
				if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_VOLUME)){
					voice->volume = SendDlgItemMessageW(hwnd, IDC_TRACK_VOLUME, TBM_GETPOS, 0, 0);
				}
				else if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_RATE)){
					voice->rate = SendDlgItemMessageW(hwnd, IDC_TRACK_RATE, TBM_GETPOS, 0, 0) - 10;
				}
				else if(hwndCtl == GetDlgItem(hwnd, IDC_TRACK_PITCH)){
					voice->pitch = SendDlgItemMessageW(hwnd, IDC_TRACK_PITCH, TBM_GETPOS, 0, 0) - 10;
				}
			}
		}
	}
	
}

static void Schedule_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_LISTBOX && lpDrawItem->CtlID == IDC_LST_SOUND)
		DrawListItem(lpDrawItem, 0);
}

static void Schedule_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	int					index;
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	wchar_t 			szBuffer[256];

	switch(id){
		case IDC_CMD_LISTEN_VOICE:
			if(codeNotify == BN_CLICKED){
				index = SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETCURSEL, 0, 0);
				if(index != LB_ERR){
					DWORD	token = SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETITEMDATA, index, 0);
					GetDlgItemTextW(hwnd, IDC_EDT_SAMPLE_VOICE, szBuffer, 255);
					Speak(m_pVoice, szBuffer, (void *)token, g_TempPVoices);
				}
			}
			break;
		case IDC_CMD_SET_DEF_VOICE:
			if(codeNotify == BN_CLICKED){
				index = SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETCURSEL, 0, 0);
				if(index != LB_ERR){
					SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETTEXT, index, (LPARAM)m_TempVoice);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SET_DEF_VOICE), false);
				}
			}
			break;
		case IDC_LST_VOICES:
			if(codeNotify == LBN_SELCHANGE){
				index = SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETCURSEL, 0, 0);
				if(index != LB_ERR){
					SendDlgItemMessageW(hwnd, IDC_LST_VOICES, LB_GETTEXT, index, (LPARAM)szBuffer);
					if(wcscmp(szBuffer, m_TempVoice) == 0)
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_SET_DEF_VOICE), false);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_SET_DEF_VOICE), true);
					PPNVOICE	pv = GetVoice(g_TempPVoices, szBuffer);
					if(pv){
						SendDlgItemMessageW(hwnd, IDC_TRACK_VOLUME, TBM_SETPOS, true, pv->volume);
						SendDlgItemMessageW(hwnd, IDC_TRACK_RATE, TBM_SETPOS, true, (pv->rate + 10));
						SendDlgItemMessageW(hwnd, IDC_TRACK_PITCH, TBM_SETPOS, true, (pv->pitch + 10));
					}
				}
			}
			break;
		case IDC_EDT_DATE_FMTS:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_EDT_DATE_FMTS, m_TempFormats.DateFormat, 128);
			}
			break;
		case IDC_EDT_SAMPLE_VOICE:
			if(codeNotify == EN_CHANGE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SAMPLE_VOICE)) > 0)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_LISTEN_VOICE), true);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_LISTEN_VOICE), false);
			}
		case IDC_EDT_TIME_FMTS:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_EDT_TIME_FMTS, m_TempFormats.TimeFormat, 128);
			}
			break;
		case IDC_DATE_HELP:
			MessageBoxW(hwnd, g_Strings.DFormats, g_Strings.DFCaption, MB_OK);
			break;
		case IDC_TIME_HELP:
			MessageBoxW(hwnd, g_Strings.TFormats, g_Strings.TFCaption, MB_OK);
			break;
		
		case IDC_CMD_ADD_SOUND:
			GetPrivateProfileStringW(L"captions", L"sound", L"Choose sound", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, L"Windows audio files (*.wav)\0*.wav\0\0", szBuffer, NULL)){
				if(NotesDirExists(g_NotePaths.SoundDir)){
					if(FileExists(g_NotePaths.SoundDir, szFileTitle)){
						if(MessageBoxW(hwnd, m_sSound3, m_sSound2, MB_OKCANCEL | MB_ICONQUESTION) == IDOK){
							CopySoundFile(szFileName, szFileTitle);
						}
					}
					else{
						CopySoundFile(szFileName, szFileTitle);
						SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_ADDSTRING, 0, (LPARAM)szFileTitle);
					}
				}
				else{
					NotesDirCreate(g_NotePaths.SoundDir);
					CopySoundFile(szFileName, szFileTitle);
					SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_ADDSTRING, 0, (LPARAM)szFileTitle);
				}
				AddSound(szFileTitle);
			}
			break;
		case IDC_CMD_DEL_SOUND:
			index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
			if(MessageBoxW(hwnd, m_sSound5, m_sSound2, MB_OKCANCEL | MB_ICONQUESTION) == IDOK){
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETTEXT, index, (LPARAM)szFileName);
				DeleteSoundFile(szFileName);
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_DELETESTRING, index, 0);
				// memcpy(&m_TempSound, &m_PrevSound, sizeof(m_TempSound));
				m_TempSound = m_PrevSound;
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
				SendMessageW(GetDlgItem(hwnd, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, true);
				RemoveSound(szFileName);
			}
			break;
		case IDC_CMD_LISTEN:
			index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
			SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETTEXT, index, (LPARAM)szBuffer);
			PreviewSound(szBuffer);			
			break;
		case IDC_LST_SOUND:
			if(codeNotify == LBN_SELCHANGE){
				index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETTEXT, index, (LPARAM)m_TempSound.custSound);
				// m_TempSound.defSound = index;
				if(index > 0){
					SendMessageW(GetDlgItem(hwnd, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, true);
				}
				else{
					SendMessageW(GetDlgItem(hwnd, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, false);
				}
			}
			break;
	}
}

//-----------	Behavior stuff	-----------
static BOOL CALLBACK Behavior_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Behavior_OnCommand);
		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_BEHAVIOR){
				if(lpnmh->code == CTVN_CHECKSTATECHANGED){
					LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
					CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
				}
				else if(lpnmh->code == NM_CUSTOMDRAW){
					LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
					return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
				}
			}
			else if(lpnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(lpnmh->idFrom, ttp->szText);
				return true;
			}
			return false;
		}
		default: return false;
	}
}

static void InitDlgBehavior(HWND hwnd){
	wchar_t		szKey[12], szBuffer[256];
	int			index = 0;

	SendDlgItemMessageW(hwnd, IDC_CHK_AUTOSAVE, BM_SETCHECK, (m_TempNoteSettings.autoSave ? BST_CHECKED : BST_UNCHECKED), 0);
	EnableWindow(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), (m_TempNoteSettings.autoSave ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_UPD_AUTOSAVE), (m_TempNoteSettings.autoSave ? true : false));
	EnableWindow(GetDlgItem(hwnd, IDC_ST_AUTOSAVE), (m_TempNoteSettings.autoSave ? true : false));
	_itow(m_TempNoteSettings.autoSValue, szKey, 10);
	SetWindowTextW(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), szKey);
	SendDlgItemMessageW(hwnd, IDC_EDT_AUTOSAVE, EM_LIMITTEXT, 2, 0);
	SendDlgItemMessageW(hwnd, IDC_UPD_AUTOSAVE, UDM_SETRANGE, 0, (LPARAM) MAKELONG(99, 1));
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETCURSEL, GetIndexOfDblClick(), 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_CLICK, CB_SETCURSEL, m_TempNextSettings.flags2, 0);
	//fill save chars values
	for(int i = 1; i <= DEF_SAVE_CHARACTERS; i++){
		_itow(i, szKey, 10);
		SendDlgItemMessageW(hwnd, IDC_CBO_DEF_SAVE_CHARS, CB_ADDSTRING, 0, (LPARAM)szKey);
	}
	SendDlgItemMessageW(hwnd, IDC_CBO_DEF_SAVE_CHARS, CB_SETCURSEL, GetSmallValue(m_TempSmallValues, SMS_DEF_SAVE_CHARACTERS) - 1, 0);
	//fill content length values
	for(int i = 1; i <= MAX_CONTENT_CHARACTERS; i++){
		_itow(i, szKey, 10);
		SendDlgItemMessageW(hwnd, IDC_CBO_CONTENT_LENGTH, CB_ADDSTRING, 0, (LPARAM)szKey);
	}
	SendDlgItemMessageW(hwnd, IDC_CBO_CONTENT_LENGTH, CB_SETCURSEL, GetSmallValue(m_TempSmallValues, SMS_CONTENT_LENGTH) - 1, 0);
	//fill default naming combo
	for(int i = 0; i < NELEMS(m_DefNaming); i++){
		_itow(i, szKey, 10);
		GetPrivateProfileStringW(S_DEF_SAVE_NAME, szKey, m_DefNaming[i], szBuffer, 255, g_NotePaths.CurrLanguagePath);
		SendDlgItemMessageW(hwnd, IDC_CBO_DEF_NAME_OPT, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	}
	SendDlgItemMessageW(hwnd, IDC_CBO_DEF_NAME_OPT, CB_SETCURSEL, GetSmallValue(m_TempSmallValues, SMS_DEF_NAMING), 0);
	//fill clean Recycle Bin Periods
	GetPrivateProfileStringW(S_CAPTIONS, L"never", L"Never", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_CLEAN_BIN, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETITEMDATA, 0, 0);
	if(m_TempNextSettings.cleanDays == 0)
		SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETCURSEL, 0, 0);
	for(int i = 1; i <= 360; i++){
		if(i <= 10 || i == 20 || i == 30 || i == 60 || i == 120 || i == 360){
			_itow(i, szKey, 10);
			index = SendDlgItemMessageW(hwnd, IDC_CBO_CLEAN_BIN, CB_ADDSTRING, 0, (LPARAM)szKey);
			SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETITEMDATA, index, i);
			if(m_TempNextSettings.cleanDays == i){
				SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_CLEAN_BIN, CB_SETCURSEL, index, 0);
			}
		}
	}
	//prepare treeview
	m_hTreeBehavior = GetDlgItem(hwnd, IDC_TVW_CHK_BEHAVIOR);
	CTreeView_Subclass(m_hTreeBehavior);
	TreeView_SetImageList(m_hTreeBehavior, m_hImlTreeCheck, TVSIL_NORMAL);

}

static void Behavior_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	BOOL				fChecked;

	switch(id){
		case IDC_CHK_AUTOSAVE:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_AUTOSAVE, BM_GETCHECK, 0, 0) == BST_CHECKED ? true : false;
				EnableWindow(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), fChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_UPD_AUTOSAVE), fChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_ST_AUTOSAVE), fChecked);
				m_TempNoteSettings.autoSave = fChecked;
			}
			break;
		case IDC_EDT_AUTOSAVE:
			if(codeNotify == EN_UPDATE && m_SettingsDialogLoaded){
				wchar_t		szValue[12];
				int			value = 1;
				GetWindowTextW(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), szValue, 12);
				if(wcslen(szValue) == 0 || _wtoi(szValue) == 0){
					wcscpy(szValue, L"1");
					SetWindowTextW(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), szValue);
				}
				value = _wtoi(szValue);
				m_TempNoteSettings.autoSValue = value;
			}
			break;
		case IDC_CBO_CLEAN_BIN:
			if(codeNotify == CBN_SELENDOK){
				int		index = SendDlgItemMessageW(hwnd, IDC_CBO_CLEAN_BIN, CB_GETCURSEL, 0, 0);
				int		data = SendDlgItemMessageW(hwnd, IDC_CBO_CLEAN_BIN, CB_GETITEMDATA, index, 0);
				m_TempNextSettings.cleanDays = data;
			}
			break;
		case IDC_CBO_SINGLE_CLICK:
			if(codeNotify == CBN_SELENDOK){
				int		index = SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_CLICK, CB_GETCURSEL, 0, 0);
				m_TempNextSettings.flags2 = index;
			}
			break;
		case IDC_CBO_DBL_CLICK:
			if(codeNotify == CBN_SELENDOK){
				int		index = SendDlgItemMessageW(hwnd, IDC_CBO_DBL_CLICK, CB_GETCURSEL, 0, 0);
				//erase previous settings
				BitOff(&m_TempNoteSettings.reserved2, SB2_NEW_NOTE);
				BitOff(&m_TempNoteSettings.reserved2, SB2_CONTROL_PANEL);
				BitOff(&m_TempNoteSettings.reserved2, SB2_PREFS);
				BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_NOTES);
				BitOff(&m_TempNoteSettings.reserved2, SB2_LOAD_NOTE);
				BitOff(&m_TempNoteSettings.reserved2, SB2_FROM_CLIPBOARD);
				BitOff(&m_TempNoteSettings.reserved2, SB2_ALL_TO_FRONT);
				BitOff(&m_TempNoteSettings.reserved2, SB2_SAVE_ALL);
				BitOff(&m_TempNoteSettings.reserved2, SB2_SHOW_HIDE);
				BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_TAGS);
				BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_BY_DATES);
				//set new settings
				BitOn(&m_TempNoteSettings.reserved2, index);
			}
			break;
		case IDC_CBO_DEF_SAVE_CHARS:
			if(codeNotify == CBN_SELENDOK){
				SetSmallValue(&m_TempSmallValues, SMS_DEF_SAVE_CHARACTERS, SendDlgItemMessageW(hwnd, IDC_CBO_DEF_SAVE_CHARS, CB_GETCURSEL, 0, 0) + 1);
			}
			break;
		case IDC_CBO_CONTENT_LENGTH:
			if(codeNotify == CBN_SELENDOK){
				SetSmallValue(&m_TempSmallValues, SMS_CONTENT_LENGTH, SendDlgItemMessageW(hwnd, IDC_CBO_CONTENT_LENGTH, CB_GETCURSEL, 0, 0) + 1);
			}
			break;
		case IDC_CBO_DEF_NAME_OPT:
			if(codeNotify == CBN_SELENDOK){
				SetSmallValue(&m_TempSmallValues, SMS_DEF_NAMING, SendDlgItemMessageW(hwnd, IDC_CBO_DEF_NAME_OPT, CB_GETCURSEL, 0, 0));
			}
			break;
	}
}

//-----------	Diary stuff	-----------

static void ShowDiaryDateSample(HWND hwnd){
	wchar_t			szBuffer[128];
	SYSTEMTIME		st;

	GetLocalTime(&st);
	GetDiaryDateString(m_TempNoteSettings.reserved1, &st, m_DiaryFormats, m_TempDiaryFormatIndex, szBuffer);
	SetWindowTextW(GetDlgItem(hwnd, IDC_ST_DF_SAMPLE), szBuffer);
}

static void GetDiarySkin(NOTE_APPEARANCE diaryApp, P_NOTE_RTHANDLES diaryRH){
	wchar_t			szSkin[128];

	wcscpy(szSkin, diaryApp.szSkin);
	wcscat(szSkin, L".skn");
	GetSkinProperties(g_hMain, diaryRH, szSkin, true);
}

static void EnableDiaryCustPart(enDiaryCustEnableStatus en){
	switch(en){
		case ENDC_ENABLED_WITH_SKIN:
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_LST_SKIN), true);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BCKG_COLOR), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CMD_SKNLESS_CFONT), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CHOOSE_COLOR), false);
			break;
		case ENDC_ENABLED_WITHOUT_SKIN:
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_LST_SKIN), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BCKG_COLOR), true);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CMD_SKNLESS_CFONT), true);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CHOOSE_COLOR), true);
			break;
		case ENDC_DISABLED:
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_LST_SKIN), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BCKG_COLOR), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CMD_SKNLESS_CFONT), false);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CHOOSE_COLOR), false);
			break;
	}
}

static void InitDlgDiary(HWND hwnd){
	BOOL		fCustom = IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS);
	wchar_t		szNumber[12];

	//fill skins names
	FillSkins(hwnd, m_TempDiaryAppearance.szSkin, true, false);
	if(!fCustom){
		EnableDiaryCustPart(ENDC_DISABLED);
		if(wcslen(m_TempDiaryAppearance.szSkin) > 0){
			SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_FINDSTRINGEXACT, -1, (LPARAM)m_TempDiaryAppearance.szSkin), 0);
			RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
		}
	}
	else{
		if(wcslen(m_TempDiaryAppearance.szSkin) > 0){
			EnableDiaryCustPart(ENDC_ENABLED_WITH_SKIN);
			SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_FINDSTRINGEXACT, -1, (LPARAM)m_TempDiaryAppearance.szSkin), 0);
			RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
		}
		else{
			EnableDiaryCustPart(ENDC_ENABLED_WITHOUT_SKIN);
		}
	}
	for(int i = 0; i < NELEMS(m_DiaryFormats); i++)
		SendDlgItemMessageW(hwnd, IDC_CBO_DIARY_FORMATS, CB_ADDSTRING, 0, (LPARAM)m_DiaryFormats[i]);
	SendDlgItemMessageW(hwnd, IDC_CBO_DIARY_FORMATS, CB_SETCURSEL, m_TempDiaryFormatIndex, 0);
	ShowDiaryDateSample(hwnd);
	//fill diary pages combo
	for(int i = MIN_DIARY_PAGES; i <= MAX_DIARY_PAGES; i++){
		_itow(i, szNumber, 10);
		SendDlgItemMessageW(hwnd, IDC_CBO_DIARY_COUNT, CB_ADDSTRING, 0, (LPARAM)szNumber);
	}
	SendDlgItemMessageW(hwnd, IDC_CBO_DIARY_COUNT, CB_SETCURSEL, GetSmallValue(m_TempSmallValues, SMS_DIARY_PAGES) - 1, 0);
	if(IsBitOn(m_TempNoteSettings.reserved1, SB1_NO_DIARY_PAGES)){
		EnableWindow(GetDlgItem(hwnd, IDC_ST_DIARY_COUNT), false);
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_DIARY_COUNT), false);
	}
}

static BOOL CALLBACK Diary_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, Diary_OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND, Diary_OnCommand);
		// HANDLE_MSG (hwnd, WM_HSCROLL, Docks_OnHScroll);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Diary_OnDrawItem);
		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_DIARY){
				if(lpnmh->code == CTVN_CHECKSTATECHANGED){
					LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
					CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
				}
				else if(lpnmh->code == NM_CUSTOMDRAW){
					LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
					return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
				}
			}
			return false;
		}
		case GCN_COLOR_SELECTED:
			if(m_TempDiaryAppearance.crWindow != (COLORREF)lParam){
				m_TempDiaryAppearance.crWindow = (COLORREF)lParam;
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
			}
			return true;
		default: return false;
	}
}

static void Diary_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC){
		if(lpDrawItem->CtlID == IDC_ST_BMP){
			//draw skin preview
			if(wcslen(m_TempDiaryAppearance.szSkin) > 0)
				DrawSkinPreview(lpDrawItem->hwndItem, &m_TempDiaryRTHandles, &m_TempDiaryAppearance, CLR_MASK, lpDrawItem, false);
		}
		else if(lpDrawItem->CtlID == IDC_ST_DOCK_SKINLESS){
			DrawSimpleNotePreview(lpDrawItem->hwndItem, lpDrawItem, &m_TempDiaryAppearance, &m_TempDiaryRTHandles);
		}
	}
}

static BOOL Diary_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc3;
	HWND		hPicker;

	GetWindowRect(GetDlgItem(hwnd, IDC_PL_1), &rc3);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc3, 2);
	hPicker = CreateGCPickerWindowW(rc3.left, rc3.top, (rc3.right - rc3.left), (rc3.bottom - rc3.top), hwnd, IDC_CHOOSE_COLOR, m_TempDiaryAppearance.crWindow);
	BringWindowToTop(hPicker);
	//prepare treeview
	m_hTreeDiary = GetDlgItem(hwnd, IDC_TVW_CHK_DIARY);
	CTreeView_Subclass(m_hTreeDiary);
	TreeView_SetImageList(m_hTreeDiary, m_hImlTreeCheck, TVSIL_NORMAL);
	return true;
}

static void Diary_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_CBO_DIARY_FORMATS:
			if(codeNotify == CBN_SELENDOK){
				m_TempDiaryFormatIndex = SendDlgItemMessageW(hwnd, IDC_CBO_DIARY_FORMATS, CB_GETCURSEL, 0, 0);
				ShowDiaryDateSample(hwnd);
			}
			break;
		case IDC_CMD_SKNLESS_CFONT:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessCaptionFont(hwnd, &m_TempDiaryAppearance.lfCaption, &m_TempDiaryAppearance.crCaption, &m_TempDiaryRTHandles, IDC_ST_DOCK_SKINLESS, true);
			}
			break;
		case IDC_LST_SKIN:
			if(codeNotify == LBN_SELCHANGE){
				SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETTEXT, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETCURSEL, 0, 0), (LPARAM)m_TempDiaryAppearance.szSkin);
				GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
			}
			break;
		case IDC_CBO_DIARY_COUNT:
			if(codeNotify == CBN_SELENDOK){
				SetSmallValue(&m_TempSmallValues, SMS_DIARY_PAGES, SendDlgItemMessageW(hwnd, IDC_CBO_DIARY_COUNT, CB_GETCURSEL, 0, 0) + 1);
			}
			break;
	}
}

//-----------	Docks stuff	-----------
static BOOL CALLBACK Docks_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, Docks_OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND, Docks_OnCommand);
		HANDLE_MSG (hwnd, WM_HSCROLL, Docks_OnHScroll);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Docks_OnDrawItem);
		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_DOCKING){
				if(lpnmh->code == CTVN_CHECKSTATECHANGED){
					LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
					CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
				}
				else if(lpnmh->code == NM_CUSTOMDRAW){
					LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
					return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
				}
			}
			return false;
		}
		case GCN_COLOR_SELECTED:
			if(m_TempDockingSettings.crWindow != (COLORREF)lParam){
				m_TempDockingSettings.crWindow = (COLORREF)lParam;
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
			}
			return true;
		default: return false;
	}
}

static BOOL Docks_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc3;
	HWND		hPicker;

	GetWindowRect(GetDlgItem(hwnd, IDC_PL_1), &rc3);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc3, 2);
	hPicker = CreateGCPickerWindowW(rc3.left, rc3.top, (rc3.right - rc3.left), (rc3.bottom - rc3.top), hwnd, IDC_CHOOSE_COLOR, m_TempDockingSettings.crWindow);
	BringWindowToTop(hPicker);
	return true;
}

static void Docks_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC){
		if(lpDrawItem->CtlID == IDC_ST_DOCK_SKIN){
			//draw skin preview
			DrawSkinPreviewDock(lpDrawItem->hwndItem, &m_TempDRTHandles, &m_TempDockingSettings, CLR_MASK, m_TempAppearance.crFont, lpDrawItem);
		}
		else if(lpDrawItem->CtlID == IDC_ST_DOCK_SKINLESS){
			DrawSimpleNotePreviewDock(lpDrawItem->hwndItem, lpDrawItem, &m_TempDockingSettings, m_TempAppearance.crFont, &m_TempDRTHandles);
		}
	}
}

static void Docks_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_CMD_SKNLESS_CFONT:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessCaptionFont(hwnd, &m_TempDockingSettings.lfCaption, &m_TempDockingSettings.crCaption, &m_TempDRTHandles, IDC_ST_DOCK_SKINLESS, false);
			}
			break;
		case IDC_EDT_DEF_SIZE_W:
			if(codeNotify == EN_UPDATE && m_TempDockingSettings.fCustSize){
				m_TempDockingSettings.custSize.cx = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, false, false);
			}
			break;
		case IDC_EDT_DEF_SIZE_H:
			if(codeNotify == EN_UPDATE && m_TempDockingSettings.fCustSize){
				m_TempDockingSettings.custSize.cy = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, false, false);
			}
			break;
		case IDC_LST_SKIN:
			if(codeNotify == LBN_SELCHANGE){
				wchar_t		szTemp[128];

				SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETTEXT, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETCURSEL, 0, 0), (LPARAM)szTemp);
				wcscpy(m_TempDockingSettings.szCustSkin, szTemp);
				GetDockSkinProperties(hwnd, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, false);
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
			}
	}
}

static void Docks_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(code == TB_THUMBPOSITION || code == TB_THUMBTRACK){
		m_TempDockingSettings.dist = pos;
		// SetDockDistance(hwnd, pos);
	}
	else if(code == TB_LINEUP || code == TB_LINEDOWN || code == TB_PAGEUP || code == TB_PAGEDOWN || code == TB_BOTTOM || code == TB_TOP){
		// SetDockDistance(hwnd, );
		m_TempDockingSettings.dist = SendDlgItemMessageW(hwnd, IDC_DOCK_SLIDER, TBM_GETPOS, 0, 0);
	}
}

// static void SetDockDistance(HWND hwnd, int val){
	// wchar_t		szText[64], szVal[32];

	// _itow(val, szVal, 10);
	// wcscpy(szText, szVal);	
	// wcscat(szText, L" px");
	// SetDlgItemTextW(hwnd, IDC_DOCK_ST_SLIDER_VAL, szText);
	// m_TempDockingSettings.dist = val;
// }

static void InitDlgDocks(HWND hwnd){
	HWND		hTrack;
	wchar_t		szSkin[64];

	hTrack = GetDlgItem(hwnd, IDC_DOCK_SLIDER);
	SendMessageW(hTrack, TBM_SETRANGE, true, MAKELONG(0, 5));
	SendMessageW(hTrack, TBM_SETTICFREQ, 1, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 1);
	SendMessageW(hTrack, TBM_SETPOS, true, m_TempDockingSettings.dist);
	// SetDockDistance(hwnd, m_TempDockingSettings.dist);
	
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_W, EM_LIMITTEXT, 3, 0);
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_H, EM_LIMITTEXT, 3, 0);
	if(m_TempDockingSettings.fCustSkin){
		FillSkins(hwnd, m_TempDockingSettings.szCustSkin, true, false);
		GetDockSkinProperties(hwnd, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, false);
	}
	else{
		if(wcslen(g_DockSettings.szCustSkin) > 0){
			wcscpy(szSkin, g_DockSettings.szCustSkin);
		}
		else{
			wcscpy(szSkin, g_Appearance.szSkin);
			szSkin[wcslen(szSkin) - 4] = '\0';
		}
		FillSkins(hwnd, szSkin, true, false);
		if(!m_TempRHandles.hbSkin){
			SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, 0, 0);
		}
		GetDockSkinProperties(hwnd, &m_TempDRTHandles, szSkin, false);
		EnableWindow(GetDlgItem(hwnd, IDC_LST_SKIN), false);
	}
	if(m_TempDockingSettings.fCustSize){
		SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempDockingSettings.custSize.cx, false);
		SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempDockingSettings.custSize.cy, false);
	}
	else{
		if(m_TempDockingSettings.custSize.cx !=0 && m_TempDockingSettings.custSize.cy != 0){
			SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempDockingSettings.custSize.cx, false);
			SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempDockingSettings.custSize.cy, false);
		}
		else{
			if(m_TempRHandles.hbSkin){
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, DEF_FLAT_WIDTH, false);
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, DEF_FLAT_HEIGHT, false);
			}
			else{
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx, false);
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy, false);
			}
		}
		EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_W), false);
		EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_H), false);
		EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_W), false);
		EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_H), false);
	}
	EnableWindow(GetDlgItem(hwnd, IDC_CHOOSE_COLOR), m_TempDockingSettings.fCustColor);
	EnableWindow(GetDlgItem(hwnd, IDC_ST_BCKG_COLOR), m_TempDockingSettings.fCustColor);
	EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), m_TempDockingSettings.fCustCaption);

	SendMessageW(GetDlgItem(hwnd, IDC_CHOOSE_COLOR), GCM_NEW_COLOR, 0, (LPARAM)m_TempDockingSettings.crWindow);

	//prepare treeview
	m_hTreeDock = GetDlgItem(hwnd, IDC_TVW_CHK_DOCKING);
	CTreeView_Subclass(m_hTreeDock);
	TreeView_SetImageList(m_hTreeDock, m_hImlTreeCheck, TVSIL_NORMAL);
}

// static void FillComboSkins(HWND hwnd, int id, const wchar_t * lpSkinCurrent){
	// WIN32_FIND_DATAW 	fd;
	// wchar_t				filespec[MAX_PATH];
	// wchar_t				szSkin[256];
	// HANDLE 				handle = INVALID_HANDLE_VALUE;
	// BOOL 				result = true;
	// LRESULT 			added;

	// wcscpy(filespec, g_NotePaths.SkinDir);
	// wcscat(filespec, __T("*.skn"));
	// handle = FindFirstFileW(filespec, &fd);
	// if(handle != INVALID_HANDLE_VALUE){
		// while(result){
			// wcscpy(szSkin, fd.cFileName);
			// szSkin[wcslen(szSkin) - 4] = '\0';
			// added = SendDlgItemMessageW(hwnd, id, CB_ADDSTRING, 0, (LPARAM)szSkin);
			// if(wcscmp(szSkin, lpSkinCurrent) == 0){
				// SendDlgItemMessageW(hwnd, id, CB_SETCURSEL, added, 0);
			// }
			// result = FindNextFileW(handle, &fd);
		// }
		// FindClose(handle);
	// }
// }

//-----------	Network stuff	-----------
static BOOL CALLBACK Network_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMLISTVIEW			*nml;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Network_OnCommand);
		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_NETWORK && lpnmh->code == CTVN_CHECKSTATECHANGED){
				LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
				CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
			}
			else if(lpnmh->idFrom == IDC_TVW_CHK_NETWORK && lpnmh->code == NM_CUSTOMDRAW){
				LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
				return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
			}
			else if(lpnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(lpnmh->idFrom, ttp->szText);
				return true;
			}
			else if(lpnmh->code == LVN_ITEMCHANGED){
				nml = (NMLISTVIEW *)lParam;
				if(nml->uNewState == 3 || nml->uNewState == 2){
					if(lpnmh->idFrom == IDC_LST_CONTACTS)
						EnableSmallToolbarFull(hwnd, IDC_TBR_CONTACTS, true, true, true);
				}
				// else{
					// if(lpnmh->idFrom == IDC_LST_CONTACTS)
						// EnableSmallToolbarFull(hwnd, IDC_TBR_CONTACTS, true, false, false);
				// }
			}
			else if(lpnmh->code == NM_DBLCLK){
				if(lpnmh->idFrom == IDC_LST_CONTACTS)
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_MODIFY, 0), (LPARAM)GetDlgItem(hwnd, IDC_TBR_CONTACTS));
			}
			else if(lpnmh->code == TVN_SELCHANGEDW){
				if(lpnmh->idFrom == IDC_TVW_CONT_GROUPS){
					NMTREEVIEWW * nmt = (NMTREEVIEWW *)lParam;
					HTREEITEM	hItem = nmt->itemNew.hItem;
					if(TreeView_GetParent(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), hItem)){
						EnableSmallToolbarFull(hwnd, IDC_TBR_CONT_GROUPS, true, false, false);
					}
					else{
						if(nmt->itemNew.lParam != 0)
							EnableSmallToolbarFull(hwnd, IDC_TBR_CONT_GROUPS, true, true, true);
						else
							EnableSmallToolbarFull(hwnd, IDC_TBR_CONT_GROUPS, true, false, false);
					}
				}
			}
			return false;
		}
		default: return false;
	}
}

static void InitDlgNetwork(HWND hwnd){
	RECT				rc;
	LVCOLUMNW			lvc = {0};
	HWND				hList, hTree;
	BOOL				fEnable;

	EnableSmallToolbarFull(hwnd, IDC_TBR_CONTACTS, true, false, false);
	EnableSmallToolbarFull(hwnd, IDC_TBR_CONT_GROUPS, true, false, false);
	//prepare treeview
	m_hTreeNetwork = GetDlgItem(hwnd, IDC_TVW_CHK_NETWORK);
	CTreeView_Subclass(m_hTreeNetwork);
	TreeView_SetImageList(m_hTreeNetwork, m_hImlTreeCheck, TVSIL_NORMAL);
	//FTP data
	SendDlgItemMessageW(hwnd, IDC_FTP_EDT_PORT, EM_LIMITTEXT, 5, 0);
	SetDlgItemTextW(hwnd, IDC_FTP_EDT_SERVER, m_TempFTPData.server);
	SetDlgItemTextW(hwnd, IDC_FTP_EDT_DIRECTORY, m_TempFTPData.directory);
	SetDlgItemTextW(hwnd, IDC_FTP_EDT_USER, m_TempFTPData.user);
	SetDlgItemTextW(hwnd, IDC_FTP_EDT_PASSWORD, m_TempFTPData.password);
	SetDlgItemInt(hwnd, IDC_FTP_EDT_PORT, m_TempFTPData.port, false);
	EnableWindow(GetDlgItem(hwnd, IDC_FTP_CMD_SYNC), (wcslen(m_TempFTPData.server) > 0 && m_TempFTPData.port > 0));
	//prepare contacts list view
	GetClientRect(GetDlgItem(hwnd, IDC_LST_CONTACTS), &rc);
	SendDlgItemMessageW(hwnd, IDC_LST_CONTACTS, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	SendDlgItemMessageW(hwnd, IDC_LST_CONTACTS, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_SUBITEMIMAGES, LVS_EX_SUBITEMIMAGES);
	ListView_SetImageList(GetDlgItem(hwnd, IDC_LST_CONTACTS), m_hImlDefCheck, LVSIL_SMALL);
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	for(int i = 0; i < NELEMS(m_ContactCols); i++){
		if(i > 0){
			lvc.cx = 136;
		}
		else{
			lvc.cx = 120;
		}
		lvc.iSubItem = i;
		lvc.pszText = m_ContactCols[i];
		SendDlgItemMessageW(hwnd, IDC_LST_CONTACTS, LVM_INSERTCOLUMNW, i, (LPARAM)&lvc);
	}
	//insert contacts
	hList = GetDlgItem(hwnd, IDC_LST_CONTACTS);
	for(LPPCONTACT lpc = g_TempContacts; lpc; lpc = lpc->next){
		InsertContactIntoList(hList, lpc->prop);
	}
	SendDlgItemMessageW(hwnd, IDC_EDT_SOCKET_PORT, EM_LIMITTEXT, 5, 0);
	SetDlgItemInt(hwnd, IDC_EDT_SOCKET_PORT, m_TempWSPort, false);
	
	//insert contacts groups
	hTree = GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS);
	TreeView_SetImageList(hTree, m_hImlDefCheck, TVSIL_NORMAL);
	for(LPPCONTGROUP pcg = g_TempContGroups; pcg; pcg = pcg->next){
		InsertContGroupToTree(hTree, pcg);
	}

	fEnable = IsBitOn(m_TempNextSettings.flags1, SB3_ENABLE_NETWORK);
	EnableWindow(GetDlgItem(hwnd, IDC_GRP_EXCHANGE), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_ST_CONTACTS), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_LST_CONTACTS), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_TBR_CONTACTS), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_TBR_CONT_GROUPS), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_ST_SOCKET_PORT), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_EDT_SOCKET_PORT), fEnable);
	EnableWindow(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), fEnable);
}

static void Network_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id){
		case IDC_FTP_EDT_SERVER:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_FTP_EDT_SERVER, m_TempFTPData.server, MAX_PATH);
				if(wcslen(m_TempFTPData.server) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_FTP_EDT_PORT)) > 0)
					EnableWindow(GetDlgItem(hwnd, IDC_FTP_CMD_SYNC), true);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_FTP_CMD_SYNC), false);
			}
			break;
		case IDC_FTP_EDT_DIRECTORY:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_FTP_EDT_DIRECTORY, m_TempFTPData.directory, MAX_PATH);
			}
			break;
		case IDC_FTP_EDT_USER:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_FTP_EDT_USER, m_TempFTPData.user, 128);
			}
			break;
		case IDC_FTP_EDT_PASSWORD:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_FTP_EDT_PASSWORD, m_TempFTPData.password, 128);
			}
			break;
		case IDC_FTP_EDT_PORT:
			if(codeNotify == EN_UPDATE){
				wchar_t		szValue[12];
				GetDlgItemTextW(hwnd, IDC_FTP_EDT_PORT, szValue, 12);
				if(wcslen(szValue) == 0 || _wtoi(szValue) == 0){
					wcscpy(szValue, L"21");
					SetWindowTextW(GetDlgItem(hwnd, IDC_FTP_EDT_PORT), szValue);
				}
				m_TempFTPData.port = _wtoi(szValue);
				if(wcslen(m_TempFTPData.server) > 0)
					EnableWindow(GetDlgItem(hwnd, IDC_FTP_CMD_SYNC), true);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_FTP_CMD_SYNC), false);
			}
			break;
		case IDC_EDT_SOCKET_PORT:
			if(codeNotify == EN_UPDATE){
				wchar_t		szValue[8];
				GetDlgItemTextW(hwnd, IDC_EDT_SOCKET_PORT, szValue, 8);
				if(wcslen(szValue) == 0 || _wtoi(szValue) == 0 || _wtoi(szValue) > 65535){
					SetDlgItemInt(hwnd, IDC_EDT_SOCKET_PORT, SOCKET_DEF_PORT, false);
					_itow(SOCKET_DEF_PORT, szValue, 10);
				}
				m_TempWSPort = (USHORT)_wtoi(szValue);
			}
			break;
		case IDC_FTP_CMD_SYNC:
			SynchronizeNow(hwnd, 1);
			break;
		case IDC_CMD_ADD_SOUND:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_CONTACTS)){
				ZeroMemory(&g_Contact, sizeof(PCONTPROP));
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOSTS), hwnd, Hosts_DlgProc, 0) == IDOK){
					InsertContactIntoList(GetDlgItem(hwnd, IDC_LST_CONTACTS), g_Contact);
					g_TempContacts = PContactsAdd(g_TempContacts, &g_Contact);
					AddContactToGroups(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), PContactsItem(g_TempContacts, g_Contact.name));
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_CONT_GROUPS)){
				ZeroMemory(&g_ContGroup, sizeof(PCONTGROUP));
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTGROUPS), hwnd, ContGroups_DlgProc, 0) == IDOK){
					PContGroupsAdd(&g_TempContGroups, &g_ContGroup, true);
					LPPCONTGROUP	lpg = PContGroupsItem(g_TempContGroups, g_ContGroup.name);
					InsertContGroupToTree(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), lpg);
				}
			}
			break;
		}
		case IDC_CMD_MODIFY:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_CONTACTS)){
				LVITEMW		lvi = {0};
				wchar_t		szBuffer[256];
				LPPCONTACT	lpc;

				ZeroMemory(&g_Contact, sizeof(PCONTPROP));
				lvi.iItem = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LST_CONTACTS), -1, LVNI_ALL | LVNI_SELECTED);
				if(lvi.iItem >= 0){
					lvi.mask = LVIF_TEXT;
					lvi.pszText = szBuffer;
					lvi.cchTextMax = 255;
					SendDlgItemMessageW(hwnd, IDC_LST_CONTACTS, LVM_GETITEMW, 0, (LPARAM)&lvi);
					lpc = PContactsItem(g_TempContacts, szBuffer);
					if(lpc){
						int		group = lpc->prop.group;
						memcpy(&g_Contact, &lpc->prop, sizeof(PCONTPROP));
						if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOSTS), hwnd, Hosts_DlgProc, 1) == IDOK){
							memcpy(&lpc->prop, &g_Contact, sizeof(PCONTPROP));
							UpdateContactInList(GetDlgItem(hwnd, IDC_LST_CONTACTS), g_Contact, lvi);
							if(group != lpc->prop.group){
								ReplaceContactInFroups(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), lpc);
							}
						}
					}
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_CONT_GROUPS)){
				wchar_t			szBuffer[128];
				TVITEMW			tvi = {0};
				tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT;
				tvi.hItem = TreeView_GetSelection(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS));
				tvi.cchTextMax = 128;
				tvi.pszText = szBuffer;
				SendDlgItemMessageW(hwnd, IDC_TVW_CONT_GROUPS, TVM_GETITEMW, 0, (LPARAM)&tvi);
				LPPCONTGROUP	lpg = PContGroupsItem(g_TempContGroups, szBuffer);
				memcpy(&g_ContGroup, lpg, sizeof(PCONTGROUP));
				
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTGROUPS), hwnd, ContGroups_DlgProc, 1) == IDOK){
					memcpy(lpg, &g_ContGroup, sizeof(PCONTGROUP));
					tvi.hItem = FindTVGroup(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), NULL, lpg->id);
					wcscpy(szBuffer, lpg->name);
					SendDlgItemMessageW(hwnd, IDC_TVW_CONT_GROUPS, TVM_SETITEMW, 0, (LPARAM)&tvi);
				}
			}
			break;
		}
		case IDC_CMD_DEL_SOUND:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_CONTACTS)){
				LVITEMW		lvi = {0};
				wchar_t		szBuffer[256];
				LPPCONTACT	lpc;

				lvi.iItem = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LST_CONTACTS), -1, LVNI_ALL | LVNI_SELECTED);
				if(lvi.iItem >= 0){
					lvi.mask = LVIF_TEXT;
					lvi.pszText = szBuffer;
					lvi.cchTextMax = 255;
					SendDlgItemMessageW(hwnd, IDC_LST_CONTACTS, LVM_GETITEMW, 0, (LPARAM)&lvi);
					lpc = PContactsItem(g_TempContacts, szBuffer);
					if(lpc){
						wchar_t		szMessage[256];

						GetPrivateProfileStringW(S_MESSAGES, L"contact_delete", L"Delete selected contact?", szMessage, 255, g_NotePaths.CurrLanguagePath);
						if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES){
							ListView_DeleteItem(GetDlgItem(hwnd, IDC_LST_CONTACTS), lvi.iItem);
							g_TempContacts = PContactsRemove(g_TempContacts, szBuffer);
							RemoveContactFromGroup(GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS), szBuffer);
						}
					}
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_CONT_GROUPS)){
				HWND			hTree = GetDlgItem(hwnd, IDC_TVW_CONT_GROUPS);
				wchar_t			szBuffer[128];
				TVITEMW			tvi = {0};
				tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT;
				tvi.hItem = TreeView_GetSelection(hTree);
				tvi.cchTextMax = 128;
				tvi.pszText = szBuffer;
				SendDlgItemMessageW(hwnd, IDC_TVW_CONT_GROUPS, TVM_GETITEMW, 0, (LPARAM)&tvi);
				LPPCONTGROUP	lpg = PContGroupsItem(g_TempContGroups, szBuffer);
				if(lpg){
					wchar_t		szMessage[256], szMessage2[256];

					GetPrivateProfileStringW(S_MESSAGES, L"contacts_group_delete_1", L"Delete selected contacts group?", szMessage, 255, g_NotePaths.CurrLanguagePath);
					GetPrivateProfileStringW(S_MESSAGES, L"contacts_group_delete_2", L"(all contacts from this group will be trasfered to group (None))", szMessage2, 255, g_NotePaths.CurrLanguagePath);
					wcscat(szMessage, L"\n");
					wcscat(szMessage, szMessage2);
					if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES){
						int		group = lpg->id;
						TransferContacts(hTree, group, 0);
						PContGroupsRemove(&g_TempContGroups, szBuffer);
						HTREEITEM	hItem = FindTVGroup(hTree, NULL, group);
						TreeView_DeleteItem(hTree, hItem);
					}
				}
			}
			break;
		}
	}
}

static void UpdateContactInList(HWND hList, PCONTPROP cp, LVITEMW lvi){
	wchar_t			szBuffer[256];

	lvi.mask = lvi.mask | LVIF_IMAGE;
	lvi.iImage = 1;
	lvi.pszText = szBuffer;
	wcscpy(szBuffer, cp.name);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 1;
	if(cp.usename)
		lvi.iImage = 0;
	else
		lvi.iImage = -1;
	wcscpy(szBuffer, cp.host);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 2;
	if(!cp.usename)
		lvi.iImage = 0;
	else
		lvi.iImage = -1;
	if(cp.address != 0)
		swprintf(szBuffer, 16, L"%d.%d.%d.%d", FIRST_IPADDRESS(cp.address), SECOND_IPADDRESS(cp.address), THIRD_IPADDRESS(cp.address), FOURTH_IPADDRESS(cp.address));
	else
		szBuffer[0] = '\0';
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	LSTCOMP			lc = {0};
	lc.hList = hList;
	lc.iSortOrder = LVS_SORTASCENDING;
	SendMessageW(hList, LVM_SORTITEMSEX, (WPARAM)&lc, (LPARAM)ListStringCompareFunc);
}

static void InsertContGroupToTree(HWND hTree, LPPCONTGROUP pcg){
	TVINSERTSTRUCTW		tvs = {0}, tvc = {0};
	HTREEITEM			hItem;

	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvs.item.iImage = 2;
	tvs.item.iSelectedImage = 2;
	tvc.hInsertAfter = TVI_SORT;
	tvc.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvc.item.iImage = 1;
	tvc.item.iSelectedImage = 1;
	tvs.item.pszText = pcg->name;
	tvs.item.lParam = pcg->id;
	hItem = (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	for(LPPCONTACT pc = g_TempContacts; pc; pc = pc->next){
		if(pc->prop.group == pcg->id){
			tvc.hParent = hItem;
			tvc.item.pszText = pc->prop.name;
			SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvc);
		}
	}
}

static void UpdateLocalSyncList(HWND hList, LPPLOCALSYNC lpls, LVITEMW lvi){
	wchar_t			szBuffer[MAX_PATH];

	lvi.pszText = szBuffer;
	lvi.iSubItem = 1;
	wcscpy(szBuffer, lpls->pathNotes);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 2;
	wcscpy(szBuffer, lpls->pathId);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);

}

static void InsertLocalSyncIntoList(HWND hList, LPPLOCALSYNC lpls){
	LVITEMW			lvi = {0};
	wchar_t			szBuffer[MAX_PATH];

	lvi.iItem = SendMessageW(hList, LVM_GETITEMCOUNT, 0, 0);
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = szBuffer;
	lvi.iImage = 3;
	wcscpy(szBuffer, lpls->name);
	SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 1;
	wcscpy(szBuffer, lpls->pathNotes);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 2;
	wcscpy(szBuffer, lpls->pathId);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void InsertContactIntoList(HWND hList, PCONTPROP cp){
	LVITEMW			lvi = {0};
	wchar_t			szBuffer[256];

	lvi.iItem = SendMessageW(hList, LVM_GETITEMCOUNT, 0, 0);
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = szBuffer;
	lvi.iImage = 1;
	wcscpy(szBuffer, cp.name);
	SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 1;
	if(cp.usename)
		lvi.iImage = 0;
	else
		lvi.iImage = -1;
	wcscpy(szBuffer, cp.host);
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	lvi.iSubItem = 2;
	if(!cp.usename)
		lvi.iImage = 0;
	else
		lvi.iImage = -1;
	if(cp.address != 0)
		swprintf(szBuffer, 16, L"%d.%d.%d.%d", FIRST_IPADDRESS(cp.address), SECOND_IPADDRESS(cp.address), THIRD_IPADDRESS(cp.address), FOURTH_IPADDRESS(cp.address));
	else
		szBuffer[0] = '\0';
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	LSTCOMP			lc = {0};
	lc.hList = hList;
	lc.iSortOrder = LVS_SORTASCENDING;
	SendMessageW(hList, LVM_SORTITEMSEX, (WPARAM)&lc, (LPARAM)ListStringCompareFunc);
}

//-----------	Protection stuff	-----------
static BOOL CALLBACK Protection_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMLISTVIEW			*nml;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Protection_OnCommand);
		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_PROTECTION && lpnmh->code == CTVN_CHECKSTATECHANGED){
				LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
				CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
			}
			else if(lpnmh->idFrom == IDC_TVW_CHK_PROTECTION && lpnmh->code == NM_CUSTOMDRAW){
				LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
				return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
			}
			else if(lpnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(lpnmh->idFrom, ttp->szText);
				return true;
			}
			else if(lpnmh->code == LVN_ITEMCHANGED){
				nml = (NMLISTVIEW *)lParam;
				if(nml->uNewState == 3 || nml->uNewState == 2){
					if(lpnmh->idFrom == IDC_LST_LOCAL_SYNC)
						EnableSmallToolbarFull(hwnd, IDC_TBR_LOCAL_SYNC, true, true, true);
				}
				else{
					if(lpnmh->idFrom == IDC_LST_LOCAL_SYNC)
						EnableSmallToolbarFull(hwnd, IDC_TBR_LOCAL_SYNC, true, false, false);
				}
			}
			else if(lpnmh->code == NM_DBLCLK){
				if(lpnmh->idFrom == IDC_LST_LOCAL_SYNC)
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_MODIFY, 0), (LPARAM)GetDlgItem(hwnd, IDC_TBR_LOCAL_SYNC));
			}
			return false;
		}
		default: return false;
	}
}

static void InitDlgProtection(HWND hwnd){
	LVCOLUMNW			lvc = {0};
	HWND				hList;

	if(IsBitOn(m_TempNoteSettings.reserved1, SB1_USE_BACKUP)){
		EnableWindow(GetDlgItem(hwnd, IDC_ST_BACKUP_LEVELS), true);
		EnableWindow(GetDlgItem(hwnd, IDC_EDT_BACKUP_COUNT), true);
		EnableWindow(GetDlgItem(hwnd, IDC_UPD_BACKUP), true);
	}
	else{
		EnableWindow(GetDlgItem(hwnd, IDC_ST_BACKUP_LEVELS), false);
		EnableWindow(GetDlgItem(hwnd, IDC_EDT_BACKUP_COUNT), false);
		EnableWindow(GetDlgItem(hwnd, IDC_UPD_BACKUP), false);
	}
	SetDlgItemInt(hwnd, IDC_EDT_BACKUP_COUNT, GetSmallValue(m_TempSmallValues, SMS_BACKUP_COUNT), false);
	SendDlgItemMessageW(hwnd, IDC_EDT_BACKUP_COUNT, EM_LIMITTEXT, 2, 0);
	SendDlgItemMessageW(hwnd, IDC_UPD_BACKUP, UDM_SETRANGE, 0, (LPARAM) MAKELONG(99, 1));
	//prepare treeview
	m_hTreeProtection = GetDlgItem(hwnd, IDC_TVW_CHK_PROTECTION);
	CTreeView_Subclass(m_hTreeProtection);
	TreeView_SetImageList(m_hTreeProtection, m_hImlTreeCheck, TVSIL_NORMAL);

	hList = GetDlgItem(hwnd, IDC_LST_LOCAL_SYNC);
	SendDlgItemMessageW(hwnd, IDC_LST_LOCAL_SYNC, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	ListView_SetImageList(hList, m_hImlDefCheck, LVSIL_SMALL);
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	for(int i = 0; i < NELEMS(m_LocalSyncCols); i++){
		if(i == 0){
			lvc.cx = 136;
		}
		else{
			lvc.cx = 240;
		}
		lvc.iSubItem = i;
		lvc.pszText = m_LocalSyncCols[i];
		SendDlgItemMessageW(hwnd, IDC_LST_LOCAL_SYNC, LVM_INSERTCOLUMNW, i, (LPARAM)&lvc);
	}
	for(LPPLOCALSYNC pSync = g_TempLocalSyncs; pSync; pSync = pSync->next){
		InsertLocalSyncIntoList(hList, pSync);
	}	
	EnableSmallToolbarFull(hwnd, IDC_TBR_LOCAL_SYNC, true, false, false);
}

static void Protection_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_EDT_BACKUP_COUNT:
			if(codeNotify == EN_UPDATE && m_SettingsDialogLoaded){
				wchar_t		szValue[12];
				int			value = 3;
				GetWindowTextW(GetDlgItem(hwnd, IDC_EDT_BACKUP_COUNT), szValue, 12);
				if(wcslen(szValue) == 0 || _wtoi(szValue) == 0){
					wcscpy(szValue, L"3");
					SetWindowTextW(GetDlgItem(hwnd, IDC_EDT_BACKUP_COUNT), szValue);
				}
				value = _wtoi(szValue);
				SetSmallValue(&m_TempSmallValues, SMS_BACKUP_COUNT, (BYTE)value);
			}
			break;
		case IDC_CMD_ADD_SOUND:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_LOCAL_SYNC)){
				ZeroMemory(&g_LocalSync, sizeof(PLOCALSYNC));
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NEW_SYNC), hwnd, NewSync_DlgProc, 0) == IDOK){
					InsertLocalSyncIntoList(GetDlgItem(hwnd, IDC_LST_LOCAL_SYNC), &g_LocalSync);
					PLocalSyncAdd(&g_TempLocalSyncs, g_LocalSync.name, g_LocalSync.pathNotes, g_LocalSync.pathId);
				}
			}
			break;
		}
		case IDC_CMD_MODIFY:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_LOCAL_SYNC)){
				LVITEMW			lvi = {0};
				wchar_t			szBuffer[256];
				LPPLOCALSYNC	lpls;

				ZeroMemory(&g_LocalSync, sizeof(PLOCALSYNC));
				lvi.iItem = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LST_LOCAL_SYNC), -1, LVNI_ALL | LVNI_SELECTED);
				if(lvi.iItem >= 0){
					lvi.mask = LVIF_TEXT;
					lvi.pszText = szBuffer;
					lvi.cchTextMax = 128;
					SendDlgItemMessageW(hwnd, IDC_LST_LOCAL_SYNC, LVM_GETITEMW, 0, (LPARAM)&lvi);
					lpls = PLocalSyncItem(g_TempLocalSyncs, szBuffer);
					if(lpls){
						memcpy(&g_LocalSync, lpls, sizeof(PLOCALSYNC));
						if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NEW_SYNC), hwnd, NewSync_DlgProc, 1) == IDOK){
							memcpy(lpls, &g_LocalSync, sizeof(PLOCALSYNC));
							UpdateLocalSyncList(GetDlgItem(hwnd, IDC_LST_LOCAL_SYNC), &g_LocalSync, lvi);
						}
					}
				}
			}
			break;
		}
		case IDC_CMD_DEL_SOUND:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_LOCAL_SYNC)){
				LVITEMW			lvi = {0};
				wchar_t			szBuffer[256];
				LPPLOCALSYNC	lpls;

				lvi.iItem = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LST_LOCAL_SYNC), -1, LVNI_ALL | LVNI_SELECTED);
				if(lvi.iItem >= 0){
					lvi.mask = LVIF_TEXT;
					lvi.pszText = szBuffer;
					lvi.cchTextMax = 255;
					SendDlgItemMessageW(hwnd, IDC_LST_LOCAL_SYNC, LVM_GETITEMW, 0, (LPARAM)&lvi);
					lpls = PLocalSyncItem(g_TempLocalSyncs, szBuffer);
					if(lpls){
						wchar_t		szMessage[256];

						GetPrivateProfileStringW(S_MESSAGES, L"local_sync_delete", L"Delete selected synchronization target?", szMessage, 255, g_NotePaths.CurrLanguagePath);
						if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES){
							ListView_DeleteItem(GetDlgItem(hwnd, IDC_LST_LOCAL_SYNC), lvi.iItem);
							PLocalSyncRemove(&g_TempLocalSyncs, szBuffer);
						}
					}
				}
			}
			break;
		}
	}
}

//-----------	Misc stuff	-----------
static BOOL CALLBACK Misc_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMLISTVIEW		* nml;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Misc_OnCommand);

		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_TVW_CHK_MISC && lpnmh->code == CTVN_CHECKSTATECHANGED){
				LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
				CTreeView_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
			}
			else if(lpnmh->idFrom == IDC_TVW_CHK_MISC && lpnmh->code == NM_CUSTOMDRAW){
				LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
				return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
			}
			else if(lpnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(lpnmh->idFrom, ttp->szText);
				return true;
			}
			else if(lpnmh->code == LVN_ITEMCHANGED){
				nml = (NMLISTVIEW *)lParam;
				if(nml->uNewState == 3 || nml->uNewState == 2){
					if(lpnmh->idFrom == IDC_LST_SEARCH_WEB)
						EnableSmallToolbarFull(hwnd, IDC_TBR_SEARCH_WEB, true, true, true);
					else
						EnableSmallToolbarFull(hwnd, IDC_TBR_EXT_PROGS, true, true, true);
				}
				else{
					if(lpnmh->idFrom == IDC_LST_SEARCH_WEB)
						EnableSmallToolbarFull(hwnd, IDC_TBR_SEARCH_WEB, true, false, false);
					else
						EnableSmallToolbarFull(hwnd, IDC_TBR_EXT_PROGS, true, false, false);
				}
			}
			else if(lpnmh->code == NM_DBLCLK){
				if(lpnmh->idFrom == IDC_LST_SEARCH_WEB)
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_MODIFY, 0), (LPARAM)GetDlgItem(hwnd, IDC_TBR_SEARCH_WEB));
				else
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_MODIFY, 0), (LPARAM)GetDlgItem(hwnd, IDC_TBR_EXT_PROGS));
			}
			return false;
		}
		default: return false;
	}
}

static int CALLBACK ExternalNameCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	wchar_t				szName1[128], szName2[128];
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = szName1;
	lvi.iSubItem = 0;
	SendDlgItemMessageW(m_Dialogs.hMisc, IDC_LST_EXT_PROGS, LVM_GETITEMTEXTW, (WPARAM)lParam1, (LPARAM)&lvi);
	lvi.pszText = szName2;
	SendDlgItemMessageW(m_Dialogs.hMisc, IDC_LST_EXT_PROGS, LVM_GETITEMTEXTW, (WPARAM)lParam2, (LPARAM)&lvi);
	if(lParamSort == LVS_SORTASCENDING)
		return _wcsicmp(szName1, szName2);
	else
		return _wcsicmp(szName2, szName1);
}

static void InitDlgMisc(HWND hwnd){
	LVCOLUMNW	lvc = {0};
	LVITEMW		lvi;
	int			index = 0, width;
	RECT		rc;

	SetDlgItemTextW(hwnd, IDC_EDT_DEF_BROWSER, m_sTempdefBrowser);
	EnableSmallToolbarFull(m_Dialogs.hMisc, IDC_TBR_TAGS, false, false, false);
	EnableSmallToolbarFull(m_Dialogs.hMisc, IDC_TBR_SEARCH_WEB, true, false, false);
	EnableSmallToolbarFull(m_Dialogs.hMisc, IDC_TBR_EXT_PROGS, true, false, false);
	for(LPPTAG pTag = m_TempPTagsPredefined; pTag; pTag = pTag->next)
		SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_ADDSTRING, 0, (LPARAM)pTag->text);
	//prepare treeview
	m_hTreeMisc = GetDlgItem(hwnd, IDC_TVW_CHK_MISC);
	CTreeView_Subclass(m_hTreeMisc);
	TreeView_SetImageList(m_hTreeMisc, m_hImlTreeCheck, TVSIL_NORMAL);
	//get scrollbar width
	width = GetSystemMetrics(SM_CXVSCROLL);
	//prepare search engines list view
	GetClientRect(GetDlgItem(hwnd, IDC_LST_SEARCH_WEB), &rc);
	SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	for(int i = 0; i < NELEMS(m_EngCols); i++){
		if(i > 0){
			lvc.cx = (rc.right - rc.left) - 76 - width;
		}
		else{
			lvc.cx = 76;
		}
		lvc.iSubItem = i;
		lvc.pszText = m_EngCols[i];
		SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_INSERTCOLUMNW, i, (LPARAM)&lvc);
	}
	//fill search engines
	for(LPPSENG pSeng = m_TempSengs; pSeng; pSeng = pSeng->next){
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT;
		lvi.iItem = index++;
		lvi.pszText = pSeng->name;
		SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
		lvi.iSubItem = 1;
		lvi.pszText = pSeng->query;
		SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_SETITEMW, 0, (LPARAM)&lvi);
	}
	//prepare external programs list view
	GetClientRect(GetDlgItem(hwnd, IDC_LST_EXT_PROGS), &rc);
	SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	for(int i = 0; i < NELEMS(m_ExtProgsColumns) - 1; i++){
		if(i > 0){
			lvc.cx = (rc.right - rc.left) - 76 - width;
		}
		else{
			lvc.cx = 76;
		}
		lvc.iSubItem = i;
		lvc.pszText = m_ExtProgsColumns[i];
		SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_INSERTCOLUMNW, i, (LPARAM)&lvc);
	}
	//fill external programs
	index = 0;
	for(LPPSENG pSeng = m_TempExternals; pSeng; pSeng = pSeng->next){
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT;
		lvi.iItem = index++;
		lvi.pszText = pSeng->name;
		SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
		lvi.iSubItem = 1;
		lvi.pszText = pSeng->query;
		SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SETITEMW, 0, (LPARAM)&lvi);
	}
	SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SORTITEMSEX, LVS_SORTASCENDING, (LPARAM)ExternalNameCompareFunc);
}

static void Misc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t		szBuffer[128], szModified[128];
	int			count, index = 0;
	LVITEMW		lvi;

	switch(id){
		case IDC_CMD_NEW_V:
			if(codeNotify == BN_CLICKED){
				m_CheckingFromButton = true;
				StartUpdateProcess();
			}
			break;
		case IDC_EDT_DEF_BROWSER:
			if(codeNotify == EN_UPDATE){
				GetDlgItemTextW(hwnd, IDC_EDT_DEF_BROWSER, m_sTempdefBrowser, MAX_PATH);
			}
			break;
		case IDC_LST_PRE_TAGS:
			if(codeNotify == LBN_SELCHANGE){
				index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0);
				if(index != LB_ERR){
					SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, index, (LPARAM)szBuffer);
					SetDlgItemTextW(hwnd, IDC_EDT_NEW_TAG, szBuffer);
					SetFocus(GetDlgItem(hwnd, IDC_EDT_NEW_TAG));
					SendDlgItemMessageW(hwnd, IDC_EDT_NEW_TAG, EM_SETSEL, 0, -1);
				}
			}
			break;
		case IDC_CMD_MODIFY:
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_TAGS)){
				//tags
				GetDlgItemTextW(hwnd, IDC_EDT_NEW_TAG, szBuffer, 128);
				_wcstrm(szBuffer);
				index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, index, (LPARAM)szModified);
				if(_wcsicmp(szBuffer, szModified) != 0){
					SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_DELETESTRING, index, 0);
					SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_INSERTSTRING, index, (LPARAM)szBuffer);
					//if new tag is conained in deleted collection
					if(TagsContains(m_TagsDeleted, szBuffer))
						//remove new tag from deleted collection
						m_TagsDeleted = TagsRemove(m_TagsDeleted, szBuffer);
					//if modified collection contains currently modified tag
					if(TagsContains(m_TagsModified, szModified))
						//replace it with new one
						TagsReplace(m_TagsModified, szBuffer, szModified);
					else
						//else add new tag to modified collection
						m_TagsModified = TagsAdd(m_TagsModified, szBuffer, szModified);
					count = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0);
					if(m_TempPTagsPredefined){
						free(m_TempPTagsPredefined);
						m_TempPTagsPredefined = NULL;
					}
					for(int i = 0; i < count; i++){
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
						m_TempPTagsPredefined = TagsAdd(m_TempPTagsPredefined, szBuffer, NULL);
					}
					SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETCURSEL, index, 0);
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_SEARCH_WEB)){
				index = SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
				if(index >= 0){
					ZeroMemory(&m_Seng, sizeof(m_Seng));
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_TEXT;
					lvi.iItem = index;
					lvi.cchTextMax = 128;
					lvi.pszText = m_Seng.name;
					SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_GETITEMW, 0, (LPARAM)&lvi);
					lvi.iSubItem = 1;
					lvi.cchTextMax = 1024;
					lvi.pszText = m_Seng.query;
					SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_GETITEMW, 0, (LPARAM)&lvi);
					wcscpy(szBuffer, m_Seng.name);
					if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SENGS), hwnd, SEngs_DlgProc, 1) == IDOK){
						LPPSENG	pSeng = SEngByName(m_TempSengs, szBuffer);
						if(pSeng){
							wcscpy(pSeng->name, m_Seng.name);
							wcscpy(pSeng->query, m_Seng.query);
							ZeroMemory(&lvi, sizeof(lvi));
							lvi.mask = LVIF_TEXT;
							lvi.iItem = index;
							lvi.pszText = m_Seng.name;
							SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_SETITEMW, 0, (LPARAM)&lvi);
							lvi.iSubItem = 1;
							lvi.pszText = m_Seng.query;
							SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_SETITEMW, 0, (LPARAM)&lvi);
						}
					}
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_EXT_PROGS)){
				//external programs
				LPPSENG	pSeng;
				index = SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
				if(index >= 0){
					ZeroMemory(&m_External, sizeof(m_External));
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_TEXT;
					lvi.iItem = index;
					lvi.cchTextMax = 128;
					lvi.pszText = m_External.name;
					SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_GETITEMW, 0, (LPARAM)&lvi);
					lvi.iSubItem = 1;
					lvi.cchTextMax = 1024;
					lvi.pszText = m_External.query;
					SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_GETITEMW, 0, (LPARAM)&lvi);
					wcscpy(szBuffer, m_External.name);
					pSeng = SEngByName(m_TempExternals, szBuffer);
					if(pSeng){
						wcscpy(m_External.commandline, pSeng->commandline);
						if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_EXTERNALS), hwnd, Externals_DlgProc, 1) == IDOK){
							wcscpy(pSeng->name, m_External.name);
							wcscpy(pSeng->query, m_External.query);
							wcscpy(pSeng->commandline, m_External.commandline);
							ZeroMemory(&lvi, sizeof(lvi));
							lvi.mask = LVIF_TEXT;
							lvi.iItem = index;
							lvi.pszText = m_External.name;
							SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SETITEMW, 0, (LPARAM)&lvi);
							lvi.iSubItem = 1;
							lvi.pszText = m_External.query;
							SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SETITEMW, 0, (LPARAM)&lvi);
							SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SORTITEMSEX, LVS_SORTASCENDING, (LPARAM)ExternalNameCompareFunc);
						}
					}
				}
			}
			break;
		case IDC_CMD_DEL_SOUND:
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_TAGS)){
				//tags
				index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, index, (LPARAM)szBuffer);
				//add tag to deleted collection
				m_TagsDeleted = TagsAdd(m_TagsDeleted, szBuffer, NULL);
				//if modified collection contains deleted tag
				if(TagsContains(m_TagsModified, szBuffer)){
					//get the original tag for modified
					TagsGetOld(m_TagsModified, szBuffer, szModified);
					//add original tag to deleted collection
					m_TagsDeleted = TagsAdd(m_TagsDeleted, szModified, NULL);
					//remove deleted tag from modified collection
					m_TagsModified = TagsRemove(m_TagsModified, szBuffer);
				}
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_DELETESTRING, index, 0);
				count = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0);
				if(m_TempPTagsPredefined){
					free(m_TempPTagsPredefined);
					m_TempPTagsPredefined = NULL;
				}
				for(int i = 0; i < count; i++){
					SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
					m_TempPTagsPredefined = TagsAdd(m_TempPTagsPredefined, szBuffer, NULL);
				}
				if(count > 0){
					SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETCURSEL, 0, 0);
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_PRE_TAGS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_PRE_TAGS));
				}
				else
					SetDlgItemTextW(hwnd, IDC_EDT_NEW_TAG, NULL);
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_SEARCH_WEB)){
				index = SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
				if(index >= 0){
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_TEXT;
					lvi.iItem = index;
					lvi.cchTextMax = 128;
					lvi.pszText = szBuffer;
					SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_GETITEMW, 0, (LPARAM)&lvi);
					if(MessageBoxW(hwnd, g_Strings.DeleteSEngine, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES){
						m_TempSengs = SEngsRemove(m_TempSengs, szBuffer);
						ListView_DeleteItem(GetDlgItem(hwnd, IDC_LST_SEARCH_WEB), index);
					}
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_EXT_PROGS)){
				index = SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
				if(index >= 0){
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_TEXT;
					lvi.iItem = index;
					lvi.cchTextMax = 128;
					lvi.pszText = szBuffer;
					SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_GETITEMW, 0, (LPARAM)&lvi);
					if(MessageBoxW(hwnd, g_Strings.DeleteExternal, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES){
						m_TempExternals = SEngsRemove(m_TempExternals, szBuffer);
						ListView_DeleteItem(GetDlgItem(hwnd, IDC_LST_EXT_PROGS), index);
					}
				}
			}
			break;
		case IDC_CMD_ADD_SOUND:{
			if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_TAGS)){
				//tags
				GetDlgItemTextW(hwnd, IDC_EDT_NEW_TAG, szBuffer, 128);
				_wcstrm(szBuffer);
				if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == LB_ERR){
					index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
					//if deleted collection contains newly added tag
					if(TagsContains(m_TagsDeleted, szBuffer))
						//remove it from deleted collection
						m_TagsDeleted = TagsRemove(m_TagsDeleted, szBuffer);
					count = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0);
					if(m_TempPTagsPredefined){
						free(m_TempPTagsPredefined);
						m_TempPTagsPredefined = NULL;
					}
					for(int i = 0; i < count; i++){
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
						m_TempPTagsPredefined = TagsAdd(m_TempPTagsPredefined, szBuffer, NULL);
					}
				}
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETCURSEL, index, 0);
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_PRE_TAGS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_PRE_TAGS));
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_SEARCH_WEB)){
				//search engines
				ZeroMemory(&m_Seng, sizeof(m_Seng));
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SENGS), hwnd, SEngs_DlgProc, 0) == IDOK){
					m_TempSengs = SEngsAdd(m_TempSengs, m_Seng.name, m_Seng.query);
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_TEXT;
					lvi.iItem = ListView_GetItemCount(GetDlgItem(hwnd, IDC_LST_SEARCH_WEB));
					lvi.pszText = m_Seng.name;
					SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
					lvi.iSubItem = 1;
					lvi.pszText = m_Seng.query;
					SendDlgItemMessageW(hwnd, IDC_LST_SEARCH_WEB, LVM_SETITEMW, 0, (LPARAM)&lvi);
				}
			}
			else if(hwndCtl == GetDlgItem(hwnd, IDC_TBR_EXT_PROGS)){
				//external programs
				ZeroMemory(&m_External, sizeof(m_External));
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_EXTERNALS), hwnd, Externals_DlgProc, 0) == IDOK){
					m_TempExternals = SEngsAddWithCommandLine(m_TempExternals, m_External.name, m_External.query, m_External.commandline);
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_TEXT;
					lvi.iItem = ListView_GetItemCount(GetDlgItem(hwnd, IDC_LST_EXT_PROGS));
					lvi.pszText = m_External.name;
					SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
					lvi.iSubItem = 1;
					lvi.pszText = m_External.query;
					SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SETITEMW, 0, (LPARAM)&lvi);
					SendDlgItemMessageW(hwnd, IDC_LST_EXT_PROGS, LVM_SORTITEMSEX, LVS_SORTASCENDING, (LPARAM)ExternalNameCompareFunc);
				}
			}
			break;
		}
		case IDC_EDT_NEW_TAG:
			if(codeNotify == EN_UPDATE){
				BOOL	fSelected = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0) >= 0 ? true : false;
				int		len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_NEW_TAG));
				if(len > 0){
					EnableSmallToolbarFull(m_Dialogs.hMisc, IDC_TBR_TAGS, true, fSelected, fSelected);
				}
				else{
					EnableSmallToolbarFull(m_Dialogs.hMisc, IDC_TBR_TAGS, false, false, fSelected);
				}
			}
			break;
		case IDC_CMD_DEF_BROWSER:
			if(codeNotify == BN_CLICKED){
				wchar_t		szBuffer[256], szFileName[MAX_PATH], szFileTitle[128];

				GetPrivateProfileStringW(L"captions", L"browser", L"Choose preferred browser", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, L"Executable files (*.exe)\0*.exe\0\0", szBuffer, NULL)){
					SetDlgItemTextW(hwnd, IDC_EDT_DEF_BROWSER, szFileName);
				}
			}
			break;
	}
}

static bool_settings_2 GetIndexOfDblClick(void){
	if(IsBitOn(g_NoteSettings.reserved2, SB2_NEW_NOTE))
		return SB2_NEW_NOTE;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_CONTROL_PANEL))
		return SB2_CONTROL_PANEL;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_PREFS))
		return SB2_PREFS;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_NOTES))
		return SB2_SEARCH_NOTES;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_LOAD_NOTE))
		return SB2_LOAD_NOTE;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_FROM_CLIPBOARD))
		return SB2_FROM_CLIPBOARD;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_ALL_TO_FRONT))
		return SB2_ALL_TO_FRONT;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_SAVE_ALL))
		return SB2_SAVE_ALL;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_SHOW_HIDE))
		return SB2_SHOW_HIDE;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_TAGS))
		return SB2_SEARCH_TAGS;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_BY_DATES))
		return SB2_SEARCH_BY_DATES;
	else
		return SB2_NEW_NOTE;
}

static HWND NoteFromClipboard(void){
	HGLOBAL			hglb;
	wchar_t			*lptstr;
	HWND			hNote = NULL;

	if(!OpenClipboard(g_hMain))
		return NULL;
	hglb = GetClipboardData(CF_UNICODETEXT); 
    if(hglb){
		lptstr = (wchar_t *)GlobalLock(hglb);
		if(lptstr){
			hNote = CreateNewNote(false);
			if(hNote){
				SendMessageW((HWND)GetPropW(hNote, PH_EDIT), EM_REPLACESEL, false, (LPARAM)lptstr);
			}
			GlobalUnlock(hglb);
		}
	}
	CloseClipboard();
	return hNote;
}

static HWND CreateNewNote(BOOL fCreateDiary){
	HWND		hNote = NULL;
	PMEMNOTE	pNote;

	pNote = AddMemNote();
	if(pNote){
		if(fCreateDiary)
			LoadNewDiaryNoteProperties(pNote, m_DiaryFormats);
		else
			LoadNoteProperties(pNote, &g_RTHandles, NULL, false);
		hNote = CreateNote(pNote, g_hInstance, false, NULL);
	}
	if(!hNote){
		MessageBoxW(g_hMain, m_sNoWindowMessage, NULL, MB_OK);
		return NULL;
	}
	if(fCreateDiary){
		//save diary note immediately
		SaveNote(pNote);
	}
	// if(g_hCPDialog)
		// SendMessageW(g_hCPDialog, PNM_RELOAD, 0, 0);
	return hNote;
}

static BOOL SpellerExists(void){
	wchar_t			szPath[MAX_PATH];

	GetModuleFileNameW(g_hInstance, szPath, MAX_PATH);
	PathRemoveFileSpecW(szPath);
	wcscat(szPath, L"\\");
	wcscat(szPath, SPELL_LIB);
	if(PathFileExistsW(szPath))
		return true;
	return false;
}

static void GetVersionNumber(void){

	char 			szPath[MAX_PATH], szBuffer[MAX_PATH];
	char 			FORMAT_STRING[] = "\\StringFileInfo\\%04x%04x\\%s";
	DWORD 			dwSize, dwBytes = 0;
	HGLOBAL 		hMem;
	UINT 			cbLang;
	LPVOID 			lpt;
	UINT 			cbBuffSize;
	WORD 			* langInfo;

	m_Version[0] = '\0';
	GetModuleFileName(g_hInstance, szPath, MAX_PATH);
	dwBytes = GetFileVersionInfoSize(szPath, &dwSize);
	if(dwBytes){
		hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, dwBytes);
		if(GetFileVersionInfo(szPath, 0, dwBytes, hMem)){
			if(VerQueryValue(hMem, "\\VarFileInfo\\Translation", (LPVOID*)&langInfo, &cbLang)){
				wsprintf(szBuffer, FORMAT_STRING, langInfo[0], langInfo[1], "FileVersion");
				if(VerQueryValue(hMem, szBuffer, &lpt, &cbBuffSize)){
					strcpy(m_Version, lpt);
				}
			}
		}
	}
}

static void StartUpdateProcess(void){
	HANDLE		hThread;
	DWORD		dwID;

	hThread = CreateThread(NULL, 0, CheckForUpdateFunc, g_hMain, 0, &dwID);
	if(hThread){
		CloseHandle(hThread);
	}
}

static void ShowNewVersionBaloon(wchar_t * szNewVersion){

	wchar_t		szBuffer[256];

	wcscpy(szBuffer, g_Strings.NewVersion1);
	wcscat(szBuffer, L" - ");
	wcscat(szBuffer, szNewVersion);
	wcscat(szBuffer, L".");
	if(m_CheckingFromButton){
		wcscat(szBuffer, L"\n");
		wcscat(szBuffer, g_Strings.NewVersion2);
		if(MessageBoxW(m_Dialogs.hMisc, szBuffer, g_Strings.CheckUpdate, MB_OKCANCEL | MB_ICONINFORMATION) == IDOK){
			OpenPage(g_hMain, DOWNLOAD_PAGE);
		}
	}
	else{
		m_TrackBaloonClick = true;
		m_nData.uFlags = NIF_INFO;
		m_nData.uTimeout = 15000;
		m_nData.dwInfoFlags = NIIF_INFO;
		wcscpy(m_nData.szInfoTitle, g_Strings.CheckUpdate);
		wcscpy(m_nData.szInfo, szBuffer);
		Shell_NotifyIconW(NIM_MODIFY, &m_nData);
	}
}

static void ShowNotification(wchar_t * lpInfo, wchar_t * lpCaption, int timeout){
	m_nData.uFlags = NIF_INFO;
	m_nData.uTimeout = timeout;
	m_nData.dwInfoFlags = NIIF_INFO;
	wcscpy(m_nData.szInfoTitle, lpCaption);
	wcscpy(m_nData.szInfo, lpInfo);
	// if(m_ShellVersion == WV_VISTA_AND_MORE && hBalloonIcon){
		// m_nData.dwInfoFlags |= (0x00000020 | 0x00000004);
		// m_nData.hBalloonIcon = hBalloonIcon;
	// }
	Shell_NotifyIconW(NIM_MODIFY, &m_nData);
}

static void ShowReceiveNotification(wchar_t * lpInfo){
	ShowNotification(lpInfo, g_Strings.ReceivedCaption, 15000);
}

static void ShowFirstBaloon(void){
	ShowNotification(g_Strings.FirstBaloonMessage, g_Strings.FirstBaloonCaption, 15000);
}

static win_version ShellVersion(void){
	DLLVERSIONINFO		dvi;
	HMODULE				hLib;
	FARPROC				hProc;
	win_version			result = WV_PRE_XP;

	dvi.cbSize = sizeof(dvi);
	hLib = LoadLibrary("shell32.dll");
	if(hLib){
		hProc = GetProcAddress(hLib, "DllGetVersion");
		if(hProc){
			__asm{
				lea eax, dvi
				push eax
				call hProc
			}
			if(dvi.dwMajorVersion == 6){
				if(dvi.dwBuildNumber > 0){
					result = WV_VISTA_AND_MORE;
				}
				else{
					result = WV_XP;
				}
			}
		}
		FreeLibrary(hLib);
	}
	return result;
}

static BOOL IsNewDLLVersion(char * szLib){
	DLLVERSIONINFO		dvi;
	HMODULE				hLib;
	FARPROC				hProc;
	BOOL				bReturn = false;

	dvi.cbSize = sizeof(dvi);
	hLib = LoadLibrary(szLib);
	if(hLib){
		hProc = GetProcAddress(hLib, "DllGetVersion");
		if(hProc){
			__asm{
				lea eax, dvi
				push eax
				call hProc
			}
			if(dvi.dwMajorVersion > 4)
				bReturn = true;
			else
				bReturn = false;
		}
		else{
			bReturn = false;
		}
		FreeLibrary(hLib);
	}
	return bReturn;
}

static void UnregisterLogonMessage(HWND hwnd){
	HMODULE						hLib;
	UNREGSESSIONMESSAGES		hProc;

	hLib = LoadLibrary("Wtsapi32.dll");
	if(hLib){
		hProc = (UNREGSESSIONMESSAGES)GetProcAddress(hLib, "WTSUnRegisterSessionNotification");
		if(hProc){
			(hProc)(hwnd);
		}
		FreeLibrary(hLib);
	}
}

static void RegisterLogonMessage(HWND hwnd){
	HMODULE						hLib;
	REGSESSIONMESSAGES			hProc;
	BOOL						result;

	hLib = LoadLibrary("Wtsapi32.dll");
	if(hLib){
		hProc = (REGSESSIONMESSAGES)GetProcAddress(hLib, "WTSRegisterSessionNotification");
		if(hProc){
			result = (BOOL)(hProc)(hwnd, NOTIFY_FOR_ALL_SESSIONS);
			if(result){
				m_TrackLogon = true;
			}
			else{
				m_TrackLogon = false;
			}
		}
		else{
			m_TrackLogon = false;
		}
		FreeLibrary(hLib);
	}
}

static win_version WinVer(void){
	OSVERSIONINFOW	osv;
	HMODULE			hLib;
	FARPROC			hProc;
	int				result = WV_PRE_XP;

	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionExW(&osv);
	
	if(osv.dwMajorVersion > 5 ){
		result = WV_VISTA_AND_MORE;
	}
	else if(osv.dwMajorVersion == 5 && osv.dwMinorVersion >=1){
		result = WV_XP;
	}
	else if(osv.dwMajorVersion == 5){
		result = WV_2003;
	}
	if(result > WV_PRE_XP){
		hLib = LoadLibrary("Wtsapi32.dll");
		if(hLib){
			hProc = GetProcAddress(hLib, "WTSRegisterSessionNotification");
			if(hProc){
				m_TrackLogon = true;
			}
			else{
				m_TrackLogon = false;
			}
			FreeLibrary(hLib);
		}
	}
	return result;
}

static void RearrangeDockWindowsOnstart(dock_type dockType){
	PDOCKHEADER			pHeader;
	PDOCKITEM			pTemp;
	int					index = 0;
	P_NOTE_DATA			pD;
	HWND				hTray = NULL;
	PMEMNOTE			pNote = NULL;

	pHeader = DHeader(dockType);
	if(pHeader->count > 0){
		SortDockList(pHeader);
		pTemp = pHeader->pNext;
		while(pTemp){
			pTemp->index = index++;
			pNote = MemNoteByHwnd(pTemp->hwnd);
			pD = pNote->pData;
			//save run-time handles
			memcpy(pNote->pSavedHandles, pNote->pRTHandles, sizeof(NOTE_RTHANDLES));
			SetDockIndex(&pD->dockData, pTemp->index);
			SaveNewDockData(pTemp->hwnd, pD->dockData);
			hTray = MoveDockWindow(pTemp->hwnd, dockType, pTemp->index);
			if(g_RTHandles.hbSkin){
				if(g_DockSettings.fCustSkin){
					SendMessageW(pTemp->hwnd,PNM_APPLY_NEW_SKIN,(WPARAM)&g_DRTHandles, 0);
				}
			}
			else{
				if(g_DockSettings.fCustColor || g_DockSettings.fCustCaption){
					ApplyDockColors(MEMNOTE(pTemp->hwnd));
				}
				RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(pTemp->hwnd);
			}

			if(!pD->onTop){
				SendMessageW(pTemp->hwnd, WM_COMMAND, MAKEWPARAM(IDM_ON_TOP, 1), 0);
			}
			pTemp = pTemp->pNext;
		}
		if(hTray)
			SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}
}

static void GetDockSkinProperties(HWND hwnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkin, BOOL fDeletePrev){
	wchar_t			szTemp[256];

	// DeleteFile("C:\\abc\\PortablePellesC\\PellesC\\Projects\\PNotes 4.0\\test.ini");
	wcscpy(szTemp, lpSkin);
	wcscat(szTemp, L".skn");
	// WritePrivateProfileString("test", "1", "1", "C:\\abc\\PortablePellesC\\PellesC\\Projects\\PNotes 4.0\\test.ini");
	pH->hbSkin = GetSkinById(hwnd, IDS_SKIN_BODY, szTemp);
	// WritePrivateProfileString("test", "2", "1", "C:\\abc\\PortablePellesC\\PellesC\\Projects\\PNotes 4.0\\test.ini");
	GetSkinProperties(hwnd, pH, szTemp, fDeletePrev);
} 

static void ShowHideByTag(wchar_t * lpTag, BOOL fShow){
	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		if(pNote->pTags){
			if(TagsContains(pNote->pTags, lpTag)){
				if(fShow){
					if(!pNote->pData->visible){
						if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
							ShowNote(pNote);
					}
				}
				else{
					if(pNote->pData->visible){
						HideNote(pNote, false);
					}
				}
			}
		}
	}
}

static void BuildFavoritesMenu(void){

	HMENU					hFav;
	int						count, id = 0;
	MITEM					mit;
	P_FAV_MENU_STRUCT		pFavs, pTemp;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;

	hFav = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_FAVORITES));
	count = GetMenuItemCount(hFav);

	EnableMenuItem(hFav, IDM_SHOW_ALL_FAVORITES, MF_BYCOMMAND | MF_GRAYED);
	for(int i = count - 1; i > 0; i--){
		FreeSingleMenu(hFav, i);
		DeleteMenu(hFav, i, MF_BYPOSITION);
	}
	count = CountFavorites();
	if(count > 0){
		pFavs = calloc(count, sizeof(FAV_MENU_STRUCT));
		if(pFavs){
			EnableMenuItem(hFav, IDM_SHOW_ALL_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			AppendMenuW(hFav, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
			pTemp = pFavs;
			for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
				if(pNote->pRTHandles->favorite == GROUP_FAVORITES && pNote->pData->idGroup != GROUP_RECYCLE){
					pTemp->id = ++id + FAVORITES_ADDITION;
					wcscpy(pTemp->pName, pNote->pData->szName);
					wcscpy(pTemp->pReserved, pNote->pFlags->id);
					pTemp++;
				}
			}
			pTemp = pFavs;
			if(count > 1){
				qsort(pTemp, count, sizeof(FAV_MENU_STRUCT), FavMenusCompare);
			}
			for(int i = 0; i < count; i++){
				mit.id = pTemp->id;
				wcscpy(mit.szText, pTemp->pName);
				wcscpy(mit.szReserved, pTemp->pReserved);
				AppendMenuW(hFav, MF_STRING, mit.id, mit.szText);
				SetMenuItemProperties(&mit, hFav, i + 2, true);
				pTemp++;
			}
			free(pFavs);
		}
	}
}


static void LoadWeekdays(wchar_t * lpLang){
	wchar_t			szBuffer[128];
	int				i, j;

	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:
			for(i = 0, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 1:
			for(i = 1, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			SetWeekday(0, 6, lpLang);
			break;
		case 2:
			for(i = 2, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 5; i <= 1; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 3:
			for(i = 3, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 4; i <= 2; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 4:
			for(i = 4, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 3; i <= 3; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 5:
			for(i = 5, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 2; i <= 4; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 6:
			SetWeekday(6, 0, lpLang);
			for(i = 0, j = 1; i <= 5; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
	}
}

static void SetWeekday(int day, int index, const wchar_t * lpFile){

	wchar_t 		szId[12];

	if(g_Weekdays[index] == NULL)
		g_Weekdays[index] = calloc(24, sizeof(wchar_t));
	_itow(day, szId, 10);
	GetPrivateProfileStringW(S_WEEKDAYS, szId, NULL, g_Weekdays[index], 24, lpFile);
}

static void FreeRepParts(void){
	for(int i = 0; i < NELEMS(g_RepParts); i++){
		if(g_RepParts[i])
			free(g_RepParts[i]);
	}
}

static void FreeWeekdays(void){
	for(int i = 0; i < NELEMS(g_Weekdays); i++){
		if(g_Weekdays[i])
			free(g_Weekdays[i]);
	}
}

static void FreeFavoritesMenu(void){

	HMENU			hFav;

	hFav = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_FAVORITES));
	FreeMenus(hFav);
}

static void FindFavorite(wchar_t * id){
	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		if(wcscmp(pNote->pFlags->id, id) == 0){
			if(DockType(pNote->pData->dockData) == DOCK_NONE){
				if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
					ShowNote(pNote);
			}
			return;
		}
	}
}

static void ShowAllFavorites(void){
	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		if(pNote->pRTHandles->favorite == GROUP_FAVORITES && pNote->pData->idGroup != GROUP_RECYCLE){
			if(DockType(pNote->pData->dockData) == DOCK_NONE){
				if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
					ShowNote(pNote);
			}
		}
	}
}

static void ExecuteExternal(int id){
	HMENU			hMenu = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_RUN_PROG));
	wchar_t			szBuffer[128];
	LPPSENG			pSeng;

	GetMenuStringW(hMenu, id, szBuffer, 128, MF_BYCOMMAND);
	pSeng = SEngByName(g_PExternalls, szBuffer);
	if(pSeng){
		if(wcslen(pSeng->commandline) == 0)
			ShellExecuteW(g_hMain, L"open", pSeng->query, NULL, NULL, SW_SHOWDEFAULT);
		else
			ShellExecuteW(g_hMain, L"open", pSeng->query, pSeng->commandline, NULL, SW_SHOWDEFAULT);
	}
}

static void ShowDiaryItem(int id){
	HMENU			hDiary;
	MENUITEMINFOW	mi = {0};
	PMITEM			pmi;
	PMEMNOTE		pNote;

	hDiary = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_DIARY));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA;
	GetMenuItemInfoW(hDiary, id, false, &mi);
	if(mi.dwItemData){
		pmi = (PMITEM)mi.dwItemData;
		pNote = MemNoteById(pmi->szReserved);
		if(pNote){
			ShowDiaryNote(pNote);
		}
	}
}

static void ShowFavorite(int id){
	HMENU			hFav;
	MENUITEMINFOW	mi;
	PMITEM			pmi;

	hFav = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_FAVORITES));
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA;
	GetMenuItemInfoW(hFav, id, false, &mi);
	if(mi.dwItemData){
		pmi = (PMITEM)mi.dwItemData;
		FindFavorite(pmi->szReserved);
	}
}

static PNAV_BUTTON GetNavButton(int id){
	for(int i = 0; i < NELEMS(m_NavButtons); i++){
		if(m_NavButtons[i].id == id)
			return &m_NavButtons[i];
	}
	return NULL;
}

static void PrepareBulletsMenu(void){
	MENUITEMINFOW	mi;
	HMENU			hFormat;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_SUBMENU | MIIM_ID;
	GetMenuItemInfoW(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_FORMAT_NOTE_TEXT), true, &mi);
	hFormat = mi.hSubMenu;
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_BULLETS), true, &mi);
	g_hBulletsMenu = mi.hSubMenu;
}

static void ToggleAllPriorityProtectComplete(int member, BOOL flag){
	BOOL			fApply;
	NOTE_DATA		data;

	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		fApply = false;
		if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
			if(!flag){
				//remove flag (if it is)
				if(IsBitOn(pNote->pData->res1, member)){
					BitOff(&pNote->pData->res1, member);
					fApply = true;
				}
			}
			else{
				//set flag (if it's not)
				if(!IsBitOn(pNote->pData->res1, member)){
					BitOn(&pNote->pData->res1, member);
					fApply = true;
				}
			}
		}
		if(fApply){
			if(pNote->pData->visible){
				RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(pNote->hwnd);
			}
			if(g_hCPDialog)
				SendMessageW(g_hCPDialog, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			if(pNote->pFlags->fromDB){
				if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
					if(IsBitOn(pNote->pData->res1, member))
						BitOn(&data.res1, member);
					else
						BitOff(&data.res1, member);
					WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				}
			}
		}
	}
}

static void ToggleAllRolled(BOOL flag){
	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
			if(!flag){
				if(pNote->pRTHandles->rolled)
					UnrollNote(pNote);
			}
			else{
				if(!pNote->pRTHandles->rolled)
					RollNote(pNote);
			}
		}
	}
}

static void ToggleAllOnTop(BOOL flag){
	NOTE_DATA		data;

	for(PMEMNOTE pNote = MemoryNotes(); pNote; pNote = pNote->next){
		if(flag){
			if(!pNote->pData->onTop){
				pNote->pData->onTop = TRUE;
				pNote->pData->prevOnTop = TRUE;
				if(pNote->pData->visible){
					SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}
		}
		else{
			if(pNote->pData->onTop){
				pNote->pData->onTop = FALSE;
				pNote->pData->prevOnTop = FALSE;
				if(pNote->pData->visible){
					SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					SendMessageW(g_hMain, PNM_ON_TOP_CHANGED, 0, 0);
				}
			}
		}
		if(pNote->pData->visible){
			RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
			ShowNoteMarks(pNote->hwnd);
		}
		if(pNote->pFlags->fromDB){
			if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
				if(pNote->pData->onTop){
					data.onTop = TRUE;
					data.prevOnTop = TRUE;
				}
				else{
					data.onTop = FALSE;
					data.prevOnTop = FALSE;
				}
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			}
		}
	}
}

void CTreeView_OnCheckStateChanged(HWND hwnd, HTREEITEM hItem, BOOL fChecked){
	if(hwnd == m_hTreeBehavior){
		if(hItem == m_ChecksTBehavior[bhSaveAlways])
			m_TempNoteSettings.saveOnExit = fChecked;
		else if(hItem == m_ChecksTBehavior[bhAskBefore])
			m_TempNoteSettings.confirmSave = fChecked;
		else if(hItem == m_ChecksTBehavior[bhConfirmDel])
			m_TempNoteSettings.confirmDelete = fChecked;
		else if(hItem == m_ChecksTBehavior[bhHideWithout]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_HIDE_WO_PROMPT);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_WO_PROMPT);
		}
		else if(hItem == m_ChecksTBehavior[bhAlwaysOnTop])
			m_TempNoteSettings.newOnTop = fChecked;
		else if(hItem == m_ChecksTBehavior[bhRelational]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_RELPOSITION);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_RELPOSITION);
		}
		else if(hItem == m_ChecksTBehavior[bhHideCompleted]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_HIDE_COMPLETED);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_HIDE_COMPLETED);
		}
		else if(hItem == m_ChecksTBehavior[bhBigIcons]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_BIG_ICONS);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_BIG_ICONS);
		}
		else if(hItem == m_ChecksTBehavior[bhNoWindowList]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_NO_ALT_TAB);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_NO_ALT_TAB);
		}
		else if(hItem == m_ChecksTBehavior[bhExcAeroPeek]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_DWM_REM_FROM_PEEK);
		}
		else if(hItem == m_ChecksTBehavior[bhCleanBin]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_CLEAN_BIN_WITH_WARNING);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_CLEAN_BIN_WITH_WARNING);
		}
	}
	else if(hwnd == m_hTreeSkins){
		if(hItem == m_ChecksTSkins[skRandomColor]){
			if(fChecked){
				BitOn(&m_TempNoteSettings.reserved1, SB1_RANDCOLOR);
				CTreeView_SetEnable(m_hTreeSkins, m_ChecksTSkins[skInvertColor], true);
			}
			else{
				BitOff(&m_TempNoteSettings.reserved1, SB1_RANDCOLOR);	
				CTreeView_SetEnable(m_hTreeSkins, m_ChecksTSkins[skInvertColor], false);
			}
		}
		else if(hItem == m_ChecksTSkins[skInvertColor]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_INVERT_TEXT);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_INVERT_TEXT);	
		}
		else if(hItem == m_ChecksTSkins[skRollUnroll]){
			m_TempNoteSettings.rollOnDblClick = fChecked;
			CTreeView_SetEnable(m_hTreeSkins, m_ChecksTSkins[skFittToCaption], fChecked);
		}
		else if(hItem == m_ChecksTSkins[skFittToCaption]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_FIT_TO_CAPTION);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_FIT_TO_CAPTION);	
		}
	}
	else if(hwnd == m_hTreeMisc){
		if(hItem == m_ChecksTMisc[msStartWithWindows])
			m_TempNoteSettings.onStartup = fChecked;
		else if(hItem == m_ChecksTMisc[msShowCPOnStart]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_SHOW_CP);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_SHOW_CP);
		}
		else if(hItem == m_ChecksTMisc[msCheckNewVOnStart])
			m_TempNoteSettings.checkOnStart = fChecked;
		else if(hItem == m_ChecksTMisc[msHideFluently]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_HIDE_FLUENTLY);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_FLUENTLY);
		}
		else if(hItem == m_ChecksTMisc[msPlaySoundOnHide]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_PLAY_HIDE_SOUND);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_PLAY_HIDE_SOUND);
		}
	}
	else if(hwnd == m_hTreeApp){
		if(hItem == m_ChecksTApp[apTransparency]){
			EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_GRP_TRANS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_LEFT_ST), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_RIGHT_ST), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_TRACK_BAR), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_VALUE), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_WARNING), fChecked);
			m_TempNoteSettings.transAllow = fChecked;
			if(fChecked && m_TempNoteSettings.showScrollbar){
				WarningScrollbarsTransparency();
			}
		}
		else if(hItem == m_ChecksTApp[apHideToolbar]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR);	
		}
		else if(hItem == m_ChecksTApp[apHideDelete]){
			if(fChecked){
				BitOn(&m_TempNoteSettings.reserved1, SB1_HIDE_DELETE);
				CTreeView_SetEnable(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], true);
			}
			else{
				BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_DELETE);	
				CTreeView_SetCheckBoxState(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], false);
				CTreeView_SetEnable(m_hTreeApp, m_ChecksTApp[apCrossInstTrg], false);
				BitOff(&m_TempNoteSettings.reserved1, SB1_CROSS_INST_TRNGL);
			}
		}
		else if(hItem == m_ChecksTApp[apCrossInstTrg]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_CROSS_INST_TRNGL);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_CROSS_INST_TRNGL);	
		}
		else if(hItem == m_ChecksTApp[apHideHide]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_HIDE_HIDE);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_HIDE_HIDE);	
		}
		else if(hItem == m_ChecksTApp[apCustomFonts]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_CUST_FONTS);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_CUST_FONTS);	
		}
		else if(hItem == m_ChecksTApp[apShowScroll]){
			m_TempNoteSettings.showScrollbar = fChecked;
			if(fChecked && m_TempNoteSettings.transAllow){
				WarningScrollbarsTransparency();
			}
		}
	}
	else if(hwnd == m_hTreeSchedule){
		if(hItem == m_ChecksTSchedule[scAllowSound]){
			int index = SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
			SendMessageW(GetDlgItem(m_Dialogs.hSchedule, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_ADD_SOUND, fChecked);
			SendMessageW(GetDlgItem(m_Dialogs.hSchedule, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, (fChecked ? (index > 0 ? true : false) : false));
			SendMessageW(GetDlgItem(m_Dialogs.hSchedule, IDC_TBR_SCHEDULE), TB_ENABLEBUTTON, IDC_CMD_LISTEN, fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hSchedule, IDC_LST_SOUND), fChecked);
			m_TempSound.allowSound = fChecked;
		}
		else if(hItem == m_ChecksTSchedule[scVisualAlert])
			m_TempNoteSettings.visualAlert = fChecked;
		else if(hItem == m_ChecksTSchedule[scTrackOverdue]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE);
		}
		else if(hItem == m_ChecksTSchedule[scDontMove]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_DONOT_CENTER);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_DONOT_CENTER);
		}
	}
	else if(hwnd == m_hTreeNetwork){
		if(hItem == m_ChecksTNetwork[ntIncDelSync]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_INC_DEL_IN_SYNC);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_INC_DEL_IN_SYNC);
		}
		else if(hItem == m_ChecksTNetwork[ntSyncStart]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_SYNC_ON_START);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_SYNC_ON_START);
		}
		else if(hItem == m_ChecksTNetwork[ntSaveBeforeSync]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_SAVE_BEFORE_SYNC);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_SAVE_BEFORE_SYNC);
		}
		else if(hItem == m_ChecksTNetwork[ntSaveBeforeSending]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_SAVE_BEFORE_SEND);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_SAVE_BEFORE_SEND);
		}
		else if(hItem == m_ChecksTNetwork[ntNotShowNotifyNewMessage]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_RECEIVE_HIDE_BALOON);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_RECEIVE_HIDE_BALOON);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowNoteClickNotify], !fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowCPClickNotify], !fChecked);
		}
		else if(hItem == m_ChecksTNetwork[ntNotPlaySoundNewMessage]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_RECEIVE_NOT_PLAY_SOUND);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_RECEIVE_NOT_PLAY_SOUND);
		}
		else if(hItem == m_ChecksTNetwork[ntShowNoteClickNotify]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_NOTE);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_NOTE);
		}
		else if(hItem == m_ChecksTNetwork[ntShowCPClickNotify]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_CP);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_CLICK_BALOON_SHOW_CP);
		}
		else if(hItem == m_ChecksTNetwork[ntNotShowSendMessage]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_SEND_HIDE_BALOON);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_SEND_HIDE_BALOON);
		}
		else if(hItem == m_ChecksTNetwork[ntShowAfterReceiving]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_SHOW_AFTER_RECEIVING);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_SHOW_AFTER_RECEIVING);
		}
		else if(hItem == m_ChecksTNetwork[ntHideAfterSend]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_HIDE_AFTER_SEND);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_HIDE_AFTER_SEND);
		}
		else if(hItem == m_ChecksTNetwork[ntNotShowContactsInMenu]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONT_MENU);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONT_MENU);
		}
		else if(hItem == m_ChecksTNetwork[ntEnableNetwork]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_ENABLE_NETWORK);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_ENABLE_NETWORK);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntSaveBeforeSending], fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowNotifyNewMessage], fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotPlaySoundNewMessage], fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowSendMessage], fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowNoteClickNotify], fChecked && !IsBitOn(m_TempNextSettings.flags1, SB3_RECEIVE_HIDE_BALOON));
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowCPClickNotify], fChecked && !IsBitOn(m_TempNextSettings.flags1, SB3_RECEIVE_HIDE_BALOON));
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntNotShowContactsInMenu], fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntHideAfterSend], fChecked);
			CTreeView_SetEnable(m_hTreeNetwork, m_ChecksTNetwork[ntShowAfterReceiving], fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_GRP_EXCHANGE), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_ST_CONTACTS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_LST_CONTACTS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_TBR_CONTACTS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_TBR_CONT_GROUPS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_TVW_CONT_GROUPS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_ST_SOCKET_PORT), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hNetwork, IDC_EDT_SOCKET_PORT), fChecked);
		}
	}
	else if(hwnd == m_hTreeProtection){
		if(hItem == m_ChecksTProtection[prEncryption]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_STORE_ENCRYPTED);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_STORE_ENCRYPTED);
		}
		else if(hItem == m_ChecksTProtection[prHideFromTray]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_HIDE_TRAY);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_HIDE_TRAY);
		}
		else if(hItem == m_ChecksTProtection[prBackupNotes]){
			EnableWindow(GetDlgItem(m_Dialogs.hProtection, IDC_ST_BACKUP_LEVELS), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hProtection, IDC_EDT_BACKUP_COUNT), fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hProtection, IDC_UPD_BACKUP), fChecked);
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_USE_BACKUP);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_USE_BACKUP);
		}
		else if(hItem == m_ChecksTProtection[prSilentBackup]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_SILENT_BACKUP);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_SILENT_BACKUP);
		}
		else if(hItem == m_ChecksTProtection[prDontShowContent]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONTENT);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_DONOT_SHOW_CONTENT);
		}
		else if(hItem == m_ChecksTProtection[prSyncLocalBin]){
			if(fChecked)
				BitOn(&m_TempNextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE);
			else
				BitOff(&m_TempNextSettings.flags1, SB3_LOCAL_SYNC_RECYCLE);
		}
	}
	else if(hwnd == m_hTreeDiary){
		if(hItem == m_ChecksTDiary[drAddWeekday]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_ADD_WEEKDAY_DIARY);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_ADD_WEEKDAY_DIARY);
			CTreeView_SetEnable(m_hTreeDiary, m_ChecksTDiary[drFullWeekday], fChecked);
			CTreeView_SetEnable(m_hTreeDiary, m_ChecksTDiary[drWeekdayAtEnd], fChecked);
			ShowDiaryDateSample(m_Dialogs.hDiary);
		}
		else if(hItem == m_ChecksTDiary[drFullWeekday]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_FULL_WEEKDAY_NAME);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_FULL_WEEKDAY_NAME);
			ShowDiaryDateSample(m_Dialogs.hDiary);
		}
		else if(hItem == m_ChecksTDiary[drWeekdayAtEnd]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_WEEKDAY_DIARY_END);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_WEEKDAY_DIARY_END);
			ShowDiaryDateSample(m_Dialogs.hDiary);
		}
		else if(hItem == m_ChecksTDiary[drNoPages]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_NO_DIARY_PAGES);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_NO_DIARY_PAGES);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_DIARY_COUNT), !fChecked);
			EnableWindow(GetDlgItem(m_Dialogs.hDiary, IDC_CBO_DIARY_COUNT), !fChecked);
		}
		else if(hItem == m_ChecksTDiary[drDiaryCust]){
			if(fChecked){
				BitOn(&m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS);
				if(m_TempDiaryAppearance.crWindow == 0)
					m_TempDiaryAppearance.crWindow = m_TempAppearance.crWindow;
				if(m_TempDRTHandles.hFCaption == 0)
					m_TempDiaryRTHandles.hFCaption = CreateFontIndirectW(&m_TempAppearance.lfCaption);
				if(wcslen(m_TempDiaryAppearance.szSkin) > 0){
					EnableDiaryCustPart(ENDC_ENABLED_WITH_SKIN);
					GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
					RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
				}
				else{
					EnableDiaryCustPart(ENDC_ENABLED_WITHOUT_SKIN);
				}
			}
			else{
				BitOff(&m_TempNoteSettings.reserved1, SB1_CUST_DIARY_SETTINGS);
				EnableDiaryCustPart(ENDC_DISABLED);
				// memcpy(&m_TempDiaryAppearance, &m_TempAppearance, sizeof(NOTE_APPEARANCE));
				m_TempDiaryAppearance = m_TempAppearance;
				if(wcscmp(m_TempDiaryAppearance.szSkin, DS_NO_SKIN) == 0)
					*m_TempDiaryAppearance.szSkin = '\0';
				GetDiarySkin(m_TempDiaryAppearance, &m_TempDiaryRTHandles);
				DeleteFont(m_TempDiaryRTHandles.hFCaption);
				m_TempDiaryRTHandles.hFCaption = CreateFontIndirectW(&m_TempDiaryAppearance.lfCaption);
				RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
				RedrawWindow(GetDlgItem(m_Dialogs.hDiary, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
			}
			SendDlgItemMessageW(m_Dialogs.hDiary, IDC_LST_SKIN, LB_SETCURSEL, SendDlgItemMessageW(m_Dialogs.hDiary, IDC_LST_SKIN, LB_FINDSTRINGEXACT, -1, (LPARAM)m_TempDiaryAppearance.szSkin), 0);
		}
		else if(hItem == m_ChecksTDiary[drAscSort]){
			if(fChecked)
				BitOn(&m_TempNoteSettings.reserved1, SB1_DIARY_SORT_ASC);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_DIARY_SORT_ASC);
		}
	}
	else if(hwnd == m_hTreeDock){
		if(hItem == m_ChecksTDock[dcInverseOrder]){
			m_TempDockingSettings.fInvOrder = fChecked;
		}
		else if(hItem == m_ChecksTDock[dcCustomSkins]){
			if(fChecked){
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_LST_SKIN), true);
				m_TempDockingSettings.fCustSkin = true;
				wchar_t		szTemp[128];
				SendDlgItemMessageW(m_Dialogs.hDocks, IDC_LST_SKIN, LB_GETTEXT, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETCURSEL, 0, 0), (LPARAM)szTemp);
				wcscpy(m_TempDockingSettings.szCustSkin, szTemp);
				GetDockSkinProperties(m_Dialogs.hDocks, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, false);
				RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
			}
			else{
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_LST_SKIN), false);
				m_TempDockingSettings.fCustSkin = false;
				if(m_TempRHandles.hbSkin)
					wcscpy(m_TempDockingSettings.szCustSkin, m_TempAppearance.szSkin);
				else
					*m_TempDockingSettings.szCustSkin = '\0';
				GetDockSkinProperties(m_Dialogs.hDocks, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, false);
				RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
			}
		}
		else if(hItem == m_ChecksTDock[dcCustSize]){
			if(fChecked){
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_W), true);
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_H), true);
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W), true);
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H), true);
				m_TempDockingSettings.fCustSize = true;
			}
			else{
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_W), false);
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_H), false);
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W), false);
				EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H), false);
				m_TempDockingSettings.fCustSize = false;
			}
		}
		else if(hItem == m_ChecksTDock[dcCustColor]){
			m_TempDockingSettings.fCustColor = fChecked;
			EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CHOOSE_COLOR), fChecked);	
			EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_BCKG_COLOR), fChecked);	
		}
		else if(hItem == m_ChecksTDock[dcCustFont]){
			m_TempDockingSettings.fCustCaption = fChecked;
			EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CMD_SKNLESS_CFONT), fChecked);
			if(!fChecked){
				CreateDefaultFont(&m_TempDockingSettings.lfCaption, true); 
				if(g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption)
					DeleteFont(m_TempDRTHandles.hFCaption);
				m_TempDRTHandles.hFCaption = CreateFontIndirectW(&m_TempDockingSettings.lfCaption);
				RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
			}
			// EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_BCKG_COLOR), fChecked);
		}
	}
}

static void GetTTText(int id, wchar_t * lpText){
	PTBRTOOLTIP		ptt = m_Tooltips;

	*lpText = '\0';
	for(int i = 0; i < NELEMS(m_Tooltips); i++){
		if(ptt->id == id){
			wcscpy(lpText, ptt->szTip);
			return;
		}
		ptt++;
	}
}

static void EnableSmallToolbarFull(HWND hDlg, int idTbr, BOOL fAdd, BOOL fModify, BOOL fDelete){
	SendDlgItemMessageW(hDlg, idTbr, TB_ENABLEBUTTON, IDC_CMD_ADD_SOUND, fAdd);
	SendDlgItemMessageW(hDlg, idTbr, TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, fDelete);
	SendDlgItemMessageW(hDlg, idTbr, TB_ENABLEBUTTON, IDC_CMD_MODIFY, fModify);
}

static void EnableSmallToolbar(HWND hwnd, int id, BOOL fHideAdd){
	SendDlgItemMessageW(hwnd, id, TB_ENABLEBUTTON, IDC_CMD_ADD_SOUND, !fHideAdd);
	SendDlgItemMessageW(hwnd, id, TB_ENABLEBUTTON, IDC_CMD_DEL_SOUND, fHideAdd);
	SendDlgItemMessageW(hwnd, id, TB_ENABLEBUTTON, IDC_CMD_MODIFY, fHideAdd);
}

static void PrepareSmallToolbars(void){
	RECT		rc;
	int			w;
	HWND		hToolbar;

	hToolbar = GetDlgItem(m_Dialogs.hSchedule, IDC_TBR_SCHEDULE);
	GetClientRect(hToolbar, &rc);
	w = (rc.right - rc.left) / 3;
	SendMessageW(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(w, rc.bottom - rc.top));
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_ScheduleButtons), m_ScheduleButtons);
	hToolbar = GetDlgItem(m_Dialogs.hMisc, IDC_TBR_TAGS);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(m_Dialogs.hMisc, IDC_TBR_SEARCH_WEB);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(m_Dialogs.hMisc, IDC_TBR_EXT_PROGS);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(m_Dialogs.hNetwork, IDC_TBR_CONTACTS);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(m_Dialogs.hNetwork, IDC_TBR_CONT_GROUPS);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(m_Dialogs.hProtection, IDC_TBR_LOCAL_SYNC);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
}

static void WarningScrollbarsTransparency(void){
	wchar_t 	szMessage[128];

	if(g_WinVer < WV_VISTA_AND_MORE){
		GetPrivateProfileStringW(S_MESSAGES, L"warning_trans_scroll", L"Allowing both transparency and scroll bars may result in hangs on systems prior to Windows Vista.", szMessage, 128, g_NotePaths.CurrLanguagePath);
		MessageBoxW(g_hOptionsDlg, szMessage, PROG_NAME, MB_OK | MB_ICONEXCLAMATION);
	}
}

static BOOL CALLBACK HelpMissing_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_CLOSE, HelpMissing_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, HelpMissing_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, HelpMissing_OnInitDialog);
		case DNLM_PROGRESS:
			UpdateDownloadProgress(hwnd, wParam, lParam);
			return true;
		case DNLM_FINISHED:
			ClearHelpMissing(hwnd);
			EndDialog(hwnd, IDCANCEL);
			ShellExecuteW(hwnd, L"open", gd_LocalFile, NULL, NULL, SW_SHOWNORMAL);
			return true;
		case DNLM_INETERROR:
			ClearHelpMissing(hwnd);
			if(wcslen(gd_ErrDesc) > 0)
				MessageBoxW(hwnd, gd_ErrDesc, PROG_NAME, MB_OK | MB_ICONERROR);
			else
				MessageBoxW(hwnd, g_Strings.InternetUnavailable, PROG_NAME, MB_OK | MB_ICONERROR);
			EndDialog(hwnd, IDCANCEL);
			return true;
	default: return false;
	}
}

static void HelpMissing_OnClose(HWND hwnd)
{
	ClearHelpMissing(hwnd);
	EndDialog(hwnd, IDCANCEL);
}

static void HelpMissing_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t			szURL[MAX_PATH * 2];

	switch(id){
		case IDOK:
			if(IsDlgButtonChecked(hwnd, IDC_CHK_NOT_SHOW) == BST_CHECKED){
				//suppress missing help message
				WritePrivateProfileStringW(S_NOTE, IK_SHOW_MISSING_HELP, L"0", g_NotePaths.INIFile);
			}
			if(IsDlgButtonChecked(hwnd, IDC_OPT_HELP_ONLINE) == BST_CHECKED){
				ClearHelpMissing(hwnd);
				EndDialog(hwnd, IDOK);
			}
			else if(IsDlgButtonChecked(hwnd, IDC_OPT_HELP_CHM) == BST_CHECKED){
				wcscpy(szURL, DOWNLOAD_ROOT);
				wcscat(szURL, DOWNLOAD_CHM);
				DownloadHelpFile(hwnd, szURL, m_sPathCHM);
					// ShellExecuteW(hwnd, L"open", m_sPathCHM, NULL, NULL, SW_SHOWNORMAL);
				// EndDialog(hwnd, IDCANCEL);
			}
			else if(IsDlgButtonChecked(hwnd, IDC_OPT_HELP_PDF) == BST_CHECKED){
				wcscpy(szURL, DOWNLOAD_ROOT);
				wcscat(szURL, DOWNLOAD_PDF);
				DownloadHelpFile(hwnd, szURL, m_sPathPDF);
					// ShellExecuteW(hwnd, L"open", m_sPathPDF, NULL, NULL, SW_SHOWNORMAL);
				// EndDialog(hwnd, IDCANCEL);
			}
			break;
		case IDCANCEL:
			ClearHelpMissing(hwnd);
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDC_OPT_HELP_ONLINE:
			if(IsDlgButtonChecked(hwnd, IDC_OPT_HELP_ONLINE) == BST_CHECKED){
				EnableWindow(GetDlgItem(hwnd, IDC_CHK_NOT_SHOW), true);
			}
			break;
		case IDC_OPT_HELP_CHM:
			if(IsDlgButtonChecked(hwnd, IDC_OPT_HELP_CHM) == BST_CHECKED){
				CheckDlgButton(hwnd, IDC_CHK_NOT_SHOW, BST_UNCHECKED);
				EnableWindow(GetDlgItem(hwnd, IDC_CHK_NOT_SHOW), false);
			}
			break;
		case IDC_OPT_HELP_PDF:
			if(IsDlgButtonChecked(hwnd, IDC_OPT_HELP_PDF) == BST_CHECKED){
				CheckDlgButton(hwnd, IDC_CHK_NOT_SHOW, BST_UNCHECKED);
				EnableWindow(GetDlgItem(hwnd, IDC_CHK_NOT_SHOW), false);
			}
			break;
	}
}

static BOOL HelpMissing_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t 		szBuffer[256];
	HMODULE			hIcons;

	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1039", L"Help file chooser", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_MISSING_HELP, g_NotePaths.CurrLanguagePath, L"Local help file is missing. Please, choose one of the options below:");
	SetDlgCtlText(hwnd, IDC_OPT_HELP_ONLINE, g_NotePaths.CurrLanguagePath, L"Continue to on-line documentation");
	SetDlgCtlText(hwnd, IDC_OPT_HELP_CHM, g_NotePaths.CurrLanguagePath, L"Download help file in CHM format");
	SetDlgCtlText(hwnd, IDC_OPT_HELP_PDF, g_NotePaths.CurrLanguagePath, L"Download help file in PDF format");
	SetDlgCtlText(hwnd, IDC_CHK_NOT_SHOW, g_NotePaths.CurrLanguagePath, L"Do not show this message again");
	CheckDlgButton(hwnd, IDC_OPT_HELP_ONLINE, BST_CHECKED);
	m_IsProgressTimer = false;
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		for(int i = IDR_ICO_PROGRESS_1, j = 0; i <= IDR_ICO_PROGRESS_6; i++, j++){
			m_ProgressIcons[j] = LoadIconW(hIcons, MAKEINTRESOURCEW(i));
		}
		FreeLibrary(hIcons);
	}
	return false;
}

static void ClearHelpMissing(HWND hwnd){
	for(int i = IDR_ICO_PROGRESS_1; i <= IDR_ICO_PROGRESS_6; i++){
		if(m_ProgressIcons[i]){
			DestroyIcon(m_ProgressIcons[i]);
			m_ProgressIcons[i] = NULL;
		}
	}
	if(m_IsProgressTimer){
		KillTimer(hwnd, TIMER_PROGRESS_ID);
		m_IsProgressTimer = false;
	}
}

static void UpdateDownloadProgress(HWND hwnd, int total, int ready){
	wchar_t			szBuffer[256], szProgress[256];
	int				position = (ready * 100) / total;

	wcscpy(szProgress, g_Strings.Downloading);
	_ltow(position, szBuffer, 10);
	wcscat(szBuffer, L"%");
	wcscat(szProgress, szBuffer);
	SetDlgItemTextW(hwnd, IDC_ST_DOWNLOAD_PERC, szProgress);
}

static void DownloadHelpFile(HWND hwnd, wchar_t * lpURL, wchar_t * lpFile){
	HANDLE		hThread;
	DWORD		dwID;

	SetDlgItemTextW(hwnd, IDC_ST_DOWNLOAD_PERC, g_Strings.Downloading);
	m_ProgressIconIndex = 0;
	m_IsProgressTimer = (BOOL)SetTimer(hwnd, TIMER_PROGRESS_ID, 200, ProgressTimerProc);
	EnableWindow(GetDlgItem(hwnd, IDC_OPT_HELP_ONLINE), false);
	EnableWindow(GetDlgItem(hwnd, IDC_OPT_HELP_CHM), false);
	EnableWindow(GetDlgItem(hwnd, IDC_OPT_HELP_PDF), false);
	EnableWindow(GetDlgItem(hwnd, IDC_CHK_NOT_SHOW), false);
	EnableWindow(GetDlgItem(hwnd, IDOK), false);
	EnableWindow(GetDlgItem(hwnd, IDCANCEL), false);
	wcscpy(gd_URL, lpURL);
	wcscpy(gd_LocalFile, lpFile);
	hThread = CreateThread(NULL, 0, DownloadFileFunc, hwnd, 0, &dwID);
	if(hThread){
		CloseHandle(hThread);
	}
}

static void BackupOnCommandLine(wchar_t * lpDirectory){
	wchar_t				szFileName[MAX_PATH];

	// if(!NotesDirExists(g_NotePaths.BackupDir))
		// NotesDirCreate(g_NotePaths.BackupDir);
	if(lpDirectory != NULL){
		ConstructAutomaticBackupName(lpDirectory, szFileName);
	}
	else{
		ConstructAutomaticBackupName(NULL, szFileName);
	}
	DoFullBackup(szFileName);
}

static void FullBackup(HWND hwnd, BOOL fAutomatic, BOOL fMessage){
	wchar_t				szBuffer[256], szFileName[MAX_PATH], szFileTitle[MAX_PATH];

	if(!NotesDirExists(g_NotePaths.BackupDir))
		NotesDirCreate(g_NotePaths.BackupDir);
	if(!fAutomatic){
		GetPrivateProfileStringW(L"captions", L"save_full_backup", L"Create full backup", szBuffer, 256, g_NotePaths.CurrLanguagePath);
		if(ShowSaveFileDlg(hwnd, szFileName, szFileTitle, FULL_BACKUP_FILTER, szBuffer, g_NotePaths.BackupDir, FULL_BACK_EXTENTION)){
			if(!DoFullBackup(szFileName)){
				fMessage = false;
			}
		}
		else
			fMessage = false;
	}
	else{
		ConstructAutomaticBackupName(NULL, szFileName);
		if(!DoFullBackup(szFileName)){
			fMessage = false;
		}
	}
	if(fMessage){
		wchar_t		szMessage[320];
		GetPrivateProfileStringW(L"messages", L"full_backup_success", L"Full backup completed successfully", szMessage, 256, g_NotePaths.CurrLanguagePath);
		MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK);
	}
}

static void ConstructAutomaticBackupName(wchar_t * lpDirectory, wchar_t * lpFileName){
	SYSTEMTIME		st;
	wchar_t			szTime[32], szDate[128];

	GetLocalTime(&st);
	GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, L"yyyy_MM_dd_", szDate, 64);
	GetTimeFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, &st, L"HH_mm_ss.", szTime, 32);
	if(lpDirectory == NULL){
		wcscpy(lpFileName, g_NotePaths.BackupDir);
	}
	else{
		CheckAndCreatePath(lpDirectory);
		wcscpy(lpFileName, lpDirectory);
	}
	wcscat(lpFileName, szDate);
	wcscat(lpFileName, szTime);
	wcscat(lpFileName, FULL_BACK_EXTENTION);
}

static void CALLBACK ProgressTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	if(m_ProgressIconIndex == 5)
		m_ProgressIconIndex = 0;
	SendDlgItemMessageW(hwnd, IDC_ST_PROGRESS, STM_SETICON, (WPARAM)m_ProgressIcons[m_ProgressIconIndex++], 0);
}

static void PrepareScheduleImagelists(HMODULE hIcons){
	HICON				hIcon;

	if(hIcons){
		//prepare image list for sound preview buttons
		hIcon = LoadIconW(hIcons, MAKEINTRESOURCEW(IDR_ICO_LOUDSPEAKER));
		g_hImlPvwNormal = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);
		ImageList_AddIcon(g_hImlPvwNormal, hIcon);;
		hIcon = LoadIconW(hIcons, MAKEINTRESOURCEW(IDR_ICO_LOUDSPEAKER_GR));
		g_hImlPvwGray = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);
		ImageList_AddIcon(g_hImlPvwGray, hIcon);
	}
}

static void SynchronizeNow(HWND hwnd, LPARAM lParam){
	PROGRESS_STRUCT	ps;

	if(IsBitOn(g_NoteSettings.reserved1, SB1_SAVE_BEFORE_SYNC)){
		SaveNotesBeforeSync();
	}
	ps = PrepareSyncStruct(hwnd, lParam);
	int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_PROGRESS), hwnd, Progress_DlgProc, (LPARAM)&ps);
	// int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_PROGRESS), hwnd, Synchronization_DlgProc, lParam);
	if(result == IDOK || result == IDYES){
		wchar_t		szMessage[256];

		GetPrivateProfileStringW(S_MESSAGES, L"sync_success", L"Synchronization completed successfully", szMessage, 256, g_NotePaths.CurrLanguagePath);
		MessageBoxW(hwnd, szMessage, PROG_NAME, MB_OK);
		if(result == IDOK)
			ReloadNotes(false);
	}
}

static void EnableMainMenus(void){
	if(IsTextInClipboard())
		EnableMenuItem(m_hPopUp, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(m_hPopUp, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_GRAYED);

	if(IsPasswordSet())
		EnableMenuItem(m_hPopUp, IDM_LOCK_PROGRAM, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(m_hPopUp, IDM_LOCK_PROGRAM, MF_BYCOMMAND | MF_GRAYED);

	if(g_RTHandles.hbSkin){
		EnableMenuItem(m_hPopUp, IDM_ALL_ROLL_ON, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_hPopUp, IDM_ALL_ROLL_OFF, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		EnableMenuItem(m_hPopUp, IDM_ALL_ROLL_ON, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPopUp, IDM_ALL_ROLL_OFF, MF_BYCOMMAND | MF_ENABLED);
	}
	
	if(!IsFTPEnabled())
		EnableMenuItem(m_hPopUp, IDM_SYNC_NOW, MF_BYCOMMAND | MF_GRAYED);
	else
		EnableMenuItem(m_hPopUp, IDM_SYNC_NOW, MF_BYCOMMAND | MF_ENABLED);

	if(IsAnyNoteVisible())
		EnableMenuItem(m_hPopUp, IDM_RELOAD_ALL, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(m_hPopUp, IDM_RELOAD_ALL, MF_BYCOMMAND | MF_GRAYED);

	if(SEngsCount(g_PExternalls) > 0)
		EnableMenuItem(m_hPopUp, IDM_RUN_PROG, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(m_hPopUp, IDM_RUN_PROG, MF_BYCOMMAND | MF_GRAYED);

	if(IsPasswordSet()){
		EnableMenuItem(m_hPopUp, IDM_CREATE_PASSWORD, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_hPopUp, IDM_CHANGE_PASSWORD, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPopUp, IDM_REMOVE_PASSWORD, MF_BYCOMMAND | MF_ENABLED);
	}
	else{
		EnableMenuItem(m_hPopUp, IDM_CREATE_PASSWORD, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPopUp, IDM_CHANGE_PASSWORD, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_hPopUp, IDM_REMOVE_PASSWORD, MF_BYCOMMAND | MF_GRAYED);
	}
}

static PROGRESS_STRUCT PrepareSyncStruct(HWND hwnd, LPARAM lParam){
	PROGRESS_STRUCT		ps = {0};

	wcscpy(gs_DataDir, g_NotePaths.DataDir);
	wcscpy(gs_DataFilePath, g_NotePaths.DataFile);

	wcscpy(ps.szLangFile, g_NotePaths.CurrLanguagePath);

	if(lParam == 0)
		g_FTPData = m_FTPData;
	else
		g_FTPData = m_TempFTPData;

	ps.hCaller = hwnd;
	wcscpy(ps.szCaptionKey, L"1040");
	wcscpy(ps.szCaptionDef, L"Synchronization");
	wcscpy(ps.szProgressKey, L"synchronizing");
	wcscpy(ps.szProgressDef, L"Synchronization in progress...");
	ps.fShowCancel = TRUE;
	ps.pStopParameter = &gs_StopSync;
	ps.lpStartAddress = FTPSyncFunc;
	return ps;
}

static void CheckAndCreatePath(wchar_t * path){
	wchar_t			szTemp[MAX_PATH];

	wcscpy(szTemp, path);
	PathRemoveFileSpecW(szTemp);
	if(!PathFileExistsW(szTemp)){
		SHCreateDirectoryExW(NULL, szTemp, NULL);
	}
}

static BOOL IsFTPEnabled(void){
	if(wcslen(m_FTPData.server) == 0)
		return false;
	return true;
}

static void StartWSServer(HWND hwnd){
	HANDLE					hThread;
	DWORD					dwID;
	
	hThread = CreateThread(NULL, 0, SocketServerFunc, hwnd, 0, &dwID);
	if(hThread){
		CloseHandle(hThread);
	}
}

static void ReceiveNewNote(void){
	PMEMNOTE				pNote = NULL;

	pNote = AddMemNote();
	LoadNoteProperties(pNote, &g_RTHandles, g_wsID, FALSE);
	if(!IsBitOn(g_NextSettings.flags1, SB3_RECEIVE_NOT_PLAY_SOUND)){
		PlaySound(MAKEINTRESOURCE(IDS_NEW_MESSAGE_SOUND), g_hInstance, SND_RESOURCE | SND_ASYNC);
	}
	if(!IsBitOn(g_NextSettings.flags1, SB3_RECEIVE_HIDE_BALOON)){
		wchar_t					szInfo[256], szDate[128], *pName;
						
		pName = PContactsNameByHost(g_PContacts, pNote->pSRStatus->recFrom);
		wcscpy(szInfo, g_Strings.Received1);
		wcscat(szInfo, L": ");
		wcscat(szInfo, pNote->pData->szName);
		wcscat(szInfo, L"\n");
		wcscat(szInfo, g_Strings.Received2);
		wcscat(szInfo, L": ");
		if(pName)
			wcscat(szInfo, pName);
		else
			wcscat(szInfo, pNote->pSRStatus->recFrom);
		wcscat(szInfo, L"\n");
		wcscat(szInfo, g_Strings.Received3);
		wcscat(szInfo, L"\n");
		ConstructDateTimeString(&pNote->pSRStatus->lastRec, szDate);
		wcscat(szInfo, szDate);
		ShowReceiveNotification(szInfo);
	}
	if(IsBitOn(g_NextSettings.flags1, SB3_SHOW_AFTER_RECEIVING)){
		ShowNote(pNote);
	}
	if(g_hCPDialog){
		SendMessageW(g_hCPDialog, PNM_CTRL_INS, 0, (LPARAM)pNote);
		SendMessageW(g_hCPDialog, PNM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
	}
}

static int CALLBACK ListStringCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	wchar_t				szName1[256], szName2[256];
	LVITEMW				lvi = {0};
	PLSTCOMP			plc = (PLSTCOMP)lParamSort;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 256;
	lvi.pszText = szName1;
	lvi.iSubItem = plc->iSubItem;
	SendMessageW(plc->hList, LVM_GETITEMTEXTW, (WPARAM)lParam1, (LPARAM)&lvi);
	lvi.pszText = szName2;
	SendMessageW(plc->hList, LVM_GETITEMTEXTW, (WPARAM)lParam2, (LPARAM)&lvi);
	if(plc->iSortOrder == LVS_SORTASCENDING)
		return _wcsicmp(szName1, szName2);
	else
		return _wcsicmp(szName2, szName1);
}

BOOL Hotdlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCITEMW		tci = {0};
	TVITEMW		tvi = {0};
	wchar_t 	szBuffer[256], szKey[16];
	RECT		rc;
	HWND		hT1, hT2, hT3, hT4, hToolbar;
	HMODULE		hIcons;

	g_hHotkeys = hwnd;

	//hot keys
	memcpy(m_TempHotkeys, m_Hotkeys, sizeof(HK_TYPE) * NELEMS(m_Hotkeys));
	//copy note hotkeys
	m_TempNoteHotkeys = (P_HK_TYPE)calloc(NoteMenuCount(), sizeof(HK_TYPE));
	memcpy(m_TempNoteHotkeys, g_NoteHotkeys, sizeof(HK_TYPE) * NoteMenuCount());
	//copy edit hotkeys
	m_TempEditHotkeys = (P_HK_TYPE)calloc(COUNT_EDIT_HOTKEYS, sizeof(HK_TYPE));
	memcpy(m_TempEditHotkeys, g_EditHotkeys, sizeof(HK_TYPE) * COUNT_EDIT_HOTKEYS);
	//copy groups
	m_TempGroups = PNGroupsCopy(&m_TempGroups, g_PGroups);

	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1047", L"Hot keys", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	//tabs
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = szBuffer;
	tci.cchTextMax = 256;
	_itow(IDC_ST_HK_MAIN_MENU, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Main menu", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_INSERTITEMW, 0, (LPARAM)&tci);
	_itow(IDC_ST_HK_SINGLE_NOTE, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Note", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_INSERTITEMW, 1, (LPARAM)&tci);
	_itow(IDC_ST_HK_EDIT, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Edit area", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_INSERTITEMW, 2, (LPARAM)&tci);
	_itow(IDC_ST_HK_GROUPS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Groups", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_INSERTITEMW, 3, (LPARAM)&tci);

	GetWindowRect(GetDlgItem(hwnd, IDC_TAB_HOT), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_ADJUSTRECT, false, (LPARAM)&rc);

	hT1 = GetDlgItem(hwnd, IDC_TVW_H_MAIN);
	hT2 = GetDlgItem(hwnd, IDC_TVW_H_NOTE);
	hT3 = GetDlgItem(hwnd, IDC_TVW_H_EDIT);
	hT4 = GetDlgItem(hwnd, IDC_TVW_H_GROUPS);

	MoveWindow(hT1, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
	MoveWindow(hT2, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, false);
	MoveWindow(hT3, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, false);
	MoveWindow(hT4, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, false);

	TreeView_SetImageList(hT1, m_hImlHotKeys, TVSIL_NORMAL);
	TreeView_SetImageList(hT2, m_hImlHotKeys, TVSIL_NORMAL);
	TreeView_SetImageList(hT3, m_hImlHotKeys, TVSIL_NORMAL);
	TreeView_SetImageList(hT4, m_hImlHotKeys, TVSIL_NORMAL);

	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		//prepare image list for small bars
		if(m_hImlSmallBars == NULL && m_hImlSmallBarsGray == NULL){
			CreateImageListsW(&m_hImlSmallBars, &m_hImlSmallBarsGray, hIcons, IDB_SMALL_BARS, IDB_SMALL_BARS_GRAY, CLR_MASK, 16, 16, 4);
		}
		FreeLibrary(hIcons);
	}

	hToolbar = GetDlgItem(hwnd, IDC_TBR_HK_MAIN);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(hwnd, IDC_TBR_HK_NOTE);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(hwnd, IDC_TBR_HK_EDIT);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	hToolbar = GetDlgItem(hwnd, IDC_TBR_HK_GROUPS);
	DoToolbarW(hToolbar, m_hImlSmallBars, m_hImlSmallBarsGray, NELEMS(m_HotKeyButtons), m_HotKeyButtons);
	
	//get all tooltips
	PTBRTOOLTIP		ptt = m_Tooltips;
	for(int i = 0; i < NELEMS(m_Tooltips); i++){
		_itow(ptt->id, szKey, 10);
		GetPrivateProfileStringW(S_OPTIONS, szKey, ptt->szTip, ptt->szTip, 128, g_NotePaths.CurrLanguagePath);
		ptt++;
	}

	InsertCommands(hT1, m_hPopUp, NULL, 0);
	InsertCommands(hT2, g_hNotePopUp, NULL, 1);
	InsertCommands(hT3, g_hEditPopUp, NULL, 2);
	InsertHotGroups(hT4, NULL, -1);

	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = TreeView_GetSelection(hT4);
	SendMessageW(hT4, TVM_GETITEMW, 0, (LPARAM)&tvi);
	DisplaySelectedHotkey(tvi, hwnd, 3);
	tvi.hItem = TreeView_GetSelection(hT3);
	SendMessageW(hT3, TVM_GETITEMW, 0, (LPARAM)&tvi);
	DisplaySelectedHotkey(tvi, hwnd, 2);
	tvi.hItem = TreeView_GetSelection(hT2);
	SendMessageW(hT2, TVM_GETITEMW, 0, (LPARAM)&tvi);
	DisplaySelectedHotkey(tvi, hwnd, 1);
	tvi.hItem = TreeView_GetSelection(hT1);
	SendMessageW(hT1, TVM_GETITEMW, 0, (LPARAM)&tvi);
	DisplaySelectedHotkey(tvi, hwnd, 0);

	SetFocus(hT1);
	return false;
}

void Hotdlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	TVITEMW			tvi = {0};
	int				tab;
	HWND			hTree = NULL;
	P_HK_TYPE		pHK = NULL;

	switch(id){
		case IDCANCEL:
			g_hHotkeys = NULL;
			CleanHotdlgOnClose();
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:
			ApplyHotKeys();
			g_hHotkeys = NULL;
			CleanHotdlgOnClose();
			EndDialog(hwnd, IDOK);
			break;
		case IDC_CMD_ADD_SOUND:
		case IDC_CMD_MODIFY:{
			tab = SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_GETCURSEL, 0, 0);
			tvi.mask = TVIF_HANDLE | TVIF_PARAM;

			switch(tab){
				case 0:
					hTree = GetDlgItem(hwnd, IDC_TVW_H_MAIN);
					break;
				case 1:
					hTree = GetDlgItem(hwnd, IDC_TVW_H_NOTE);
					break;
				case 2:
					hTree = GetDlgItem(hwnd, IDC_TVW_H_EDIT);
					break;
				case 3:
					hTree = GetDlgItem(hwnd, IDC_TVW_H_GROUPS);
					break;
			}
			tvi.hItem = TreeView_GetSelection(hTree);
			SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
			switch(tab){
				case 0:
					pHK = GetHotKeyByID(tvi.lParam, m_TempHotkeys, NELEMS(m_TempHotkeys));
					break;
				case 1:
					pHK = GetHotKeyByID(tvi.lParam, m_TempNoteHotkeys, NoteMenuCount());
					break;
				case 2:
					pHK = GetHotKeyByID(tvi.lParam, m_TempEditHotkeys, COUNT_EDIT_HOTKEYS);
					break;
				case 3:{
					TVITEMW		tvip = {0};
					tvip.mask = TVIF_PARAM | TVIF_HANDLE;
					tvip.hItem = TreeView_GetParent(hTree, tvi.hItem);
					SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvip);
					LPPNGROUP	lpg = (LPPNGROUP)tvip.lParam;
					if(tvi.lParam == HKG_SHOW)
						pHK = &lpg->hotKeyShow;
					else
						pHK = &lpg->hotKeyHide;
					break;
				}
			}
			memcpy(&m_HKCurrent, pHK, sizeof(m_HKCurrent));
			if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOT_KEY), hwnd, Hot_DlgProc, tab) == IDOK){
				if(pHK->fsModifiers != m_HKCurrent.fsModifiers || pHK->vk != m_HKCurrent.vk){
					memcpy(pHK, &m_HKCurrent, sizeof(m_HKCurrent));
					switch(tab){
						case 0:
							SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_MAIN, pHK->szKey);
							break;
						case 1:
							SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_NOTE, pHK->szKey);
							break;
						case 2:
							SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_EDIT, pHK->szKey);
							break;
						case 3:
							SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_GROUPS, pHK->szKey);
							break;
					}		
				}
			}
			switch(tab){
				case 0:
					EnableSmallToolbar(hwnd, IDC_TBR_HK_MAIN, (wcslen(pHK->szKey) > 0));
					break;
				case 1:
					EnableSmallToolbar(hwnd, IDC_TBR_HK_NOTE, (wcslen(pHK->szKey) > 0));
					break;
				case 2:
					EnableSmallToolbar(hwnd, IDC_TBR_HK_EDIT, (wcslen(pHK->szKey) > 0));
					break;
				case 3:
					EnableSmallToolbar(hwnd, IDC_TBR_HK_GROUPS, (wcslen(pHK->szKey) > 0));
					break;
			}
			ZeroMemory(&m_HKCurrent, sizeof(m_HKCurrent));
			break;
		}
		case IDC_CMD_DEL_SOUND:{
			tab = SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_GETCURSEL, 0, 0);
			tvi.mask = TVIF_HANDLE | TVIF_PARAM;

			if(MessageBoxW(hwnd, g_Strings.HK3, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
				switch(tab){
					case 0:
						hTree = GetDlgItem(hwnd, IDC_TVW_H_MAIN);
						break;
					case 1:
						hTree = GetDlgItem(hwnd, IDC_TVW_H_NOTE);
						break;
					case 2:
						hTree = GetDlgItem(hwnd, IDC_TVW_H_EDIT);
						break;
					case 3:
						hTree = GetDlgItem(hwnd, IDC_TVW_H_GROUPS);
						break;
				}
				tvi.hItem = TreeView_GetSelection(hTree);
				SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
				switch(tab){
					case 0:
						pHK = GetHotKeyByID(tvi.lParam, m_TempHotkeys, NELEMS(m_TempHotkeys));
						break;
					case 1:
						pHK = GetHotKeyByID(tvi.lParam, m_TempNoteHotkeys, NoteMenuCount());
						break;
					case 2:
						pHK = GetHotKeyByID(tvi.lParam, m_TempEditHotkeys, COUNT_EDIT_HOTKEYS);
						break;
					case 3:{
						TVITEMW		tvip = {0};
						tvip.mask = TVIF_PARAM | TVIF_HANDLE;
						tvip.hItem = TreeView_GetParent(hTree, tvi.hItem);
						SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvip);
						LPPNGROUP	lpg = (LPPNGROUP)tvip.lParam;
						if(tvi.lParam == HKG_SHOW)
							pHK = &lpg->hotKeyShow;
						else
							pHK = &lpg->hotKeyHide;
						break;
					}
				}
				pHK->fsModifiers = 0;
				pHK->identifier = 0;
				pHK->szKey[0] = '\0';
				pHK->vk = 0;
				switch(tab){
					case 0:
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_MAIN, pHK->szKey);
						EnableSmallToolbar(hwnd, IDC_TBR_HK_MAIN, false);
						break;
					case 1:
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_NOTE, pHK->szKey);
						EnableSmallToolbar(hwnd, IDC_TBR_HK_NOTE, false);
						break;
					case 2:
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_EDIT, pHK->szKey);
						EnableSmallToolbar(hwnd, IDC_TBR_HK_EDIT, false);
						break;
					case 3:
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY_GROUPS, pHK->szKey);
						EnableSmallToolbar(hwnd, IDC_TBR_HK_GROUPS, false);
						break;
				}
			}
			break;
		}
	}
}

BOOL CALLBACK Hotdlg_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	LPNMHDR			pnmh;

	switch(msg){
		// HANDLE_MSG (hwnd, WM_CLOSE, Hotdlg_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Hotdlg_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Hotdlg_OnInitDialog);
		case WM_NOTIFY:
			pnmh = (LPNMHDR)lParam;
			if(pnmh->code == TCN_SELCHANGE){
				int		len = 0;
				switch(SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_GETCURSEL, 0, 0)){
					case 0:
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_MAIN), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_MAIN), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_MAIN), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_GROUPS), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_GROUPS), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_GROUPS), SW_HIDE);
						len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_ST_HOTKEY_MAIN));
						if(IsSuitableHKSelected(hwnd, IDC_TVW_H_MAIN))
							EnableSmallToolbar(hwnd, IDC_TBR_HK_MAIN, len > 0);
						else
							EnableSmallToolbarFull(hwnd, IDC_TBR_HK_MAIN, false, false, false);
						break;
					case 1:
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_NOTE), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_NOTE), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_NOTE), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_GROUPS), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_GROUPS), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_GROUPS), SW_HIDE);
						len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_ST_HOTKEY_NOTE));
						if(IsSuitableHKSelected(hwnd, IDC_TVW_H_NOTE))
							EnableSmallToolbar(hwnd, IDC_TBR_HK_NOTE, len > 0);
						else
							EnableSmallToolbarFull(hwnd, IDC_TBR_HK_NOTE, false, false, false);
						break;
					case 2:
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_EDIT), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_EDIT), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_EDIT), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_GROUPS), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_GROUPS), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_GROUPS), SW_HIDE);
						len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_ST_HOTKEY_EDIT));
						if(IsSuitableHKSelected(hwnd, IDC_TVW_H_EDIT))
							EnableSmallToolbar(hwnd, IDC_TBR_HK_EDIT, len > 0);
						else
							EnableSmallToolbarFull(hwnd, IDC_TBR_HK_EDIT, false, false, false);
						break;
					case 3:
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_EDIT), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_NOTE), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_MAIN), SW_HIDE);
						ShowWindow(GetDlgItem(hwnd, IDC_TVW_H_GROUPS), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_ST_HOTKEY_GROUPS), SW_SHOW);
						ShowWindow(GetDlgItem(hwnd, IDC_TBR_HK_GROUPS), SW_SHOW);
						len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_ST_HOTKEY_GROUPS));
						if(IsSuitableHKSelected(hwnd, IDC_TVW_H_GROUPS))
							EnableSmallToolbar(hwnd, IDC_TBR_HK_GROUPS, len > 0);
						else
							EnableSmallToolbarFull(hwnd, IDC_TBR_HK_GROUPS, false, false, false);
						break;
				}
			}
			else if(pnmh->code == TVN_SELCHANGEDW){
				LPNMTREEVIEWW		lpnm = (LPNMTREEVIEWW)lParam;
				DisplaySelectedHotkey(lpnm->itemNew, hwnd, SendDlgItemMessageW(hwnd, IDC_TAB_HOT, TCM_GETCURSEL, 0, 0));
			}
			else if(pnmh->code == TTN_NEEDTEXTW){
				//show toolbar button tooltip
				TOOLTIPTEXTW 	* ttp = (TOOLTIPTEXTW *)lParam;
				GetTTText(pnmh->idFrom, ttp->szText);
				return true;
			}
			return FALSE;
		default:
			return FALSE;
	}
}

static void ApplyHotKeys(void){
	BOOL			fHotkeysChanged = false;

	//main hot keys
	if(CheckHotKeysChanges(m_Hotkeys, m_TempHotkeys, NELEMS(m_Hotkeys), true)){
		SaveHotKeys(S_HOTKEYS, m_Hotkeys, NELEMS(m_Hotkeys));
		fHotkeysChanged = true;
	}
	//note hotkeys
	if(CheckHotKeysChanges(g_NoteHotkeys, m_TempNoteHotkeys, NoteMenuCount(), false)){
		SaveHotKeys(S_NOTE_HOTKEYS, g_NoteHotkeys, NoteMenuCount());
		fHotkeysChanged = true;
	}
	//edit hotkeys
	if(CheckHotKeysChanges(g_EditHotkeys, m_TempEditHotkeys, COUNT_EDIT_HOTKEYS, false)){
		SaveHotKeys(S_EDIT_HOTKEYS, g_EditHotkeys, COUNT_EDIT_HOTKEYS);
		fHotkeysChanged = true;
	}
	//groups hotkeys
	if(CheckGroupsHotkeysChanges(g_PGroups, m_TempGroups)){
		fHotkeysChanged = true;
	}
	if(fHotkeysChanged)
		CreatePopUp();
}

static BOOL IsSuitableHKSelected(HWND hwnd, int tvwId){
	TVITEMW				ltvi = {0};

	ltvi.mask = TVIF_HANDLE | TVIF_IMAGE;
	ltvi.hItem = TreeView_GetSelection(GetDlgItem(hwnd, tvwId));
	SendDlgItemMessageW(hwnd, tvwId, TVM_GETITEMW, 0, (LPARAM)&ltvi);
	return ltvi.iImage;
}

static void DisplaySelectedHotkey(TVITEMW tvi, HWND hwnd, int tab){
	HWND				hEdit = NULL;
	P_HK_TYPE			phk = NULL;
	TVITEMW				ltvi = {0}, tvp = {0};
	HTREEITEM			hParent;

	tvp.mask = TVIF_HANDLE | TVIF_PARAM;
	ltvi.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM;
	ltvi.hItem = tvi.hItem;
	
	switch(tab){
		case 0:
			hEdit = GetDlgItem(hwnd, IDC_ST_HOTKEY_MAIN);
			SendDlgItemMessageW(hwnd, IDC_TVW_H_MAIN, TVM_GETITEMW, 0, (LPARAM)&ltvi);
			if(ltvi.iImage == 0){
				EnableSmallToolbarFull(hwnd, IDC_TBR_HK_MAIN, false, false, false);
			}
			else{
				phk = GetHotKeyByID(tvi.lParam, m_TempHotkeys, NELEMS(m_TempHotkeys));
				if(phk)
					EnableSmallToolbar(hwnd, IDC_TBR_HK_MAIN, (wcslen(phk->szKey) > 0));
				else
					EnableSmallToolbar(hwnd, IDC_TBR_HK_MAIN, false);
			}
			break;
		case 1:
			hEdit = GetDlgItem(hwnd, IDC_ST_HOTKEY_NOTE);
			SendDlgItemMessageW(hwnd, IDC_TVW_H_NOTE, TVM_GETITEMW, 0, (LPARAM)&ltvi);
			if(ltvi.iImage == 0){
				EnableSmallToolbarFull(hwnd, IDC_TBR_HK_NOTE, false, false, false);
			}
			else{
				phk = GetHotKeyByID(tvi.lParam, m_TempNoteHotkeys, NoteMenuCount());
				if(phk)
					EnableSmallToolbar(hwnd, IDC_TBR_HK_NOTE, (wcslen(phk->szKey) > 0));
				else
					EnableSmallToolbar(hwnd, IDC_TBR_HK_NOTE, false);
			}
			break;
		case 2:
			hEdit = GetDlgItem(hwnd, IDC_ST_HOTKEY_EDIT);
			SendDlgItemMessageW(hwnd, IDC_TVW_H_EDIT, TVM_GETITEMW, 0, (LPARAM)&ltvi);
			if(ltvi.iImage == 0){
				EnableSmallToolbarFull(hwnd, IDC_TBR_HK_EDIT, false, false, false);
			}
			else{
				phk = GetHotKeyByID(tvi.lParam, m_TempEditHotkeys, COUNT_EDIT_HOTKEYS);
				if(phk)
					EnableSmallToolbar(hwnd, IDC_TBR_HK_EDIT, (wcslen(phk->szKey) > 0));
				else
					EnableSmallToolbar(hwnd, IDC_TBR_HK_EDIT, false);
			}
			break;
		case 3:
			hEdit = GetDlgItem(hwnd, IDC_ST_HOTKEY_GROUPS);
			SendDlgItemMessageW(hwnd, IDC_TVW_H_GROUPS, TVM_GETITEMW, 0, (LPARAM)&ltvi);
			if(ltvi.iImage == 0){
				EnableSmallToolbarFull(hwnd, IDC_TBR_HK_GROUPS, false, false, false);
			}
			else{
				hParent = TreeView_GetParent(GetDlgItem(hwnd, IDC_TVW_H_GROUPS), ltvi.hItem);
				tvp.hItem = hParent;
				SendDlgItemMessageW(hwnd, IDC_TVW_H_GROUPS, TVM_GETITEMW, 0, (LPARAM)&tvp);
				LPPNGROUP	ppg = (LPPNGROUP)tvp.lParam;
				if(ltvi.lParam == HKG_SHOW)
					phk = &ppg->hotKeyShow;
				else
					phk = &ppg->hotKeyHide;
				if(phk)
					EnableSmallToolbar(hwnd, IDC_TBR_HK_GROUPS, (wcslen(phk->szKey) > 0));
				else
					EnableSmallToolbar(hwnd, IDC_TBR_HK_GROUPS, false);
			}
			break;
	}

	SetWindowTextW(hEdit, NULL);
	if(phk && (phk->fsModifiers || phk->vk)){
		SetWindowTextW(hEdit, phk->szKey);
	}
}

static void InsertHotGroups(HWND hTree, HTREEITEM htiParent, int idParent){
	LPPNGROUP			pTemp = m_TempGroups;
	HTREEITEM			hInserted;
	TVINSERTSTRUCTW		tvs = {0};
	wchar_t				szShow[256], szHide[256];

	GetPrivateProfileStringW(S_CAPTIONS, L"show_group", L"Show group", szShow, 256, g_NotePaths.CurrLanguagePath);
	GetPrivateProfileStringW(S_CAPTIONS, L"hide_group", L"Hide group", szHide, 256, g_NotePaths.CurrLanguagePath);

	tvs.hInsertAfter = TVI_LAST;
	tvs.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvs.itemex.iImage = 1;
	tvs.itemex.iSelectedImage = 1;

	while(pTemp){
		if(pTemp->parent == idParent){
			hInserted = InsertSingleHotGroup(hTree, htiParent, pTemp);
			tvs.hParent = hInserted;
			tvs.itemex.pszText = szShow;
			tvs.itemex.lParam = HKG_SHOW;
			SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
			tvs.itemex.pszText = szHide;
			tvs.itemex.lParam = HKG_HIDE;
			SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
			InsertHotGroups(hTree, hInserted, pTemp->id);
			TreeView_Expand(hTree, hInserted, TVE_EXPAND);
		}
		pTemp = pTemp->next;
	}
	TreeView_SelectItem(hTree, TreeView_GetRoot(hTree));
	TreeView_EnsureVisible(hTree, TreeView_GetRoot(hTree));
}

static HTREEITEM InsertSingleHotGroup(HWND hTree, HTREEITEM hItem, LPPNGROUP pTemp){
	TVINSERTSTRUCTW		tvs = {0};
	HTREEITEM			hti;
	wchar_t				szBuffer[128];

	tvs.hParent = hItem;
	tvs.hInsertAfter = TVI_LAST; //TVI_SORT;
	tvs.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvs.itemex.iImage = 0;
	tvs.itemex.iSelectedImage = 0;
	tvs.itemex.lParam = (LONG_PTR)pTemp;

	wcscpy(szBuffer, pTemp->szName);
	tvs.itemex.pszText = szBuffer;
	hti = (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	return hti;
}

static void InsertCommands(HWND hTree, HMENU hMenu, HTREEITEM htiParent, int tab){
	TVINSERTSTRUCTW		tvs = {0};
	HTREEITEM			hti;
	TVITEMW				tvi = {0};
	MENUITEMINFOW		mi = {0};
	PMITEM 				pmi;
	int					count;
	wchar_t				szBuffer[128], *ptr;

	tvs.hParent = htiParent;
	tvs.hInsertAfter = TVI_LAST;
	tvs.itemex.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	count = GetMenuItemCount(hMenu);

	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
	for(int i = 0; i < count; i++){
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if(mi.wID > 0){
			if(htiParent){
				tvi.mask = TVIF_PARAM | TVIF_HANDLE;
				tvi.hItem = htiParent;
				SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
				switch(tab){
					case 0:
						if(tvi.lParam == IDM_DIARY && mi.wID != IDM_TODAY_DIARY)
							continue;
						else if(tvi.lParam == IDM_SHOW_GROUPS && mi.wID != IDM_SHOW_ALL && mi.wID != IDM_SHOW_INCOMING)
							continue;
						else if(tvi.lParam == IDM_HIDE_GROUPS && mi.wID != IDM_HIDE_ALL && mi.wID != IDM_HIDE_INCOMING)
							continue;
						else if(tvi.lParam == IDM_SHOW_BY_TAG || tvi.lParam == IDM_HIDE_BY_TAG || tvi.lParam == IDM_LANGUAGES)
							continue;
						break;
					case 1:
						if(tvi.lParam == IDM_SEND_INTRANET && mi.wID != IDM_ADD_CONTACT && mi.wID != IDM_SEND_TO_SEVERAL && mi.wID != IDM_ADD_CONT_GROUP && mi.wID != IDM_SELECT_CONT_GROUP)
							continue;
						else if(tvi.lParam == IDM_LINKS && mi.wID != IDM_MANAGE_LINKS)
							continue;
						else if(tvi.lParam == IDM_NOTE_GROUPS)
							continue;
						break;
					case 2:
						if(mi.wID != IDM_INSERT_PICTURE && mi.wID != IDM_INSERT_SMILIE && mi.wID != IDM_INSERT_DATETIME && mi.wID != IDM_SPELL_AUTO && mi.wID != IDM_SPELL_NOW && mi.wID != IDM_SPELL_DOWNLOAD)
							continue;
						break;
				}
			}
			else{
				switch(tab){
					case 0:
						break;
					case 1:
						if(mi.wID == IDM_SAVE || mi.wID == IDM_PRINT_NOTE)
							continue;
						break;
					case 2:
						if(mi.wID != IDM_COPY_AS_TEXT && mi.wID != IDM_PASTE_AS_TEXT && mi.wID != IDM_INSERT && mi.wID != IDM_SPELL_CHECK)
							continue;
						break;
				}
			}
			if(mi.dwItemData){
				pmi = (PMITEM)mi.dwItemData;
				if(mi.hSubMenu){
					tvs.itemex.iImage = 0;
					tvs.itemex.iSelectedImage = 0;
				}
				else{
					tvs.itemex.iImage = 1;
					tvs.itemex.iSelectedImage = 1;
				}
				wcscpy(szBuffer, pmi->szText);
				wcstok(szBuffer, L"\t", &ptr);
				tvs.itemex.pszText = szBuffer;
				tvs.itemex.lParam = mi.wID;
				hti = (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
				if(mi.hSubMenu)
					InsertCommands(hTree, mi.hSubMenu, hti, tab);
				TreeView_Expand(hTree, hti, TVE_EXPAND);
			}
		}
	}
	TreeView_SelectItem(hTree, TreeView_GetRoot(hTree));
	TreeView_EnsureVisible(hTree, TreeView_GetRoot(hTree));
}

static void CleanHotdlgOnClose(void){
	if(m_TempNoteHotkeys){
		free(m_TempNoteHotkeys);
		m_TempNoteHotkeys = NULL;
	}
	if(m_TempEditHotkeys){
		free(m_TempEditHotkeys);
		m_TempEditHotkeys = NULL;
	}
	if(m_TempGroups){
		PNGroupsFree(&m_TempGroups);
	}
}


static void GetGroupChecks(void){
	int 		i = 0;
	LPPNGROUP	pg = g_PGroups;

	m_GChecks = calloc(PNGroupsCount(g_PGroups) + 1, sizeof(GRCHECKED));
	while(pg){
		m_GChecks[i++].id = pg->id;
		pg = pg->next;
	}
	m_GChecks[i].id = GROUP_DIARY;
}

static void FreeGroupChecks(void){
	free(m_GChecks);
	m_GChecks = NULL;
}

static void GetAllGroups(void){

	wchar_t				*pw, *pGroups;
	PNGROUP				pg;
	int					result = 0;
	int					ub = 1024;

	if(g_PGroups == NULL){
		pGroups = calloc(ub, sizeof(wchar_t));
		if(pGroups){
			result = GetPrivateProfileStringW(S_GROUPS, NULL, NULL, pGroups, ub, g_NotePaths.INIFile);
			while(result == ub - 2){
				ub *= 2;
				free(pGroups);
				pGroups = calloc(ub, sizeof(wchar_t));
				if(pGroups){
					result = GetPrivateProfileStringW(S_GROUPS, NULL, NULL, pGroups, ub, g_NotePaths.INIFile);
				}
			}
			pw = pGroups;
			while(*pw){
				ZeroMemory(&pg.hotKeyShow, sizeof(HK_TYPE));
				ZeroMemory(&pg.hotKeyHide, sizeof(HK_TYPE));
				*pg.szLock = '\0';
				result = GetPrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg), g_NotePaths.INIFile);
				if(!result){
					result = GetPrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg) - sizeof(COLORREF) * 3 - sizeof(BOOL) * 4 - sizeof(wchar_t) * 64, g_NotePaths.INIFile);
				}
				if(!result){
					result = GetPrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg) - sizeof(COLORREF) * 3 - sizeof(BOOL) * 4 - sizeof(wchar_t) * 64 - sizeof(wchar_t) * 256, g_NotePaths.INIFile);
				}
				if(!result){
					result = GetPrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg) - sizeof(COLORREF) * 3 - sizeof(BOOL) * 4 - sizeof(wchar_t) * 64 - sizeof(HK_TYPE) * 2 - sizeof(wchar_t) * 256, g_NotePaths.INIFile);
				}
				if(result){
					if(_wtoi(pw) != pg.id){
						pg.id = _wtoi(pw);
						WritePrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg), g_NotePaths.INIFile);
					}
					if(pg.crWindow == 0)
						pg.crWindow = g_Appearance.crWindow;
					if(pg.crCaption == 0)
						pg.crCaption = g_Appearance.crCaption;
					if(pg.crFont == 0)
						pg.crFont = g_Appearance.crFont;
					if(pg.szSkin[0] == '\0')
						wcscpy(pg.szSkin, g_Appearance.szSkin);

					g_PGroups = PNGroupsAdd(&g_PGroups, &pg);
					if(pg.hotKeyShow.fsModifiers != 0 || pg.hotKeyShow.vk != 0)
						RegisterHotKey(g_hMain, pg.hotKeyShow.id, pg.hotKeyShow.fsModifiers, pg.hotKeyShow.vk);
					if(pg.hotKeyHide.fsModifiers != 0 || pg.hotKeyHide.vk != 0)
						RegisterHotKey(g_hMain, pg.hotKeyHide.id, pg.hotKeyHide.fsModifiers, pg.hotKeyHide.vk);
				}
				pw += wcslen(pw) + 1;
			}
			free(pGroups);
		}
	}
}

BOOL CALLBACK ContGroups_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, ContGroups_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, ContGroups_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, ContGroups_OnClose);
		default: return FALSE;
	}
}

void ContGroups_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			wchar_t			szBuffer[256];
			LPPCONTGROUP	pConstants = NULL;
			int				mode = (int)GetPropW(hwnd, CONTACTS_GROUP_PROP);

			pConstants = (mode == 0 ? g_TempContGroups : (mode == 2 ? g_PContGroups : NULL));
			GetDlgItemTextW(hwnd, IDC_EDT_CONTACT_NAME, szBuffer, 255);
			if((mode == 0 || mode == 2) && PContGroupsContains(pConstants, szBuffer)){
				GetPrivateProfileStringW(S_MESSAGES, L"contacts_group_exists", L"Contacts group with the same name already exists", szBuffer, 255, g_NotePaths.CurrLanguagePath);
				MessageBoxW(hwnd, szBuffer, PROG_NAME, MB_OK | MB_ICONERROR);
				break;
			}
			wcscpy(g_ContGroup.name, szBuffer);
			
			RemovePropW(hwnd, CONTACTS_GROUP_PROP);
			EndDialog(hwnd, IDOK);
			break;
		}
		case IDC_EDT_CONTACT_NAME:
			if(codeNotify == EN_CHANGE){
				ContGroups_EnableOK(hwnd);
			}
			break;
	}
}

void ContGroups_OnClose(HWND hwnd)
{
	RemovePropW(hwnd, CONTACTS_GROUP_PROP);
	EndDialog(hwnd, 0);
}

BOOL ContGroups_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t		szBuffer[256];

	SetPropW(hwnd, CONTACTS_GROUP_PROP, (HANDLE)lParam);
	if(lParam == 0 || lParam == 2){
		GetPrivateProfileStringW(S_CAPTIONS, L"new_contacts_group", L"New group of contacts", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	}
	else if(lParam == 1){
		GetPrivateProfileStringW(S_CAPTIONS, L"edit_contacts_group", L"Edit group of contacts", szBuffer, 255, g_NotePaths.CurrLanguagePath);
		SetDlgItemTextW(hwnd, IDC_EDT_CONTACT_NAME, g_ContGroup.name);
	}
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_CONTACT_NAME, g_NotePaths.CurrLanguagePath, L"Name");
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, L"OK");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");

	return TRUE;
}

static void ContGroups_EnableOK(HWND hwnd){
	if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_CONTACT_NAME)) == 0){
		EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
		return;
	}
	EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
}

static HTREEITEM FindTVGroup(HWND hTree, HTREEITEM hStart, int id){
	HTREEITEM		hItem = NULL;
	TVITEMW			tvi = {0};

	if(hStart == NULL)
		hItem = TreeView_GetRoot(hTree);
	else
		hItem = TreeView_GetNextSibling(hTree, hStart);
	
	if(hItem){
		tvi.mask = TVIF_PARAM | TVIF_HANDLE;
		tvi.hItem = hItem;
		TreeView_GetItem(hTree, &tvi);
		if(tvi.lParam == id)
			return hItem;
		else
			hItem = FindTVGroup(hTree, hItem, id);
	}
	return hItem;
}

static HTREEITEM FindTVContact(HWND hTree, HTREEITEM hGStart, HTREEITEM hCStart, wchar_t * lpName){
	HTREEITEM		hContact = NULL, hGroup = NULL;
	TVITEMW			tvi = {0};
	wchar_t			szBuffer[128];

	if(hGStart == NULL)
		hGroup = TreeView_GetRoot(hTree);
	else if(hCStart == NULL)
		hGroup = TreeView_GetNextSibling(hTree, hGStart);
	else
		hGroup = hGStart;

	if(hCStart == NULL)
		hContact = TreeView_GetChild(hTree, hGroup);
	else
		hContact = TreeView_GetNextSibling(hTree, hCStart);
	
	if(hGroup){
		if(hContact){
			tvi.mask = TVIF_TEXT | TVIF_HANDLE;
			tvi.hItem = hContact;
			tvi.pszText = szBuffer;
			tvi.cchTextMax = 128;
			SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
			if(_wcsicmp(szBuffer, lpName) == 0)
				return hContact;
			else
				hContact = FindTVContact(hTree, hGroup, hContact, lpName);
		}
		else{
			hContact = FindTVContact(hTree, hGroup, hContact, lpName);
		}
	}
	return hContact;
}

static void ReplaceContactInFroups(HWND hTree, LPPCONTACT lpc){
	RemoveContactFromGroup(hTree, lpc->prop.name);
	AddContactToGroups(hTree, lpc);
}

static void RemoveContactFromGroup(HWND hTree, wchar_t * lpName){
	HTREEITEM			hItem;

	hItem = FindTVContact(hTree, NULL, NULL, lpName);
	if(hItem){
		TreeView_DeleteItem(hTree, hItem);
		RedrawWindow(hTree, NULL, NULL, RDW_INVALIDATE);
	}
}

static void AddContactToGroups(HWND hTree, LPPCONTACT lpc){
	TVINSERTSTRUCTW		tvc = {0};
	HTREEITEM			hItem;

	tvc.hInsertAfter = TVI_SORT;
	tvc.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvc.item.iImage = 1;
	tvc.item.iSelectedImage = 1;
	tvc.item.pszText = lpc->prop.name;
	hItem = FindTVGroup(hTree, NULL, lpc->prop.group);
	if(hItem){
		tvc.hParent = hItem;
		SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvc);
		RedrawWindow(hTree, NULL, NULL, RDW_INVALIDATE);
	}
}

static void TransferContacts(HWND hTree, int groupOld, int groupNew){
	for(LPPCONTACT lpc = g_TempContacts; lpc; lpc = lpc->next){
		if(lpc->prop.group == groupOld){
			RemoveContactFromGroup(hTree, lpc->prop.name);
			lpc->prop.group = groupNew;
			AddContactToGroups(hTree, lpc);
		}
	}
}

static void LoadLanguageNames(void){
	HRSRC				hRes;
	HANDLE				hFile;
	HGLOBAL				hResLoad;
	LPBYTE				lpRes;
	DWORD				size, dwRead;
	wchar_t				*pBuffer, *w1, *w2, *p1, *p2, path[MAX_PATH];
	BOOL				fFound = false;

	PathCombineW(path, g_NotePaths.DictDir, DICT_FILE);
	hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		size = GetFileSize(hFile, NULL);
		pBuffer = calloc(size + 2, sizeof(BYTE));
		if(pBuffer){
			ReadFile(hFile, pBuffer, size, &dwRead, NULL);
			if(dwRead == size){
				w1 = wcstok(pBuffer, L"\n\r", &p1);;
				while(w1){
					w2 = wcstok(w1, L"=", &p2);
					addEntry(g_LangNames, w2, p2);
					w1 = wcstok(NULL, L"\n\r", &p1);
				}
				fFound = true;
			}
			free(pBuffer);
		}
		CloseHandle(hFile);
	}
	if(fFound){
		return;
	}
	hRes = FindResource(NULL, "#8010", RT_RCDATA);
	if(hRes){
		hResLoad = LoadResource(NULL, hRes);
		if(hResLoad){
			lpRes = LockResource(hResLoad);
			size = SizeofResource(NULL, hRes);
			pBuffer = calloc(size + 2, sizeof(BYTE));
			memcpy(pBuffer, lpRes, size);
			w1 = wcstok(pBuffer, L"\n\r", &p1);;
			while(w1){
				w2 = wcstok(w1, L"=", &p2);
				addEntry(g_LangNames, w2, p2);
				w1 = wcstok(NULL, L"\n\r", &p1);
			}
			free(pBuffer);
		}
	}
}

static void LoadDictionaries(void){
	WIN32_FIND_DATAW	fd = {0};
	HANDLE				hFile;
	wchar_t				szFile[MAX_PATH], szSpellDir[MAX_PATH];
	BOOL				fResult = TRUE;

	wcscpy(szFile, g_NotePaths.DictDir);
	wcscpy(szSpellDir, g_NotePaths.DictDir);
	wcscat(szFile, L"*.aff");

	hFile = FindFirstFileW(szFile, &fd);
	if(hFile != INVALID_HANDLE_VALUE){
		AddDictFiles(szSpellDir, fd.cFileName);
		while(fResult){
			fResult = FindNextFileW(hFile, &fd);
			if(fResult){
				AddDictFiles(szSpellDir, fd.cFileName);
			}
		}
		FindClose(hFile);
	}
	//load language names
	LoadLanguageNames();
}

static int GetCodePage(char * key){
	PHENTRY				pe;
	wchar_t				wkey[128];

	MultiByteToWideChar(CP_ACP, 0, key, -1, wkey, 128);
	pe = getEntry(g_CodePages, wkey);
	if(pe)
		return _wtoi(pe->value);
	return CP_ACP;
}

static void CheckAndCreateDictionariesDirectory(void){
	if(*g_NotePaths.DictDir == '\0'){
		GetModuleFileNameW(NULL, g_NotePaths.DictDir, MAX_PATH);
		PathRemoveFileSpecW(g_NotePaths.DictDir);
		wcscat(g_NotePaths.DictDir, SPELL_DIR);
	}
	if(!PathFileExistsW(g_NotePaths.DictDir)){
		CreateDirectoryW(g_NotePaths.DictDir, NULL);
	}
}

static void CheckAndCreateDictionariesFile(void){
	HRSRC				hRes;
	HGLOBAL				hResLoad;
	HANDLE				hFile;
	LPBYTE				lpRes;
	DWORD				size, dwWritten;
	wchar_t				*pBuffer, path[MAX_PATH];

	PathCombineW(path, g_NotePaths.DictDir, DICT_FILE);
	if(PathFileExistsW(path)){
		return;
	}
	hRes = FindResource(NULL, "#8010", RT_RCDATA);
	if(hRes){
		hResLoad = LoadResource(NULL, hRes);
		if(hResLoad){
			lpRes = LockResource(hResLoad);
			size = SizeofResource(NULL, hRes);
			pBuffer = calloc(size + 2, sizeof(BYTE));
			if(pBuffer){
				memcpy(pBuffer, lpRes, size);
				hFile = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(hFile != INVALID_HANDLE_VALUE){
					WriteFile(hFile, pBuffer, size, &dwWritten, NULL);
					CloseHandle(hFile);
				}
				free(pBuffer);
			}
		}
	}
}

static void LoadCodePages(void){
	HRSRC				hRes;
	HGLOBAL				hResLoad;
	LPBYTE				lpRes;
	DWORD				size;
	wchar_t				*pBuffer, *w1, *w2, *p1, *p2;

	hRes = FindResource(NULL, "#8009", RT_RCDATA);
	if(hRes){
		hResLoad = LoadResource(NULL, hRes);
		if(hResLoad){
			lpRes = LockResource(hResLoad);
			size = SizeofResource(NULL, hRes);
			pBuffer = calloc(size + 2, sizeof(BYTE));
			memcpy(pBuffer, lpRes, size);
			w1 = wcstok(pBuffer, L"\n\r", &p1);;
			while(w1){
				w2 = wcstok(w1, L"=", &p2);
				addEntry(g_CodePages, w2, p2);
				w1 = wcstok(NULL, L"\n\r", &p1);
			}
			free(pBuffer);
		}
	}

}

static void AddDictFiles(wchar_t * szSpellDir, wchar_t * szAff){
	wchar_t				szTempAff[MAX_PATH], szTempDict[MAX_PATH], szAffName[128], szDicName[128];
	wchar_t				szKey[16], szTotal[MAX_PATH * 2];

	wcscpy(szAffName, szAff);
	wcscpy(szDicName, szAff);
	PathRemoveExtensionW(szDicName);
	wcscat(szDicName, L".dic");
	wcscpy(szTempDict, szSpellDir);
	wcscat(szTempDict, szDicName);
	wcscpy(szTempAff, szSpellDir);
	wcscat(szTempAff, szAffName);
	if(PathFileExistsW(szTempDict)){
		wcscpy(szTotal, szTempAff);
		wcscat(szTotal, L"|");
		wcscat(szTotal, szTempDict);
		PathRemoveExtensionW(szDicName);
		wcscpy(szKey, szDicName);
		addEntry(g_DictFiles, szKey, szTotal);
	}
}

static void BuildSpellMenu(void){
	HMENU				h1;
	int					id = SPELL_DICT_ADDITION + 1, count;
	MITEM				mit = {0};
	BOOL				fSep = FALSE;

	mit.xCheck = 23;
	mit.yCheck = 0;
	mit.xPos = -1;
	mit.yPos = -1;


	h1 = GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_SPELL_CHECK));
	
	count = GetMenuItemCount(h1);
	if(count > 5){
		for(int i = count - 1; i >= 5; i--){
			FreeSingleMenu(h1, i);
			DeleteMenu(h1, i, MF_BYPOSITION);
		}
	}
	for(int i = 0; i < HASH_SIZE; i++){
		if(g_DictFiles[i]){
			for(PHENTRY pd = g_DictFiles[i]; pd; pd = pd->next){
				PHENTRY		pl = getEntry(g_LangNames, pd->key);
				if(pl){
					fSep = TRUE;
					wcscpy(mit.szText, pl->value);
					wcscpy(mit.szReserved, pl->key);
					mit.id = id++;
					AppendMenuW(h1, MF_STRING, mit.id, mit.szText);
					if(wcscmp(pl->key, g_Spell.spellCulture) == 0)
						CheckMenuItem(h1, mit.id, MF_BYCOMMAND | MF_CHECKED);
					SetMenuItemProperties(&mit, h1, mit.id, false);
				}
			}
		}
	}
	if(fSep){
		InsertMenuW(h1, 5, MF_BYPOSITION | MF_OWNERDRAW | MF_SEPARATOR, 0, NULL);
	}
}
