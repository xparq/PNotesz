#ifndef __SHARED_H__
#define __SHARED_H__

#include <wchar.h>
#include <shlwapi.h>
#include <commctrl.h>

#include "structures.h"
#include "docking.h"
#include "menu.h"
#include "tags.h"
#include "tts.h"
#include "groups.h"
#include "spelling.h"
#include "htable.h"

//edit control styles
#define	E_STYLE_WITHOUT_SCROLL	WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL
#define	E_STYLE_WITH_SCROLL		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL | WS_VSCROLL

typedef HRESULT (WINAPI* LPFN_DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);

typedef enum _ddiff {SECOND, MINUTE, HOUR, DAY, WEEK, MONTH, YEAR} ddiff;
typedef enum _ordinal_days {first, second, third, fourth, last} ordinal_days;

BOOL IsPasswordSet(void);
BOOL IsAnyHiddenInGroup(int group);
BOOL IsGroupLocked(int group);
BOOL IsDiaryLocked(void);
BOOL UnlockGroup(int group);
BOOL UnlockNote(PMEMNOTE pNote);
void ShowNotesByDate(SYSTEMTIME st);
SYSTEMTIME DateSubtract(int days);
int CALLBACK ReceivedAtCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int CALLBACK SentAtCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int CALLBACK DateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int CALLBACK DeletedDateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int CALLBACK CreationDateCompareFunct(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
DWORD SharedOutStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
BOOL IsEmptyString(wchar_t * lpString);
void SaveContacts(void);
BOOL SendNoteToContact(HWND hCaller, PMEMNOTE pNote, wchar_t * lpContact);
void BuildContactsMenu(HMENU hMenu, int indexContact, int indexGroup);
void BuildExternalMenu(HMENU hMenu);
void BuildShowHideByTagsMenu(HMENU hMenu);
HWND GetNeededArrow(int result);
void MeasureSkinlessNoteRectangles(PMEMNOTE pNote, int cx, int cy, wchar_t * lpCaption);
BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);
void CheckSavedRectangle(LPRECT lpRect);
SIZE GetScreenMetrics(void);
void GetSystemStartTime(void);
void ApplyHotDialogLanguage(HWND hwnd);
void HKeysAlreadyRegistered(wchar_t * lpKey);
BOOL RestrictedHotKey(wchar_t * szKey);
void EncryptAll(wchar_t * lpHash);
void DecryptAll(wchar_t * lpHash);
void DrawComboItem(const DRAWITEMSTRUCT * lpd);
short DockIndex(int dockData);
int DockType(int dockData);
void BuildDiaryMenu(HMENU hParent);
BOOL AreDatesEqual(LPSYSTEMTIME lpst1, LPSYSTEMTIME lpst2);
void SaveNewDockData(HWND hwnd, int dockData);
void SetDockIndex(int * dockData, short index);
HWND MoveDockWindow(HWND hwnd, int dockType, short index);
void BitOff(int * data, int position);
void BitOn(int * data, int position);
BOOL IsBitOn(int data, int position);
int GetBit(int data, int position);
void ConstructDateTimeString(const LPSYSTEMTIME lpSt, wchar_t * lpBuffer);
void GetPNotesMessage(const wchar_t * lpKey, const wchar_t * lpDefault, wchar_t * lpMessage);
void SetGroupText(HWND hwnd, int idGroup, int id, const wchar_t * lpFile, const wchar_t * lpDefault);
BOOL DoesAnySkinExist(wchar_t * lpSkin);
void SetDlgCtlText(HWND hwnd, int id, const wchar_t * lpFile, const wchar_t * lpDefault);
void SetMenuText(int id, const wchar_t * lcpSection, const wchar_t * lcpFile, wchar_t * lpDefault, wchar_t * lpBuffer);
void SetMenuTextWithAccelerator(int id, const wchar_t * lpSection, const wchar_t * lpFile, wchar_t * lpDefault, wchar_t * lpBuffer, const wchar_t * lpAcc);
void FillSkins(HWND hwnd, const wchar_t * lpSkinCurrent, BOOL fAllSkins, BOOL fShowNoSkin);
BOOL FileExists(wchar_t * lpDir, wchar_t * lpFile);
void DrawSkinPreview(HWND hwnd, P_NOTE_RTHANDLES pH, P_NOTE_APPEARANCE pA, COLORREF crMask, const DRAWITEMSTRUCT * lpDI, BOOL drawSimpleIfNoSkin);
void GetSkinProperties(HWND hWnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkinFile, BOOL bDeletePrevious);
void ChangeSkinPreview(HWND hwnd, int idList, int idPvw, P_NOTE_RTHANDLES pH, wchar_t * lpTempSkin, BOOL fDeletePrev);
BOOL IsFontsEqual(const LOGFONTW * lf1, const LOGFONTW * lf2);
void ShowPopUp(HWND hwnd, HMENU hMenu);
void SetMIText(PMITEM lpMI, const int size, int id, const wchar_t * lpText);
void CenterWindow(HWND hwnd, BOOL fSetDefSize);
void GetTooltip(wchar_t * lpTip, PMITEM pItems, int size, UINT id);
void GetSubPathW(wchar_t * lpResult, const wchar_t * lpFile);
int ParseDateFormat(wchar_t * lpFormat, wchar_t * lpDFormat, wchar_t * lpTFormat);
void FillSkinlessNoteCaption(HDC hdc, LPRECT lprc, COLORREF clr, LPRECT lprGrip);
void DrawSimpleNotePreview(HWND hwnd, const DRAWITEMSTRUCT * lpDI, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH);
BOOL ChangeSkinlessColor(HWND hwnd, LPCOLORREF lpcrWindow, int id);
int ChangeSkinlessCaptionFont(HWND hwnd, LPLOGFONTW plfCaption, LPCOLORREF lpcrCaption, P_NOTE_RTHANDLES ptH, int id, BOOL fDeleteOld);
void SendNoteByEmail(PMEMNOTE pNote);
BOOL NotesDirExists(wchar_t * lpDir);
BOOL NotesDirCreate(wchar_t * lpDir);
void CreateColorMenu(HMENU hMenu, int * idMenu, const int idStart, wchar_t * lpCaption);
int GetMaxSizeMenuID(void);
void CreateSizeMenu(HMENU * pMenu, BOOL fCreate);
void RemoveSizeMenu(void);
void SetCFSize(int size);
int GetCFSize(void);
UINT APIENTRY CCHookProc(HWND hdlg,	UINT uiMsg,	WPARAM wParam, LPARAM lParam);
UINT APIENTRY CFHookProc(HWND hdlg,	UINT uiMsg,	WPARAM wParam, LPARAM lParam);
void ConstructSaveMessage(wchar_t * lpNoteName, wchar_t * lpBuffer);
HBITMAP GetSkinById(HWND hWnd, UINT id, const wchar_t* szFile);
void LoadSearchHistory(HWND hwnd);
void LoadReplaceHistory(HWND hwnd);
int SharedFindProc(HWND hEdit);
int SharedReplaceProc(HWND hEdit);
void SaveSearchHistory(HWND hwnd);
void SaveReplaceHistory(HWND hwnd);
BYTE GetSmallValue(SMALLVALUES sv, int position);
void SetSmallValue(PSMALLVALUES psv, int position, BYTE value);
void PrepareDockMenu(PMEMNOTE pNote, HMENU hMenu, int index);
int GetMenuPosition(HMENU hMenu, int id);
void DrawSimpleNotePreviewDock(HWND hwnd, const DRAWITEMSTRUCT * lpDI, P_PNDOCK pA, COLORREF crFont, P_NOTE_RTHANDLES pH);
void DrawSkinPreviewDock(HWND hwnd, P_NOTE_RTHANDLES pH, P_PNDOCK pA, COLORREF crMask, COLORREF crFont, const DRAWITEMSTRUCT * lpDI);
void ApplyDockColors(PMEMNOTE pNote);
void SetMIImageIndex(PMITEM lpMI, const int size, int id, const int idImg);
int GetMIImageIndex(PMITEM lpMI, const int size, int id);
void PrepareMenuRecursive(PMITEM pMenus, int size, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection);
PMITEM GetMItem(PMITEM lpMI, const int size, int id);
void GetMIText(PMITEM lpMI, const int size, int id, wchar_t * lpText);
void GetMIReserved(PMITEM lpMI, const int size, int id, wchar_t * lpRes);
void FrameSkinlessNote(HDC hdc, LPRECT lprc, COLORREF clr);
HWND GetUnvisibleNoteEdit(PMEMNOTE pNote);
DWORD CALLBACK InStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
void ReadNoteRTFFile(wchar_t * lpID, HWND hEdit, wchar_t * lpCopyFrom);
void ReadRestoreRTFFile(HWND hEdit, wchar_t * lpPath);
void ReadSimpleRTFFile(HWND hEdit, wchar_t * lpID);
BOOL IsTextInClipboard(void);
int DateDiff(const SYSTEMTIME d1, const SYSTEMTIME d2, ddiff diffType);
void PrepareSingleMenuWithAccelerator(PMITEM pMenus, int size, int id, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection, P_HK_TYPE lpKeys, int keysCount);
void PrepareMenuRecursiveWithAccelerators(PMITEM pMenus, int size, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection, P_HK_TYPE lpKeys, int keysCount);
P_HK_TYPE GetHotKeyByID(int id, P_HK_TYPE phk, int count);
int ItemImageIndex(PMEMNOTE pNote);
void SetProperWeekday(int dow, wchar_t * lpType);
void ParseScheduleType(P_SCHEDULE_TYPE pS, wchar_t * lpType);
void GetScheduleDescription(int id, wchar_t * lpBuffer, const wchar_t * lpFile, const wchar_t * lpDefault);
void GetNoteTempFileName(wchar_t * lpPath);
BOOL IsDateEmpty(LPSYSTEMTIME lpd);
short RealDayOfWeek(int dof);
short OrdinalDayOfWeek(int dof);
BOOL IsLeapYear(int year);
int DaysInMonth(SYSTEMTIME st);
ordinal_days DayOfWeekOrdinal(SYSTEMTIME st);
void SetMenuTextByAnotherID(PMITEM pMenus, int size, HMENU hMenu, int idMenu, int idFrom, wchar_t * lpLangFile, wchar_t * lpSection);
BOOL IsVisualStyleEnabled(void);
BOOL IsDLLVersionForStyle(void);
long CompareSystemTime(const SYSTEMTIME d1, const SYSTEMTIME d2);
void PreviewSound(wchar_t * lpSound);
void RestorePlacement(HWND hwnd, BOOL fSetDefSize);
void ClearRTHandles(P_NOTE_RTHANDLES pH);
void GetFormattedDateTime(wchar_t * lpDate);
void AddVoicesToControl(HWND hControl, UINT uAddMsg, UINT uSetDataMsg, UINT uSetCurrSelMsg, wchar_t * lpDefVoice);
int GetHotkeyID(P_HK_TYPE hotKeys, int count, int mod, int key);
void SaveGroup(LPPNGROUP ppg);
void SaveContactsGroups(void);
void RemoveDoubleComma(wchar_t * s);
void RemoveDoubleSpace(wchar_t * s);
BOOL CALLBACK ContGroups_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ContGroups_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void ContGroups_OnClose(HWND hwnd);
BOOL ContGroups_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Hotdlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
BOOL Hotdlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
BOOL CALLBACK Hotdlg_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitSpeller(void);
void OpenPage(HWND hwnd, wchar_t * lpAddress);
void ChangeNoteLookByGroup(PMEMNOTE pNote, int newGroup);
void DisableInput(BOOL value);
void _wcsltrm(wchar_t * s);
void _wcsrtrm(wchar_t * s);
void _wcstrm(wchar_t * s);
void _wcsrpc(wchar_t * src, wchar_t c1, wchar_t c2);
wchar_t * _wcssrpc(wchar_t * src, const wchar_t * s1, const wchar_t * s2);
int __cdecl GroupMenusCompare(const void *p1, const void *p2);
int __cdecl FavMenusCompare(const void *p1, const void *p2);
int __cdecl DiaryMenusCompareDesc(const void *pp1, const void *pp2);
int __cdecl DiaryMenusCompareAsc(const void *pp1, const void *pp2);
int __cdecl SengsCompareAsc(const void *p1, const void *p2);
int __cdecl ContGroupsMenusCompare(const void *p1, const void *p2);

DARROWS					g_DArrows;
NOTE_RTHANDLES			g_RTHandles, g_DRTHandles, g_DiaryRTHandles;
NOTE_SETTINGS			g_NoteSettings;
NEXT_SETTINGS			g_NextSettings;
NOTE_APPEARANCE			g_Appearance, g_DiaryAppearance;
SOUND_TYPE				g_Sound;
NOTE_PATHS				g_NotePaths;
PNSTRINGS				g_Strings;
NT_DT_FORMATS			g_DTFormats;
FONT_TYPE				g_SelectedFont;
SMALLVALUES				g_SmallValues;
HBITMAP					g_hBmpMenuNormal, g_hBmpMenuGray, g_hBmpEditNormal, g_hBmpEditGray;
HBITMAP					g_hBmpMisc;
BOOL					g_IsClosing;
wchar_t					g_CurrentFontFace[LF_FACESIZE];
DLGPROC					g_SearchProc, g_SearchTagsProc;
HFONT					g_hMenuFont;
HMENU					g_hNoteMainMenu, g_hNotePopUp;
HMENU					g_hEditMainMenu, g_hEditPopUp;
HMENU					g_hSizePopUp, g_hDropMenu, g_hDropPopUp;
HMENU					g_hBulletsMenu;
HWND					g_hCPDialog, g_hOptionsDlg, g_hComboDialog, g_hLoginDialog, g_hTags;
HWND					g_hSearchDialog, g_hAdjust, g_hMain, g_hNotesParent, g_hSearchTags, g_hSearchDates;
HWND					g_hFindDialog, g_hSearchTitlesDialog, g_hOverdueDialog, g_hSearchEverywhereDialog;
HWND					g_hLastModal, g_hPinToWindow, g_hHotkeys, g_hSyncFolders, g_hSpellChecking;
wchar_t					g_SearchString[256], g_ReplaceString[256];
wchar_t					g_DefBrowser[MAX_PATH], g_NewNoteName[128];
int						g_Flags;
HINSTANCE				g_hInstance;
HMODULE					g_hSmilies;
int						g_FormatColorID, g_FormatHighlightID, g_ColorsMenuID;
wchar_t					* g_Weekdays[7], * g_RepParts[7];
EMPTY_NOTES				g_Empties;
HANDLE					g_hHeap;
int						g_OverdueCount;
HCURSOR					g_CurV, g_CurH;
int						g_WinVer;
BOOL					g_VSEnabled;
BOOL					g_LockedState, g_UnlockFromMouse;
BOOL					g_StopInput;
wchar_t					* g_Sounds[256];
SYSTEMTIME				g_StartTimeSystem, g_StartTimeProg, g_LastStartTimeProg;
int						g_DiaryFormatIndex;
short					g_LangID, g_SubLangID;
LPPTAG					g_PTagsPredefined;
LPPSENG					g_PSengs, g_PExternalls;
// wchar_t					g_TempLink[28];
HWND					g_hTEditSimple, g_hTEditRich, g_hTEditContent;
P_HK_TYPE				g_EditHotkeys, g_NoteHotkeys;
HIMAGELIST				g_hImlPvwNormal, g_hImlPvwGray;
HWND					g_hLinksDlg, g_hSeveralContacts, g_hMultiGroups;
LPPCONTACT				g_PContacts;
LPPNGROUP				g_PGroups;
LPPCONTGROUP			g_PContGroups;
LPPLOCALSYNC			g_PLocalSyncs;
PNSPELL					g_Spell;
PHENTRY					g_DictFiles[HASH_SIZE], g_LangNames[HASH_SIZE], g_CodePages[HASH_SIZE], g_Cultures[HASH_SIZE];
BOOL					g_DisableInput;
#endif	// SHARED_H__
