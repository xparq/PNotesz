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

#include <olectl.h>

#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "note.h"
#include "toolbar.h"
#include "splitcontainer.h"
#include "print.h"
#include "gsearchbox.h"
#include "gradients.h"
#include "hotkeys.h"
#include "shared.h"
#include "memorynotes.h"
#include "groups.h"
#include "enums.h"
#include "tags.h"
#include "fdialogs.h"
#include "contacts.h"
#include "hosts.h"
#include "sockerrors.h"
#include "sockets.h"
#include "multicontacts.h"
#include "searchdates.h"
#include "login.h"
#include "multigroups.h"
#include "contgroups.h"
#include "gcpicker.h"

/** Constants ********************************************************/
#define	PROP_GRP					L"_ngroup"
#define	PROP_IT						L"_iconcusttype"
#define	PROP_GROUP_IML				L"_groupiml"
#define	PWRD_PROTECTED_GROUP		L" ***"
#define OCR_NORMAL  				32512
#define OCR_NO  					32648
#define	SEARCH_COUNT				512

/** Macros ********************************************************/
#define	LPARAM_FROM_LPCTSTR(id)		(LPARAM)(id)

static enum group_visibility {
	VISIBLE_ONLY = 2, 
	HIDDEN_ONLY = 4, 
	BOTH_VH = 6
};

static enum columns {
	COL_NAME, 
	COL_PRIORITY, 
	COL_COMPLETED, 
	COL_PROTECTED, 
	COL_PASSWORD,
	COL_PIN,
	COL_FAVORITES,
	COL_SR, 
	COL_GROUP, 
	COL_CREATED,
	COL_SAVED,
	COL_DELETED, 
	COL_SCHEDULE, 
	COL_TAGS, 
	COL_CONTENT, 
	COL_SENT_TO,
	COL_SENT_AT,
	COL_RECEIVED_FROM,
	COL_RECEIVED_AT,
	COL_ID,
	COL_BACK_ORIGINAL, 
	COL_BACK_TIME,

	COL_MAX
};

static enum selected_event {
	SEL_TREE_ROOT, 
	SEL_TREE_ORD, 
	SEL_TREE_REC, 
	SEL_TREE_FAV, 
	SEL_LIST_SELECTED_ON_ROOT_SINGLE, 
	SEL_LIST_SELECTED_ON_ROOT_MULTIPLE, 
	SEL_LIST_DESELECTED_ON_ROOT, 
	SEL_LIST_ORD_SELECTED_SINGLE, 
	SEL_LIST_ORD_SELECTED_MULTIPLE, 
	SEL_LIST_ORD_DESELECTED, 
	SEL_LIST_REC_SELECTED_SINGLE, 
	SEL_LIST_REC_SELECTED_MULTIPLE, 
	SEL_LIST_REC_DESELECTED, 
	SEL_LIST_FAV_SELECTED_SINGLE, 
	SEL_LIST_FAV_SELECTED_MULTIPLE, 
	SEL_LIST_FAV_DESELECTED, 
	SEL_LIST_AFTER_COMMAND_SINGLE, 
	SEL_LIST_AFTER_COMMAND_MULTIPLE, 
	SEL_TREE_SEARCH_RESULTS, 
	SEL_TREE_BACKUP, 
	SEL_LIST_BACKUP_SELECTED_SINGLE, 
	SEL_LIST_BACKUP_SELECTED_MULTIPLE, 
	SEL_LIST_BACKUP_DESELECTED, 
	SEL_LIST_DIARY_SELECTED_SINGLE, 
	SEL_LIST_DIARY_SELECTED_MULTIPLE
};

typedef struct _COLVISIBLE{
	int				col;
	int				width;
	int				defWidth;
}COLVISIBLE, *P_COLVISIBLE;

typedef struct _CPENUM{
	int				group;
	int				result;
	int				reserved;
	int				total;
}CPENUM, *P_CPENUM;

typedef struct _DRAGSTRUCT{
	BOOL			fStarted;
	HWND			hCurrent;
	HIMAGELIST		hIml;
	BOOL			fDropAllowed;
	int				idGroup;
	HTREEITEM		target;
	HTREEITEM		source;
}DRAGSTRUCT, *P_DRAGSTRUCT;

/** List View columns sorting ********************************************************/
typedef struct _LVSORT_TYPE {
	int				iSortName;
	int				iSortPriority;
	int				iSortCompleted;
	int				iSortProtected;
	int				iSortPassword;
	int				iSortSaved;
	int				iSortCreated;
	int				iSortDeleted;
	int				iSortSchedule;
	int				iSortTags;
	int				iSortGroup;
	int				iSortBackOrigin;
	int				iSortBackDate;
	int				iSortFavorites;
	int				iSortContent;
	int				iSortSR;
	int				iSortSentTo;
	int				iSortSentAt;
	int				iSortReceivedFrom;
	int				iSortReceivedAt;
	int				iSortPin;
	int				iSortID;
}LVSORT_TYPE;

typedef struct _LVSORT_MEMBER {
	int				column;
	int				order;
}LVSORT_MEMBER, *P_LVSORT_MEMBER;

/** Prototypes ********************************************************/
static BOOL CALLBACK Control_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void Control_OnClose(HWND hwnd);
static void Control_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Control_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Control_OnSize(HWND hwnd, UINT state, int cx, int cy);
static void Control_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Control_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Control_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void Control_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void Control_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void Control_OnSysColorChange(HWND hwnd);
static void PrepareList(HWND hDlg, HWND hList);
static void UpdateListView(void);
static void UpdateControlStatus(void);
static void ApplyControlPanelLanguage(HWND hwnd, BOOL fRefreshTree);
static void BuildColumnsVisibilityMenu(void);
static void ReplaceItem(HWND hList, PMEMNOTE pNote);
static void DeleteItem(HWND hList, LPARAM lParam);
static void InsertItem(HWND hList, PMEMNOTE pNote, int index);
static void EnableCommands(int flag);
static void SendNotesMessage(HWND hwnd, int id);
static void EnableMenus(UINT fEnabled, BOOL bMultiSelect);
static int CALLBACK BackDateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK NameCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK PriorityCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK CompletedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK ProtectedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK PasswordCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK PinCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK IDCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK FavoritesCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK SRCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static void ChangeListViewView(HWND hwnd, int view);
static void PrepareTree(HWND hwnd);
static void EnableTreeMenus(HTREEITEM hi);
static void EnableTreeMenusForRoot(void);
static void DisableTreeToolbar(void);
static BOOL CALLBACK NewGroup_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NewGroup_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL NewGroup_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void NewGroup_OnDestroy(HWND hwnd);
static void NewGroup_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void FillSkinsCombo(HWND hwnd, const wchar_t * lpSkinCurrent);
static int FillGroupColors(HWND hCombo1, HWND hCombo2);
static BOOL CALLBACK Bitmaps_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void Bitmaps_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Bitmaps_OnDestroy(HWND hwnd);
static BOOL Bitmaps_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static HTREEITEM InsertGroup(HTREEITEM hItem, LPPNGROUP ppg, BOOL fSetParent, BOOL fSave);
static void ShowAllGroups(int iParent, HTREEITEM hItem);
static BOOL CALLBACK TreeView_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void TreeView_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void TreeLButtonDown(HWND hwnd, int x, int y, UINT keyFlags);
static BOOL CALLBACK Tree_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void ClearDragStructure(void);
static BOOL IsTreeItemSuitable(void);
static void FindItemById(HTREEITEM hItem, int id);
static int GetSelectedTVItemId(void);
static void DeleteTreeGroup(HTREEITEM hItem, BOOL fGetSibling);
static int GetTVItemId(HTREEITEM hItem);
static void ShowTreePopUp(void);
static void ShowRecyclePopUp(void);
static HTREEITEM GetDropHighLightedItem(int x, int y);
static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL IsItemFromChildBranch(HTREEITEM hDragged, HTREEITEM hHighlighted);
static HTREEITEM InsertTreeviewItems(void);
static void DefineFavoriteStatus(int index);
static BOOL DeleteItems(void);
static BOOL DeleteBackups(void);
static BOOL EmptyBin(void);
static BOOL RestoreFromBin(int group);
static BOOL RestoreAllFromBin(void);
static void InsertNotesIntoList(P_CPENUM pcp);
static void CheckGroupVisibility(P_CPENUM pcp);
static void HideGroup(HTREEITEM hItem);
static void DeleteNotesGroup(int group);
static void ShowGroup(HTREEITEM hItem);
static void RollUnrollItems(void);
static void HideItems(void);
static void ShowItems(void);
static void TogglePins(void);
static void SendItemByEmail(void);
static void SendItemAsAttachment(void);
static void SaveItem(void);
static void SaveItemAs(void);
static void AddItemsToFavorites(void);
static void ToggleItemsPriority(void);
static void ToggleItemsProtection(void);
static void ToggleItemsCompleted(void);
static void RestoreCenterItem(int operation);
static void RestoreNoteFromBackup(void);
static void RestoreBackupToNote(void);
static void AdjustItem(int type);
static void PrintItem(void);
static PMEMNOTE SelectedNote(void);
static LONG_PTR * SelectedPItems(INT_PTR * pCount);
static void UpdateNoteItem(PMEMNOTE pNote);
static void SetItemGroupName(PMEMNOTE pNote, int index);
static void SetItemSentToReceivedFrom(PMEMNOTE pNote, int index, int subIndex);
static void SetItemSentReceivedDate(PMEMNOTE pNote, int index, int subIndex);
static void SetItemChangeDate(PMEMNOTE pNote, int index);
static void SetItemDetetedDate(PMEMNOTE pNote, int index);
static void SetItemCreatedDate(PMEMNOTE pNote, int index);
static void SetItemTags(PMEMNOTE pNote, int index);
static void SetItemContent(PMEMNOTE pNote, int index);
static void SetItemPriority(PMEMNOTE pNote, int index);
static void SetItemCompleted(PMEMNOTE pNote, int index);
static void SetItemProtected(PMEMNOTE pNote, int index);
static void SetItemPassword(PMEMNOTE pNote, int index);
static void SetItemPin(PMEMNOTE pNote, int index);
static void SetItemFavorite(PMEMNOTE pNote, int index);
static void SetItemSR(PMEMNOTE pNote, int index);
static void SetItemID(PMEMNOTE pNote, int index);
static void SetItemScheduleDescription(PMEMNOTE pNote, int index);
static void ShowButtonDropDown(HMENU hMenuParent, int buttonId, int menuId);
// static void ShowTreeViewDropDown(int buttonId, int menuId);
static void UpdateGroupText(int id, HTREEITEM hItem);
static void ShowHidePreview(void);
static BOOL CALLBACK DTree_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL DTree_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void DTree_OnSize(HWND hwnd, UINT state, int cx, int cy);
static void DTree_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void DTree_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void DTree_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void DTree_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void SetPreviewBackgroundColor(BOOL clear, COLORREF color);
static void ShowNotePreview(int selCount);
static BOOL ChoosePreviewColor(void);
static void ReselectAfterCommand(void);
static void CollectGroups(HTREEITEM hItem);
static BOOL IsGroupInCollection(int group);
static void QuickSearchInGroup(wchar_t * lpString, BOOL fromBin);
static void QuickSearchAllNotes(wchar_t * lpString, BOOL fromBin);
static void QuickSearchRecycleBin(wchar_t * lpString);
static void SelectSearchResultsItem(void);
static int CountOfBackupCopies(void);
static void InsertBackupItem(HWND hList, PWIN32_FIND_DATAW pfd, int index);
static void UpdateNameCaption(wchar_t * lpCaption);
static void UpdateBackupItems(wchar_t * lpId);
static void DeleteBackupItems(wchar_t * lpId);
static void UpdateBackupControlStatus(void);
static void UpdateDiaryNames(void);
static void SelectedBackItem(wchar_t * lpBuffer);
static void ShowBackPreview(int selCount);
static void CreateRebarBackground(HWND hwnd, HWND hToolbar, HBITMAP * phbm);
static HWND AddRebar(HWND hParent, HWND hToolbar);
static void DrawToolbarButton(HWND hToolbar, HDC hdc, LPRECT lprc, int state, int id);
static void CheckBranchVisibility(HTREEITEM hItem, BOOL fGetSibling);
static void ShowBranch(HTREEITEM hItem, BOOL fGetSibling);
static void HideBranch(HTREEITEM hItem, BOOL fGetSibling);
static void SetTVItemText(HTREEITEM hItem, wchar_t * lpText);
static void AddCountToGroupCaption(int count, wchar_t * lpText);
static BOOL CALLBACK GetGoupsIconsIdentifiersProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);
static void FillTreeImageList(void);
static void ReapplyIconNumbers(HWND hList);
static int GetMaxParam(HWND hList);
static void AddGroupIcon(HWND hwnd);
static void RemoveGroupIcon(HWND hwnd);
static void RefreshTreeviewItemsIcons(HTREEITEM hItem, int lastIndex);
static void ShowHideColumn(int column, BOOL show);
static BOOL CALLBACK PreviewProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void DeleteSelectedFromBin(void);
static void SendItemsToContact(wchar_t * lpContact);
static void SendItemsToMultipleContacts(void);
static void EnableLockButtonsForDiary(void);
static void ToggleLock(void);
static void ChangeButtonImageIndex(int cmd, int index);

/** Module variables ********************************************************/
static HIMAGELIST			m_hImlSmall, m_hImlLarge, m_hImlTbrNormal, m_hImlTbrGray, m_hImlTreeNormal;
static HIMAGELIST			m_hImlDragSmall = 0, m_hImlDragLarge = 0;
static HWND					m_hVertSplitter, m_hHorSplitter, m_hTreeMain, m_hListMain, m_hDTree, m_hCtrlPanel;
static HWND					m_hTbrMain, m_hTbrTree, m_hStbControl, m_hEditPreview, m_hSearchBox, m_hBitmaps, m_hGroups;
static HWND					m_hRebarMain, m_hRebarTree;
static HBITMAP				m_RebarBitmap, m_hBmpCtrlNormal, m_hBmpCtrlGray;
static HMENU				m_hMenuControl = 0, m_hPUMenuControl, m_hMenuTree = 0, m_hPUMenuTree;
static HMENU				m_hMenuRecycle = 0, m_hPUMenuRecycle, m_hMenuCtrlTemp, m_hPUMenuCV;
static HMENU				m_hMainPopupMenu;
static LVSORT_TYPE			m_LVSortRegular = {LVS_SORTASCENDING, LVS_SORTASCENDING};
static LVSORT_TYPE			m_LVSortBack = {LVS_SORTASCENDING, LVS_SORTASCENDING};
static int					m_View = LVS_REPORT;
static DRAGSTRUCT			m_DragStruct;
static HCURSOR				m_CurNo, m_CurArrow;
static HICON				m_hIconSearch, m_hIconSearchDis;
static int					m_OffVert, m_OffHor, m_xMouse, m_yMouse, m_Up, m_Down;
static HTREEITEM			m_tItem = NULL, m_tSelected = NULL, m_tHighlighted = NULL, m_tRightClick = NULL;
static LVSORT_MEMBER		m_lvsmRegular = {0, LVS_SORTASCENDING}, m_lvsmBack = {0, LVS_SORTASCENDING};
static BOOL					m_TreeDragReady = FALSE, m_TreeImagesExist;
static int					m_PvwColor;
static BOOL					m_PvwUseColor;
static PMEMNOTE				* m_pSearchResults;
static int					m_iSearchCount, * m_pGroupsArray, m_iGroupsCount;
static wchar_t				m_QSString[256], m_QSGroup[256], m_RegularNameCaption[256], m_BackupNameCaption[256];
static COLORREF				m_clrHot = RGB(255, 230, 181), m_clrSel = RGB(255, 174, 106), m_clrFrame = RGB(75, 75, 111);
static int					m_BranchVisibility = 0;
static int					m_gOffset, m_BitmapIndex = 0;
static wchar_t				m_TempLib[MAX_PATH], m_ImagesPath[MAX_PATH];
static int					m_ColsOrder[COL_MAX];
static int					m_WTreeTbr, m_WTreeMain;

/** Toolbar buttons ********************************************************/
static TBBUTTON 			m_TBBtnArray[] = {{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{0,IDM_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{1,IDM_LOAD_NOTE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{2,IDM_NOTE_FROM_CLIPBOARD,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{3,IDM_DUPLICATE_NOTE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{4,IDM_DIARY,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{5,IDM_SAVE,0,TBSTYLE_BUTTON,0,0,0,0},
							{6,IDM_SAVE_AS,0,TBSTYLE_BUTTON,0,0,0,0},
							{7,IDM_SAVE_AS_TEXT,0,TBSTYLE_BUTTON,0,0,0,0},
							{8,IDM_RESTORE_BACKUP,0,TBSTYLE_BUTTON,0,0,0,0},
							{9,IDM_PRINT_NOTE,0,TBSTYLE_BUTTON,0,0,0,0},
							{10,IDM_ADJUST,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{11,IDM_DEL,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{12,IDM_SAVE_ALL,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{21,IDM_BACK_SYNC,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{13,IDM_PLACE_VISIBILITY,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{14,IDM_EMAIL,0,TBSTYLE_BUTTON,0,0,0,0},
							{15,IDM_ATTACHMENT,0,TBSTYLE_BUTTON,0,0,0,0},
							{16,IDM_SEND_INTRANET,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{17,IDM_TAGS,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{18,IDM_NOTE_MARKS,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{19,IDM_ADD_TO_FAVORITES,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{44,IDM_PASSWORD,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{22,IDM_RUN_PROG,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{23,IDM_EMPTY_BIN,0,TBSTYLE_BUTTON,0,0,0,0},
							{24,IDM_RESTORE_NOTE,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{25,IDM_PREVIEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{26,IDM_PVW_COLOR_SET,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{27,IDM_V_VIEW,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{28,IDM_OPTIONS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{41,IDM_HOT_KEYS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{29,IDM_SEARCH_SUBMENU,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{-1,IDM_DUMMY_ID,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{30,IDM_HELP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{31,IDM_PAYPAL,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}};
static TBBUTTON				m_TreeButtons[] = {
							{32,IDM_NEW_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{33,IDM_NEW_SUBGROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{34,IDM_EDIT_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{35,IDM_DELETE_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{36,IDM_CUSTOMIZE_ICONS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{37,IDM_SHOW_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{38,IDM_SHOW_INCLUDE_SG,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{39,IDM_HIDE_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{40,IDM_HIDE_INCLUDE_SG,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{42,IDM_LOCK_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{43,IDM_UNLOCK_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}
							};

/** Menu items ********************************************************/
static MITEM				m_CtrlMenus[] = {{IDM_NEW, 0, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Note", L""}, 
							{IDM_LOAD_NOTE, 1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Load Note", L""},
							{IDM_NOTE_FROM_CLIPBOARD, 2, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Note From Clipboard", L""}, 
							{IDM_DUPLICATE_NOTE, 3, 0, -1, -1, MT_REGULARITEM, FALSE, L"Duplicate Note", L""}, 
							{IDM_DIARY, 4, 0, -1, -1, MT_REGULARITEM, FALSE, L"Diary", L""},
							{IDM_TODAY_DIARY, -1, 0, -1, 0, MT_REGULARITEM, FALSE, L"Today", L""},
							{IDM_SAVE, 5, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save", L""}, 
							{IDM_SAVE_AS, 6, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save As...", L""}, 
							{IDM_SAVE_AS_TEXT, 7, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save As Text File", L""}, 
							{IDM_RESTORE_BACKUP, 8, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore From Backup", L""}, 
							{IDM_PRINT_NOTE, 9, 0, -1, -1, MT_REGULARITEM, FALSE, L"Print", L""}, 
							{IDM_ADJUST, 10, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust", L""}, 
							{IDM_ADJUST_APPEARANCE, 44, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Appearance", L""}, 
							{IDM_ADJUST_SCHEDULE, 45, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Schedule", L""}, 
							{IDM_DEL, 11, 0, -1, -1, MT_REGULARITEM, FALSE, L"Delete", L""}, 
							{IDM_SAVE_ALL, 12, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save All", L""}, 
							{IDM_BACK_SYNC, 21, 0, -1, -1, MT_REGULARITEM, false, L"Backup/Synchronization", L""},
							{IDM_BACKUP_FULL, -1, -1, -1, -1, MT_REGULARITEM, false, L"Create full backup", L""},
							{IDM_RESTORE_FULL, -1, -1, -1, -1, MT_REGULARITEM, false, L"Restore from full backup", L""},
							{IDM_PLACE_VISIBILITY, 13, 0, -1, -1, MT_REGULARITEM, FALSE, L"Placement/Visibility", L""}, 
							{IDM_DOCK, 46, 0, -1, -1, MT_REGULARITEM, FALSE, L"Dock", L""},
							{IDM_DOCK_NONE, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
							{IDM_DOCK_LEFT, 47, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
							{IDM_DOCK_TOP, 48, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
							{IDM_DOCK_RIGHT, 49, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
							{IDM_DOCK_BOTTOM, 50, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
							{IDM_EMAIL, 14, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send", L""},
							{IDM_ATTACHMENT, 15, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send As Attachment", L""},
							{IDM_SEND_INTRANET, 16, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send Via Network", L""},
							{IDM_TAGS, 17, 0, -1, -1, MT_REGULARITEM, FALSE, L"Tags", L""},
							{IDM_MANAGE_TAGS, 17, 0, -1, -1, MT_REGULARITEM, FALSE, L"Tags", L""},
							{IDM_NOTE_MARKS, 18, 0, -1, -1, MT_REGULARITEM, FALSE, L"Switches", L""},
							{IDM_HIGH_PRIORITY, 71, 0, -1, -1, MT_REGULARITEM, FALSE, L"Toggle High Priority", L""},
							{IDM_TOGGLE_PROTECTION, 72, 0, -1, -1, MT_REGULARITEM, FALSE, L"Toggle Protection Mode", L""},
							{IDM_LOCK_NOTE, 42, 0, -1, -1, MT_REGULARITEM, FALSE, L"Set Note Password", L""},
							{IDM_MARK_AS_COMPLETED, 73, 0, -1, -1, MT_REGULARITEM, FALSE, L"Mark As Completed", L""},
							{IDM_ROLL_UNROLL, 74, 0, -1, -1, MT_REGULARITEM, FALSE, L"Roll/Unroll", L""},
							{IDM_ADD_PIN, 76, 0, -1, -1, MT_REGULARITEM, FALSE, L"Pin To Window", L""},
							{IDM_ADD_TO_FAVORITES, 19, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add To Favorites", L""},
							{IDM_PASSWORD, 75, 0, -1, -1, MT_REGULARITEM, false, L"Password", L""},
							{IDM_CREATE_PASSWORD, -1, -1, -1, -1, MT_REGULARITEM, false, L"Create Password", L""},
							{IDM_CHANGE_PASSWORD, -1, -1, -1, -1, MT_REGULARITEM, false, L"Change Password", L""},
							{IDM_REMOVE_PASSWORD, -1, -1, -1, -1, MT_REGULARITEM, false, L"Remove Password", L""},
							{IDM_SYNC_NOW, -1, -1, -1, -1, MT_REGULARITEM, false, L"Synchronize", L""},
							{IDM_LOCAL_SYNC, -1, -1, -1, -1, MT_REGULARITEM, false, L"Manual Local Synchronization", L""},
							{IDM_RUN_PROG, 22, 0, -1, -1, MT_REGULARITEM, false, L"Run", L""},
							{IDM_EMPTY_BIN, 23, 0, -1, -1, MT_REGULARITEM, FALSE, L"Empty Recycle Bin", L""},
							{IDM_RESTORE_NOTE, 24, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore Note", L""},
							{IDM_PREVIEW, -1, 0, 51, 0, MT_REGULARITEM, FALSE, L"Preview", L""}, 
							{IDM_PVW_COLOR_SET, 26, 0, -1, -1, MT_REGULARITEM, FALSE, L"Preview Window Background Settings", L""},
							{IDM_PVW_USE_COLOR, -1, 0, 49, 0, MT_REGULARITEM, FALSE, L"Use Specified Color", L""}, 
							{IDM_PVW_COLOR_VALUE, 63, 0, -1, -1, MT_REGULARITEM, FALSE, L"Choose Color", L""}, 
							{IDM_DOCK_ALL, 46, 0, -1, -1, MT_REGULARITEM, FALSE, L"Docking (all notes)", L""}, 
							{IDM_DOCK_NONE_ALL, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
							{IDM_DOCK_LEFT_ALL, 47, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
							{IDM_DOCK_TOP_ALL, 48, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
							{IDM_DOCK_RIGHT_ALL, 49, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
							{IDM_DOCK_BOTTOM_ALL, 50, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
							{IDM_VISIBILITY, 52, 0, -1, -1, MT_REGULARITEM, FALSE, L"Visibility", L""}, 
							{IDM_SHOW, 64, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show", L""}, 
							{IDM_SHOW_ALL, 65, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show All", L""}, 
							{IDM_SHOW_BY_TAG, 69, 0, -1, -1, MT_REGULARITEM, false, L"Show By Tag", L""},
							{IDM_HIDE_BY_TAG, 70, 0, -1, -1, MT_REGULARITEM, false, L"Hide By Tag", L""},
							{IDM_HIDE, 66, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide", L""},
							{IDM_HIDE_ALL, 67, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide All", L""},
							{IDM_ALL_TO_FRONT, 68, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bring All To Front", L""},
							{IDM_SHOW_CENTER, 53, 0, -1, -1, MT_REGULARITEM, FALSE, L"Centralize", L""},
							{IDM_RESTORE_PLACEMENT, 54, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore Default Placement", L""},
							{IDM_ADD_CONTACT, 80, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add Contact", L""},
							{IDM_SEND_TO_SEVERAL, 82, 0, -1, -1, MT_REGULARITEM, FALSE, L"Select Contact", L""},
							{IDM_ADD_CONT_GROUP, 81, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add Group", L""},
							{IDM_SELECT_CONT_GROUP, 83, 0, -1, -1, MT_REGULARITEM, FALSE, L"Select Group", L""},
							{IDM_V_VIEW, 27, 0, -1, -1, MT_REGULARITEM, FALSE, L"Views", L""}, 
							{IDM_V_ICONS, 55, 0, 59, 0, MT_REGULARITEM, FALSE, L"Icons", L""}, 
							{IDM_V_SMALL, 56, 0, 60, 0, MT_REGULARITEM, FALSE, L"Small Icons", L""}, 
							{IDM_V_LIST, 57, 0, 61, 0, MT_REGULARITEM, FALSE, L"List", L""}, 
							{IDM_V_REPORT, 58, 0, 62, 0, MT_REGULARITEM, FALSE, L"Details", L""}, 
							{IDM_OPTIONS, 28, 0, -1, -1, MT_REGULARITEM, FALSE, L"Options", L""},
							{IDM_HOT_KEYS, 41, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hot Keys", L""},
							{IDM_SEARCH_SUBMENU, 29, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search", L""},
							{IDM_SEARCH_IN_NOTES, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search In Notes", L""},
							{IDM_SEARCH_BY_TAGS, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search By Tags", L""},
							{IDM_SEARCH_TARGET, -1, 0, 51, 0, MT_REGULARITEM, FALSE, L"Include Notes From \"Recycle Bin\" In Quick Search", L""}, 
							{IDM_CLEAR_QUICK_SEARCH, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Clear Quick Search", L""},						
							{IDM_HELP, 30, 0, -1, -1, MT_REGULARITEM, FALSE, L"Help", L""},
							{IDM_PAYPAL, 31, 0, -1, -1, MT_REGULARITEM, FALSE, L"Support PNotes Project", L""}};
static MITEM				m_TreeMenu[] = {{IDM_NEW_GROUP, 32, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Group", L""}, 
							{IDM_NEW_SUBGROUP, 33, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Subgroup", L""},
							{IDM_EDIT_GROUP, 34, 0, -1, -1, MT_REGULARITEM, FALSE, L"Modify Group", L""}, 
							{IDM_DELETE_GROUP, 35, 0, -1, -1, MT_REGULARITEM, FALSE, L"Delete Group", L""},
							{IDM_CUSTOMIZE_ICONS, 36, 0, -1, -1, MT_REGULARITEM, FALSE, L"Customize Icons", L""},
							{IDM_SHOW_GROUP, 37, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show Group", L""}, 
							{IDM_SHOW_INCLUDE_SG, 38, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show group (include subgroups)", L""},
							{IDM_HIDE_GROUP, 39, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide Group", L""},
							{IDM_HIDE_INCLUDE_SG, 40, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide group (include subgroups)", L""},
							{IDM_LOCK_GROUP, 42, 0, -1, -1, MT_REGULARITEM, FALSE, L"Set Group Password", L""},
							{IDM_UNLOCK_GROUP, 43, 0, -1, -1, MT_REGULARITEM, FALSE, L"Remove Group Password", L""}
							};
static MITEM				m_RecycleMenu[] = {{IDM_EMPTY_BIN, 23, 0, -1, -1, MT_REGULARITEM, FALSE, L"Empty Recycle Bin", L""},
							{IDM_RESTORE_ALL_BIN, 24, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore All Notes", L""}};

/** Columns visibility ********************************************************/
static COLVISIBLE			m_ColsVisibility[22] = {
							{COL_NAME, 160, 160}, 
							{COL_PRIORITY, 32, 32}, 
							{COL_COMPLETED, 32, 32}, 
							{COL_PROTECTED, 32, 32},
							{COL_PASSWORD, 32, 32},
							{COL_PIN, 32, 32},
							{COL_FAVORITES, 32, 32},
							{COL_SR, 32, 32},
							{COL_GROUP, 120, 120},
							{COL_CREATED, 160, 160},
							{COL_SAVED, 160, 160},
							{COL_DELETED, 160, 160},
							{COL_SCHEDULE, 160, 160},
							{COL_TAGS, 160, 160},
							{COL_CONTENT, 160, 160},
							{COL_SENT_TO, 160, 160},
							{COL_SENT_AT, 160, 160},
							{COL_RECEIVED_FROM, 160, 160},
							{COL_RECEIVED_AT, 160, 160},
							{COL_ID, 120, 120},
							{COL_BACK_ORIGINAL, 160, 160}, 
							{COL_BACK_TIME, 160, 160}};
/** Columns default order ********************************************************/
static int m_DefColsOrder[COL_MAX] = {
	COL_NAME, 
	COL_PRIORITY, 
	COL_COMPLETED, 
	COL_PROTECTED, 
	COL_PASSWORD,
	COL_PIN,
	COL_FAVORITES,
	COL_SR, 
	COL_GROUP, 
	COL_CREATED,
	COL_SAVED,
	COL_DELETED, 
	COL_SCHEDULE, 
	COL_TAGS, 
	COL_CONTENT, 
	COL_SENT_TO,
	COL_SENT_AT,
	COL_RECEIVED_FROM,
	COL_RECEIVED_AT,
	COL_ID,
	COL_BACK_ORIGINAL, 
	COL_BACK_TIME
};

void ShowControlPanel(void){
	//show dialog
	CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTROL), NULL, Control_DlgProc, 0);
}

static BOOL CALLBACK Tree_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg){
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, TreeView_OnMouseMove);
		case WM_KEYDOWN:
			if(wParam == VK_DELETE){
				int 	tvid = GetTVItemId(m_tSelected);
				if(tvid > GROUP_ROOT && tvid != GROUP_RECYCLE && tvid != GROUP_SEARCH_RESULTS){
					SendMessageW(m_hCtrlPanel, WM_COMMAND, MAKEWPARAM(IDM_DELETE_GROUP, 0), (LPARAM)m_hTbrTree);
				}	
				return FALSE;	
			}
			else{
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
			}
		case WM_LBUTTONDOWN:
			CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
			TreeLButtonDown(hwnd, LOWORD(lParam), HIWORD(lParam), wParam);
			return FALSE;
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static void TreeLButtonDown(HWND hwnd, int x, int y, UINT keyFlags)
{
	TVHITTESTINFO	tvh;
	int				group;

	if(!m_TreeDragReady && keyFlags == MK_LBUTTON){
		tvh.hItem = NULL;
		tvh.pt.x = x;
		tvh.pt.y = y;
		tvh.flags = TVHT_ONITEMICON | TVHT_ONITEMLABEL;
		TreeView_HitTest(m_hTreeMain, &tvh);
		group = GetTVItemId(tvh.hItem);
		if(tvh.hItem && group != GROUP_ROOT){
			m_TreeDragReady = TRUE;
		}
	}
}

static void TreeView_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	TVHITTESTINFO	tvh;
	RECT			rc;

	if(m_TreeDragReady && keyFlags == MK_LBUTTON){
		tvh.hItem = NULL;
		tvh.pt.x = x;
		tvh.pt.y = y;
		tvh.flags = TVHT_ONITEMICON | TVHT_ONITEMLABEL;
		TreeView_HitTest(m_hTreeMain, &tvh);
		if(tvh.hItem){
			ClearDragStructure();
			TreeView_GetItemRect(m_hTreeMain, tvh.hItem, &rc, TRUE);
			m_xMouse = x - rc.left;
			m_yMouse = y - rc.top;
			m_DragStruct.source = tvh.hItem;
			m_DragStruct.fStarted = TRUE;
			m_DragStruct.hCurrent = m_hTreeMain;
			m_DragStruct.idGroup = GetTVItemId(tvh.hItem);
			
			m_DragStruct.hIml = TreeView_CreateDragImage(m_hTreeMain, tvh.hItem);
			ImageList_BeginDrag(m_DragStruct.hIml, 0, 0, 0);
			ImageList_DragEnter(m_hCtrlPanel, x, y);
			SetCapture(m_hCtrlPanel);
		}
	}
}

static void ClearDragStructure(void){
	ImageList_EndDrag();
	if(m_DragStruct.hIml)
		ImageList_Destroy(m_DragStruct.hIml);
	ZeroMemory(&m_DragStruct, sizeof(m_DragStruct));
}

static BOOL IsTreeItemSuitable(void){
	HTREEITEM		hItem;
	TVITEMW		tvi;

	hItem = TreeView_GetSelection(m_hTreeMain);
	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
	if(tvi.lParam != -1)
		return TRUE;
	else
		return FALSE;
}

static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	LPNMHEADER		lpnm;
	HDITEMW			hdm = {0};
	int				group;

	switch(uMsg){
		case WM_KEYDOWN:
			if((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000 && wParam == VK_A){
				LVITEMW		lvi;
				ZeroMemory(&lvi, sizeof(lvi));
				lvi.mask = LVIF_STATE;
				lvi.state = LVIS_SELECTED;
				lvi.stateMask = LVIS_SELECTED;
				SendMessageW(hwnd, LVM_SETITEMSTATE, -1, (LPARAM)&lvi);
				return FALSE;
			}
			else if(wParam == VK_DELETE){
				group = GetTVItemId(m_tSelected);
				if(group != GROUP_RECYCLE)
					SendMessageW(m_hCtrlPanel, WM_COMMAND, MAKEWPARAM(IDM_DEL, 0), (LPARAM)m_hTbrMain);
				else
					DeleteSelectedFromBin();
				return FALSE;
			}
			else{
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			}
		case WM_NOTIFY:
			lpnm = (LPNMHEADER)lParam;
			group = GetTVItemId(m_tSelected);
			switch(lpnm->hdr.code){
				case NM_RCLICK:
					//prepare columns visibility menu
					BuildColumnsVisibilityMenu();
					m_hMenuCtrlTemp = m_hPUMenuCV;
					ShowPopUp(m_hCtrlPanel, m_hPUMenuCV);
					return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
				case HDN_DIVIDERDBLCLICK:
				case HDN_DIVIDERDBLCLICKW:
				case HDN_BEGINTRACK:
				case HDN_BEGINTRACKW:
					if(group == GROUP_ROOT){
						switch(lpnm->iItem){
							case COL_PRIORITY:
							case COL_COMPLETED:
							case COL_PROTECTED:
							case COL_PASSWORD:
							case COL_PIN:
							case COL_FAVORITES:
							case COL_SR:
							case COL_DELETED:
							case COL_BACK_ORIGINAL:
							case COL_BACK_TIME:
								return TRUE;
							case COL_NAME: 
							case COL_SCHEDULE:
							case COL_TAGS:
							case COL_CONTENT:	
							case COL_SENT_TO:
							case COL_SENT_AT:	
							case COL_RECEIVED_FROM:
							case COL_RECEIVED_AT:
							case COL_ID:
							case COL_GROUP:
							case COL_SAVED:
							case COL_CREATED:
								if(m_ColsVisibility[lpnm->iItem].width == 0)
									return TRUE;
								if(lpnm->hdr.code == HDN_DIVIDERDBLCLICKW || lpnm->hdr.code == HDN_DIVIDERDBLCLICK){
									CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
									hdm.mask = HDI_WIDTH;
									SendMessageW(lpnm->hdr.hwndFrom, HDM_GETITEMW, lpnm->iItem, (LPARAM)&hdm);
									m_ColsVisibility[lpnm->iItem].width = hdm.cxy;
									WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
									return FALSE;
								}
								else{
									return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
								}
						}
					}
					else if(group == GROUP_BACKUP){
						switch(lpnm->iItem){
							case COL_PRIORITY:
							case COL_COMPLETED:
							case COL_PROTECTED:
							case COL_PASSWORD:
							case COL_PIN:
							case COL_FAVORITES:
							case COL_SR:
							case COL_GROUP:
							case COL_SAVED:
							case COL_CREATED:
							case COL_DELETED:
							case COL_SCHEDULE:
							case COL_TAGS:
							case COL_CONTENT:
							case COL_SENT_TO:
							case COL_SENT_AT:
							case COL_RECEIVED_FROM:
							case COL_RECEIVED_AT:
							case COL_ID:
								return TRUE;
							default:
								if(m_ColsVisibility[lpnm->iItem].width == 0)
									return TRUE;
								if(lpnm->hdr.code == HDN_DIVIDERDBLCLICKW || lpnm->hdr.code == HDN_DIVIDERDBLCLICK){
									CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
									hdm.mask = HDI_WIDTH;
									SendMessageW(lpnm->hdr.hwndFrom, HDM_GETITEMW, lpnm->iItem, (LPARAM)&hdm);
									m_ColsVisibility[lpnm->iItem].width = hdm.cxy;
									WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
									return FALSE;
								}
								else{
									return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
								}
						}
					}
					else if(group == GROUP_RECYCLE){
						switch(lpnm->iItem){
							case COL_PRIORITY:
							case COL_COMPLETED:
							case COL_PROTECTED:
							case COL_PASSWORD:
							case COL_PIN:
							case COL_FAVORITES:
							case COL_SR:
							case COL_BACK_ORIGINAL:
							case COL_BACK_TIME:
								return TRUE;
							default:
								if(lpnm->iItem == COL_GROUP){
									if(group > GROUP_ROOT)
										return TRUE;	
								}
								if(m_ColsVisibility[lpnm->iItem].width == 0)
									return TRUE;
								if(lpnm->hdr.code == HDN_DIVIDERDBLCLICKW || lpnm->hdr.code == HDN_DIVIDERDBLCLICK){
									CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
									hdm.mask = HDI_WIDTH;
									SendMessageW(lpnm->hdr.hwndFrom, HDM_GETITEMW, lpnm->iItem, (LPARAM)&hdm);
									m_ColsVisibility[lpnm->iItem].width = hdm.cxy;
									WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
									return FALSE;
								}
								else{
									return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
								}
						}
					}
					else{
						switch(lpnm->iItem){
							case COL_PRIORITY:
							case COL_COMPLETED:
							case COL_PROTECTED:
							case COL_PASSWORD:
							case COL_PIN:
							case COL_FAVORITES:
							case COL_SR:
							case COL_DELETED:
							case COL_BACK_ORIGINAL:
							case COL_BACK_TIME:
								return TRUE;
							case COL_NAME: 
							case COL_SCHEDULE:
							case COL_TAGS:
							case COL_CONTENT:	
							case COL_SENT_TO:
							case COL_SENT_AT:	
							case COL_RECEIVED_FROM:
							case COL_RECEIVED_AT:
							case COL_ID:
							case COL_GROUP:
							case COL_SAVED:
							case COL_CREATED:
								if(lpnm->iItem == COL_GROUP){
									if(group > GROUP_ROOT)
										return TRUE;	
								}
								if(m_ColsVisibility[lpnm->iItem].width == 0)
									return TRUE;
								if(lpnm->hdr.code == HDN_DIVIDERDBLCLICKW || lpnm->hdr.code == HDN_DIVIDERDBLCLICK){
									CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
									hdm.mask = HDI_WIDTH;
									SendMessageW(lpnm->hdr.hwndFrom, HDM_GETITEMW, lpnm->iItem, (LPARAM)&hdm);
									m_ColsVisibility[lpnm->iItem].width = hdm.cxy;
									WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
									return FALSE;
								}
								else{
									return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
								}
						}
					}
				case HDN_ENDTRACKW:{
					if(lpnm->pitem){
						if((lpnm->pitem->mask & HDI_WIDTH) == HDI_WIDTH){
							m_ColsVisibility[lpnm->iItem].width = lpnm->pitem->cxy;
							WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
						}
					}
					return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
				}
				default:
					return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			}
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
	}
}

static void ReselectAfterCommand(void){
	int			count = ListView_GetSelectedCount(m_hListMain);

	if(count == 1){
		EnableCommands(SEL_LIST_AFTER_COMMAND_SINGLE);
	}
	else if(count > 1){
		EnableCommands(SEL_LIST_AFTER_COMMAND_MULTIPLE);
	}
}

static BOOL CALLBACK Control_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LVITEMW			lvi;
    LPNMHDR			lpnm;
	NMLISTVIEW		* nml;
	NMTREEVIEWW		* nmt;
	TOOLTIPTEXTW 	* ttp;
	wchar_t			szTooltip[128];
	int				tvid, count;
	PMEMNOTE		pNote;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_CLOSE, Control_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Control_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Control_OnInitDialog);
		HANDLE_MSG (hwnd, WM_SIZE, Control_OnSize);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Control_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, Control_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, Control_OnMouseMove);
		HANDLE_MSG (hwnd, WM_LBUTTONUP, Control_OnLButtonUp);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Control_OnInitMenuPopup);
		HANDLE_MSG (hwnd, WM_SYSCOLORCHANGE, Control_OnSysColorChange);

		case PNM_SELECT_INCOMING:
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_INCOMING);
			SendMessageW(m_hTreeMain, TVM_SELECTITEM, TVGN_CARET, (LPARAM)m_tItem);
			return TRUE;
		case PNM_CTRL_DIARY_FROM_RB:
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_ROOT);
			if(m_tItem == TreeView_GetSelection(m_hTreeMain)){
				UpdateListView();
			}
			else{
				m_tItem = NULL;
				FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_DIARY);
				if(m_tItem == TreeView_GetSelection(m_hTreeMain)){
					UpdateListView();
				}
			}
			return TRUE;
		case PNM_CTRL_DIARY_CHANGE_LANG:
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_DIARY);
			if(m_tItem == TreeView_GetSelection(m_hTreeMain)){
				UpdateDiaryNames();
			}
			BuildDiaryMenu(m_hPUMenuControl);
			return TRUE;
		case PNM_CTRL_BACK_UPDATE:
			UpdateBackupItems((wchar_t *)lParam);
			return TRUE;
		case PNM_CTRL_BACK_DELETE:
			DeleteBackupItems((wchar_t *)lParam);
			return TRUE;
		case PNM_CTRL_FAV_STATUS_CHANGED:
			if(GetSelectedTVItemId() != GROUP_FAVORITES){
				UpdateNoteItem((PMEMNOTE)lParam);
				FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_FAVORITES);
				UpdateGroupText(GROUP_FAVORITES, m_tItem);
			}
			else
				UpdateListView();
			return TRUE;
		case PNM_CTRL_ITEM_UPDATE:
			UpdateNoteItem((PMEMNOTE)lParam);
			return TRUE;
		case PNM_UPDATE_CTRL_STATUS:
			UpdateControlStatus();
			return TRUE;
		case PNM_CTRL_INS:{
			//insert only if appropriate group is currently open
			int				id = GetSelectedTVItemId();

			pNote = (PMEMNOTE)lParam;
			if(pNote->pData->idGroup == id || id == -1){
				InsertItem(m_hListMain, pNote, SendMessageW(m_hListMain, LVM_GETITEMCOUNT, 0, 0));
			}
			return TRUE;
		}
		case PNM_CTRL_UPD_LANG:
			ApplyControlPanelLanguage(hwnd, TRUE);
			return TRUE;
		case PNM_CLEAN_BIN:
			UpdateListView();	
			UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTreeMain, TreeView_GetRoot(m_hTreeMain)));
			return TRUE;
		case PNM_RELOAD:
			UpdateListView();
			return TRUE;
		case PNM_CTRL_GROUP_UPDATE:
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), wParam);
			UpdateGroupText(wParam, m_tItem);
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), lParam);
			UpdateGroupText(lParam, m_tItem);
			UpdateListView();
			return TRUE;
		case PNM_CTRL_UPD:
			ReplaceItem(m_hListMain, (PMEMNOTE)lParam);
			return TRUE;
		case PNM_CTRL_DEL:
			DeleteItem(m_hListMain, lParam);
			return TRUE;
		case WM_NOTIFY:
	    	lpnm = (LPNMHDR)lParam;
			switch(lpnm->code){
				case NM_CUSTOMDRAW:{
					//if(lpnm->hwndFrom == m_hTbrMain || lpnm->hwndFrom == m_hTbrTree){
						if(g_VSEnabled){
							LPNMTBCUSTOMDRAW	lpcd = (LPNMTBCUSTOMDRAW)lParam;
							if((lpcd->nmcd.dwDrawStage & CDDS_ITEMPREPAINT) == CDDS_ITEMPREPAINT){
								if((lpcd->nmcd.uItemState & CDIS_HOT) == CDIS_HOT){
									DrawToolbarButton(m_hTbrMain, lpcd->nmcd.hdc, &lpcd->nmcd.rc, lpcd->nmcd.uItemState, lpcd->nmcd.dwItemSpec);
									return CDRF_SKIPDEFAULT;
								}
							}
						}
						else{
							return CDRF_DODEFAULT;
						}
					//}
					//else{
						//return CDRF_DODEFAULT;
					//}
				}
				case TBN_DROPDOWN:{
					LPNMTOOLBARW	lpnmtbr = (LPNMTOOLBARW)lParam;
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(lpnmtbr->iItem, 0), 0);
					return TBDDRET_TREATPRESSED;
				}
				case TVN_SELCHANGEDW:
					SetPreviewBackgroundColor(TRUE, 0);
					SetWindowTextW(m_hEditPreview, NULL);
					nmt = (NMTREEVIEWW *)lParam;
					m_tSelected = nmt->itemNew.hItem;
					UpdateListView();
					tvid = GetTVItemId(m_tSelected);
					if(tvid > GROUP_ROOT && tvid != GROUP_DIARY){
						EnableWindow(m_hSearchBox, TRUE);
						EnableCommands(SEL_TREE_ORD);
						EnableTreeMenus(m_tSelected);
					}
					else if(tvid == GROUP_ROOT){
						EnableWindow(m_hSearchBox, TRUE);
						EnableCommands(SEL_TREE_ROOT);
						EnableTreeMenusForRoot();
					}
					else if(tvid == GROUP_RECYCLE){
						EnableWindow(m_hSearchBox, TRUE);
						EnableCommands(SEL_TREE_REC);
						DisableTreeToolbar();
					}
					else if(tvid == GROUP_SEARCH_RESULTS){
						UpdateGroupText(GROUP_SEARCH_RESULTS, m_tSelected);
						EnableWindow(m_hSearchBox, FALSE);
						EnableCommands(SEL_TREE_SEARCH_RESULTS);
						DisableTreeToolbar();
					}
					else if(tvid == GROUP_BACKUP){
						EnableWindow(m_hSearchBox, FALSE);
						EnableCommands(SEL_TREE_BACKUP);
						DisableTreeToolbar();
					}
					else if(tvid == GROUP_DIARY){
						EnableWindow(m_hSearchBox, TRUE);
						EnableCommands(SEL_TREE_REC);
						DisableTreeToolbar();
						EnableLockButtonsForDiary();
					}
					else if(tvid == GROUP_FAVORITES){
						EnableWindow(m_hSearchBox, FALSE);
						EnableCommands(SEL_TREE_REC);
						DisableTreeToolbar();
					}
					else if(tvid == GROUP_INCOMING){
						EnableWindow(m_hSearchBox, TRUE);
						EnableCommands(SEL_TREE_REC);
						DisableTreeToolbar();
					}
					return TRUE;
				case LVN_COLUMNCLICK:
					m_TreeDragReady = FALSE;
					//sort list view by columns
					nml = (NMLISTVIEW *)lParam;
					tvid = GetSelectedTVItemId();
					if(tvid != GROUP_BACKUP){
						switch(nml->iSubItem){
							case COL_NAME:
								m_lvsmRegular.column = COL_NAME;
								if(m_LVSortRegular.iSortName == LVS_SORTASCENDING)
									m_LVSortRegular.iSortName = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortName = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortName;
								if(tvid != GROUP_DIARY)
									SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								else
									SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortName, (LPARAM)CreationDateCompareFunct);
								break;
							case COL_PRIORITY:
								m_lvsmRegular.column = COL_PRIORITY;
								if(m_LVSortRegular.iSortPriority == LVS_SORTASCENDING)
									m_LVSortRegular.iSortPriority = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortPriority = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortPriority, (LPARAM)PriorityCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortPriority;
								break;
							case COL_COMPLETED:
								m_lvsmRegular.column = COL_COMPLETED;
								if(m_LVSortRegular.iSortCompleted == LVS_SORTASCENDING)
									m_LVSortRegular.iSortCompleted = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortCompleted = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortCompleted, (LPARAM)CompletedCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortCompleted;
								break;
							case COL_PROTECTED:
								m_lvsmRegular.column = COL_PROTECTED;
								if(m_LVSortRegular.iSortProtected == LVS_SORTASCENDING)
									m_LVSortRegular.iSortProtected = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortProtected = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortProtected, (LPARAM)ProtectedCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortProtected;
								break;
							case COL_PASSWORD:
								m_lvsmRegular.column = COL_PASSWORD;
								if(m_LVSortRegular.iSortPassword == LVS_SORTASCENDING)
									m_LVSortRegular.iSortPassword = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortPassword = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortPassword, (LPARAM)PasswordCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortPassword;
								break;
							case COL_PIN:
								m_lvsmRegular.column = COL_PIN;
								if(m_LVSortRegular.iSortPin == LVS_SORTASCENDING)
									m_LVSortRegular.iSortPin = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortPin = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortPin, (LPARAM)PinCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortPin;
								break;
							case COL_FAVORITES:
								m_lvsmRegular.column = COL_FAVORITES;
								if(m_LVSortRegular.iSortFavorites == LVS_SORTASCENDING)
									m_LVSortRegular.iSortFavorites = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortFavorites = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortFavorites, (LPARAM)FavoritesCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortFavorites;
								break;
							case COL_GROUP:
								m_lvsmRegular.column = COL_GROUP;
								if(m_LVSortRegular.iSortGroup == LVS_SORTASCENDING)
									m_LVSortRegular.iSortGroup = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortGroup = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortGroup;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								break;
							case COL_TAGS:
								m_lvsmRegular.column = COL_TAGS;
								if(m_LVSortRegular.iSortTags == LVS_SORTASCENDING)
									m_LVSortRegular.iSortTags = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortTags = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortTags;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								break;
							case COL_SR:
								m_lvsmRegular.column = COL_SR;
								if(m_LVSortRegular.iSortSR == LVS_SORTASCENDING)
									m_LVSortRegular.iSortSR = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortSR = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortSR;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortSR, (LPARAM)SRCompareFunc);
								break;
							case COL_CONTENT:
								m_lvsmRegular.column = COL_CONTENT;
								if(m_LVSortRegular.iSortContent == LVS_SORTASCENDING)
									m_LVSortRegular.iSortContent = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortContent = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortContent;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								break;
							case COL_SAVED:
								m_lvsmRegular.column = COL_SAVED;
								if(m_LVSortRegular.iSortSaved == LVS_SORTASCENDING)
									m_LVSortRegular.iSortSaved = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortSaved = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortSaved, (LPARAM)DateCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortSaved;
								break;
							case COL_CREATED:
								m_lvsmRegular.column = COL_CREATED;
								if(m_LVSortRegular.iSortCreated == LVS_SORTASCENDING)
									m_LVSortRegular.iSortCreated = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortCreated = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortCreated, (LPARAM)CreationDateCompareFunct);
								m_lvsmRegular.order = m_LVSortRegular.iSortCreated;
								break;
							case COL_DELETED:
								m_lvsmRegular.column = COL_DELETED;
								if(m_LVSortRegular.iSortDeleted == LVS_SORTASCENDING)
									m_LVSortRegular.iSortDeleted = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortDeleted = LVS_SORTASCENDING;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortDeleted, (LPARAM)DeletedDateCompareFunc);
								m_lvsmRegular.order = m_LVSortRegular.iSortDeleted;
								break;
							case COL_SCHEDULE:
								m_lvsmRegular.column = COL_SCHEDULE;
								if(m_LVSortRegular.iSortSchedule == LVS_SORTASCENDING)
									m_LVSortRegular.iSortSchedule = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortSchedule = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortSchedule;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								break;
							case COL_SENT_TO:
								m_lvsmRegular.column = COL_SENT_TO;
								if(m_LVSortRegular.iSortSentTo == LVS_SORTASCENDING)
									m_LVSortRegular.iSortSentTo = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortSentTo = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortSentTo;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								break;
							case COL_SENT_AT:
								m_lvsmRegular.column = COL_SENT_AT;
								if(m_LVSortRegular.iSortSentAt == LVS_SORTASCENDING)
									m_LVSortRegular.iSortSentAt = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortSentAt = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortSentAt;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortSentAt, (LPARAM)SentAtCompareFunc);
								break;
							case COL_RECEIVED_FROM:
								m_lvsmRegular.column = COL_RECEIVED_FROM;
								if(m_LVSortRegular.iSortReceivedFrom == LVS_SORTASCENDING)
									m_LVSortRegular.iSortReceivedFrom = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortReceivedFrom = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortReceivedFrom;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
								break;
							case COL_RECEIVED_AT:
								m_lvsmRegular.column = COL_RECEIVED_AT;
								if(m_LVSortRegular.iSortReceivedAt == LVS_SORTASCENDING)
									m_LVSortRegular.iSortReceivedAt = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortReceivedAt = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortReceivedAt;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortReceivedAt, (LPARAM)ReceivedAtCompareFunc);
								break;
							case COL_ID:
								m_lvsmRegular.column = COL_ID;
								if(m_LVSortRegular.iSortID == LVS_SORTASCENDING)
									m_LVSortRegular.iSortID = LVS_SORTDESCENDING;
								else
									m_LVSortRegular.iSortID = LVS_SORTASCENDING;
								m_lvsmRegular.order = m_LVSortRegular.iSortID;
								SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortID, (LPARAM)IDCompareFunc);
								break;
						}
					}
					else{
						switch(nml->iSubItem){
							case COL_NAME:
								m_lvsmBack.column = COL_NAME;
								if(m_LVSortBack.iSortName == LVS_SORTASCENDING)
									m_LVSortBack.iSortName = LVS_SORTDESCENDING;
								else
									m_LVSortBack.iSortName = LVS_SORTASCENDING;
								m_lvsmBack.order = m_LVSortBack.iSortName;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmBack, (LPARAM)NameCompareFunc);
								break;
							case COL_BACK_ORIGINAL:
								m_lvsmBack.column = COL_BACK_ORIGINAL;
								if(m_LVSortBack.iSortBackOrigin == LVS_SORTASCENDING)
									m_LVSortBack.iSortBackOrigin = LVS_SORTDESCENDING;
								else
									m_LVSortBack.iSortBackOrigin = LVS_SORTASCENDING;
								m_lvsmBack.order = m_LVSortBack.iSortBackOrigin;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmBack, (LPARAM)NameCompareFunc);
								break;
							case COL_BACK_TIME:
								m_lvsmBack.column = COL_BACK_TIME;
								if(m_LVSortBack.iSortBackDate == LVS_SORTASCENDING)
									m_LVSortBack.iSortBackDate = LVS_SORTDESCENDING;
								else
									m_LVSortBack.iSortBackDate = LVS_SORTASCENDING;
								m_lvsmBack.order = m_LVSortBack.iSortBackDate;
								SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)m_LVSortBack.iSortBackDate, (LPARAM)BackDateCompareFunc);
								break;
						}
					}
					break;
				case LVN_ITEMCHANGED:{
					int			group = 0;
					m_TreeDragReady = FALSE;
					//enable or disable toolbar buttons and menu items depending on list view items selection state
					nml = (NMLISTVIEW *)lParam;
					tvid = GetTVItemId(m_tSelected);
					count = ListView_GetSelectedCount(m_hListMain);
					if(tvid != GROUP_BACKUP){
						ZeroMemory(&lvi, sizeof(lvi));
						lvi.iItem = nml->iItem;
						lvi.mask = LVIF_PARAM;
						SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
						pNote = (PMEMNOTE)lvi.lParam;
						group = pNote->pData->idGroup;
					}
					
					if(nml->uNewState == 3 || nml->uNewState == 2){
						if(tvid == GROUP_ROOT){
							ShowNotePreview(count);
							if(count == 1){
								DefineFavoriteStatus(nml->iItem);
								EnableCommands(SEL_LIST_SELECTED_ON_ROOT_SINGLE);
							}
							else{
								EnableCommands(SEL_LIST_SELECTED_ON_ROOT_MULTIPLE);
							}
							
						}
						else if(tvid == GROUP_SEARCH_RESULTS){
							ShowNotePreview(count);
							if(count == 1){
								DefineFavoriteStatus(nml->iItem);
								EnableCommands(SEL_LIST_SELECTED_ON_ROOT_SINGLE);
							}
							else{
								EnableCommands(SEL_LIST_SELECTED_ON_ROOT_MULTIPLE);
							}
							// EnableCommands(SEL_TREE_SEARCH_RESULTS);
						}
						else if(tvid == GROUP_BACKUP){
							ShowBackPreview(count);
							if(count == 1){
								EnableCommands(SEL_LIST_BACKUP_SELECTED_SINGLE);
							}
							else{
								EnableCommands(SEL_LIST_BACKUP_SELECTED_MULTIPLE);
							}
						}
						else{
							ShowNotePreview(count);
							if(count > 1){
								//multiple items selected
								if(group > GROUP_RECYCLE || group == GROUP_INCOMING){
									if(group != GROUP_DIARY)
										EnableCommands(SEL_LIST_ORD_SELECTED_MULTIPLE);
									else
										EnableCommands(SEL_LIST_DIARY_SELECTED_MULTIPLE);
								}
								else if(group == GROUP_RECYCLE){
									EnableCommands(SEL_LIST_REC_SELECTED_MULTIPLE);
								}
							}
							else{
								//single item selected
								if(group > GROUP_RECYCLE || group == GROUP_INCOMING){
									DefineFavoriteStatus(nml->iItem);
									if(group != GROUP_DIARY)
										EnableCommands(SEL_LIST_ORD_SELECTED_SINGLE);
									else
										EnableCommands(SEL_LIST_DIARY_SELECTED_SINGLE);
								}
								else if(group == GROUP_RECYCLE){
									EnableCommands(SEL_LIST_REC_SELECTED_SINGLE);
								}
							}
						}
					}
					else{
						SetPreviewBackgroundColor(TRUE, 0);
						SetWindowTextW(m_hEditPreview, NULL);
						if(tvid == GROUP_ROOT){
							if(count == 1){
								DefineFavoriteStatus(nml->iItem);
								EnableCommands(SEL_LIST_SELECTED_ON_ROOT_SINGLE);
							}
							else if(count == 0){
								EnableCommands(SEL_LIST_DESELECTED_ON_ROOT);
							}
							else{
								EnableCommands(SEL_LIST_SELECTED_ON_ROOT_MULTIPLE);
							}
						}
						else if(tvid == GROUP_SEARCH_RESULTS){
							EnableCommands(SEL_TREE_SEARCH_RESULTS);
						}
						else if(tvid == GROUP_BACKUP){
							if(count == 0)
								EnableCommands(SEL_LIST_BACKUP_DESELECTED);
							else if(count == 1)
								EnableCommands(SEL_LIST_BACKUP_SELECTED_SINGLE);
							else
								EnableCommands(SEL_LIST_BACKUP_SELECTED_MULTIPLE);
						}
						else{
							//deselected
							if(count == 0){
								if(group > GROUP_RECYCLE || group == GROUP_INCOMING){
									EnableCommands(SEL_LIST_ORD_DESELECTED);
								}
								else if(group == GROUP_RECYCLE){
									EnableCommands(SEL_LIST_REC_DESELECTED);
								}
							}
							else if(count == 1){
								//single item selected
								if(group > GROUP_RECYCLE || group == GROUP_INCOMING){
									DefineFavoriteStatus(nml->iItem);
									if(group != GROUP_DIARY)
										EnableCommands(SEL_LIST_ORD_SELECTED_SINGLE);
									else
										EnableCommands(SEL_LIST_DIARY_SELECTED_SINGLE);
								}
								else if(group == GROUP_RECYCLE){
									EnableCommands(SEL_LIST_REC_SELECTED_SINGLE);
								}
							}
							else{
								//multiple items selected
								if(group > GROUP_RECYCLE || group == GROUP_INCOMING){
									if(group != GROUP_DIARY)
										EnableCommands(SEL_LIST_ORD_SELECTED_MULTIPLE);
									else
										EnableCommands(SEL_LIST_DIARY_SELECTED_MULTIPLE);
								}
								else if(group == GROUP_RECYCLE){
									EnableCommands(SEL_LIST_REC_SELECTED_MULTIPLE);
								}
							}
						}
					}
					return TRUE;
				}
				case NM_RCLICK:
					//show popup menu
					if(lpnm->idFrom == IDC_LVW_CONTROL){
						int	id = GetSelectedTVItemId();
						int index = -1;
						
						m_hMenuCtrlTemp = m_hPUMenuControl;
						index = SendDlgItemMessageW(hwnd, IDC_LVW_CONTROL, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
						if(index >= 0){
							LVITEMW		lvi;

							ZeroMemory(&lvi, sizeof(lvi));
							lvi.iItem = index;
							if(id != GROUP_BACKUP){
								lvi.mask = LVIF_PARAM;
								SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
								PrepareDockMenu((PMEMNOTE)lvi.lParam, m_hMenuCtrlTemp, GetMenuPosition(m_hMenuCtrlTemp, IDM_DOCK));
							}
						}
						ShowPopUp(hwnd, m_hPUMenuControl);
					}
					else if(lpnm->idFrom == IDC_CTRL_TREEVIEW){
						m_tHighlighted = TreeView_GetDropHilight(m_hTreeMain);
						//in case we are on selected item the highlighted item will be NULL
						if(m_tHighlighted == NULL)
							m_tHighlighted = m_tSelected;
						tvid = GetTVItemId(m_tHighlighted);
						if(tvid > GROUP_RECYCLE){
							EnableTreeMenus(m_tHighlighted);
							m_hMenuCtrlTemp = m_hPUMenuTree;
							ShowTreePopUp();
							EnableTreeMenus(m_tSelected);
						}
						else if(tvid == GROUP_RECYCLE){
							m_hMenuCtrlTemp = m_hPUMenuRecycle;
							DisableTreeToolbar();
							ShowRecyclePopUp();
						}
					}
					return TRUE;
				case NM_DBLCLK:{
					int				id = GetSelectedTVItemId();
					if(id != GROUP_BACKUP){
						//show note window on double click on item
						if(lpnm->idFrom == IDC_LVW_CONTROL && id != GROUP_RECYCLE){
							//do not apply to Recycle Bin
							LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;
							ShowItems();
							SendMessageW(m_hListMain, LVM_ENSUREVISIBLE, lpnmitem->iItem, FALSE);
							ShowNotePreview(ListView_GetSelectedCount(m_hListMain));
						}
					}
					else{
						if(lpnm->idFrom == IDC_LVW_CONTROL){
							SendMessageW(g_hCPDialog, WM_COMMAND, MAKEWPARAM(IDM_RESTORE_BACKUP, 0), 0);
						}
					}
					return TRUE;
				}
				case TTN_NEEDTEXTW:
					//show toolbar button tooltip
					if((lpnm->idFrom >= IDM_NEW && lpnm->idFrom < IDM_NEW_GROUP) || (lpnm->idFrom >= IDM_DUPLICATE_NOTE && lpnm->idFrom <= IDM_HOT_KEYS)){
						GetTooltip(szTooltip, m_CtrlMenus, NELEMS(m_CtrlMenus), lpnm->idFrom);
						ttp = (TOOLTIPTEXTW *)lParam;
						wcscpy(ttp->szText, szTooltip);
					}
					else if(lpnm->idFrom >= IDM_NEW_GROUP){
						GetTooltip(szTooltip, m_TreeMenu, NELEMS(m_TreeMenu), lpnm->idFrom);
						ttp = (TOOLTIPTEXTW *)lParam;
						wcscpy(ttp->szText, szTooltip);
					}
					return TRUE;
				case LVN_BEGINDRAG:{
					POINT			pt;
					RECT			rc;
					LVITEMW			lvi;
					NMLISTVIEW *plv = (NMLISTVIEW *)lParam;
					int				count, group;

					group = GetTVItemId(m_tSelected);
					if(!IsTreeItemSuitable()){
						SetCursor(m_CurNo);
					}
					count = ListView_GetSelectedCount(m_hListMain);
					ClearDragStructure();
					rc.left = LVIR_BOUNDS;
					ListView_GetItemRect(m_hListMain, plv->iItem, &rc, LVIR_BOUNDS);
					m_xMouse = plv->ptAction.x - rc.left;
					m_yMouse = plv->ptAction.y - rc.top;
					m_DragStruct.fStarted = TRUE;
					m_DragStruct.hCurrent = m_hListMain;
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.mask = LVIF_PARAM;
					lvi.iItem = plv->iItem;
					SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
					if(group != GROUP_BACKUP)
						m_DragStruct.idGroup = ((PMEMNOTE)lvi.lParam)->pData->idGroup;
					pt.x = 8;
					pt.y = 8;
					if(count == 1){
						m_DragStruct.hIml = ListView_CreateDragImage(m_hListMain, plv->iItem, &pt);
					}
					else{
						BOOL		bFirst = TRUE;
						int			pos, height = 0, firstHeight = 0, offset = 16;
						HIMAGELIST 	hOneImageList, hTempImageList;
						IMAGEINFO 	imf;

						pos = ListView_GetNextItem(m_hListMain, -1, LVNI_SELECTED);
						while(pos != -1){
							if(bFirst){
								m_DragStruct.hIml = ListView_CreateDragImage(m_hListMain, pos, &pt);
								ImageList_GetImageInfo(m_DragStruct.hIml, 0, &imf);
								height = firstHeight = imf.rcImage.bottom;
								bFirst = FALSE;
							}
							else{
								hOneImageList = ListView_CreateDragImage(m_hListMain, pos, &pt);
								hTempImageList = ImageList_Merge(m_DragStruct.hIml, 0, hOneImageList, 0, offset, height);
								ImageList_Destroy(m_DragStruct.hIml);
								ImageList_Destroy(hOneImageList);
								m_DragStruct.hIml = hTempImageList;
								ImageList_GetImageInfo(m_DragStruct.hIml, 0, &imf);
								height = imf.rcImage.bottom;
								offset += firstHeight;
								if(pos <= plv->iItem){
									m_xMouse += firstHeight;
									m_yMouse += firstHeight;
								}
							}
							pos = ListView_GetNextItem(m_hListMain, pos, LVNI_SELECTED);
						}
					}
					ImageList_BeginDrag(m_DragStruct.hIml, 0, 0, 0);
					ImageList_DragEnter(hwnd, plv->ptAction.x, plv->ptAction.y);
					SetCapture(hwnd);
					return TRUE;
				}
				default:
					if(lpnm->hwndFrom == m_hSearchBox){
						P_GSBNTF pgsbn = (P_GSBNTF)lParam;
						if(pgsbn->nmh.code == GSBNC_STRING){
							//TODO - quick search here
							BOOL	fromBin = FALSE;
							wcscpy(m_QSString, pgsbn->lpString);
							if((GetMenuState(m_hPUMenuControl, IDM_SEARCH_TARGET, MF_BYCOMMAND) & MF_CHECKED) == MF_CHECKED)
								fromBin = TRUE;
							if(m_pSearchResults){
								free(m_pSearchResults);
								m_pSearchResults = 0;
								m_iSearchCount = 0;
							}
							if(m_pGroupsArray){
								free(m_pGroupsArray);
								m_pGroupsArray = 0;
								m_iGroupsCount = 0;
							}
							m_pSearchResults = calloc(SEARCH_COUNT, sizeof(PMEMNOTE));
							int		group = GetTVItemId(m_tSelected);
							if(group == GROUP_ROOT){
								QuickSearchAllNotes(pgsbn->lpString, fromBin);
							}
							else if(group == GROUP_RECYCLE){
								QuickSearchRecycleBin(pgsbn->lpString);
							}
							else{
								CollectGroups(TreeView_GetSelection(m_hTreeMain));
								QuickSearchInGroup(pgsbn->lpString, fromBin);
							}
						}
						else if(lpnm->code == GSBNC_CLEAR){
							free(m_pSearchResults);
							m_pSearchResults = 0;
							m_iSearchCount = 0;
							free(m_pGroupsArray);
							m_pGroupsArray = 0;
							m_iGroupsCount = 0;
							*m_QSString = '\0';
							*m_QSGroup = '\0';
							UpdateListView();
							FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_SEARCH_RESULTS);
							UpdateGroupText(GROUP_SEARCH_RESULTS, m_tItem);
						}
						return TRUE;
					}
					return FALSE;
			}
		default: return FALSE;
	}
}

static void Control_OnSysColorChange(HWND hwnd)
{
	REBARBANDINFOW	band;

	//redraw all on system colors change
	g_VSEnabled = IsVisualStyleEnabled();
	CreateRebarBackground(hwnd, m_hTbrMain, &m_RebarBitmap);
	band.cbSize = sizeof(band);
	band.fMask = RBBIM_BACKGROUND | RBBIM_SIZE | RBBIM_CHILDSIZE | RBBIM_CHILD | RBBIM_STYLE;
	SendMessage(m_hRebarMain, RB_GETBANDINFO, 0, (LPARAM)&band);
	band.hbmBack = m_RebarBitmap;
	SendMessage(m_hRebarMain, RB_SETBANDINFO, 0, (LPARAM)&band);
	RedrawWindow(m_hRebarMain, NULL, NULL, RDW_INVALIDATE);
	SendMessage(m_hRebarTree, RB_GETBANDINFO, 0, (LPARAM)&band);
	band.hbmBack = m_RebarBitmap;
	SendMessage(m_hRebarTree, RB_SETBANDINFO, 0, (LPARAM)&band);
	RedrawWindow(m_hRebarTree, NULL, NULL, RDW_INVALIDATE);
}

static void Control_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	if(hMenu == m_hPUMenuControl){
		BuildContactsMenu(m_hPUMenuControl, 78, 79);
		BuildShowHideByTagsMenu(GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_TAGS)));
		if(SendMessageW(g_hMain, PNM_IS_FTP_ENABLED, 0, 0))
			EnableMenuItem(m_hPUMenuControl, IDM_SYNC_NOW, MF_BYCOMMAND | MF_ENABLED);
		else
			EnableMenuItem(m_hPUMenuControl, IDM_SYNC_NOW, MF_BYCOMMAND | MF_GRAYED);
	}
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
}

static void Control_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	LVITEMW		lvi;
	int			count, lastId, currId, targetId, itemGroup = 0, targetGroup;
	LPPNGROUP	ppg;
	HTREEITEM	hItem;
	
	if(m_DragStruct.fStarted){
		ImageList_DragShowNolock(FALSE);
		TreeView_SelectDropTarget(m_hTreeMain, NULL);
		if(m_DragStruct.target){
			if(m_DragStruct.hCurrent == m_hListMain){
				currId = GetTVItemId(m_tSelected);
				if(currId == GROUP_BACKUP){
					if(DeleteBackups()){
						SendMessageW(hwnd, PNM_RELOAD, 0, 0);
						UpdateGroupText(GROUP_BACKUP, TreeView_GetSelection(m_hTreeMain));
					}
					return;
				}
				count = ListView_GetItemCount(m_hListMain);
				targetGroup = GetTVItemId(m_DragStruct.target);
				for(int i = 0; i < count; i++){
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.iItem = i;
					lvi.mask = LVIF_PARAM | LVIF_STATE;
					lvi.stateMask = LVIS_SELECTED;
					SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
					if((lvi.state & LVIS_SELECTED) == LVIS_SELECTED){
						PMEMNOTE		pNote = (PMEMNOTE)lvi.lParam;
						NOTE_DATA		data;

						itemGroup = pNote->pData->idGroup;
						if(targetGroup > GROUP_RECYCLE){
							//dragging onto regular group
							if(itemGroup > GROUP_RECYCLE || itemGroup == GROUP_INCOMING){
								pNote->pData->idGroup = targetGroup;
								//we should save only new group, leaving other data as is
								if(pNote->pFlags->fromDB){
									if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
										data.idGroup = pNote->pData->idGroup;
										WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
									}
								}
								ChangeNoteLookByGroup(pNote, targetGroup);
							}
							else if(itemGroup == GROUP_RECYCLE){
								//restore item to target group
								RestoreFromBin(targetGroup);
							}
						}
						else if(targetGroup == GROUP_FAVORITES){
							//dragging onto favorites - add only those, which are not in favorites
							if(pNote->pRTHandles->favorite == 0){
								AddToFavorites(pNote);
							}
						}
						else if(targetGroup == GROUP_RECYCLE){
							//dragging onto recycle bin
							if(itemGroup > GROUP_RECYCLE || itemGroup == GROUP_INCOMING){
								DeleteToBin(pNote, -1);
								if(itemGroup != currId){
									FindItemById(TreeView_GetRoot(m_hTreeMain), itemGroup);
									UpdateGroupText(itemGroup, m_tItem);
								}
							}
						}
					}
				}
				if(GetTVItemId(m_tSelected) == GROUP_FAVORITES){
					UpdateGroupText(GROUP_FAVORITES, m_tSelected);
				}
				else{
					UpdateGroupText(itemGroup, m_tSelected);
					//update favorites
					if(targetGroup == GROUP_RECYCLE){
						FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_FAVORITES);
						UpdateGroupText(GROUP_FAVORITES, m_tItem);
					}
				}
				UpdateGroupText(targetGroup, m_DragStruct.target);
				TreeView_SelectItem(m_hTreeMain, m_tSelected);
				UpdateListView();
			}
			else if(m_DragStruct.hCurrent == m_hTreeMain){
				currId = GetTVItemId(m_DragStruct.source);
				targetId = GetTVItemId(m_DragStruct.target);
				if(currId > GROUP_RECYCLE){
					//LockWindowUpdate(m_hCtrlPanel);
					if(targetId > GROUP_RECYCLE){
						lastId = GetTVItemId(m_tSelected);
						ppg = PNGroupsGroupById(g_PGroups, currId);
						ppg->parent = targetId;
						SaveGroup(ppg);
						hItem = InsertTreeviewItems();
						TreeView_Expand(m_hTreeMain, hItem, TVE_EXPAND);
						//select previously selected item
						m_tItem = NULL;
						FindItemById(TreeView_GetRoot(m_hTreeMain), lastId);
						if(m_tItem){
							TreeView_SelectItem(m_hTreeMain, m_tItem);
						}
					}
					else if(targetId == GROUP_RECYCLE && currId > 0){
						SendMessageW(m_hCtrlPanel, WM_COMMAND, MAKEWPARAM(IDM_DELETE_GROUP, 0), (LPARAM)m_hTbrTree);
					}
					//LockWindowUpdate(NULL);
				}
			}
		}
		ClearDragStructure();
		m_TreeDragReady = FALSE;
		ReleaseCapture();
	}
}

static void Control_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	HTREEITEM	hItem;
	int			id, selectedGroup;
	RECT		rc;
	POINT		pt;

	if(m_DragStruct.fStarted && keyFlags == MK_LBUTTON){
		pt.x = x;
		pt.y = y;
		x += m_OffHor - m_xMouse;
		y += m_OffVert - m_yMouse;
		ImageList_DragMove(x, y);
		ImageList_DragShowNolock(FALSE);
		hItem = GetDropHighLightedItem(pt.x, pt.y);
		if(m_DragStruct.hCurrent == m_hListMain){
			if(hItem){
				id = GetTVItemId(hItem);
				selectedGroup = GetTVItemId(m_tSelected);
				if(id == GROUP_ROOT 
				|| id == m_DragStruct.idGroup 
				|| id == GROUP_SEARCH_RESULTS 
				|| id == GROUP_DIARY 
				|| id == GROUP_BACKUP
				|| id == GROUP_INCOMING
				|| selectedGroup == GROUP_SEARCH_RESULTS 
				|| (selectedGroup == GROUP_BACKUP && id != GROUP_RECYCLE)
				|| (selectedGroup == GROUP_FAVORITES && id != GROUP_RECYCLE)
				|| (selectedGroup == GROUP_ROOT && id != GROUP_RECYCLE && id != GROUP_FAVORITES)
				|| (selectedGroup == GROUP_DIARY && id != GROUP_RECYCLE)
				|| (selectedGroup == GROUP_RECYCLE && id == GROUP_FAVORITES)){
					SetCursor(m_CurNo);
					m_DragStruct.target = NULL;
				}
				else{
					SetCursor(m_CurArrow);
					TreeView_SelectDropTarget(m_hTreeMain, hItem);
					m_DragStruct.target = hItem;
				}
			}
			else{
				SetCursor(m_CurArrow);
				m_DragStruct.target = NULL;
			}
		}
		else{
			GetWindowRect(m_hListMain, &rc);
			MapWindowPoints(HWND_DESKTOP, m_hCtrlPanel, (LPPOINT)&rc, 2);
			if(PtInRect(&rc, pt)){
				SetCursor(m_CurNo);
				m_DragStruct.target = NULL;
			}
			else if(m_DragStruct.idGroup == GROUP_DIARY || m_DragStruct.idGroup == GROUP_SEARCH_RESULTS || m_DragStruct.idGroup == GROUP_INCOMING || m_DragStruct.idGroup == GROUP_BACKUP || m_DragStruct.idGroup == GROUP_RECYCLE || m_DragStruct.idGroup == GROUP_FAVORITES){
				SetCursor(m_CurNo);
				m_DragStruct.target = NULL;
			}
			else{
				if(hItem){
					id = GetTVItemId(hItem);
					if(id == m_DragStruct.idGroup 
					|| ((m_DragStruct.idGroup == GROUP_ROOT || m_DragStruct.idGroup == 0) && id == GROUP_RECYCLE) 
					|| IsItemFromChildBranch(m_DragStruct.source, hItem) 
					|| id == GROUP_SEARCH_RESULTS 
					|| id == GROUP_DIARY 
					|| id == GROUP_BACKUP 
					|| id == GROUP_INCOMING
					|| id == GROUP_FAVORITES){
						SetCursor(m_CurNo);
						m_DragStruct.target = NULL;
					}
					else{
						SetCursor(m_CurArrow);
						TreeView_SelectDropTarget(m_hTreeMain, hItem);
						m_DragStruct.target = hItem;
					}
				}
				else{
					SetCursor(m_CurArrow);
					m_DragStruct.target = NULL;
				}
			}
		}
		ImageList_DragShowNolock(TRUE);
	}
}

static void Control_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(m_hMenuCtrlTemp == m_hPUMenuControl)
		DrawMItem(lpDrawItem, m_hBmpCtrlNormal, m_hBmpCtrlGray, CLR_MASK);
	else if(m_hMenuCtrlTemp == m_hPUMenuRecycle)
		DrawMItem(lpDrawItem, m_hBmpCtrlNormal, m_hBmpCtrlGray, CLR_MASK);
	else if(m_hMenuCtrlTemp == m_hPUMenuCV)
		DrawMItem(lpDrawItem, m_hBmpCtrlNormal, m_hBmpCtrlGray, CLR_MASK);
}

static void Control_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	if(m_hMenuCtrlTemp == m_hPUMenuControl)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
	else if(m_hMenuCtrlTemp == m_hPUMenuRecycle)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
	else if(m_hMenuCtrlTemp == m_hPUMenuCV)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void Control_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	RECT		rc, rc2;
	REBARBANDINFOW	band;
	int				y = m_Up;

	if(state != SIZE_MINIMIZED){
		//size and position list view and toolbar
		GetWindowRect(m_hTbrMain, &rc);
		if(cx < m_WTreeMain){
			y *= 2;
			//y += 1;
		}
		//move and repaint rebar control
		band.cbSize = sizeof(band);
		// band.fMask = RBBIM_BACKGROUND | RBBIM_SIZE | RBBIM_CHILDSIZE | RBBIM_CHILD | RBBIM_STYLE;
		band.fMask = RBBIM_BACKGROUND | RBBIM_SIZE | RBBIM_CHILDSIZE | RBBIM_CHILD | RBBIM_STYLE | RBBS_VARIABLEHEIGHT;
		SendMessage(m_hRebarMain, RB_GETBANDINFO, 0, (LPARAM)&band);
		band.cx = cx;
		band.cyMinChild = y;
		SendMessage(m_hRebarMain, RB_SETBANDINFO, 0, (LPARAM)&band);

		MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
		// MoveWindow(m_hTbrMain, rc.left, rc.top, cx, cy, TRUE);
		MoveWindow(m_hTbrMain, rc.left, rc.top, cx, y, TRUE);
		GetWindowRect(m_hStbControl, &rc2);
		MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc2, 2);
		MoveWindow(m_hStbControl, rc2.left, cy - (rc2.bottom - rc2.top), cx, rc2.bottom - rc2.top, TRUE);

		// MoveWindow(m_hVertSplitter, 0, y, cx, (rc2.top - y), TRUE);
		SendMessageW(m_hVertSplitter, SPM_SET_NEW_TOP, y, 0);
		SendMessageW(m_hVertSplitter, SPM_MAIN_RESIZED, (WPARAM)hwnd, MAKELPARAM(cx, cy));

		for(int i = 0; i < NELEMS(m_TBBtnArray); i++){
			if(m_TBBtnArray[i].idCommand == IDM_DUMMY_ID){
				SendMessageW(m_hTbrMain, TB_GETITEMRECT, i, (LPARAM)&rc);
				MoveWindow(m_hSearchBox, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
				break;
			}
		}
		// GetWindowRect(m_hDTree, &rc);
		// MoveWindow(m_hDTree, 0, 0, rc.right - rc.left - 1, rc.bottom - rc.top, TRUE);
		// MoveWindow(m_hDTree, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		GetClientRect(m_hDTree, &rc);
		DTree_OnSize(m_hDTree, SIZE_RESTORED, rc.right - rc.left, rc.bottom - rc.top);
		// SendMessageW(m_hDTree, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
	}
}

static void Control_OnClose(HWND hwnd)
{
	WINDOWPLACEMENT		wp;
	HTREEITEM			hItem;
	TVITEMW				tvi;
	wchar_t				szId[12];

	//close all possible dialogs
	if(g_hMultiGroups)
		EndDialog(g_hMultiGroups, IDCANCEL);
	if(g_hSeveralContacts)
		EndDialog(g_hSeveralContacts, IDCANCEL);
	if(m_hBitmaps)
		EndDialog(m_hBitmaps, IDCANCEL);
	if(m_hGroups)
		EndDialog(m_hGroups, IDCANCEL);

	//clean up and quit
	if(*m_TempLib != '\0'){
		DeleteFileW(m_TempLib);
		*m_TempLib = '\0';
	}
	if(m_RebarBitmap){
		DeleteBitmap(m_RebarBitmap);
		m_RebarBitmap = NULL;
	}
	if(m_hBmpCtrlNormal){
		DeleteBitmap(m_hBmpCtrlNormal);
		m_hBmpCtrlNormal = NULL;
	}
	if(m_hBmpCtrlGray){
		DeleteBitmap(m_hBmpCtrlGray);
		m_hBmpCtrlGray = NULL;
	}
	if(m_hIconSearch){
		DestroyIcon(m_hIconSearch);
		m_hIconSearch = NULL;
	}
	if(m_hIconSearchDis){
		DestroyIcon(m_hIconSearchDis);
		m_hIconSearchDis = NULL;
	}
	if(m_hImlSmall){
		ImageList_Destroy(m_hImlSmall);
		m_hImlSmall = NULL;
	}
	if(m_hImlLarge){
		ImageList_Destroy(m_hImlLarge);
		m_hImlLarge = NULL;
	}
	if(m_hImlTbrNormal){
		ImageList_Destroy(m_hImlTbrNormal);
		m_hImlTbrNormal = NULL;
	}
	if(m_hImlTbrGray){
		ImageList_Destroy(m_hImlTbrGray);
		m_hImlTbrGray = NULL;
	}
	if(m_hImlTreeNormal){
		ImageList_Destroy(m_hImlTreeNormal);
		m_hImlTreeNormal = NULL;
	}
	if(m_hImlDragSmall){
		ImageList_Destroy(m_hImlDragSmall);
		m_hImlDragSmall = NULL;
	}
	if(m_hImlDragLarge){
		ImageList_Destroy(m_hImlDragLarge);
		m_hImlDragLarge = NULL;
	}
	if(m_pSearchResults){
		free(m_pSearchResults);
		m_pSearchResults = 0;
		m_iSearchCount = 0;
	}
	if(m_pGroupsArray){
		free(m_pGroupsArray);
		m_pGroupsArray = 0;
		m_iGroupsCount = 0;
	}
	if(m_hMenuControl){
		FreeMenus(m_hPUMenuControl);
		DestroyMenu(m_hMenuControl);
		m_hMenuControl = 0;
	}
	if(m_hMenuTree){
		FreeMenus(m_hPUMenuTree);
		DestroyMenu(m_hMenuTree);
		m_hMenuTree = 0;
	}
	if(m_hMenuRecycle){
		FreeMenus(m_hPUMenuRecycle);
		DestroyMenu(m_hMenuRecycle);
		m_hMenuRecycle = 0;
	}
	if(m_hPUMenuCV){
		FreeMenus(m_hPUMenuCV);
		DestroyMenu(m_hPUMenuCV);
		m_hPUMenuCV = 0;
	}
	//save last window position
	ZeroMemory(&wp, sizeof(wp));
	wp.length = sizeof(wp);
	GetWindowPlacement(hwnd, &wp);
	if(wp.showCmd != SW_SHOWMINIMIZED){
		WritePrivateProfileStructW(S_CP_DATA, IK_CP_POSITION, &wp, sizeof(wp), g_NotePaths.INIFile);
	}
	//save last selected group
	hItem = TreeView_GetSelection(m_hTreeMain);
	if(hItem){
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem = hItem;
		if(TreeView_GetItem(m_hTreeMain, &tvi)){
			_itow(tvi.lParam, szId, 10);
		}
		else{
			wcscpy(szId, L"-1");
		}
	}
	else{
		wcscpy(szId, L"-1");
	}
	WritePrivateProfileStringW(S_CP_DATA, IK_CP_LAST_GROUP, szId, g_NotePaths.INIFile);
	//save sort order
	WritePrivateProfileStructW(S_CP_DATA, IK_CP_SORT_DATA, &m_lvsmRegular, sizeof(m_lvsmRegular), g_NotePaths.INIFile);
	WritePrivateProfileStructW(S_CP_DATA, IK_CP_BU_SORT_DATA, &m_lvsmBack, sizeof(m_lvsmBack), g_NotePaths.INIFile);
	//save columns order
	SendMessageW(m_hListMain, LVM_GETCOLUMNORDERARRAY, COL_MAX, (LPARAM)m_ColsOrder);
	WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLS_ORDER, m_ColsOrder, sizeof(m_ColsOrder), g_NotePaths.INIFile);
	//enable next dialog appearance
	g_hCPDialog = NULL;
	EndDialog(hwnd, 0);
}

static void Control_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	int			idGroup, index, result = 0;
	BOOL		showPreview = FALSE;
	wchar_t		value[8];
		
	//the actions are clear
	switch(id){
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
					SendItemsToContact(g_Contact.name);
				}
			}
			showPreview = TRUE;
			break;
		}
		case IDM_SELECT_CONT_GROUP:{
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUPS_SEND), hwnd, Multigroups_DlgProc, (LPARAM)hwnd);
			if(result == IDOK){
				SendItemsToMultipleContacts();
			}
			showPreview = TRUE;
			break;
		}
		case IDM_SEND_TO_SEVERAL:{
			result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SEND_SEVERAL), hwnd, Multicontacts_DlgProc, (LPARAM)hwnd);
			if(result == IDOK){
				SendItemsToMultipleContacts();
			}
			showPreview = TRUE;
			break;
		}
		case IDM_LOCK_NOTE:
			ToggleLock();
			showPreview = TRUE;
			break;
		case IDM_DUPLICATE_NOTE:{
			PMEMNOTE	pNote = SelectedNote();
			if(pNote)
				DuplicateNote(pNote);
			showPreview = TRUE;
			break;
		}
		case IDM_PASSWORD:{
			HMENU			hPassword = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_PASSWORD));
			BOOL			fPassword = IsPasswordSet();
			int				count = GetMenuItemCount(hPassword);
			//enable password items
			for(int i = 0; i < count; i++){
				if((i == 0 && !fPassword) || (i > 0 && fPassword))
					EnableMenuItem(hPassword, i, MF_BYPOSITION | MF_ENABLED);
				else
					EnableMenuItem(hPassword, i, MF_BYPOSITION | MF_GRAYED);
			}
			ShowButtonDropDown(m_hPUMenuControl, IDM_PASSWORD, IDM_PASSWORD);
			showPreview = TRUE;
			break;
		}
		case IDM_DIARY:
			ShowButtonDropDown(m_hPUMenuControl, IDM_DIARY, IDM_DIARY);
			showPreview = TRUE;
			break;
		case IDM_PLACE_VISIBILITY:
			ShowButtonDropDown(m_hPUMenuControl, IDM_PLACE_VISIBILITY, IDM_PLACE_VISIBILITY);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_TARGET:
			if((GetMenuState(m_hPUMenuControl, IDM_SEARCH_TARGET, MF_BYCOMMAND) & MF_CHECKED) == MF_CHECKED){
				CheckMenuItem(m_hPUMenuControl, IDM_SEARCH_TARGET, MF_UNCHECKED | MF_BYCOMMAND);
				_itow(0, value, 10);
			}
			else{
				CheckMenuItem(m_hPUMenuControl, IDM_SEARCH_TARGET, MF_CHECKED | MF_BYCOMMAND);
				_itow(1, value, 10);
			}
			WritePrivateProfileStringW(S_CP_DATA, IK_CP_ALL_IN_SEARCH, value, g_NotePaths.INIFile);
			break;
		case IDM_PVW_COLOR_VALUE:
			if(ChoosePreviewColor()){
				_itow(m_PvwColor, value, 10);
				WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_COLOR, value, g_NotePaths.INIFile);
			}
			showPreview = TRUE;
			break;
		case IDM_PVW_USE_COLOR:
			m_PvwUseColor = !m_PvwUseColor;
			_itow(m_PvwUseColor, value, 10);
			if(m_PvwUseColor)
				CheckMenuItem(m_hPUMenuControl, IDM_PVW_USE_COLOR, MF_CHECKED | MF_BYCOMMAND);
			else
				CheckMenuItem(m_hPUMenuControl, IDM_PVW_USE_COLOR, MF_UNCHECKED | MF_BYCOMMAND);
			WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_USE_COLOR, value, g_NotePaths.INIFile);

			showPreview = TRUE;
			break;
		case IDM_V_VIEW:
			ShowButtonDropDown(m_hPUMenuControl, IDM_V_VIEW, IDM_V_VIEW);
			showPreview = TRUE;
			break;
		case IDM_BACK_SYNC:
			if(SendMessageW(g_hMain, PNM_IS_FTP_ENABLED, 0, 0))
				EnableMenuItem(m_hPUMenuControl, IDM_SYNC_NOW, MF_BYCOMMAND | MF_ENABLED);
			else
				EnableMenuItem(m_hPUMenuControl, IDM_SYNC_NOW, MF_BYCOMMAND | MF_GRAYED);
			ShowButtonDropDown(m_hPUMenuControl, IDM_BACK_SYNC, IDM_BACK_SYNC);
			showPreview = TRUE;
			break;
		case IDM_SEND_INTRANET:
			BuildContactsMenu(m_hPUMenuControl, 78, 79);
			ShowButtonDropDown(m_hPUMenuControl, IDM_SEND_INTRANET, IDM_SEND_INTRANET);
			showPreview = TRUE;
			break;
		case IDM_RUN_PROG:
			ShowButtonDropDown(m_hPUMenuControl, IDM_RUN_PROG, IDM_RUN_PROG);
			showPreview = TRUE;
			break;
		case IDM_PVW_COLOR_SET:
			ShowButtonDropDown(m_hPUMenuControl, IDM_PVW_COLOR_SET, IDM_PVW_COLOR_SET);
			showPreview = TRUE;
			break;
		case IDM_NOTE_MARKS:
			ShowButtonDropDown(m_hPUMenuControl, IDM_NOTE_MARKS, IDM_NOTE_MARKS);
			showPreview = TRUE;
			break;
		case IDM_DOCK_ALL:
			ShowButtonDropDown(m_hPUMenuControl, IDM_DOCK_ALL, IDM_DOCK_ALL);
			showPreview = TRUE;
			break;
		case IDM_CLEAR_QUICK_SEARCH:
			SendMessageW(m_hSearchBox, GSBM_CLEAR, 0, 0);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_SUBMENU:
			if(SendMessageW(m_hSearchBox, GSBM_GET_STATUS, 0, 0))
				EnableMenuItem(m_hMenuControl, IDM_CLEAR_QUICK_SEARCH, MF_BYCOMMAND | MF_ENABLED);
			else
				EnableMenuItem(m_hMenuControl, IDM_CLEAR_QUICK_SEARCH, MF_BYCOMMAND | MF_GRAYED);
			ShowButtonDropDown(m_hPUMenuControl, IDM_SEARCH_SUBMENU, IDM_SEARCH_SUBMENU);
			showPreview = TRUE;
			break;
		case IDM_VISIBILITY:
			ShowButtonDropDown(m_hPUMenuControl, IDM_VISIBILITY, IDM_VISIBILITY);
			showPreview = TRUE;
			break;
		case IDM_HOT_KEYS:
			if(!g_hHotkeys)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOT), hwnd, Hotdlg_DlgProc, 0);
			else
				BringWindowToTop(g_hHotkeys);
			showPreview = TRUE;
			break;
		case IDM_TODAY_DIARY:
		case IDM_OPTIONS:
		case IDM_BACKUP_FULL:
		case IDM_RESTORE_FULL:
		case IDM_SYNC_NOW:
		case IDM_LOCAL_SYNC:
		case IDM_LOAD_NOTE:
		case IDM_SAVE_ALL:
		case IDM_DOCK_NONE_ALL:
		case IDM_DOCK_LEFT_ALL:
		case IDM_DOCK_TOP_ALL:
		case IDM_DOCK_RIGHT_ALL:
		case IDM_DOCK_BOTTOM_ALL:
		case IDM_ALL_TO_FRONT:
		case IDM_HELP:
		case IDM_PAYPAL:
		case IDM_CREATE_PASSWORD:
		case IDM_CHANGE_PASSWORD:
		case IDM_REMOVE_PASSWORD:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(id, 0), 0);
			showPreview = TRUE;
			break;
		case IDM_V_ICONS:
			ChangeListViewView(m_hListMain, LVS_ICON);
			showPreview = TRUE;
			break;
		case IDM_V_LIST:
			ChangeListViewView(m_hListMain, LVS_LIST);
			showPreview = TRUE;
			break;
		case IDM_V_REPORT:
			ChangeListViewView(m_hListMain, LVS_REPORT);
			showPreview = TRUE;
			break;
		case IDM_V_SMALL:
			ChangeListViewView(m_hListMain, LVS_SMALLICON);
			showPreview = TRUE;
			break;
		case IDM_NEW:
			idGroup = GetTVItemId(m_tSelected);
			if(idGroup < 0)
				idGroup = 0;
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_NEW, idGroup), 0);
			showPreview = TRUE;
			break;
		case IDM_NOTE_FROM_CLIPBOARD:
			idGroup = GetTVItemId(m_tSelected);
			if(idGroup < 0)
				idGroup = 0;
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_NOTE_FROM_CLIPBOARD, idGroup), 0);
			showPreview = TRUE;
			break;
		case IDM_SHOW_ALL:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_SHOW_ALL, 0), 0);
			EnableTreeMenus(m_tSelected);
			showPreview = TRUE;
			break;
		case IDM_HIDE_ALL:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_HIDE_ALL, 0), 0);
			EnableTreeMenus(m_tSelected);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_IN_NOTES:
			if(!g_hSearchDialog)
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_FIND), NULL, g_SearchProc, 0);
			else
				BringWindowToTop(g_hSearchDialog);
			SetForegroundWindow(g_hSearchDialog);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_BY_TAGS:
			if(!g_hSearchTags)
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SEARCH_TAGS), NULL, (DLGPROC)g_SearchTagsProc, 0);
			else
				BringWindowToTop(g_hSearchTags);
			SetForegroundWindow(g_hSearchTags);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_BY_DATE:
			if(!g_hSearchDates)
				CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SEARCH_BY_DATE), NULL, (DLGPROC)SearchDates_DlgProc, 0);
			else
				BringWindowToTop(g_hSearchDates);
			SetForegroundWindow(g_hSearchDates);
			showPreview = TRUE;
			break;
		case IDM_EMPTY_BIN:
			if(EmptyBin()){
				SendMessageW(hwnd, PNM_RELOAD, 0, 0);	
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTreeMain, TreeView_GetRoot(m_hTreeMain)));
			}
			showPreview = TRUE;
			break;
		case IDM_DEL:
			if(GetSelectedTVItemId() != GROUP_BACKUP){
				if(DeleteItems()){
					SendMessageW(hwnd, PNM_RELOAD, 0, 0);
					UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTreeMain, TreeView_GetRoot(m_hTreeMain)));
				}
			}
			else{
				if(DeleteBackups()){
					SendMessageW(hwnd, PNM_RELOAD, 0, 0);
					UpdateGroupText(GROUP_BACKUP, TreeView_GetSelection(m_hTreeMain));
				}
			}
			showPreview = TRUE;
			break;
		case IDM_RESTORE_ALL_BIN:
			if(RestoreAllFromBin()){
				SendMessageW(hwnd, PNM_RELOAD, 0, 0);
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTreeMain, TreeView_GetRoot(m_hTreeMain)));
			}
			showPreview = TRUE;
			break;
		case IDM_RESTORE_NOTE:
			if(RestoreFromBin(-1)){
				SendMessageW(hwnd, PNM_RELOAD, 0, 0);
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTreeMain, TreeView_GetRoot(m_hTreeMain)));
			}
			showPreview = TRUE;
			break;
		case IDM_ROLL_UNROLL:
			RollUnrollItems();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADD_PIN:
			TogglePins();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_HIDE:
			HideItems();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SHOW:
			ShowItems();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_PRINT_NOTE:
			PrintItem();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_EMAIL:
			SendItemByEmail();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ATTACHMENT:
			SendItemAsAttachment();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_MANAGE_TAGS:{
			PMEMNOTE pNote = SelectedNote();
			if(pNote){
				if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_TAGS), hwnd, Tags_DlgProc, (LPARAM)pNote) == IDOK){
					if(pNote->pFlags->fromDB){
						SaveTags(pNote);
					}
					SendMessageW(hwnd, PNM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
				}
			}
			break;
		}
		case IDM_TAGS:
			BuildShowHideByTagsMenu(GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_TAGS)));
			ShowButtonDropDown(m_hPUMenuControl, IDM_TAGS, IDM_TAGS);
			showPreview = TRUE;
			break;
		case IDM_SAVE:
			SaveItem();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SAVE_AS:
			SaveItemAs();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SAVE_AS_TEXT:{
			PMEMNOTE	pNote = SelectedNote();
			if(pNote){
				SaveNoteAsTextFile(pNote);
			}
			showPreview = true;
			break;
		}
		case IDM_RESTORE_BACKUP:
			if(GetSelectedTVItemId() == GROUP_BACKUP){
				RestoreBackupToNote();
			}
			else{
				RestoreNoteFromBackup();
			}
			showPreview = TRUE;
			break;
		case IDM_SHOW_CENTER:
		case IDM_RESTORE_PLACEMENT:
			RestoreCenterItem(id);
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADJUST:
			if(g_hAdjust == NULL){
				EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_ENABLED);
				EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_ENABLED);
			}
			else{
				EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_GRAYED);
			}
			ShowButtonDropDown(m_hPUMenuControl, IDM_ADJUST, IDM_ADJUST);
			showPreview = TRUE;
			break;
		case IDM_ADJUST_APPEARANCE:
			AdjustItem(0);
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADJUST_SCHEDULE:
			AdjustItem(1);
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADD_TO_FAVORITES:
			AddItemsToFavorites();
			ReselectAfterCommand();
			index = SendMessageW(m_hListMain, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED);
			DefineFavoriteStatus(index);
			showPreview = TRUE;
			break;
		case IDM_HIGH_PRIORITY:
			ToggleItemsPriority();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_TOGGLE_PROTECTION:
			ToggleItemsProtection();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_MARK_AS_COMPLETED:
			ToggleItemsCompleted();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_PREVIEW:
			ShowHidePreview();
			showPreview = TRUE;
			break;
		case IDM_DOCK_LEFT:
		case IDM_DOCK_TOP:
		case IDM_DOCK_RIGHT:
		case IDM_DOCK_BOTTOM:
		case IDM_DOCK_NONE:
			SendNotesMessage(hwnd, id);
			EnableTreeMenus(m_tSelected);
			showPreview = TRUE;
			break;
		default:
			if(hwndCtl == 0 && codeNotify == 0){
				if(id > DIARY_ADDITION && id <= (DIARY_ADDITION + MAX_DIARY_PAGES + 1)){	//diary menu
					SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(id, 0), 0);
					showPreview = TRUE;
				}
				else if(id >= COLUMN_VIS_ADDITION && id < COLUMN_VIS_ADDITION + NELEMS(m_ColsVisibility)){	//columns visibility menu
					int		checked = GetMenuState(m_hPUMenuCV, id, MF_BYCOMMAND);
					if((checked & MF_CHECKED) == MF_CHECKED)
						ShowHideColumn(id - COLUMN_VIS_ADDITION, FALSE);
					else
						ShowHideColumn(id - COLUMN_VIS_ADDITION, TRUE);
				}
				else if(id >= EXTERNALS_ADDITION && id < EXTERNALS_ADDITION + EXTERNALS_MAX){
					SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(id, 0), 0);
					showPreview = TRUE;
					break;
				}
				else if(id >= SHOW_BY_TAG_ADDITION && id < SHOW_BY_TAG_ADDITION + TAGS_MAX_MENU){
					SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(id, 0), 0);
					showPreview = TRUE;
					break;
				}
				else if(id >= HIDE_BY_TAG_ADDITION && id < HIDE_BY_TAG_ADDITION + TAGS_MAX_MENU){
					SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(id, 0), 0);
					showPreview = TRUE;
					break;
				}
				else if(id >= CONTACTS_ADDITION && id < CONTACTS_ADDITION + CONTACTS_MAX){
					wchar_t			szBuffer[256];
					HMENU			hMenu = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_SEND_INTRANET));

					GetMenuStringW(hMenu, id, szBuffer, 256, MF_BYCOMMAND);
					SendItemsToContact(szBuffer);
				}
			}
			break;		
	}
	if(showPreview){
		if(GetSelectedTVItemId() != GROUP_BACKUP)
			ShowNotePreview(ListView_GetSelectedCount(m_hListMain));
		else
			ShowBackPreview(ListView_GetSelectedCount(m_hListMain));
	}
}

static BOOL Control_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT				rc;
	WINDOWPLACEMENT		wp;
	int					lastId, cx, cy;
	NMLISTVIEW			nml;
	HMODULE				hIcons;

	*m_QSString = '\0';
	*m_QSGroup = '\0';
	m_hCtrlPanel = hwnd;
	//prevent dialog from addidtional appearance
	g_hCPDialog = hwnd;
	//load columns order
	if(!GetPrivateProfileStructW(S_CP_DATA, IK_CP_COLS_ORDER, m_ColsOrder, sizeof(m_ColsOrder), g_NotePaths.INIFile)){
		memcpy(m_ColsOrder, m_DefColsOrder, sizeof(m_ColsOrder));
	}
	//get main menu
	m_hMainPopupMenu = (HMENU)SendMessageW(g_hMain, PNM_GET_MAIN_MENU, 0, 0);
	//get group images file
	GetSubPathW(m_ImagesPath, GROUP_IMAGES_FILE);
	//create image lists
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		m_hBmpCtrlNormal = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_TBR_ST));
		m_hBmpCtrlGray = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_TBR_ST_GRAY));
		if(!IsBitOn(g_NextSettings.flags1, SB3_BIG_ICONS))
			CreateImageListsW(&m_hImlTbrNormal, &m_hImlTbrGray, hIcons, IDB_TBR_ST, IDB_TBR_ST_GRAY, CLR_MASK, 16, 16, NELEMS(m_TBBtnArray));
		else
			CreateImageListsW(&m_hImlTbrNormal, &m_hImlTbrGray, hIcons, IDB_TBR_BIG, IDB_TBR_BIG_GRAY, CLR_MASK, 24, 24, NELEMS(m_TBBtnArray));
		FreeLibrary(hIcons);
	}
	ImageList_GetIconSize(m_hImlTbrNormal, &cx, &cy);

	//get needed handles
	m_hTbrMain = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_TRANSPARENT | TBSTYLE_WRAPABLE, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, g_hInstance, NULL);
	//create toolbar image lists and set toolbar
	DoToolbarW(m_hTbrMain, m_hImlTbrNormal, m_hImlTbrGray, NELEMS(m_TBBtnArray), m_TBBtnArray);
	//change buttons images
	if(!IsBitOn(g_NextSettings.flags1, SB3_BIG_ICONS)){
		ChangeButtonImageIndex(IDM_PASSWORD, 75);
		ChangeButtonImageIndex(IDM_NOTE_MARKS, 18);
	}
	else{
		ChangeButtonImageIndex(IDM_PASSWORD, 44);
		ChangeButtonImageIndex(IDM_NOTE_MARKS, 45);
	}
	m_hListMain = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, NULL, LVS_REPORT | LVS_SHOWSELALWAYS | LVS_AUTOARRANGE | LVS_SHAREIMAGELISTS | WS_VISIBLE | WS_CHILD, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, (HMENU)IDC_LVW_CONTROL, g_hInstance, NULL);
	// m_hListMain = GetDlgItem(hwnd, IDC_LVW_CONTROL);
	m_hStbControl = CreateWindowExW(0, STATUSCLASSNAMEW, NULL, SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE, 0, 0, CW_USEDEFAULT, 22, hwnd, NULL, g_hInstance, NULL);
	//get needed offsets
	m_OffVert = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	m_OffHor = GetSystemMetrics(SM_CXEDGE);
	//create rich edit
	m_hEditPreview = CreateWindowExW(WS_EX_STATICEDGE, RICHEDIT_CLASSW, NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_NOHIDESEL | WS_VSCROLL, 0, 0, 0, 0, hwnd, NULL, g_hInstance, NULL);
	//subclass preview window
	SetWindowLongPtrW(m_hEditPreview, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(m_hEditPreview, GWLP_WNDPROC, (LONG_PTR)PreviewProc));
	//add tree view dialog
	m_hDTree = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTROL_TREE), hwnd, DTree_DlgProc, 0);
	
	//add splitter
	m_Up = cy + 2 * HIWORD(SendMessageW(m_hTbrMain, TB_GETPADDING, 0, 0));
	GetWindowRect(m_hStbControl, &rc);
	m_Down = rc.bottom - rc.top;
	m_hVertSplitter = CreateMainPanel(SPLIT_VERT, g_hInstance, hwnd, g_CurV, 36, 0, m_Up, 0, m_Down, 288);
	m_hHorSplitter = CreateMainPanel(SPLIT_HORZ, g_hInstance, hwnd, g_CurH, 36, 0, 0, 0, 0, -1);
	SendMessageW(m_hHorSplitter, SPM_ADD_CHILD, (WPARAM)m_hListMain, CHILD_TOP);
	SendMessageW(m_hHorSplitter, SPM_ADD_CHILD, (WPARAM)m_hEditPreview, CHILD_BOTTOM);
	SendMessageW(m_hVertSplitter, SPM_ADD_CHILD, (WPARAM)m_hDTree, CHILD_LEFT);
	SendMessageW(m_hVertSplitter, SPM_ADD_CHILD, (WPARAM)m_hHorSplitter, CHILD_RIGHT);

	//load needed cursors
	m_CurNo = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NO), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	m_CurArrow = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	//clear drag structure
	ZeroMemory(&m_DragStruct, sizeof(m_DragStruct));
	
	SendMessageW(m_hTbrMain, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	SetWindowLongPtrW(m_hListMain, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(m_hListMain, GWLP_WNDPROC, (LONG_PTR)List_Proc));
	SetWindowLongPtrW(m_hTreeMain, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(m_hTreeMain, GWLP_WNDPROC, (LONG_PTR)Tree_Proc));

	//set icon
	SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN)));
	
	PrepareTree(hwnd);
	PrepareList(hwnd, m_hListMain);
	
	SendMessageW(m_hTbrTree, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	//create rebar back bitmaps
	CreateRebarBackground(hwnd, m_hTbrMain, &m_RebarBitmap);
	//create rebars
	m_hRebarMain = AddRebar(hwnd, m_hTbrMain);
	m_hRebarTree = AddRebar(m_hDTree, m_hTbrTree);

	//set text for controls and menus
	ApplyControlPanelLanguage(hwnd, FALSE);
	//allow subitems to have images
	// SendMessageW(m_hListMain, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
	SendMessageW(m_hListMain, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_SUBITEMIMAGES | LVS_EX_HEADERDRAGDROP , LVS_EX_SUBITEMIMAGES | LVS_EX_HEADERDRAGDROP);
	//prepare status bar
	SendMessageW(m_hStbControl, SB_SIMPLE, FALSE, 0);
	//fill list view
	UpdateListView();
	
	//get last selected item
	lastId = GetPrivateProfileIntW(S_CP_DATA, IK_CP_LAST_GROUP, -1, g_NotePaths.INIFile);
	m_tItem = NULL;
	FindItemById(TreeView_GetRoot(m_hTreeMain), lastId);
	if(m_tItem){
		TreeView_SelectItem(m_hTreeMain, m_tItem);
	}
	//sort items
	ZeroMemory(&nml, sizeof(nml));
	nml.hdr.code = LVN_COLUMNCLICK;
	nml.hdr.hwndFrom = m_hListMain;
	nml.hdr.idFrom = IDC_LVW_CONTROL;
	nml.iItem = -1;
	if(lastId != GROUP_BACKUP){
		nml.iSubItem = m_lvsmRegular.column;
		switch(m_lvsmRegular.column){
			case COL_NAME:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortName = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortName = LVS_SORTASCENDING;
				break;
			case COL_PRIORITY:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortPriority = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortPriority = LVS_SORTASCENDING;
				break;
			case COL_COMPLETED:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortCompleted = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortCompleted = LVS_SORTASCENDING;
				break;
			case COL_PROTECTED:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortProtected = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortProtected = LVS_SORTASCENDING;
				break;
			case COL_PASSWORD:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortPassword = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortPassword = LVS_SORTASCENDING;
				break;
			case COL_PIN:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortPin = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortPin = LVS_SORTASCENDING;
				break;
			case COL_FAVORITES:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortFavorites = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortFavorites = LVS_SORTASCENDING;
				break;
			case COL_GROUP:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortGroup = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortGroup = LVS_SORTASCENDING;
				break;
			case COL_SAVED:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortSaved = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortSaved = LVS_SORTASCENDING;
				break;
			case COL_CREATED:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortCreated = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortCreated = LVS_SORTASCENDING;
				break;
			case COL_DELETED:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortDeleted = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortDeleted = LVS_SORTASCENDING;
				break;
			case COL_SCHEDULE:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortSchedule = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortSchedule = LVS_SORTASCENDING;
				break;
			case COL_TAGS:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortTags = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortTags = LVS_SORTASCENDING;
				break;
			case COL_CONTENT:
				if(m_lvsmRegular.order == LVS_SORTASCENDING)
					m_LVSortRegular.iSortContent = LVS_SORTDESCENDING;
				else
					m_LVSortRegular.iSortContent = LVS_SORTASCENDING;
				break;
		}	
		SendMessageW(hwnd, WM_NOTIFY, 0, (LPARAM)&nml);
		if(g_DisableInput)
			EnableWindow(hwnd, false);
	}

	//position window
	ZeroMemory(&wp, sizeof(wp));
	wp.length = sizeof(wp);
	if(GetPrivateProfileStructW(S_CP_DATA, IK_CP_POSITION, &wp, sizeof(wp), g_NotePaths.INIFile)){
		wp.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement(hwnd, &wp);
		MoveWindow(hwnd, wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right - wp.rcNormalPosition.left + 1, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top, TRUE);
	}
	else{
		ShowWindow(hwnd, SW_SHOW);
		// MoveWindow(hwnd, 0, 0, 720, 560, TRUE);
	}
	//hide preview window
	if((GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_VISIBLE, 0, g_NotePaths.INIFile) == 0))
		SendMessageW(m_hHorSplitter, SPM_HIDE_CHILD, 0, CHILD_BOTTOM);
	else{
		CheckMenuItem(m_hPUMenuControl, IDM_PREVIEW, MF_CHECKED | MF_BYCOMMAND);
		SendMessageW(m_hTbrMain, TB_CHECKBUTTON, IDM_PREVIEW, (LPARAM)MAKELONG(TBSTATE_CHECKED, 0));
	}
	//get preview color settings
	m_PvwUseColor = (BOOL)GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_USE_COLOR, 0, g_NotePaths.INIFile);
	m_PvwColor = GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_COLOR, -1, g_NotePaths.INIFile);
	if(m_PvwUseColor)
		CheckMenuItem(m_hPUMenuControl, IDM_PVW_USE_COLOR, MF_CHECKED | MF_BYCOMMAND);
	else
		CheckMenuItem(m_hPUMenuControl, IDM_PVW_USE_COLOR, MF_UNCHECKED | MF_BYCOMMAND);
	SetPreviewBackgroundColor(TRUE, 0);
	//get search settings
	if(GetPrivateProfileIntW(S_CP_DATA, IK_CP_ALL_IN_SEARCH, 0, g_NotePaths.INIFile) != 0){
		CheckMenuItem(m_hPUMenuControl, IDM_SEARCH_TARGET, MF_CHECKED | MF_BYCOMMAND);
	}
	CheckMenuItem(m_hPUMenuControl, IDM_V_REPORT, MF_BYCOMMAND | MF_CHECKED);

	if(!m_TreeImagesExist){
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_CUSTOMIZE_ICONS, FALSE);
		EnableMenuItem(m_hPUMenuTree, IDM_CUSTOMIZE_ICONS, MF_BYCOMMAND | MF_GRAYED);
	}

	//build diary menu
	BuildDiaryMenu(m_hPUMenuControl);
	
	// SetForegroundWindow(hwnd);
	SetFocus(m_hTreeMain);
	
	return FALSE;
}

static void SetPreviewBackgroundColor(BOOL clear, COLORREF color){
	if(!clear){
		//set the color
		if(!m_PvwUseColor){
			//use note's color
			RichEdit_SetBkgndColor(m_hEditPreview, 0, color);
		}
		else{
			//use predefined color
			if(m_PvwColor != -1)
				//if the color is really defined
				RichEdit_SetBkgndColor(m_hEditPreview, 0, m_PvwColor);
			else
				//if not - use system window background color
				RichEdit_SetBkgndColor(m_hEditPreview, 1, 0);
		}
	}
	else{
		//clear edit box
		if(!m_PvwUseColor){
			//clear with system window background color
			RichEdit_SetBkgndColor(m_hEditPreview, 1, 0);
		}
		else{
			//clear with predefined color
			if(m_PvwColor != -1)
				//clear with predefined color
				RichEdit_SetBkgndColor(m_hEditPreview, 0, m_PvwColor);
			else
				//clear with system window background color
				RichEdit_SetBkgndColor(m_hEditPreview, 1, 0);
		}
	}
}

static BOOL ChoosePreviewColor(void){
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
	cc.hwndOwner = g_hCPDialog;
	if(m_PvwColor != -1)
		cc.rgbResult = m_PvwColor;
	else
		cc.rgbResult = RGB(255, 255, 255);
	cc.lCustData = (int)szBuffer;
	cc.lpfnHook = CCHookProc;
	if(ChooseColorW(&cc)){
		if(m_PvwColor != cc.rgbResult){
			m_PvwColor = cc.rgbResult;
			return TRUE;
		}
	}
	return FALSE;
}

static void ChangeListViewView(HWND hwnd, int view){
	int			style, id = IDM_V_REPORT;

	m_View = view;
	style = GetWindowLongPtrW(hwnd, GWL_STYLE);
	style &= (~LVS_TYPEMASK);
	style |= view;
	SetWindowLongPtrW(hwnd, GWL_STYLE, style);
	switch(view){
		case LVS_ICON:
			id = IDM_V_ICONS;
			break;
		case LVS_SMALLICON:
			id = IDM_V_SMALL;
			break;
		case LVS_LIST:
			id = IDM_V_LIST;
			break;
		case LVS_REPORT:
			id = IDM_V_REPORT;
			break;
	}
	for(int i = IDM_V_ICONS; i <= IDM_V_REPORT; i++){
		if(i == id)
			CheckMenuItem(m_hPUMenuControl, i, MF_BYCOMMAND | MF_CHECKED);
		else
			CheckMenuItem(m_hPUMenuControl, i, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

static void EnableCommands(int flag){
	int 			count;
	TBBUTTONINFOW	tbi;
	HMENU			hDiary = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_DIARY));
	HMENU			hTags = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_TAGS));
	HMENU			hIntranet = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_SEND_INTRANET));
	HMENU			hRun = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_RUN_PROG));
	HMENU			hPassword = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_PASSWORD));
	HMENU			hShowByTags, hHideByTags;
	BOOL			fPassword = IsPasswordSet();

	//disable all menus
	EnablemenuItems(m_hPUMenuControl, MF_GRAYED);
	//enable password items
	count = GetMenuItemCount(hPassword);
	for(int i = 0; i < count; i++){
		if((i == 0 && !fPassword) || (i > 0 && fPassword)){
			EnableMenuItem(hPassword, i, MF_BYPOSITION | MF_ENABLED);
		}
	}
	//enable all diary items
	count = GetMenuItemCount(hDiary);
	for(int i = 0; i < count; i++){
		EnableMenuItem(hDiary, i, MF_BYPOSITION | MF_ENABLED);
	}
	//enable all tags items
	hShowByTags = GetSubMenu(hTags, GetMenuPosition(hTags, IDM_SHOW_BY_TAG));
	hHideByTags = GetSubMenu(hTags, GetMenuPosition(hTags, IDM_HIDE_BY_TAG));
	count = GetMenuItemCount(hShowByTags);
	for(int i = 0; i < count; i++){
		EnableMenuItem(hShowByTags, i, MF_BYPOSITION | MF_ENABLED);
		EnableMenuItem(hHideByTags, i, MF_BYPOSITION | MF_ENABLED);
	}
	//enable all run items
	count = GetMenuItemCount(hRun);
	for(int i = 0; i < count; i++){
		EnableMenuItem(hRun, i, MF_BYPOSITION | MF_ENABLED);
	}
	//enable all contacts items
	count = GetMenuItemCount(hIntranet);
	for(int i = 0; i < count; i++){
		EnableMenuItem(hIntranet, i, MF_BYPOSITION | MF_ENABLED);
	}
	//disable all toolbar buttons
	count = ToolBar_ButtonCount(m_hTbrMain);
	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_BYINDEX | TBIF_COMMAND | TBIF_STYLE;
	for(int i = 0; i < count; i++){
		SendMessageW(m_hTbrMain, TB_GETBUTTONINFOW, i, (LPARAM)&tbi);
		SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, tbi.idCommand, FALSE);
	}
	switch(flag){
		case SEL_TREE_SEARCH_RESULTS:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_TREE_ROOT:
		case SEL_LIST_DESELECTED_ON_ROOT:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			break;
		case SEL_TREE_ORD:
		case SEL_LIST_ORD_DESELECTED:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			break;
		case SEL_TREE_REC:
		case SEL_LIST_REC_DESELECTED:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_EMPTY_BIN, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_EMPTY_BIN, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_DIARY_SELECTED_SINGLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ADJUST, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ADJUST, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_CENTER, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_CENTER, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_PLACEMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_EMAIL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_EMAIL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SEND_INTRANET, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_ENABLED);
			}
			if(!g_hTags){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_MANAGE_TAGS, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_MANAGE_TAGS, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PRINT_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_PRINT_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(m_hPUMenuControl, IDM_LOCK_NOTE, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_ORD_SELECTED_SINGLE:
		case SEL_LIST_AFTER_COMMAND_SINGLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DUPLICATE_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DUPLICATE_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_AS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_AS, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_AS_TEXT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_AS_TEXT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ADJUST, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ADJUST, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_CENTER, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_CENTER, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_PLACEMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_EMAIL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_EMAIL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SEND_INTRANET, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_ENABLED);
			}
			if(!g_hTags){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_MANAGE_TAGS, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_MANAGE_TAGS, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ADD_TO_FAVORITES, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PRINT_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_PRINT_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			if(NotesDirExists(g_NotePaths.BackupDir) && IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_BACKUP, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_BACKUP, MF_BYCOMMAND | MF_ENABLED);
			}
			EnableMenuItem(m_hPUMenuControl, IDM_LOCK_NOTE, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_DIARY_SELECTED_MULTIPLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_ORD_SELECTED_MULTIPLE:
		case SEL_LIST_AFTER_COMMAND_MULTIPLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ADD_TO_FAVORITES, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SEND_INTRANET, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_ENABLED);
			}
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			break;
		// case SEL_LIST_ORD_DESELECTED:

			// break;
		case SEL_LIST_REC_SELECTED_SINGLE:
		case SEL_LIST_REC_SELECTED_MULTIPLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_EMPTY_BIN, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_EMPTY_BIN, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_NOTE, MF_BYCOMMAND | MF_ENABLED);
			break;
		// case SEL_LIST_REC_DESELECTED:

			// break;
		case SEL_LIST_SELECTED_ON_ROOT_SINGLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DUPLICATE_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DUPLICATE_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_AS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_AS, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_AS_TEXT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_AS_TEXT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ADJUST, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ADJUST, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_CENTER, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_CENTER, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_PLACEMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_EMAIL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_EMAIL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SEND_INTRANET, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ADD_TO_FAVORITES, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PRINT_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_PRINT_NOTE, MF_BYCOMMAND | MF_ENABLED);
			if(!g_hTags){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_MANAGE_TAGS, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_MANAGE_TAGS, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			if(NotesDirExists(g_NotePaths.BackupDir) && IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_BACKUP, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_BACKUP, MF_BYCOMMAND | MF_ENABLED);
			}
			EnableMenuItem(m_hPUMenuControl, IDM_LOCK_NOTE, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_SELECTED_ON_ROOT_MULTIPLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);;
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SEND_INTRANET, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_ENABLED);
			}
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			break;
		case SEL_LIST_BACKUP_SELECTED_SINGLE:
			if(NotesDirExists(g_NotePaths.BackupDir) && IsBitOn(g_NoteSettings.reserved1, SB1_USE_BACKUP)){
				SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RESTORE_BACKUP, TRUE);
				EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_BACKUP, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_BACKUP_SELECTED_MULTIPLE:
			SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_BACKUP_DESELECTED:

			break;
		case SEL_TREE_BACKUP:
			
			break;
		case SEL_LIST_FAV_SELECTED_SINGLE:

			break;
		case SEL_LIST_FAV_SELECTED_MULTIPLE:

			break;
		case SEL_LIST_FAV_DESELECTED:

			break;
	}
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PLACE_VISIBILITY, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_PLACE_VISIBILITY, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_V_VIEW, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_V_VIEW, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_V_ICONS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_V_LIST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_V_REPORT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_V_SMALL, MF_BYCOMMAND | MF_ENABLED);

	EnableMenuItem(m_hPUMenuControl, IDM_DOCK_NONE_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_DOCK_LEFT_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_DOCK_TOP_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_DOCK_RIGHT_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_DOCK_BOTTOM_ALL, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_OPTIONS, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_OPTIONS, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HOT_KEYS, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_HOT_KEYS, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_SEARCH_SUBMENU, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_SEARCH_SUBMENU, MF_BYCOMMAND | MF_ENABLED);

	if(flag != SEL_TREE_REC && flag != SEL_LIST_REC_SELECTED_SINGLE && flag != SEL_LIST_REC_SELECTED_MULTIPLE && flag != SEL_LIST_REC_DESELECTED){
		SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_VISIBILITY, TRUE);
		EnableMenuItem(m_hPUMenuControl, IDM_VISIBILITY, MF_BYCOMMAND | MF_ENABLED);
	}
	else{
		SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_VISIBILITY, FALSE);
		EnableMenuItem(m_hPUMenuControl, IDM_VISIBILITY, MF_BYCOMMAND | MF_GRAYED);
	}

	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_HELP, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_HELP, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_ALL_TO_FRONT, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_ALL_TO_FRONT, MF_BYCOMMAND | MF_ENABLED);

	EnableMenuItem(m_hPUMenuControl, IDM_SEARCH_IN_NOTES, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_SEARCH_BY_TAGS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_SEARCH_TARGET, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_CLEAR_QUICK_SEARCH, MF_BYCOMMAND | MF_ENABLED);

	if(g_hAdjust){
		EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPUMenuControl, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_ENABLED);
	}
	//always enable preview background items
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PREVIEW, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_PREVIEW, MF_BYCOMMAND | MF_ENABLED);
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PVW_COLOR_SET, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_PVW_COLOR_SET, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_PVW_USE_COLOR, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_PVW_COLOR_VALUE, MF_BYCOMMAND | MF_ENABLED);
	//always enable marks items
	EnableMenuItem(m_hPUMenuControl, IDM_HIGH_PRIORITY, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_TOGGLE_PROTECTION, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_MARK_AS_COMPLETED, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_ROLL_UNROLL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_ADD_PIN, MF_BYCOMMAND | MF_ENABLED);
	//always enable paypal button
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PAYPAL, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_PAYPAL, MF_BYCOMMAND | MF_ENABLED);
	//always enable diary menu
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_DIARY, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_DIARY, MF_BYCOMMAND | MF_ENABLED);
	//always enable protection menu
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_BACK_SYNC, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_BACK_SYNC, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_BACKUP_FULL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_FULL, MF_BYCOMMAND | MF_ENABLED);
	// if(SendMessageW(g_hMain, PNM_IS_FTP_ENABLED, 0, 0))
		// EnableMenuItem(m_hPUMenuControl, IDM_SYNC_NOW, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_LOCAL_SYNC, MF_BYCOMMAND | MF_ENABLED);
	//always enable external programs item
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_RUN_PROG, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_RUN_PROG, MF_BYCOMMAND | MF_ENABLED);
	//always enable show/hide by tags items
	EnableMenuItem(m_hPUMenuControl, IDM_SHOW_BY_TAG, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuControl, IDM_HIDE_BY_TAG, MF_BYCOMMAND | MF_ENABLED);
	//always enable tags item
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_TAGS, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_TAGS, MF_BYCOMMAND | MF_ENABLED);
	//always enable password item
	SendMessageW(m_hTbrMain, TB_ENABLEBUTTON, IDM_PASSWORD, TRUE);
	EnableMenuItem(m_hPUMenuControl, IDM_PASSWORD, MF_BYCOMMAND | MF_ENABLED);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: EnableMenus
 Created  : Fri May 25 22:29:15 2007
 Modified : Fri May 25 22:29:25 2007

 Synopsys : Enables or disables menu items
 Input    : fEnabled - enable flag
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void EnableMenus(UINT fEnabled, BOOL bMultiSelect){
	EnableMenuItem(m_hPUMenuControl, IDM_SAVE, MF_BYCOMMAND | fEnabled);
	EnableMenuItem(m_hPUMenuControl, IDM_SAVE_AS, MF_BYCOMMAND | fEnabled);
	EnableMenuItem(m_hPUMenuControl, IDM_SAVE_AS_TEXT, MF_BYCOMMAND | fEnabled);
	if(!bMultiSelect){
		EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | fEnabled);
		EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | fEnabled);
		EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | fEnabled);
		EnableMenuItem(m_hPUMenuControl, IDM_DOCK, MF_BYCOMMAND | fEnabled);
	}
	else{
		EnableMenuItem(m_hPUMenuControl, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPUMenuControl, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPUMenuControl, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPUMenuControl, IDM_DOCK, MF_BYCOMMAND | MF_GRAYED);
	}
	EnableMenuItem(m_hPUMenuControl, IDM_SHOW_CENTER, MF_BYCOMMAND | fEnabled);

	if(fEnabled == MF_ENABLED){
		if(g_RTHandles.hbSkin)
			EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_GRAYED);
		else
			EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
	}
	else
		EnableMenuItem(m_hPUMenuControl, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_GRAYED);

	EnableMenuItem(m_hPUMenuControl, IDM_EMAIL, MF_BYCOMMAND | fEnabled);
	EnableMenuItem(m_hPUMenuControl, IDM_ATTACHMENT, MF_BYCOMMAND | fEnabled);
	if(IsBitOn(g_NextSettings.flags1, SB3_ENABLE_NETWORK)){
		EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | fEnabled);
	}
	else{
		EnableMenuItem(m_hPUMenuControl, IDM_SEND_INTRANET, MF_BYCOMMAND | MF_GRAYED);
	}
	EnableMenuItem(m_hPUMenuControl, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | fEnabled);

	if(!g_hAdjust){
		EnableMenuItem(m_hPUMenuControl, IDM_ADJUST, MF_BYCOMMAND | fEnabled);
	}
	else{
		EnableMenuItem(m_hPUMenuControl, IDM_ADJUST, MF_BYCOMMAND | MF_GRAYED);
	}
}

static void DisableTreeToolbar(void){
	TBBUTTONINFOW	tbi;
	int				count;

	count = ToolBar_ButtonCount(m_hTbrTree);
	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_BYINDEX | TBIF_COMMAND | TBIF_STYLE;
	for(int i = 0; i < count; i++){
		SendMessageW(m_hTbrTree, TB_GETBUTTONINFOW, i, (LPARAM)&tbi);
		if(tbi.idCommand != IDM_CUSTOMIZE_ICONS)
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, tbi.idCommand, FALSE);
		else{
			if(!m_TreeImagesExist){
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, tbi.idCommand, FALSE);
			}
		}
	}
}

static void EnableLockButtonsForDiary(void){
	if(IsDiaryLocked()){
		EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_ENABLED);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, false);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, true);
	}
	else{
		EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, true);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, false);
	}
}

static void EnableTreeMenusForRoot(void){
	EnableMenuItem(m_hPUMenuTree, IDM_NEW_GROUP, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hPUMenuTree, IDM_NEW_SUBGROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_EDIT_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_SHOW_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_HIDE_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_GROUP, TRUE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_SUBGROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_EDIT_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_INCLUDE_SG, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_INCLUDE_SG, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, FALSE);
}

static void EnableTreeMenus(HTREEITEM hItem){
	LPPNGROUP		ppg;
	BOOL			bEnabled, bMenu = FALSE;
	CPENUM			cp;
	TVITEMW			tvi;
	int				id = 0, idRoot, idCurrent, idSelected;

	if(hItem == m_tHighlighted)
		bMenu = TRUE;

	idRoot = GetTVItemId(TreeView_GetRoot(m_hTreeMain));
	idSelected = GetTVItemId(m_tSelected);
	idCurrent = GetTVItemId(hItem);
	if(!hItem){
		bEnabled = FALSE;
	}
	else{
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.hItem = hItem;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
		id = tvi.lParam;
		if(id >= 0 && id != GROUP_DIARY)
			bEnabled = TRUE;
		else
			bEnabled = FALSE;
	}
	
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_GROUP, TRUE);
	EnableMenuItem(m_hPUMenuTree, IDM_NEW_GROUP, MF_BYCOMMAND | MF_ENABLED);

	if(bEnabled){
		if(bMenu){
			EnableMenuItem(m_hPUMenuTree, IDM_NEW_SUBGROUP, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(m_hPUMenuTree, IDM_EDIT_GROUP, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(m_hPUMenuTree, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_ENABLED);
		}
		if(idSelected != idRoot){
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_SUBGROUP, TRUE);
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_EDIT_GROUP, TRUE);
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, TRUE);
		}
		ppg = PNGroupsGroupById(g_PGroups, id);
		if(ppg){
			cp.group = ppg->id;
			cp.result = 0;
			CheckGroupVisibility(&cp);
			if((cp.result & BOTH_VH) == BOTH_VH){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_ENABLED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, TRUE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, TRUE);
				}
			}
			else if((cp.result & VISIBLE_ONLY) == VISIBLE_ONLY){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_ENABLED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, FALSE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, TRUE);
				}
			}
			else if((cp.result & HIDDEN_ONLY) == HIDDEN_ONLY){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, TRUE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, FALSE);
				}
			}
			else if(cp.result == 0){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, FALSE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, FALSE);
				}
			}
			if(!*ppg->szLock){
				EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_ENABLED);
				EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, TRUE);
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, FALSE);
			}
			else{
				EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_ENABLED);
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, FALSE);
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, TRUE);
			}
			m_BranchVisibility = 0;
			CheckBranchVisibility(hItem, FALSE);
			if((m_BranchVisibility & BOTH_VH) == BOTH_VH){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_INCLUDE_SG, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_INCLUDE_SG, MF_BYCOMMAND | MF_ENABLED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_INCLUDE_SG, TRUE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_INCLUDE_SG, TRUE);
				}
			}
			else if((m_BranchVisibility & VISIBLE_ONLY) == VISIBLE_ONLY){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_INCLUDE_SG, MF_BYCOMMAND | MF_ENABLED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_INCLUDE_SG, FALSE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_INCLUDE_SG, TRUE);
				}
			}
			else if ((m_BranchVisibility & HIDDEN_ONLY) == HIDDEN_ONLY){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_INCLUDE_SG, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_INCLUDE_SG, TRUE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_INCLUDE_SG, FALSE);
				}
			}
			else if(m_BranchVisibility == 0){
				if(bMenu){
					EnableMenuItem(m_hPUMenuTree, IDM_SHOW_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hPUMenuTree, IDM_HIDE_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_INCLUDE_SG, FALSE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_INCLUDE_SG, FALSE);
				}
			}
		}
		
		//always disable delete for "General" - id = 0
		if(idCurrent == 0){
		// if(idHighlighted == 0){
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, FALSE);
			EnableMenuItem(m_hPUMenuTree, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else{
		if(bMenu){
			EnableMenuItem(m_hPUMenuTree, IDM_NEW_SUBGROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hPUMenuTree, IDM_EDIT_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hPUMenuTree, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hPUMenuTree, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hPUMenuTree, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hPUMenuTree, IDM_SHOW_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hPUMenuTree, IDM_HIDE_INCLUDE_SG, MF_BYCOMMAND | MF_GRAYED);
			if(id != GROUP_DIARY){
				EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
			}
			else{
				if(IsDiaryLocked()){
					EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_ENABLED);
				}
				else{
					EnableMenuItem(m_hPUMenuTree, IDM_LOCK_GROUP, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hPUMenuTree, IDM_UNLOCK_GROUP, MF_BYCOMMAND | MF_GRAYED);
				}
			}
		}
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_SUBGROUP, FALSE);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_EDIT_GROUP, FALSE);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, FALSE);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, bEnabled);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, bEnabled);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_INCLUDE_SG, bEnabled);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_INCLUDE_SG, bEnabled);
		if(id != GROUP_DIARY){
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, bEnabled);
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, bEnabled);
		}
		else{
			if(IsDiaryLocked()){
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, false);
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, true);
			}
			else{
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_LOCK_GROUP, true);
				SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_UNLOCK_GROUP, false);
			}
		}
	}
	if(m_TreeImagesExist){
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_CUSTOMIZE_ICONS, TRUE);
		EnableMenuItem(m_hPUMenuTree, IDM_CUSTOMIZE_ICONS, MF_BYCOMMAND | MF_ENABLED);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SendNotesMessage
 Created  : Fri May 25 22:26:00 2007
 Modified : Fri May 25 22:26:00 2007

 Synopsys : Sends WM_COMMAND message for each selected list view item
 Input    : hwnd - dialog handle
            id - message id
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void SendNotesMessage(HWND hwnd, int id){

	LVITEMW		lvi;
	int			count = 0;
	LONG_PTR	* pNotes, * pTemp, * pFree;
	PMEMNOTE	pNote;

	//get selected items count
	count = SendMessageW(m_hListMain, LVM_GETSELECTEDCOUNT, 0, 0);
	//prepare lParams (PMEMNOTE) array - some items become unselected during message exchange, 
	//so we cannot send message from within this loop
	if(count > 0){
		pNotes = calloc(count, sizeof(PMEMNOTE));
		pFree = pTemp = pNotes;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = -1;
		while((lvi.iItem = SendMessageW(m_hListMain, LVM_GETNEXTITEM, lvi.iItem, LVNI_ALL | LVNI_SELECTED)) != -1){
			SendMessageW(m_hListMain, LVM_GETITEM, 0, (LPARAM)&lvi);
			*pNotes++ = lvi.lParam;
		}
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pTemp++;
			SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(id, 0), 0);
		}
		free(pFree);
	}
}

static LONG_PTR * SelectedPItems(INT_PTR * pCount){
	LVITEMW		lvi;
	LONG_PTR	* pNotes, * pTemp = NULL;

	//get selected items count
	*pCount = SendMessageW(m_hListMain, LVM_GETSELECTEDCOUNT, 0, 0);
	//prepare lParams (PMEMNOTE) array - some items become unselected during message exchange, 
	//so we cannot send message from within this loop
	if(*pCount > 0){
		pNotes = calloc(*pCount, sizeof(PMEMNOTE));
		pTemp = pNotes;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = -1;
		while((lvi.iItem = SendMessageW(m_hListMain, LVM_GETNEXTITEM, lvi.iItem, LVNI_ALL | LVNI_SELECTED)) != -1){
			SendMessageW(m_hListMain, LVM_GETITEM, 0, (LPARAM)&lvi);
			*pNotes++ = lvi.lParam;
		}
	}
	return pTemp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PrepareList
 Created  : Fri May 25 22:24:27 2007
 Modified : Fri May 25 22:24:27 2007

 Synopsys : Prepares and sets list view columns, image lists, toolbar image 
            lists and builds toolbar
 Input    : hDlg - dialog handle
            hList - list view handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void PrepareList(HWND hDlg, HWND hList){
	
	HBITMAP			hBmp;
	LVCOLUMNW		lvc;
	// int				widthRegular[] = {0, 0, 0, 0, 0};
	// int				widthBackup[] = {0, 0};
	wchar_t			szSearchPrompt[256];
	TBBUTTONINFOW	tbi;
	RECT			rc;
	HMODULE			hIcons;

	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		//prepare and set list view image lists
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_CTRL_SMALL));
		m_hImlSmall = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 6, 6);
		ImageList_AddMasked(m_hImlSmall, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		//set list view small image list
		ListView_SetImageList(hList, m_hImlSmall, LVSIL_SMALL);
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_CTRL_LARGE));
		m_hImlLarge = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 6, 6);
		ImageList_AddMasked(m_hImlLarge, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		m_hIconSearch = LoadIcon(hIcons, MAKEINTRESOURCE(IDR_ICO_SEARCH));
		m_hIconSearchDis = LoadIcon(hIcons, MAKEINTRESOURCE(IDR_ICO_SEARCH_DIS));
		FreeLibrary(hIcons);
	}
	//set listview normal image list
	ListView_SetImageList(hList, m_hImlLarge, LVSIL_NORMAL);
	SendMessageW(hDlg, PNM_CTRL_UPD, 0, 0);
	// //create toolbar image lists and set toolbar
	// DoToolbarW(m_hTbrMain, m_hImlTbrNormal, m_hImlTbrGray, NELEMS(m_TBBtnArray), m_TBBtnArray);
	//prepare search box
	GetPrivateProfileStringW(S_CAPTIONS, IK_SEARCH_PROMPT, L"Quick search", szSearchPrompt, 256, g_NotePaths.CurrLanguagePath);
	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_SIZE;
	tbi.cx = 148;
	SendMessageW(m_hTbrMain, TB_SETBUTTONINFOW, IDM_DUMMY_ID, (LPARAM)&tbi);
	SendMessageW(m_hTbrMain, TB_GETRECT, IDM_DUMMY_ID, (LPARAM)&rc);
	m_hSearchBox = CreateGSearchBoxWindow(g_hInstance, m_hTbrMain, m_hIconSearch, m_hIconSearchDis, szSearchPrompt, rc.left + 2, rc.top + 1, rc.right - rc.left - 4, rc.bottom - rc.top);

	//get width of toolbar
	m_WTreeMain = 0;
	for(int i = 0; i < NELEMS(m_TBBtnArray); i++){
		SendMessageW(m_hTbrMain, TB_GETITEMRECT, i, (LPARAM)&rc);
		m_WTreeMain += (rc.right - rc.left);
	}

	//get saved sort data
	int		*plvsr = (int *)&m_LVSortRegular, *plvsb = (int *)&m_LVSortBack;
	GetPrivateProfileStructW(S_CP_DATA, IK_CP_SORT_DATA, &m_lvsmRegular, sizeof(m_lvsmRegular), g_NotePaths.INIFile);
	*(plvsr + m_lvsmRegular.column) = m_lvsmRegular.order;
	GetPrivateProfileStructW(S_CP_DATA, IK_CP_BU_SORT_DATA, &m_lvsmBack, sizeof(m_lvsmBack), g_NotePaths.INIFile);
	*(plvsb + m_lvsmBack.column) = m_lvsmBack.order;
	//get columns width
	GetPrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
	// GetPrivateProfileStructW(S_CP_DATA, IK_CP_COL_WIDTH, widthRegular, sizeof(widthRegular), g_NotePaths.INIFile);
	// GetPrivateProfileStructW(S_CP_DATA, IK_CP_BU_COL_WIDTH, widthBackup, sizeof(widthBackup), g_NotePaths.INIFile);
	//insert list view columns
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.cx = m_ColsVisibility[COL_NAME].width;//(widthRegular[0] == 0) ? 160 : widthRegular[0];
	lvc.iSubItem = COL_NAME;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_NAME, (LPARAM)&lvc);
	//priority
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_IMAGE;
	lvc.fmt = LVCFMT_CENTER | LVCFMT_IMAGE | LVCFMT_COL_HAS_IMAGES;
	lvc.cx = m_ColsVisibility[COL_PRIORITY].width;
	lvc.iSubItem = COL_PRIORITY;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_PRIORITY, (LPARAM)&lvc);
	//completed
	lvc.iSubItem = COL_COMPLETED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	//protected
	lvc.iSubItem = COL_PROTECTED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_PROTECTED, (LPARAM)&lvc);
	//password
	lvc.iSubItem = COL_PASSWORD;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_PASSWORD, (LPARAM)&lvc);
	//pin
	lvc.iSubItem = COL_PIN;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_PIN, (LPARAM)&lvc);
	//favorites
	lvc.iSubItem = COL_FAVORITES;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_FAVORITES, (LPARAM)&lvc);
	//sent/received
	lvc.iSubItem = COL_SR;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_SR, (LPARAM)&lvc);
	//group
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = m_ColsVisibility[COL_GROUP].width;//(widthRegular[1] == 0) ? 120 : widthRegular[1];
	lvc.iSubItem = COL_GROUP;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_GROUP, (LPARAM)&lvc);
	//created
	lvc.cx = m_ColsVisibility[COL_CREATED].width;//(widthRegular[2] == 0) ? 160 : widthRegular[2];
	lvc.iSubItem = COL_CREATED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_CREATED, (LPARAM)&lvc);
	//saved
	lvc.cx = m_ColsVisibility[COL_SAVED].width;//(widthRegular[2] == 0) ? 160 : widthRegular[2];
	lvc.iSubItem = COL_SAVED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_SAVED, (LPARAM)&lvc);
	//deleted
	lvc.cx = m_ColsVisibility[COL_DELETED].width;//(widthRegular[2] == 0) ? 160 : widthRegular[2];
	lvc.iSubItem = COL_DELETED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_DELETED, (LPARAM)&lvc);
	//schedule
	lvc.cx = m_ColsVisibility[COL_SCHEDULE].width;//(widthRegular[3] == 0) ? 160 : widthRegular[3];
	lvc.iSubItem = COL_SCHEDULE;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_SCHEDULE, (LPARAM)&lvc);
	// m_ColsWidth[COL_SCHEDULE] = lvc.cx;
	//tags
	lvc.cx = m_ColsVisibility[COL_TAGS].width;//(widthRegular[4] == 0) ? 160 : widthRegular[4];
	lvc.iSubItem = COL_TAGS;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_TAGS, (LPARAM)&lvc);
	//content
	lvc.cx = m_ColsVisibility[COL_CONTENT].width;
	lvc.iSubItem = COL_CONTENT;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_CONTENT, (LPARAM)&lvc);
	//sent to:
	lvc.cx = m_ColsVisibility[COL_SENT_TO].width;
	lvc.iSubItem = COL_SENT_TO;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_SENT_TO, (LPARAM)&lvc);
	//sent at:
	lvc.cx = m_ColsVisibility[COL_SENT_AT].width;
	lvc.iSubItem = COL_SENT_AT;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_SENT_AT, (LPARAM)&lvc);
	//received from:
	lvc.cx = m_ColsVisibility[COL_RECEIVED_FROM].width;
	lvc.iSubItem = COL_RECEIVED_FROM;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_RECEIVED_FROM, (LPARAM)&lvc);
	//received at:
	lvc.cx = m_ColsVisibility[COL_RECEIVED_AT].width;
	lvc.iSubItem = COL_RECEIVED_AT;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_RECEIVED_AT, (LPARAM)&lvc);
	//id
	lvc.cx = m_ColsVisibility[COL_ID].width;
	lvc.iSubItem = COL_ID;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_ID, (LPARAM)&lvc);
	//backup origin
	lvc.cx = m_ColsVisibility[COL_BACK_ORIGINAL].width;//(widthBackup[0] == 0) ? 160 : widthBackup[0];
	lvc.iSubItem = COL_BACK_ORIGINAL;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_BACK_ORIGINAL, (LPARAM)&lvc);
	// m_ColsWidth[COL_BACK_ORIGINAL] = lvc.cx;
	//backup time
	lvc.cx = m_ColsVisibility[COL_BACK_TIME].width;//(widthBackup[1] == 0) ? 160 : widthBackup[1];
	lvc.iSubItem = COL_BACK_TIME;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_BACK_TIME, (LPARAM)&lvc);
	// m_ColsWidth[COL_BACK_TIME] = lvc.cx;

	//set columns order
	SendMessageW(hList, LVM_SETCOLUMNORDERARRAY, COL_MAX, (LPARAM)m_ColsOrder);
}

static void ShowBackPreview(int selCount){
	wchar_t			szBuffer[128], filespec[MAX_PATH];

	if(selCount == 1){
		SelectedBackItem(szBuffer);
		if(wcslen(szBuffer) > 0){
			COLORREF		color;
			
			OleTranslateColor(0x800000 | GetSysColor(COLOR_WINDOW), NULL, &color);
			SetPreviewBackgroundColor(FALSE, color);
			wcscpy(filespec, g_NotePaths.BackupDir);
			wcscat(filespec, szBuffer);
			wcscat(filespec, BACK_NOTE_EXTENTION);
			ReadRestoreRTFFile(m_hEditPreview, filespec);
		}
	}
	else{
		SetPreviewBackgroundColor(TRUE, 0);
		SetWindowTextW(m_hEditPreview, NULL);
	}
}

static void ShowNotePreview(int selCount){
	if(selCount == 1){
		PMEMNOTE		pNote = SelectedNote();

		if((pNote->pFlags->locked || IsGroupLocked(pNote->pData->idGroup)) && IsBitOn(g_NextSettings.flags1, SB3_DONOT_SHOW_CONTENT)){
			SetWindowTextW(m_hEditPreview, L"************************");
			return;
		}
		if(pNote->pRTHandles->hbSkin){
			COLORREF		color;
			
			OleTranslateColor(0x800000 | GetSysColor(COLOR_WINDOW), NULL, &color);
			SetPreviewBackgroundColor(FALSE, color);
		}
		else{
			SetPreviewBackgroundColor(FALSE, pNote->pAppearance->crWindow);
		}
		ReadNoteRTFFile(pNote->pFlags->id, m_hEditPreview, NULL);
	}
	else{
		SetPreviewBackgroundColor(TRUE, 0);
		SetWindowTextW(m_hEditPreview, NULL);
	}
}

static void PrepareTree(HWND hwnd){
	HTREEITEM			hItem;
	RECT				rc = {0};

	FillTreeImageList();
	//insert all items
	hItem = InsertTreeviewItems();
	//create toolbar image list and set toolbar
	DoToolbarW(m_hTbrTree, m_hImlTbrNormal, m_hImlTbrGray, NELEMS(m_TreeButtons), m_TreeButtons);
	//expand tree view
	TreeView_Expand(m_hTreeMain, hItem, TVE_EXPAND);
	//get width of toolbar
	m_WTreeTbr = 0;
	for(int i = 0; i < NELEMS(m_TreeButtons); i++){
		SendMessageW(m_hTbrTree, TB_GETITEMRECT, i, (LPARAM)&rc);
		m_WTreeTbr += (rc.right - rc.left);
	}
}

static HTREEITEM InsertTreeviewItems(void){

	TVINSERTSTRUCTW		tvs;
	wchar_t 			szBuffer[128], szCount[16];
	HTREEITEM			hRootItem, hItem;

	m_gOffset = 0;
	TreeView_DeleteAllItems(m_hTreeMain);
	ZeroMemory(&tvs, sizeof(tvs));
	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_FIRST;
	tvs.item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
	tvs.item.lParam = GROUP_ROOT;
	tvs.item.iImage = 0;
	tvs.item.iSelectedImage = 0;
	tvs.item.pszText = szBuffer;
	GetPrivateProfileStringW(S_CAPTIONS, L"group_main", DS_ALL_NOTES_GROUP, szBuffer, 128, g_NotePaths.CurrLanguagePath);
	_itow(CountNotesAlive(), szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	hRootItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	m_gOffset++;

	tvs.hInsertAfter = TVI_LAST;

	tvs.item.lParam = GROUP_RECYCLE;
	tvs.item.iImage = 1;
	tvs.item.iSelectedImage = 1;
	GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	tvs.item.pszText = szBuffer;
	hItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_RECYCLE, hItem);
	m_gOffset++;

	tvs.item.lParam = GROUP_DIARY;
	tvs.item.iImage = 4;
	tvs.item.iSelectedImage = 4;
	GetPrivateProfileStringW(S_CAPTIONS, L"diary", L"Diary", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	tvs.item.pszText = szBuffer;
	hItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_DIARY, hItem);
	m_gOffset++;

	tvs.item.lParam = GROUP_SEARCH_RESULTS;
	tvs.item.iImage = 2;
	tvs.item.iSelectedImage = 2;
	GetPrivateProfileStringW(S_CAPTIONS, IK_SEARCH_RESULTS, L"Search results", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	tvs.item.pszText = szBuffer;
	hItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_SEARCH_RESULTS, hItem);
	m_gOffset++;

	tvs.item.lParam = GROUP_BACKUP;
	tvs.item.iImage = 3;
	tvs.item.iSelectedImage = 3;
	GetPrivateProfileStringW(S_CAPTIONS, IK_BACKUP_CAPTION, L"Backup", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	tvs.item.pszText = szBuffer;
	hItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_BACKUP, hItem);
	m_gOffset++;

	tvs.item.lParam = GROUP_FAVORITES;
	tvs.item.iImage = 5;
	tvs.item.iSelectedImage = 5;
	GetPrivateProfileStringW(S_CAPTIONS, IK_FAVORITES, L"Favorites", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	tvs.item.pszText = szBuffer;
	hItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_FAVORITES, hItem);
	m_gOffset++;

	tvs.item.lParam = GROUP_INCOMING;
	tvs.item.iImage = 6;
	tvs.item.iSelectedImage = 6;
	GetPrivateProfileStringW(S_CAPTIONS, IK_INCOMING, L"Incoming", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	tvs.item.pszText = szBuffer;
	hItem = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_INCOMING, hItem);
	m_gOffset++;
	ShowAllGroups(-1, hRootItem);
	return hRootItem;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: UpdateListView
 Created  : Fri May 25 22:19:15 2007
 Modified : Fri May 25 22:19:41 2007

 Synopsys : Clears and fills list view
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void UpdateListView(void){
	CPENUM		cp;
	TVITEMW		tvi;
	wchar_t		szBuffer[128], szCaption[128], szTemp[128], szText[512], szTotal[128], szSubtotal[128];
	HDC			hdc;
	int			sbParts[3];
	RECT		rc;
	LPPNGROUP	ppg;
	
	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = TreeView_GetSelection(m_hTreeMain);
	SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);

	switch(tvi.lParam){
		case GROUP_ROOT:	//root
			GetPrivateProfileStringW(S_CAPTIONS, L"group_main", DS_ALL_NOTES_GROUP, szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		case GROUP_RECYCLE:	//recycle
			GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		case GROUP_SEARCH_RESULTS:	//search results
			GetPrivateProfileStringW(S_CAPTIONS, IK_SEARCH_RESULTS, L"Search results", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		case GROUP_BACKUP:		//backup
			GetPrivateProfileStringW(S_CAPTIONS, IK_BACKUP_CAPTION, L"Backup", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		case GROUP_DIARY:		//diary
			GetPrivateProfileStringW(S_CAPTIONS, L"diary", L"Diary", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		case GROUP_FAVORITES:	//favorites
			GetPrivateProfileStringW(S_CAPTIONS, IK_FAVORITES, L"Favorites", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		case GROUP_INCOMING:	//incoming
			GetPrivateProfileStringW(S_CAPTIONS, IK_INCOMING, L"Incoming", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			break;
		default:
			ppg = PNGroupsGroupById(g_PGroups, tvi.lParam);
			if(ppg){
				wcscpy(szBuffer, ppg->szName);
			}
			break;
	}
	
	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1004", DS_CTRL_PANEL, szCaption, 256, g_NotePaths.CurrLanguagePath);
	wcscat(szCaption, L" [");
	wcscat(szCaption, szBuffer);
	wcscat(szCaption, L"]");
	SetWindowTextW(m_hCtrlPanel, szCaption);

	GetPrivateProfileStringW(S_STATUS, L"group", DS_GROUP, szText, 512, g_NotePaths.CurrLanguagePath);
	wcscat(szText, L" ");
	wcscat(szText, szBuffer);
	LockWindowUpdate(m_hListMain);
	switch(tvi.lParam){
		case GROUP_ROOT:
		case GROUP_RECYCLE:
		case GROUP_SEARCH_RESULTS:
		case GROUP_FAVORITES:
			if(tvi.lParam == GROUP_SEARCH_RESULTS && wcslen(m_QSString) > 0){
				wcscat(szText, L" (");
				wcscat(szText, m_QSGroup);
				wcscat(szText, L", ");
				wcscat(szText, m_QSString);
				wcscat(szText, L")");
			}
			//make backup columns unvisible
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_BACK_ORIGINAL, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_BACK_TIME, 0);
			//make all columns visible (except backup columns)
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_GROUP, m_ColsVisibility[COL_GROUP].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PRIORITY, m_ColsVisibility[COL_PRIORITY].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PROTECTED, m_ColsVisibility[COL_PROTECTED].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PASSWORD, m_ColsVisibility[COL_PASSWORD].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PIN, m_ColsVisibility[COL_PIN].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_COMPLETED, m_ColsVisibility[COL_COMPLETED].width);
			if(tvi.lParam != GROUP_FAVORITES && tvi.lParam != GROUP_RECYCLE)
				SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_FAVORITES, m_ColsVisibility[COL_FAVORITES].width);
			else
				SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_FAVORITES, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SR, m_ColsVisibility[COL_SR].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SCHEDULE, m_ColsVisibility[COL_SCHEDULE].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_TAGS, m_ColsVisibility[COL_TAGS].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_CONTENT, m_ColsVisibility[COL_CONTENT].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_NAME, m_ColsVisibility[COL_NAME].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SAVED, m_ColsVisibility[COL_SAVED].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_CREATED, m_ColsVisibility[COL_CREATED].width);
			if(tvi.lParam == GROUP_RECYCLE)
				SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_DELETED, m_ColsVisibility[COL_DELETED].width);
			else
				SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_DELETED, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SENT_TO, m_ColsVisibility[COL_SENT_TO].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SENT_AT, m_ColsVisibility[COL_SENT_AT].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_RECEIVED_FROM, m_ColsVisibility[COL_RECEIVED_FROM].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_RECEIVED_AT, m_ColsVisibility[COL_RECEIVED_AT].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_ID, m_ColsVisibility[COL_ID].width);
			//make group column visible
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_GROUP, m_ColsVisibility[COL_GROUP].width);
			break;
		case GROUP_BACKUP:
			//make all columns unvisible (except backup columns)
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_GROUP, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PRIORITY, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PROTECTED, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PASSWORD, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PIN, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_COMPLETED, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_FAVORITES, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SR, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SCHEDULE, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_TAGS, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_CONTENT, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SENT_TO, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SENT_AT, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_RECEIVED_FROM, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_RECEIVED_AT, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_ID, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SAVED, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_CREATED, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_DELETED, 0);
			//make backup columns visible
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_BACK_ORIGINAL, m_ColsVisibility[COL_BACK_ORIGINAL].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_BACK_TIME, m_ColsVisibility[COL_BACK_TIME].width);
			break;
		default:
			//make backup columns unvisible
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_BACK_ORIGINAL, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_BACK_TIME, 0);
			//make all columns visible (except backup columns)
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_GROUP, m_ColsVisibility[COL_GROUP].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PRIORITY, m_ColsVisibility[COL_PRIORITY].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PROTECTED, m_ColsVisibility[COL_PROTECTED].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PASSWORD, m_ColsVisibility[COL_PASSWORD].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_PIN, m_ColsVisibility[COL_PIN].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_COMPLETED, m_ColsVisibility[COL_COMPLETED].width);
			if(tvi.lParam != GROUP_DIARY)
				SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_FAVORITES, m_ColsVisibility[COL_FAVORITES].width);
			else
				SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_FAVORITES, 0);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SR, m_ColsVisibility[COL_SR].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SCHEDULE, m_ColsVisibility[COL_SCHEDULE].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_TAGS, m_ColsVisibility[COL_TAGS].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_CONTENT, m_ColsVisibility[COL_CONTENT].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SENT_TO, m_ColsVisibility[COL_SENT_TO].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SENT_AT, m_ColsVisibility[COL_SENT_AT].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_RECEIVED_FROM, m_ColsVisibility[COL_RECEIVED_FROM].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_RECEIVED_AT, m_ColsVisibility[COL_RECEIVED_AT].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_ID, m_ColsVisibility[COL_ID].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_NAME, m_ColsVisibility[COL_NAME].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_SAVED, m_ColsVisibility[COL_SAVED].width);
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_CREATED, m_ColsVisibility[COL_CREATED].width);
			//make group column unvisible
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_GROUP, 0);
			//make deleted column unvisible
			SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, COL_DELETED, 0);
			break;
	}
	LockWindowUpdate(NULL);

	GetPrivateProfileStringW(S_STATUS, L"group_notes", DS_NOTES_IN_GROUP, szSubtotal, 128, g_NotePaths.CurrLanguagePath);
	wcscat(szSubtotal, L" ");

	GetPrivateProfileStringW(S_STATUS, L"total", DS_TOTAL_NOTES, szTotal, 128, g_NotePaths.CurrLanguagePath);
	wcscat(szTotal, L" ");

	cp.group = tvi.lParam;
	cp.result = 0;
	cp.total = 0;

	//LockWindowUpdate(m_hListMain);
	SendMessageW(m_hListMain, LVM_DELETEALLITEMS, 0, 0);
	
	if(tvi.lParam > GROUP_RECYCLE || tvi.lParam == GROUP_INCOMING){
		EnableCommands(SEL_TREE_ORD);
	}
	else if(tvi.lParam == GROUP_RECYCLE){
		EnableCommands(SEL_TREE_REC);
	}

	InsertNotesIntoList(&cp);

	ShowNotePreview(ListView_GetSelectedCount(m_hListMain));
	//LockWindowUpdate(NULL);

	_itow(cp.result, szTemp, 10);
	wcscat(szSubtotal, szTemp);
	_itow(cp.total, szTemp, 10);
	wcscat(szTotal, szTemp);
	hdc = GetDC(m_hStbControl);
	SetRectEmpty(&rc);
	DrawTextW(hdc, szText, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
	ReleaseDC(m_hStbControl, hdc);
	sbParts[0] = (rc.right - rc.left) + 4;
	sbParts[1] = sbParts[0] + 180;
	sbParts[2] = -1;
	SendMessageW(m_hStbControl, SB_SETPARTS, 3, (LPARAM)sbParts);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 0 | SBT_POPOUT, (LPARAM)szText);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 1 | SBT_POPOUT, (LPARAM)szSubtotal);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 2 | SBT_POPOUT, (LPARAM)szTotal);

	UpdateGroupText(tvi.lParam, tvi.hItem);
}

static void UpdateNameCaption(wchar_t * lpCaption){
	LVCOLUMNW	lvc;

	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_TEXT;
	lvc.pszText = lpCaption;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_NAME, (LPARAM)&lvc);
}

static void InsertNotesIntoList(P_CPENUM pcp){

	PMEMNOTE	pTemp = MemoryNotes();
	int			item;

	if(pcp->group != GROUP_SEARCH_RESULTS && pcp->group != GROUP_BACKUP && pcp->group != GROUP_FAVORITES){
		while(pTemp){
			pcp->total++;
			if(pcp->group != GROUP_ROOT){
				//for group -1 show all notes
				if(pTemp->pData->idGroup != pcp->group){
					//do not show note from another group - continue enumeration
					goto _next;
				}
			}
			else{
				if(pTemp->pData->idGroup == GROUP_RECYCLE){
					//do not show deleted note - continue enumeration
					goto _next;
				}
			}
			//insert item into needed position
			item = SendMessageW(m_hListMain, LVM_GETITEMCOUNT, 0, 0);
			InsertItem(m_hListMain, pTemp, item);
			pcp->result++;
			_next:
			pTemp = pTemp->next;
		}
		UpdateNameCaption(m_RegularNameCaption);
	}
	else if(pcp->group == GROUP_SEARCH_RESULTS){
		for(int i = 0; i < m_iSearchCount; i++){	
			//insert item into needed position
			item = SendMessageW(m_hListMain, LVM_GETITEMCOUNT, 0, 0);
			InsertItem(m_hListMain, m_pSearchResults[i], item);
			pcp->result++;
		}
		pcp->total = NotesCount();
		UpdateNameCaption(m_RegularNameCaption);
	}
	else if(pcp->group == GROUP_FAVORITES){
		while(pTemp){
			pcp->total++;
			if(pTemp->pRTHandles->favorite == GROUP_FAVORITES && pTemp->pData->idGroup != GROUP_RECYCLE){
				//insert item into needed position
				item = SendMessageW(m_hListMain, LVM_GETITEMCOUNT, 0, 0);
				InsertItem(m_hListMain, pTemp, item);
				pcp->result++;
			}
			pTemp = pTemp->next;
		}
		UpdateNameCaption(m_RegularNameCaption);
	}
	else if(pcp->group == GROUP_BACKUP){
		//load all backup items
		WIN32_FIND_DATAW		fd;
		wchar_t					filespec[MAX_PATH];
		HANDLE					handle = INVALID_HANDLE_VALUE;
		BOOL					result = TRUE;

		wcscpy(filespec, g_NotePaths.BackupDir);
		wcscat(filespec, L"*");
		wcscat(filespec, BACK_NOTE_EXTENTION);
		handle = FindFirstFileW(filespec, &fd);
		if(handle != INVALID_HANDLE_VALUE){
			while(result){
				//insert item into needed position
				item = SendMessageW(m_hListMain, LVM_GETITEMCOUNT, 0, 0);
				InsertBackupItem(m_hListMain, &fd, item);
				pcp->result++;
				result = FindNextFileW(handle, &fd);
			}
			FindClose(handle);
		}
		pcp->total = NotesCount();
		UpdateNameCaption(m_BackupNameCaption);
	}
	//restore the previous sort order
	if(pcp->group != GROUP_BACKUP){
		switch(m_lvsmRegular.column){
			case COL_NAME:
				m_lvsmRegular.order = m_LVSortRegular.iSortName;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
			case COL_PRIORITY:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortPriority, (LPARAM)PriorityCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortPriority;
				break;
			case COL_COMPLETED:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortCompleted, (LPARAM)CompletedCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortCompleted;
				break;
			case COL_PROTECTED:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortProtected, (LPARAM)ProtectedCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortProtected;
				break;
			case COL_PASSWORD:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortPassword, (LPARAM)PasswordCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortPassword;
				break;
			case COL_PIN:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortPin, (LPARAM)PinCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortPin;
				break;
			case COL_FAVORITES:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortFavorites, (LPARAM)FavoritesCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortFavorites;
				break;
			case COL_SR:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortSR, (LPARAM)SRCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortSR;
				break;
			case COL_GROUP:
				m_lvsmRegular.order = m_LVSortRegular.iSortGroup;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
			case COL_SAVED:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortSaved, (LPARAM)DateCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortSaved;
				break;
			case COL_CREATED:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortCreated, (LPARAM)CreationDateCompareFunct);
				m_lvsmRegular.order = m_LVSortRegular.iSortCreated;
				break;
			case COL_DELETED:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortDeleted, (LPARAM)DeletedDateCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortDeleted;
				break;
			case COL_SCHEDULE:
				m_lvsmRegular.order = m_LVSortRegular.iSortSchedule;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
			case COL_TAGS:
				m_lvsmRegular.order = m_LVSortRegular.iSortTags;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
			case COL_SENT_TO:
				m_lvsmRegular.order = m_LVSortRegular.iSortSentTo;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
			case COL_SENT_AT:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortSentAt, (LPARAM)SentAtCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortSentAt;
				break;
			case COL_RECEIVED_FROM:
				m_lvsmRegular.order = m_LVSortRegular.iSortReceivedFrom;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
			case COL_RECEIVED_AT:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortReceivedAt, (LPARAM)ReceivedAtCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortReceivedAt;
				break;
			case COL_ID:
				SendMessageW(m_hListMain, LVM_SORTITEMS, (WPARAM)m_LVSortRegular.iSortID, (LPARAM)IDCompareFunc);
				m_lvsmRegular.order = m_LVSortRegular.iSortID;
				break;
			case COL_CONTENT:
				m_lvsmRegular.order = m_LVSortRegular.iSortContent;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmRegular, (LPARAM)NameCompareFunc);
				break;
		}
	}
	else{
		switch(m_lvsmBack.column){
			case COL_NAME:
				m_lvsmBack.order = m_LVSortBack.iSortName;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmBack, (LPARAM)NameCompareFunc);
				break;
			case COL_BACK_ORIGINAL:
				m_lvsmBack.order = m_LVSortBack.iSortBackOrigin;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)&m_lvsmBack, (LPARAM)NameCompareFunc);
				break;
			case COL_BACK_TIME:
				m_lvsmBack.order = m_LVSortBack.iSortBackDate;
				SendMessageW(m_hListMain, LVM_SORTITEMSEX, (WPARAM)m_LVSortBack.iSortBackDate, (LPARAM)BackDateCompareFunc);
				break;
		}
	}
}

static void UpdateBackupControlStatus(void){
	wchar_t		szTemp[128], szSubtotal[128];

	GetPrivateProfileStringW(S_STATUS, L"group_notes", DS_NOTES_IN_GROUP, szSubtotal, 128, g_NotePaths.CurrLanguagePath);
	wcscat(szSubtotal, L" ");
	
	_itow(CountOfBackupCopies(), szTemp, 10);
	wcscat(szSubtotal, szTemp);
	
	SendMessageW(m_hStbControl, SB_SETTEXTW, 1 | SBT_POPOUT, (LPARAM)szSubtotal);
}

static void UpdateControlStatus(void){
	wchar_t		szTemp[128], szTotal[128], szSubtotal[128];

	GetPrivateProfileStringW(S_STATUS, L"group_notes", DS_NOTES_IN_GROUP, szSubtotal, 128, g_NotePaths.CurrLanguagePath);
	wcscat(szSubtotal, L" ");
	GetPrivateProfileStringW(S_STATUS, L"total", DS_TOTAL_NOTES, szTotal, 128, g_NotePaths.CurrLanguagePath);
	wcscat(szTotal, L" ");
	_itow(NotesCount(), szTemp, 10);
	wcscat(szTotal, szTemp);
	_itow(SendMessageW(m_hListMain, LVM_GETITEMCOUNT, 0, 0), szTemp, 10);
	wcscat(szSubtotal, szTemp);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 1 | SBT_POPOUT, (LPARAM)szSubtotal);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 2 | SBT_POPOUT, (LPARAM)szTotal);
}

static void BuildColumnsVisibilityMenu(void){
	int			count, group;
	MITEM		mit;
	LVCOLUMNW	lvc = {0};
	wchar_t		szBuffer[256];

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = 0;
	mit.xCheck = 51;
	mit.yCheck = 0;

	if(m_hPUMenuCV){
		count = GetMenuItemCount(m_hPUMenuCV);
		for(int i = count - 1; i > 0; i--){
			FreeSingleMenu(m_hPUMenuCV, i);
			DeleteMenu(m_hPUMenuCV, i, MF_BYPOSITION);
		}
		DestroyMenu(m_hPUMenuCV);
		m_hPUMenuCV = NULL;
	}

	m_hPUMenuCV = CreatePopupMenu();
	count = NELEMS(m_ColsVisibility);
	group = GetTVItemId(m_tSelected);

	for(int j = 0, i = COL_NAME; i <= COL_BACK_TIME; i++){
		mit.id = m_ColsVisibility[i].col + COLUMN_VIS_ADDITION;
		switch(i){
			case COL_PRIORITY:
			case COL_COMPLETED:
			case COL_PROTECTED:
			case COL_PASSWORD:
			case COL_PIN:
			case COL_FAVORITES:
			case COL_SR:
				lvc.mask = LVCF_WIDTH;
				if(i == COL_PRIORITY)
					GetPrivateProfileStringW(S_COLUMNS, L"8", L"Priority", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				else if(i == COL_COMPLETED)
					GetPrivateProfileStringW(S_COLUMNS, L"9", L"Completed", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				else if(i == COL_PROTECTED)
					GetPrivateProfileStringW(S_COLUMNS, L"10", L"Protected", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				else if(i == COL_PASSWORD)
					GetPrivateProfileStringW(S_COLUMNS, L"20", L"Password protected", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				else if(i == COL_PIN)
					GetPrivateProfileStringW(S_COLUMNS, L"21", L"Pinned", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				else if(i == COL_FAVORITES)
					GetPrivateProfileStringW(S_COLUMNS, L"11", L"Favorites", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				else if(i == COL_SR)
					GetPrivateProfileStringW(S_COLUMNS, L"14", L"Sent/Received", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				SendMessageW(m_hListMain, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
				wcscpy(mit.szText, szBuffer);
				break;
			default:
				lvc.mask = LVCF_TEXT | LVCF_WIDTH;
				lvc.cchTextMax = 256;
				lvc.pszText = mit.szText;
				SendMessageW(m_hListMain, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
				break;
		}
		if(group != GROUP_BACKUP){
			if(i == COL_BACK_ORIGINAL || i == COL_BACK_TIME)
				continue;
		}
		else{
			if(i != COL_NAME && i != COL_BACK_ORIGINAL && i != COL_BACK_TIME)
				continue;
		}
		AppendMenuW(m_hPUMenuCV, MF_STRING, mit.id, mit.szText);
		// m_ColsVisibility[i].width = lvc.cx;
		if(lvc.cx > 0)
			CheckMenuItem(m_hPUMenuCV, j, MF_BYPOSITION | MF_CHECKED);
		if(i == COL_NAME){
			EnableMenuItem(m_hPUMenuCV, j, MF_BYPOSITION | MF_GRAYED);
		}
		else if(i == COL_FAVORITES){
			if(group == GROUP_DIARY || group == GROUP_RECYCLE || group == GROUP_FAVORITES){
				EnableMenuItem(m_hPUMenuCV, j, MF_BYPOSITION | MF_GRAYED);
			}
		}
		else if(i == COL_GROUP){
			if(group > GROUP_ROOT || group == GROUP_INCOMING){
				EnableMenuItem(m_hPUMenuCV, j, MF_BYPOSITION | MF_GRAYED);
			}
		}
		else if(i == COL_DELETED){
			if(group != GROUP_RECYCLE){
				EnableMenuItem(m_hPUMenuCV, j, MF_BYPOSITION | MF_GRAYED);
			}
		}
		SetMenuItemProperties(&mit, m_hPUMenuCV, j, TRUE);
		j++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ApplyControlPanelLanguage
 Created  : Fri May 25 22:15:18 2007
 Modified : Fri May 25 22:15:18 2007

 Synopsys : Sets dialog caption prepares and set text for popup menus
 Input    : hwnd - dialog handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void ApplyControlPanelLanguage(HWND hwnd, BOOL fRefreshTree){

	wchar_t 		szBuffer[256];
	LVCOLUMNW		lvc;

	//prepare popup menu
	if(m_hPUMenuControl){
		FreeMenus(m_hPUMenuControl);
	}
	if(m_hMenuControl){
		DestroyMenu(m_hMenuControl);
	}
	m_hMenuControl = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_CTRL_PANEL));
	m_hPUMenuControl = GetSubMenu(m_hMenuControl, 0);
	
	//prepare treeview popup menu
	if(m_hPUMenuTree){
		FreeMenus(m_hPUMenuTree);
	}
	if(m_hMenuTree){
		DestroyMenu(m_hMenuTree);
	}
	m_hMenuTree = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_TREE_MENU));
	m_hPUMenuTree = GetSubMenu(m_hMenuTree, 0);

	//prepare recycle bin popup menu
	if(m_hPUMenuRecycle){
		FreeMenus(m_hPUMenuRecycle);
	}
	if(m_hMenuRecycle){
		DestroyMenu(m_hMenuRecycle);
	}
	m_hMenuRecycle = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_RECYCLE_MENU));
	m_hPUMenuRecycle = GetSubMenu(m_hMenuRecycle, 0);

	//set list view columns text
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	//name column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_NAME, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"0", L"Note Name", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_NAME, (LPARAM)&lvc);
	wcscpy(m_RegularNameCaption, szBuffer);
	GetPrivateProfileStringW(S_COLUMNS, L"4", L"Name", m_BackupNameCaption, 256, g_NotePaths.CurrLanguagePath);
	//group column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_GROUP, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"1", L"Group", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_GROUP, (LPARAM)&lvc);
	//created column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_CREATED, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"19", L"Created", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_CREATED, (LPARAM)&lvc);
	//saved column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_SAVED, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"2", L"Last Saved", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_SAVED, (LPARAM)&lvc);
	//deleted column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_DELETED, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"13", L"Deleted", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_DELETED, (LPARAM)&lvc);
	//schedule column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_SCHEDULE, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"3", L"Schedule Type", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_SCHEDULE, (LPARAM)&lvc);
	//tags column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_TAGS, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"7", L"Tags", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_TAGS, (LPARAM)&lvc);
	//content column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_CONTENT, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"12", L"Content", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_CONTENT, (LPARAM)&lvc);
	//sent to column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_SENT_TO, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"15", L"Sent To", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_SENT_TO, (LPARAM)&lvc);
	//sent at column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_SENT_AT, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"16", L"Sent At", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_SENT_AT, (LPARAM)&lvc);
	//received from column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_RECEIVED_FROM, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"17", L"Received From", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_RECEIVED_FROM, (LPARAM)&lvc);
	//received at column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_RECEIVED_AT, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"18", L"Received At", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_RECEIVED_AT, (LPARAM)&lvc);
	//id column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_ID, (LPARAM)&lvc);
	wcscpy(szBuffer, L"ID");
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_ID, (LPARAM)&lvc);
	//backup origin column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_BACK_ORIGINAL, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"5", L"Original note", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_BACK_ORIGINAL, (LPARAM)&lvc);
	//backup time column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_BACK_TIME, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"6", L"Saved time", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_BACK_TIME, (LPARAM)&lvc);
	lvc.mask = LVCF_IMAGE;
	//priority column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_PRIORITY, (LPARAM)&lvc);
	lvc.iImage = 17;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_PRIORITY, (LPARAM)&lvc);
	//completed column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	lvc.iImage = 18;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	//protected column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_PROTECTED, (LPARAM)&lvc);
	lvc.iImage = 19;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_PROTECTED, (LPARAM)&lvc);
	//password column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_PASSWORD, (LPARAM)&lvc);
	lvc.iImage = 26;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_PASSWORD, (LPARAM)&lvc);
	//pin column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_PIN, (LPARAM)&lvc);
	lvc.iImage = 28;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_PIN, (LPARAM)&lvc);
	//favorites column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_FAVORITES, (LPARAM)&lvc);
	lvc.iImage = 21;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_FAVORITES, (LPARAM)&lvc);
	//sent/received column
	SendMessageW(m_hListMain, LVM_GETCOLUMNW, COL_SR, (LPARAM)&lvc);
	lvc.iImage = 23;
	SendMessageW(m_hListMain, LVM_SETCOLUMNW, COL_SR, (LPARAM)&lvc);
	//set quick search text
	GetPrivateProfileStringW(S_CAPTIONS, IK_SEARCH_PROMPT, L"Quick search", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendMessageW(m_hSearchBox, GSBM_SET_DEF_PROMT, 0, (LPARAM)szBuffer);

	if(PathFileExistsW(g_NotePaths.CurrLanguagePath)){
		//set menu items text
		PrepareMenuRecursive(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hPUMenuControl, g_NotePaths.CurrLanguagePath, S_MENU);
		PrepareMenuRecursive(m_TreeMenu, NELEMS(m_TreeMenu), m_hPUMenuTree, g_NotePaths.CurrLanguagePath, S_MENU);
		PrepareMenuRecursive(m_RecycleMenu, NELEMS(m_RecycleMenu), m_hPUMenuRecycle, g_NotePaths.CurrLanguagePath, S_MENU);
		SetMenuTextByAnotherID(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hPUMenuControl, IDM_DOCK_NONE_ALL, IDM_DOCK_NONE, g_NotePaths.CurrLanguagePath, S_MENU);
		SetMenuTextByAnotherID(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hPUMenuControl, IDM_DOCK_LEFT_ALL, IDM_DOCK_LEFT, g_NotePaths.CurrLanguagePath, S_MENU);
		SetMenuTextByAnotherID(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hPUMenuControl, IDM_DOCK_RIGHT_ALL, IDM_DOCK_RIGHT, g_NotePaths.CurrLanguagePath, S_MENU);
		SetMenuTextByAnotherID(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hPUMenuControl, IDM_DOCK_TOP_ALL, IDM_DOCK_TOP, g_NotePaths.CurrLanguagePath, S_MENU);
		SetMenuTextByAnotherID(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hPUMenuControl, IDM_DOCK_BOTTOM_ALL, IDM_DOCK_BOTTOM, g_NotePaths.CurrLanguagePath, S_MENU);
	}
	//initially disable some menus
	EnableMenus(MF_GRAYED, FALSE);

	//refresh treeview captions
	if(fRefreshTree){
		//all groups
		GetPrivateProfileStringW(S_CAPTIONS, L"group_main", DS_ALL_NOTES_GROUP, szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountNotesAlive(), szBuffer);
		SetTVItemText(TreeView_GetRoot(m_hTreeMain), szBuffer);
		//recycle bin
		GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountNotesInGroup(GROUP_RECYCLE), szBuffer);
		FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_RECYCLE);
		SetTVItemText(m_tItem, szBuffer);
		//search results
		GetPrivateProfileStringW(S_CAPTIONS, IK_SEARCH_RESULTS, L"Search results", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountNotesInGroup(GROUP_SEARCH_RESULTS), szBuffer);
		FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_SEARCH_RESULTS);
		SetTVItemText(m_tItem, szBuffer);
		//backup
		GetPrivateProfileStringW(S_CAPTIONS, IK_BACKUP_CAPTION, L"Backup", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountNotesInGroup(GROUP_BACKUP), szBuffer);
		FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_BACKUP);
		SetTVItemText(m_tItem, szBuffer);
		//general
		GetPrivateProfileStringW(S_CAPTIONS, L"group_default", DS_GENERAL_GROUP_NAME, szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountNotesInGroup(0), szBuffer);
		FindItemById(TreeView_GetRoot(m_hTreeMain), 0);
		SetTVItemText(m_tItem, szBuffer);
		//diary
		GetPrivateProfileStringW(S_CAPTIONS, L"diary", L"Diary", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountNotesInGroup(GROUP_DIARY), szBuffer);
		FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_DIARY);
		if(IsDiaryLocked())
			wcscat(szBuffer, PWRD_PROTECTED_GROUP);
		SetTVItemText(m_tItem, szBuffer);
		//favorites
		GetPrivateProfileStringW(S_CAPTIONS, IK_FAVORITES, L"Favorites", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		AddCountToGroupCaption(CountFavorites(), szBuffer);
		FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_FAVORITES);
		SetTVItemText(m_tItem, szBuffer);
	}
	//for status bar
	UpdateListView();
	BuildExternalMenu(m_hPUMenuControl);
}

static void AddCountToGroupCaption(int count, wchar_t * lpText){
	wchar_t		szCount[32];

	_itow(count, szCount, 10);
	wcscat(lpText, L" (");
	wcscat(lpText, szCount);
	wcscat(lpText, L")");
}

static void SetTVItemText(HTREEITEM hItem, wchar_t * lpText){
	TVITEMW			tvi;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_TEXT;
	tvi.hItem = hItem;
	tvi.pszText = lpText;
	SendMessageW(m_hTreeMain, TVM_SETITEMW, 0, (LPARAM)&tvi);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DeleteItem
 Created  : Fri May 25 22:10:51 2007
 Modified : Fri May 25 22:10:51 2007

 Synopsys : Removes list view item (by it's lParam member)
 Input    : hList - list view handle
            lParam - needed lParam memeber
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void DeleteItem(HWND hList, LPARAM lParam){

	LVFINDINFOW		lvf;
	int				index;

	lvf.flags = LVFI_PARAM;
	lvf.lParam = lParam;
	index = SendMessageW(hList, LVM_FINDITEMW, -1, (LPARAM)&lvf);
	if(index != -1){
		SendMessageW(hList, LVM_DELETEITEM, index, 0);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ReplaceItem
 Created  : Fri May 25 22:09:23 2007
 Modified : Fri May 25 22:09:23 2007

 Synopsys : Replaces list view item. There was a problem in updating items 
            (Unicode string were not displayed correctly) on some 
            computers, so I should just insert new item and remove old one.
 Input    : hList - list view handle
            hNote - note window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void ReplaceItem(HWND hList, PMEMNOTE pNote){

	LVFINDINFOW		lvf;
	int				index;

	lvf.flags = LVFI_PARAM;
	lvf.lParam = (LONG)pNote;
	
	index = SendMessageW(hList, LVM_FINDITEMW, -1, (LPARAM)&lvf);
	if(index != -1){
		InsertItem(hList, pNote, index);
		SendMessageW(hList, LVM_DELETEITEM, index + 1, 0);
	}
}

static void DeleteBackupItems(wchar_t * lpId){
	LVFINDINFOW			lvf;
	int					index = 0;
	wchar_t				szId[128];

	if(GetSelectedTVItemId() == GROUP_BACKUP){
		wcscpy(szId, lpId);
		wcscat(szId, L"_");
		ZeroMemory(&lvf, sizeof(lvf));
		lvf.flags = LVFI_PARTIAL;
		while(index != -1){
			index = SendMessageW(m_hListMain, LVM_FINDITEMW, -1, (LPARAM)&lvf);
			if(index != -1){
				SendMessageW(m_hListMain, LVM_DELETEITEM, index, 0);
			}
		}
	}
	FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_BACKUP);
	UpdateGroupText(GROUP_BACKUP, m_tItem);
}

static void UpdateDiaryNames(void){
	LVITEMW				lvi = {0};
	wchar_t				szBuffer[128];
	int					count;

	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = szBuffer;
	count = ListView_GetItemCount(m_hListMain);
	for(int i = 0; i < count; i++){
		lvi.iItem = i;
		SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if(lvi.lParam){
			PMEMNOTE pNote = (PMEMNOTE)lvi.lParam;
			wcscpy(szBuffer, pNote->pData->szName);
			SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
		}
	}
}

static void UpdateBackupItems(wchar_t * lpId){
	WIN32_FIND_DATAW	fd;
	int					index = -1;
	LVITEMW				lvi;
	LVFINDINFOW			lvf;
	wchar_t				szId[128], filespec[MAX_PATH], szDate[128];
	HANDLE				handle = INVALID_HANDLE_VALUE;
	BOOL				result = TRUE;
	PMEMNOTE			pNote;
	SYSTEMTIME			st;
	FILETIME			lft;

	if(GetSelectedTVItemId() == GROUP_BACKUP){
		ZeroMemory(&lvi, sizeof(lvi));
		ZeroMemory(&lvf, sizeof(lvf));
		lvf.flags = LVFI_STRING;
		wcscpy(filespec, g_NotePaths.BackupDir);
		wcscat(filespec, lpId);
		wcscat(filespec, L"_*");
		wcscat(filespec, BACK_NOTE_EXTENTION);
		handle = FindFirstFileW(filespec, &fd);
		if(handle != INVALID_HANDLE_VALUE){
			while(result){
				wcscpy(szId, fd.cFileName);
				szId[wcslen(szId) - wcslen(BACK_NOTE_EXTENTION)] = '\0';
				lvf.psz = szId;
				result = FindNextFileW(handle, &fd);
				index = SendMessageW(m_hListMain, LVM_FINDITEMW, -1, (LPARAM)&lvf);
				if(index != -1){
					//update existing item
					lvi.iItem = index;
				}
				else{
					//insert new item
					lvi.mask = LVIF_IMAGE | LVIF_TEXT;
					lvi.iItem = ListView_GetItemCount(m_hListMain);
					lvi.pszText = szId;
					lvi.iImage = 20;
					lvi.iSubItem = 0;
					//insert item
					SendMessageW(m_hListMain, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
				}
				lvi.mask = LVIF_TEXT;
				//update saved time
				lvi.iSubItem = COL_BACK_TIME;
				FileTimeToLocalFileTime(&fd.ftLastWriteTime, &lft);
				FileTimeToSystemTime(&lft, &st);
				ConstructDateTimeString(&st, szDate);
				lvi.pszText = szDate;
				SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
				//update origin
				pNote = MemNoteById(lpId);
				if(pNote){
					lvi.pszText = pNote->pData->szName;
					lvi.iSubItem = COL_BACK_ORIGINAL;
					SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
				}
			}
			FindClose(handle);
		}
	}
	FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_BACKUP);
	UpdateGroupText(GROUP_BACKUP, m_tItem);

	UpdateBackupControlStatus();
	ShowBackPreview(ListView_GetSelectedCount(m_hListMain));
}

static void UpdateNoteItem(PMEMNOTE pNote){
	int					index;
	LVITEMW				lvi;
	LVFINDINFOW			lvf;

	ZeroMemory(&lvf, sizeof(lvf));
	lvf.flags = LVFI_PARAM;
	lvf.lParam = (LONG)pNote;
	index = SendMessageW(m_hListMain, LVM_FINDITEMW, -1, (LPARAM)&lvf);
	if(index == -1){
		return;
	}
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = index;

	//update the name column
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iImage = ItemImageIndex(pNote);
	lvi.pszText = pNote->pData->szName;
	lvi.iSubItem = COL_NAME;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
	//set item priority
	SetItemPriority(pNote, index);
	//set item completed
	SetItemCompleted(pNote, index);
	//set item protected
	SetItemProtected(pNote, index);
	//set item password state
	SetItemPassword(pNote, index);
	//set item pinned state
	SetItemPin(pNote, index);
	//set item favorite
	SetItemFavorite(pNote, index);
	//set item sent/received status
	SetItemSR(pNote, index);
	//set item group name
	SetItemGroupName(pNote, index);
	//set item tags
	SetItemTags(pNote, index);
	//set item content
	SetItemContent(pNote, index);
	//set item sent to
	SetItemSentToReceivedFrom(pNote, index, COL_SENT_TO);
	//set item received from
	SetItemSentToReceivedFrom(pNote, index, COL_RECEIVED_FROM);
	//set item sent at
	SetItemSentReceivedDate(pNote, index, COL_SENT_AT);
	//set item received at
	SetItemSentReceivedDate(pNote, index, COL_RECEIVED_AT);
	//set item id
	SetItemID(pNote, index);
	//set item change date
	if(pNote->pData->stChanged.wDay != 0){
		SetItemChangeDate(pNote, index);
	}
	//set item created date
	if(pNote->pCreated->wDay != 0){
		SetItemCreatedDate(pNote, index);
	}
	//set item deleted date
	if(pNote->pRealDeleted->wDay != 0){
		SetItemDetetedDate(pNote, index);
	}
	//set item schedule description
	SetItemScheduleDescription(pNote, index);
	//ensure note visible
	SendMessageW(m_hListMain, LVM_ENSUREVISIBLE, index, FALSE);
	ShowNotePreview(ListView_GetSelectedCount(m_hListMain));
}

static void SetItemSentToReceivedFrom(PMEMNOTE pNote, int index, int subIndex){
	wchar_t 			szBuffer[256];
	LVITEMW				lvi = {0};

	szBuffer[0] = '\0';
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szBuffer;
	lvi.iItem = index;
	lvi.iSubItem = subIndex;
	switch(subIndex){
		case COL_SENT_TO:
			if(pNote->pSRStatus->lastSent.wDay != 0){
				wcscpy(szBuffer, pNote->pSRStatus->sentTo);
			}
			break;
		case COL_RECEIVED_FROM:
			if(pNote->pSRStatus->lastRec.wDay != 0){
				wcscpy(szBuffer, pNote->pSRStatus->recFrom);
			}
			break;
	}
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemGroupName(PMEMNOTE pNote, int index){
	wchar_t 			szBuffer[256];
	int					treeGroup;
	LVITEMW				lvi;
	LPPNGROUP			pg;

	treeGroup = GetTVItemId(TreeView_GetSelection(m_hTreeMain));

	szBuffer[0] = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szBuffer;
	lvi.iItem = index;
	lvi.iSubItem = COL_GROUP;

	if(pNote->pData->idGroup > GROUP_RECYCLE){
		//print group name
		if(pNote->pData->idGroup != GROUP_DIARY){
			pg = PNGroupsGroupById(g_PGroups, pNote->pData->idGroup);
			if(pg){
				wcscpy(szBuffer, pg->szName);
			}
		}
		else{
			GetPrivateProfileStringW(S_CAPTIONS, L"diary", L"Diary", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		}
	}
	else{
		if(treeGroup > GROUP_RECYCLE){
			if(pNote->pData->idGroup == GROUP_INCOMING)
				GetPrivateProfileStringW(S_CAPTIONS, IK_INCOMING, L"Incoming", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			else
				//print "Recycle Bin" for total group
				GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		}
		else{
			//print previous group name
			if(pNote->pData->idPrevGroup != GROUP_DIARY && pNote->pData->idPrevGroup != GROUP_INCOMING){
				pg = PNGroupsGroupById(g_PGroups, pNote->pData->idPrevGroup);
				if(pg)
					wcscpy(szBuffer, pg->szName);
			}
			else if(pNote->pData->idPrevGroup == GROUP_DIARY){
				GetPrivateProfileStringW(S_CAPTIONS, L"diary", L"Diary", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			}
			else if(pNote->pData->idPrevGroup == GROUP_INCOMING){
				GetPrivateProfileStringW(S_CAPTIONS, IK_INCOMING, L"Incoming", szBuffer, 128, g_NotePaths.CurrLanguagePath);
			}
		}
	}
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemContent(PMEMNOTE pNote, int index){
	LVITEMW				lvi = {0};
	wchar_t 			*pBuffer = NULL, *pText = NULL, *p = NULL, *t = NULL;
	int					count = GetSmallValue(g_SmallValues, SMS_CONTENT_LENGTH);
	int					length = 0;
	HWND				hEdit;
	GETTEXTLENGTHEX		gtl = {0};
	GETTEXTEX			gt = {0};
	
	if((pNote->pFlags->locked || IsGroupLocked(pNote->pData->idGroup)) && IsBitOn(g_NextSettings.flags1, SB3_DONOT_SHOW_CONTENT)){
		lvi.mask = LVIF_TEXT;
		lvi.iItem = index;
		lvi.iSubItem = COL_CONTENT;
		lvi.pszText = L"************************";
		SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
		return;
	}

	hEdit = g_hTEditContent;
	ReadSimpleRTFFile(hEdit, pNote->pFlags->id);

	gtl.flags = GTL_DEFAULT | GTL_PRECISE;
	gtl.codepage = 1200;
	length = SendMessageW(hEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, 0) + 1;
	
	gt.cb = length + 1;
	gt.flags = GT_DEFAULT;
	gt.codepage = 1200;

	pBuffer = calloc(count + 1, sizeof(wchar_t));
	pText = calloc(length + 1, sizeof(wchar_t));
	if(pBuffer && pText){
		SendMessageW(hEdit, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)pText);
		p = pBuffer;
		t = pText;
		for(int i = 0; i < count; i++){
			if(*t != '\n' && *t != '\t' && *t != '\r'){
				*p++ = *t++;
			}
			else{
				*p++ = ' ';
				t++;
			}
			if(!*t)
				break;
		}
		lvi.mask = LVIF_TEXT;
		lvi.iItem = index;
		lvi.iSubItem = COL_CONTENT;
		lvi.pszText = pBuffer;
		SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);

		free(pBuffer);
		free(pText);
	}
	if(!pNote->pData->visible){
		SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)L"");
	}
}

static void SetItemID(PMEMNOTE pNote, int index){
	LVITEMW				lvi = {0};

	lvi.mask = LVIF_TEXT;
	lvi.iItem = index;
	lvi.iSubItem = COL_ID;
	lvi.pszText = pNote->pFlags->id;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemTags(PMEMNOTE pNote, int index){
	wchar_t 			*pTags, szBuffer[] = {0};
	LVITEMW				lvi = {0};
	LPPTAG				pTemp;
	int					count = TagsCount(pNote->pTags);
	//get note tags
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = index;
	lvi.iSubItem = COL_TAGS;
	if(count > 0){
		pTags = calloc(count * 128, sizeof(wchar_t));
		if(pTags){
			lvi.pszText = pTags;
			pTemp = pNote->pTags;
			while(pTemp){
				if(*pTemp->text){
					wcscat(pTags, pTemp->text);
					wcscat(pTags, L",");
				}
				pTemp = pTemp->next;
			}
			if(*pTags){
				wchar_t		*p = wcsrchr(pTags, ',');
				if(p)
					*p = '\0';
			}
			SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
		}
	}
	else{
		lvi.pszText = szBuffer;
		SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
	}
}

static void SetItemSentReceivedDate(PMEMNOTE pNote, int index, int subIndex){
	wchar_t 			szDate[128];
	LVITEMW				lvi = {0};

	szDate[0] = '\0';
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szDate;
	lvi.iItem = index;
	lvi.iSubItem = subIndex;
	switch(subIndex){
		case COL_SENT_AT:
			if(pNote->pSRStatus->lastSent.wDay != 0){
				ConstructDateTimeString(&pNote->pSRStatus->lastSent, szDate);
			}
			break;
		case COL_RECEIVED_AT:
			if(pNote->pSRStatus->lastRec.wDay != 0){
				ConstructDateTimeString(&pNote->pSRStatus->lastRec, szDate);
			}
			break;
	}
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemChangeDate(PMEMNOTE pNote, int index){
	wchar_t 			szDate[128];
	LVITEMW				lvi;

	//get changed time
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szDate;
	lvi.iItem = index;
	lvi.iSubItem = COL_SAVED;
	ConstructDateTimeString(&pNote->pData->stChanged, szDate);
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemCreatedDate(PMEMNOTE pNote, int index){
	wchar_t 			szDate[128];
	LVITEMW				lvi;

	//get changed time
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szDate;
	lvi.iItem = index;
	lvi.iSubItem = COL_CREATED;
	ConstructDateTimeString(pNote->pCreated, szDate);
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemDetetedDate(PMEMNOTE pNote, int index){
	wchar_t 			szDate[128];
	LVITEMW				lvi;

	//get deleted time
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szDate;
	lvi.iItem = index;
	lvi.iSubItem = COL_DELETED;
	ConstructDateTimeString(pNote->pRealDeleted, szDate);
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemPriority(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_PRIORITY;
	if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY))
		lvi.iImage = 17;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemCompleted(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_COMPLETED;
	if(IsBitOn(pNote->pData->res1, NB_COMPLETED))
		lvi.iImage = 18;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemFavorite(PMEMNOTE pNote, int index){
	LVITEMW				lvi;
	// wchar_t				szBuffer[256];

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_FAVORITES;
	// GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	// if(wcslen(szBuffer) > 0)
	if(pNote->pRTHandles->favorite == GROUP_FAVORITES)
		lvi.iImage = 21;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemSR(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_SR;
	if(pNote->pSRStatus->lastSent.wDay != 0 && pNote->pSRStatus->lastRec.wDay != 0)
		lvi.iImage = 23;
	else if(pNote->pSRStatus->lastSent.wDay != 0)
		lvi.iImage = 24;
	else if(pNote->pSRStatus->lastRec.wDay != 0)
		lvi.iImage = 25;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemProtected(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_PROTECTED;
	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		lvi.iImage = 19;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemPassword(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_PASSWORD;
	if(pNote->pFlags->locked)
		lvi.iImage = 26;
	else if(IsGroupLocked(pNote->pData->idGroup))
		lvi.iImage = 27;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemPin(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_PIN;
	if(pNote->pPin)
		lvi.iImage = 28;
	else
		lvi.iImage = -1;
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemScheduleDescription(PMEMNOTE pNote, int index){
	wchar_t 			szBuffer[256], szDate[128];
	LVITEMW				lvi;

	szBuffer[0] = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = index;
	lvi.iSubItem = COL_SCHEDULE;
	lvi.pszText = szBuffer;
	switch(pNote->pSchedule->scType){
		case SCH_NO:
			SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
			return;
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
	ParseScheduleType(pNote->pSchedule, szDate);
	wcscat(szBuffer, L" ");
	wcscat(szBuffer, szDate);
	SendMessageW(m_hListMain, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: InsertItem
 Created  : Fri May 25 21:58:50 2007
 Modified : Fri May 25 21:58:50 2007

 Synopsys : Inserts new item into list view
 Input    : hList - list view handle
            hNote - note window handle
            index - index to insert item at
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void InsertItem(HWND hList, PMEMNOTE pNote, int index){

	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	
	lvi.lParam = (LONG)pNote;
	lvi.iItem = index;
	lvi.pszText = pNote->pData->szName;
	lvi.iImage = ItemImageIndex(pNote);
	//insert item
	SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	//set item priority
	SetItemPriority(pNote, index);
	//set item completed
	SetItemCompleted(pNote, index);
	//set item protected
	SetItemProtected(pNote, index);
	//set item password state
	SetItemPassword(pNote, index);
	//set item pinned state
	SetItemPin(pNote, index);
	//set item favorite
	SetItemFavorite(pNote, index);
	//set item sent/received status
	SetItemSR(pNote, index);
	//set item group name
	SetItemGroupName(pNote, index);
	//set item tags
	SetItemTags(pNote, index);
	//set item content
	SetItemContent(pNote, index);
	//set item sent to
	SetItemSentToReceivedFrom(pNote, index, COL_SENT_TO);
	//set item received from
	SetItemSentToReceivedFrom(pNote, index, COL_RECEIVED_FROM);
	//set item sent at
	SetItemSentReceivedDate(pNote, index, COL_SENT_AT);
	//set item received at
	SetItemSentReceivedDate(pNote, index, COL_RECEIVED_AT);
	//set item id
	SetItemID(pNote, index);
	//set item change date
	if(pNote->pData->stChanged.wDay != 0){
		SetItemChangeDate(pNote, index);
	}
	//set item created date
	if(pNote->pCreated->wDay != 0){
		SetItemCreatedDate(pNote, index);
	}
	//set item deleted date
	if(pNote->pRealDeleted->wDay != 0){
		SetItemDetetedDate(pNote, index);
	}
	//set item schedule description
	if(pNote->pSchedule->scType != SCH_NO){
		SetItemScheduleDescription(pNote, index);
	}
}

static void InsertBackupItem(HWND hList, PWIN32_FIND_DATAW pfd, int index){
	LVITEMW				lvi;
	wchar_t				szId[128], szNoteId[128], szDate[128];
	PMEMNOTE			pNote;
	SYSTEMTIME			st;
	FILETIME			lft;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.pszText = szId;
	wcscpy(szId, pfd->cFileName);
	szId[wcslen(szId) - wcslen(BACK_NOTE_EXTENTION)] = '\0';
	lvi.iImage = 20;
	//insert item
	SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	wcscpy(szNoteId, szId);
	szNoteId[wcslen(szNoteId) - 2] = '\0';
	pNote = MemNoteById(szNoteId);
	if(pNote){
		lvi.mask = LVIF_TEXT;
		lvi.pszText = pNote->pData->szName;
		lvi.iSubItem = COL_BACK_ORIGINAL;
		SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	}
	FileTimeToLocalFileTime(&pfd->ftLastWriteTime, &lft);
	FileTimeToSystemTime(&lft, &st);
	ConstructDateTimeString(&st, szDate);
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szDate;
	lvi.iSubItem = COL_BACK_TIME;
	SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static int CALLBACK BackDateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	LVITEMW				lvi;
	WIN32_FIND_DATAW	fd1, fd2;
	wchar_t				file1[MAX_PATH], file2[MAX_PATH], szBuffer1[128], szBuffer2[128];
	HANDLE				handle = INVALID_HANDLE_VALUE;

	*szBuffer1 = '\0';
	*szBuffer2 = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.iItem = lParam1;
	lvi.pszText = szBuffer1;
	SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
	lvi.iItem = lParam2;
	lvi.pszText = szBuffer2;
	SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);

	wcscpy(file1, g_NotePaths.BackupDir);
	wcscat(file1, szBuffer1);
	wcscat(file1, BACK_NOTE_EXTENTION);
	wcscpy(file2, g_NotePaths.BackupDir);
	wcscat(file2, szBuffer2);
	wcscat(file2, BACK_NOTE_EXTENTION);

	handle = FindFirstFileW(file1, &fd1);
	if(handle != INVALID_HANDLE_VALUE){
		FindClose(handle);
		handle = FindFirstFileW(file2, &fd2);
		if(handle != INVALID_HANDLE_VALUE){
			FindClose(handle);
			if((int)lParamSort == LVS_SORTASCENDING)
				return CompareFileTime(&fd1.ftLastWriteTime, &fd2.ftLastWriteTime);
			else
				return CompareFileTime(&fd2.ftLastWriteTime, &fd1.ftLastWriteTime);
		}
	}
	return 0;
}

static int CALLBACK PriorityCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	P_NOTE_DATA		pD1, pD2;
	int				r1, r2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;

	r1 = GetBit(pD1->res1, NB_HIGH_PRIORITY);
	r2 = GetBit(pD2->res1, NB_HIGH_PRIORITY);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

static int CALLBACK ProtectedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	P_NOTE_DATA		pD1, pD2;
	int				r1, r2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;

	r1 = GetBit(pD1->res1, NB_PROTECTED);
	r2 = GetBit(pD2->res1, NB_PROTECTED);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

static int CALLBACK IDCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	PMEMNOTE		p1, p2;
	int				result = 0;

	p1 = (PMEMNOTE)lParam1;
	p2 = (PMEMNOTE)lParam2;
	
	result = _wcsicmp(p1->pFlags->id, p2->pFlags->id);

	if((int)lParamSort == LVS_SORTASCENDING)
		return (result);
	else
		return (result * (-1));
}

static int CALLBACK PinCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	PMEMNOTE		p1, p2;
	int				pin1 = 0, pin2 = 0;

	p1 = (PMEMNOTE)lParam1;
	p2 = (PMEMNOTE)lParam2;

	if(p1->pPin)
		pin1 = 1;
	if(p2->pPin)
		pin2 = 1;
	if((int)lParamSort == LVS_SORTASCENDING)
		return (pin1 - pin2);
	else
		return (pin2 - pin1);
}

static int CALLBACK PasswordCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	PMEMNOTE		p1, p2;

	p1 = (PMEMNOTE)lParam1;
	p2 = (PMEMNOTE)lParam2;

	if((int)lParamSort == LVS_SORTASCENDING){
		if(p1->pFlags->locked != p2->pFlags->locked)
			return (p1->pFlags->locked - p2->pFlags->locked);
		else
			return IsGroupLocked(p1->pData->idGroup) - IsGroupLocked(p2->pData->idGroup);
	}
	else{
		if(p1->pFlags->locked != p2->pFlags->locked)
			return (p2->pFlags->locked - p1->pFlags->locked);
		else
			return IsGroupLocked(p2->pData->idGroup) - IsGroupLocked(p1->pData->idGroup);
	}
}

static int CALLBACK FavoritesCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	PMEMNOTE		p1, p2;
	int				len1 = 0, len2 = 0;
	// wchar_t			szBuffer[256];

	p1 = (PMEMNOTE)lParam1;
	p2 = (PMEMNOTE)lParam2;

	if(p1->pRTHandles->favorite == GROUP_FAVORITES)
		len1 = 1;
	if(p2->pRTHandles->favorite == GROUP_FAVORITES)
		len2 = 1;
	// GetPrivateProfileStringW(S_FAVORITES, p1->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	// len1 = wcslen(szBuffer);
	// GetPrivateProfileStringW(S_FAVORITES, p2->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	// len2 = wcslen(szBuffer);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (len1 - len2);
	else
		return (len2 - len1);
}

static int CALLBACK SRCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	LPSRSTATUS		pD1, pD2;
	int				r1 = 0, r2 = 0;

	pD1 = ((PMEMNOTE)lParam1)->pSRStatus;
	pD2 = ((PMEMNOTE)lParam2)->pSRStatus;

	if(pD1->lastSent.wDay != 0 && pD1->lastRec.wDay != 0)
		r1 = 3;
	else if(pD1->lastSent.wDay != 0)
		r1 = 2;
	else if(pD1->lastRec.wDay != 0)
		r1 = 1;

	if(pD2->lastSent.wDay != 0 && pD2->lastRec.wDay != 0)
		r2 = 3;
	else if(pD2->lastSent.wDay != 0)
		r2 = 2;
	else if(pD2->lastRec.wDay != 0)
		r2 = 1;

	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

static int CALLBACK CompletedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	P_NOTE_DATA		pD1, pD2;
	int				r1, r2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;

	r1 = GetBit(pD1->res1, NB_COMPLETED);
	r2 = GetBit(pD2->res1, NB_COMPLETED);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

static int CALLBACK NameCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	wchar_t				szName1[128], szName2[128];
	LVITEMW				lvi;
	P_LVSORT_MEMBER		plvs = (P_LVSORT_MEMBER)lParamSort;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = szName1;
	lvi.iSubItem = plvs->column;
	SendMessageW(m_hListMain, LVM_GETITEMTEXTW, (WPARAM)lParam1, (LPARAM)&lvi);
	lvi.pszText = szName2;
	SendMessageW(m_hListMain, LVM_GETITEMTEXTW, (WPARAM)lParam2, (LPARAM)&lvi);
	if(plvs->order == LVS_SORTASCENDING)
		return _wcsicmp(szName1, szName2);
	else
		return _wcsicmp(szName2, szName1);
}

static BOOL CALLBACK NewGroup_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, NewGroup_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, NewGroup_OnInitDialog);
		HANDLE_MSG (hwnd, WM_DESTROY, NewGroup_OnDestroy);
		HANDLE_MSG (hwnd, WM_DRAWITEM, NewGroup_OnDrawItem);

		case GCN_COLOR_SELECTED:{
			LPPNGROUP		ppg = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
			if(lParam == 0)
				lParam = DEF_TEXT_COLOR;
			if(ppg->crWindow != (COLORREF)lParam){
				ppg->crWindow = (COLORREF)lParam;
				ppg->customCRWindow = true;
			}
			return true;
		}
		default: return FALSE;
	}
}

static void NewGroup_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	int		data = SendMessageW(lpDrawItem->hwndItem, CB_GETITEMDATA, lpDrawItem->itemID, 0);
	RECT	rc = lpDrawItem->rcItem;
	HBRUSH	hbr;

	InflateRect(&rc, -1, -1);
	hbr = CreateSolidBrush((COLORREF)data);
	FillRect(lpDrawItem->hDC, &rc, hbr);
	DeleteBrush(hbr);
}

static void NewGroup_OnDestroy(HWND hwnd)
{
	LPPNGROUP		ppg;

	m_hGroups = NULL;
	ppg = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
	if(ppg){
		free(ppg);
	}
	RemovePropW(hwnd, PROP_GRP);
}

static void NewGroup_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			EndDialog(hwnd, id);
			break;
		case IDOK:{
			LPPNGROUP	ppg = (LPPNGROUP)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			LPPNGROUP	temp = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
			ppg->image = temp->image;
			GetDlgItemTextW(hwnd, IDC_EDT_GROUP_NAME, ppg->szName, 128);
			if(ppg->crWindow != temp->crWindow){
				ppg->crWindow = temp->crWindow;
				ppg->customCRWindow = temp->customCRWindow;
				ApplyNewBackgroundColorForGroup(ppg);
			}
			if(ppg->crCaption != temp->crCaption){
				ppg->crCaption = temp->crCaption;
				ppg->customCRCaption = temp->customCRCaption;
				ApplyNewCaptionColorForGroup(ppg);
			}
			if(ppg->crFont != temp->crFont){
				ppg->crFont = temp->crFont;
				ppg->customCRFont = temp->customCRFont;
				ApplyNewTextColorForGroup(ppg);
			}
			EndDialog(hwnd, id);
			break;
		}
		case IDC_CBO_CF_COLOR:{
			if(codeNotify == CBN_SELENDOK){
				LPPNGROUP	temp = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
				COLORREF 	color = (COLORREF)SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_GETCURSEL, 0, 0), 0);
				if(color == 0)
					color = DEF_TEXT_COLOR;
				if(temp->crCaption != color){
					temp->crCaption = color;
					temp->customCRCaption = true;
				}
			}
			break;
		}
		case IDC_CBO_FN_COLOR:{
			if(codeNotify == CBN_SELENDOK){
				LPPNGROUP	temp = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
				COLORREF 	color = (COLORREF)SendDlgItemMessageW(hwnd, IDC_CBO_FN_COLOR, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_FN_COLOR, CB_GETCURSEL, 0, 0), 0);
				if(color == 0)
					color = DEF_TEXT_COLOR;
				if(temp->crFont != color){
					temp->crFont = color;
					temp->customCRFont = true;
				}
			}
			break;
		}
		case IDC_EDT_GROUP_NAME:{
			if(codeNotify == EN_CHANGE){
				int len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_GROUP_NAME));
				if(len > 0)
					EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			}
			break;
		}
		case IDC_CMD_GROUP_BMP:
			if(codeNotify == BN_CLICKED){
				LPPNGROUP	temp = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
				int 		result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CHOOSE_BMP), hwnd, Bitmaps_DlgProc, temp->image);
				if(result > -1){
					int		imageIndex = result + m_gOffset;
					SendDlgItemMessageW(hwnd, IDC_ST_GROUP_BMP, STM_SETIMAGE, IMAGE_ICON, (LPARAM)ImageList_GetIcon(m_hImlTreeNormal, imageIndex, ILD_TRANSPARENT));
					temp->image = result;
				}
			}
			break;
		case IDC_CMD_STD_VIEW:
			if(codeNotify == BN_CLICKED){
				COLORREF	color;
				int			data, count, index;
				LPPNGROUP	temp = (LPPNGROUP)GetPropW(hwnd, PROP_GRP);
				temp->crWindow = g_Appearance.crWindow;
				temp->customCRWindow = false;
				SendDlgItemMessageW(hwnd, IDC_CHOOSE_COLOR, GCM_NEW_COLOR, 0, temp->crWindow);
				count = SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_GETCOUNT, 0, 0);
				temp->crCaption = g_Appearance.crCaption;
				temp->customCRCaption = false;
				color = temp->crCaption;
				if(color == DEF_TEXT_COLOR)
					color = 0;
				for(int i = 0; i < count; i++){
					data = SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_GETITEMDATA, i, 0);
					if(color == data){
						SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_SETCURSEL, i, 0);
						break;
					}
				}
				temp->crFont = g_Appearance.crFont;
				temp->customCRFont = false;
				color = temp->crFont;
				if(color == DEF_TEXT_COLOR)
					color = 0;
				for(int i = 0; i < count; i++){
					data = SendDlgItemMessageW(hwnd, IDC_CBO_FN_COLOR, CB_GETITEMDATA, i, 0);
					if(color == data){
						SendDlgItemMessageW(hwnd, IDC_CBO_FN_COLOR, CB_SETCURSEL, i, 0);
						break;
					}
				}
				wcscpy(temp->szSkin, g_Appearance.szSkin);
				temp->customSkin = false;
				index = SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_FINDSTRINGEXACT, -1, (LPARAM)temp->szSkin);
				SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_SETCURSEL, index, 0);
			}
			break;
	}
}

static BOOL NewGroup_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPPNGROUP		ppg, temp;
	int				count, data, style, imageIndex, images = ImageList_GetImageCount(m_hImlTreeNormal);
	wchar_t 		szBuffer[256];
	RECT			rc;
	HWND			hPicker;
	COLORREF		crWindow, crCaption, crFont;

	m_hGroups = hwnd;
	ppg = (LPPNGROUP)lParam;
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)ppg);
	if(ppg->image + m_gOffset < images)
		imageIndex = ppg->image + m_gOffset;
	else
		imageIndex = m_gOffset;
	temp = calloc(1, sizeof(PNGROUP));
	memcpy(temp, ppg, sizeof(PNGROUP));
	
	if(ppg->id == NEW_GROUP_ID){
		GetPrivateProfileStringW(S_CAPTIONS, L"group_new", DS_GROUP_CAPTION_NEW, szBuffer, 128, g_NotePaths.CurrLanguagePath);
		temp->crWindow = g_Appearance.crWindow;
	}
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"group_edit", DS_GROUP_CAPTION_EDIT, szBuffer, 128, g_NotePaths.CurrLanguagePath);
		SetDlgItemTextW(hwnd, IDC_EDT_GROUP_NAME, ppg->szName);
	}
	SetPropW(hwnd, PROP_GRP, (HANDLE)temp);

	SetWindowTextW(hwnd, szBuffer);

	SetDlgCtlText(hwnd, IDC_ST_GROUP_NAME, g_NotePaths.CurrLanguagePath, DS_CHOOSE_GROUP_NAME);
	SetDlgCtlText(hwnd, IDC_CMD_GROUP_BMP, g_NotePaths.CurrLanguagePath, DS_CHOOSE_GROUP_ICON);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_GRP_SKINLESS_PROPS, g_NotePaths.CurrLanguagePath, L"Skinless notes");
	SetDlgCtlText(hwnd, IDC_ST_BCKG_COLOR, g_NotePaths.CurrLanguagePath, L"Background");
	SetDlgCtlText(hwnd, IDC_CMD_FN_COLOR, g_NotePaths.CurrLanguagePath, L"Font color");
	SetDlgCtlText(hwnd, IDC_CMD_CF_COLOR, g_NotePaths.CurrLanguagePath, L"Caption font color");
	SetDlgCtlText(hwnd, IDC_GRP_SKINS, g_NotePaths.CurrLanguagePath, L"Skins");
	SetDlgCtlText(hwnd, IDC_CMD_STD_VIEW, g_NotePaths.CurrLanguagePath, L"Standard view");

	style = GetWindowLongPtrW(GetDlgItem(hwnd, IDC_ST_GROUP_BMP), GWL_STYLE);
	style |= SS_ICON;
	SetWindowLongPtrW(GetDlgItem(hwnd, IDC_ST_GROUP_BMP), GWL_STYLE, style);
	SendDlgItemMessageW(hwnd, IDC_ST_GROUP_BMP, STM_SETIMAGE, IMAGE_ICON, (LPARAM)ImageList_GetIcon(m_hImlTreeNormal, imageIndex, ILD_TRANSPARENT));

	SetFocus(GetDlgItem(hwnd, IDC_EDT_GROUP_NAME));
	SendDlgItemMessageW(hwnd, IDC_EDT_GROUP_NAME, EM_SETSEL, 0, -1);

	count = FillGroupColors(GetDlgItem(hwnd, IDC_CBO_CF_COLOR), GetDlgItem(hwnd, IDC_CBO_FN_COLOR));
	if(ppg->crCaption == DEF_TEXT_COLOR)
		crCaption = 0;
	else
		crCaption = ppg->crCaption;
	if(ppg->crFont == DEF_TEXT_COLOR)
		crFont = 0;
	else
		crFont = ppg->crFont;
	for(int i = 0; i < count; i++){
		data = SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_GETITEMDATA, i, 0);
		if(crCaption == data){
			SendDlgItemMessageW(hwnd, IDC_CBO_CF_COLOR, CB_SETCURSEL, i, 0);
			break;
		}
	}
	for(int i = 0; i < count; i++){
		data = SendDlgItemMessageW(hwnd, IDC_CBO_FN_COLOR, CB_GETITEMDATA, i, 0);
		if(crFont == data){
			SendDlgItemMessageW(hwnd, IDC_CBO_FN_COLOR, CB_SETCURSEL, i, 0);
			break;
		}
	}
	FillSkinsCombo(hwnd, ppg->szSkin);
	if(g_RTHandles.hbSkin == NULL)
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_SKINS), false);

	if(ppg->crWindow == 0)
		crWindow = g_Appearance.crWindow;
	else
		crWindow = ppg->crWindow;
	GetWindowRect(GetDlgItem(hwnd, IDC_PL_1), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	hPicker = CreateGCPickerWindowW(rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), hwnd, IDC_CHOOSE_COLOR, crWindow);
	BringWindowToTop(hPicker);
	return FALSE;
}

static int FillGroupColors(HWND hCombo1, HWND hCombo2){
	int			index;

	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"0");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 0);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"0");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 0);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"8388608");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 8388608);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"8388608");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 8388608);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"32768");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 32768);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"32768");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 32768);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"8421376");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 8421376);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"8421376");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 8421376);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"128");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 128);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"128");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 128);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"8388736");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 8388736);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"8388736");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 8388736);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"32896");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 32896);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"32896");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 32896);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"12632256");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 12632256);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"12632256");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 12632256);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"8421504");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 8421504);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"8421504");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 8421504);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"16711680");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 16711680);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"16711680");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 16711680);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"65280");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 65280);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"65280");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 65280);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"16776960");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 16776960);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"16776960");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 16776960);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"255");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 255);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"255");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 255);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"16711935");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 16711935);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"16711935");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 16711935);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"65535");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 65535);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"65535");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 65535);
	index = SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)L"16777215");
	SendMessageW(hCombo1, CB_SETITEMDATA, index, 16777215);
	index = SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)L"16777215");
	SendMessageW(hCombo2, CB_SETITEMDATA, index, 16777215);

	return(SendMessageW(hCombo1, CB_GETCOUNT, 0, 0));
}

static void FillSkinsCombo(HWND hwnd, const wchar_t * lpSkinCurrent){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	wchar_t				szSkin[256];
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL 				result = TRUE;
	LRESULT 			added;

	wcscpy(filespec, g_NotePaths.SkinDir);
	wcscat(filespec, L"*.skn");
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szSkin, fd.cFileName);
			szSkin[wcslen(szSkin) - 4] = '\0';
			added = SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_ADDSTRING, 0, (LPARAM)szSkin);
			if(wcscmp(fd.cFileName, lpSkinCurrent) == 0){
				SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_SETCURSEL, added, 0);
			}
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
}

static BOOL CALLBACK Bitmaps_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR			lpnm;

	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Bitmaps_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Bitmaps_OnInitDialog);
	HANDLE_MSG (hwnd, WM_DESTROY, Bitmaps_OnDestroy);

	case WM_NOTIFY:{
		BOOL	isCust = (BOOL)GetPropW(hwnd, PROP_IT);
    	lpnm = (LPNMHDR)lParam;
		switch(lpnm->code){
			case NM_DBLCLK:
				if(!isCust){
					if(lpnm->idFrom == IDC_LST_CHOOSE_BMP)
						SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
					return TRUE;
				}
				else
					return FALSE;
			case LVN_ITEMCHANGED:
				if(isCust){
					if(SendDlgItemMessageW(hwnd, IDC_LST_CHOOSE_BMP, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED) >= 0){
						if(ListView_GetItemCount(GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP)) > 1)
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), TRUE);
						else
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), FALSE);
					}
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), FALSE);
				}
				return FALSE;
			default:
				return FALSE;
				}
		}
	default: return FALSE;
	}
}

static void Bitmaps_OnDestroy(HWND hwnd)
{
	m_hBitmaps = NULL;
	RemovePropW(hwnd, PROP_GROUP_IML);
	RemovePropW(hwnd, PROP_IT);
}

static void Bitmaps_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:{
			int result;
			if((BOOL)GetPropW(hwnd, PROP_IT))
				result = IDCANCEL;
			else
				result = -1;
			EndDialog(hwnd, result);
			break;
		}
		case IDOK:{
			int index = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP), -1, LVNI_SELECTED);
			int result;
			if((BOOL)GetPropW(hwnd, PROP_IT))
				result = IDOK;
			else
				result = index;
			EndDialog(hwnd, result);
			break;
		}
		case IDC_CMD_ADD_SOUND:
			AddGroupIcon(hwnd);
			break;
		case IDC_CMD_DEL_SOUND:
			RemoveGroupIcon(hwnd);
			break;
	}
}

static BOOL Bitmaps_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LVITEMW			lvi;
	int 			count, selected = -1;
	wchar_t			szKey[12];
	HIMAGELIST		iml;
	wchar_t 		szBuffer[256];
	RECT			rc, rc1;
	HWND			hList;
	wchar_t			szTempPath[MAX_PATH - 14];
	HMODULE			hGroups;

	m_hBitmaps = hwnd;
	if(lParam >= 0)
		SetPropW(hwnd, PROP_IT, (HANDLE)FALSE);
	else{
		SetPropW(hwnd, PROP_IT, (HANDLE)TRUE);
		GetTempPathW(MAX_PATH - 14, szTempPath);
		GetTempFileNameW(szTempPath, L"gim", 0, m_TempLib);
		CopyFileW(m_ImagesPath, m_TempLib, FALSE);
	}

	hList = GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP);
	
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_CMD_ADD_SOUND, g_NotePaths.CurrLanguagePath, L"Add");
	SetDlgCtlText(hwnd, IDC_CMD_DEL_SOUND, g_NotePaths.CurrLanguagePath, L"Remove");
	SetDlgCtlText(hwnd, IDC_ST_GR_ICONS_TEXT, g_NotePaths.CurrLanguagePath, L"In order to add/remove icons use appropriate toolbar button or context menu item.");
	if(lParam >= 0){
		GetPrivateProfileStringW(S_OPTIONS, L"1014", DS_PICK_ICON, szBuffer, 128, g_NotePaths.CurrLanguagePath);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_ADD_SOUND), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), SW_HIDE);
	}
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"customize_icons", L"Customize icons", szBuffer, 128, g_NotePaths.CurrLanguagePath);
		ShowWindow(GetDlgItem(hwnd, IDC_ST_GR_ICONS_TEXT), SW_HIDE);
	}
	SetWindowTextW(hwnd, szBuffer);

	iml = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 0);
	SetPropW(hwnd, PROP_GROUP_IML, (HANDLE)iml);
	hGroups = LoadLibraryExW(m_ImagesPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hGroups){
		EnumResourceNames(hGroups, RT_BITMAP, EnumResNameProc, (LPARAM)iml);
	}

	count = ImageList_GetImageCount(iml);
	ListView_SetImageList(hList, iml, LVSIL_SMALL);
	
	for(int i = 0; i < count; i++){		
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
		lvi.iImage = i;
		lvi.iItem = i;
		lvi.lParam = i + 1;
		if(i == lParam){
			lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
			lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
			selected = i;
		}
		_itow(i + 1, szKey, 10);
		lvi.pszText = szKey;
		SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	}
	
	if(lParam < 0){
		m_BitmapIndex = 0;
		//assign resource id to each list view item
		EnumResourceNames(hGroups, RT_BITMAP, GetGoupsIconsIdentifiersProc, (LPARAM)hList);
	}
	else{
		if(selected >= 0)
			ListView_EnsureVisible(hList, selected, FALSE);
	}

	if(hGroups){
		FreeLibrary(hGroups);
		hGroups = NULL;
	}
	GetWindowRect(hList, &rc);
	GetClientRect(hwnd, &rc1);
	MoveWindow(hList, 0, 0, rc1.right - rc1.left, rc.bottom - rc.top, TRUE);
	SetFocus(hList);
	return FALSE;
}

static BOOL CALLBACK GetGoupsIconsIdentifiersProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam){
	if(IS_INTRESOURCE(lpszName)){
		LVITEMW		lvi;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = m_BitmapIndex;
		SendMessageW((HWND)lParam, LVM_GETITEMW, 0, (LPARAM)&lvi);
		lvi.lParam = LPARAM_FROM_LPCTSTR(lpszName);
		SendMessageW((HWND)lParam, LVM_SETITEMW, 0, (LPARAM)&lvi);
		m_BitmapIndex++;
	}
	return TRUE;
}

static void ShowAllGroups(int iParent, HTREEITEM hItem){
	LPPNGROUP		pTemp = g_PGroups;
	HTREEITEM		hInserted;

	while(pTemp){
		if(pTemp->parent == iParent){
			hInserted = InsertGroup(hItem, pTemp, TRUE, FALSE);
			ShowAllGroups(pTemp->id, hInserted);
		}
		pTemp = pTemp->next;
	}
}

static HTREEITEM InsertGroup(HTREEITEM hItem, LPPNGROUP ppg, BOOL fSetParent, BOOL fSave){
	TV_INSERTSTRUCTW	tvs;
	wchar_t				szBuffer[128], szCount[16];
	HTREEITEM			hInserted = NULL;
	int					count, images, imageIndex;

	images = ImageList_GetImageCount(m_hImlTreeNormal);

	if(fSetParent){
		TVITEMW	tvi;
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem = hItem;
		SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
		ppg->parent = tvi.lParam;
	}

	ZeroMemory(&tvs, sizeof(tvs));
	tvs.hParent = hItem;
	tvs.hInsertAfter = TVI_SORT;
	tvs.item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
	if(fSave){
		ppg->id = PNGroupsNewId(g_PGroups);
		PNGroupsAdd(&g_PGroups,ppg);
		SaveGroup(ppg);
	}
	tvs.item.lParam = ppg->id;
	if(ppg->image + m_gOffset < images)
		imageIndex = ppg->image + m_gOffset;
	else
		imageIndex = m_gOffset;
	tvs.item.iImage = imageIndex;
	tvs.item.iSelectedImage = imageIndex;
	wcscpy(szBuffer, ppg->szName);
	count = CountNotesInGroup(ppg->id);
	_itow(count, szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	if(IsGroupLocked(ppg->id))
		wcscat(szBuffer, PWRD_PROTECTED_GROUP);
	tvs.item.pszText = szBuffer;
	
	hInserted = (HTREEITEM)SendMessageW(m_hTreeMain, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	SendMessageW(m_hTreeMain, TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem);
	
	return hInserted;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FindItemById
 Created  : Thu Mar  6 08:51:37 2008
 Modified : Thu Mar  6 08:51:37 2008

 Synopsys : Recursively searches for tree view item by group id
 Input    : hItem - tree view item to start from
            id - group id
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void FindItemById(HTREEITEM hItem, int id){
	HTREEITEM	hi;
	TVITEMW		tvi;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
	if(tvi.lParam == id){
		m_tItem = hItem;
		return;
	}
	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi){
		FindItemById(hi, id);
	}
	hi = TreeView_GetNextSibling(m_hTreeMain, hItem);
	if(hi){
		FindItemById(hi, id);
	}
}

static void RefreshTreeviewItemsIcons(HTREEITEM hItem, int lastIndex){
	HTREEITEM	hi;
	TVITEMW		tvi;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvi.hItem = hItem;
	SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
	if(tvi.iImage > lastIndex){
		tvi.iImage = m_gOffset;
		tvi.iSelectedImage = m_gOffset;
		SendMessageW(m_hTreeMain, TVM_SETITEMW, 0, (LPARAM)&tvi);
	}
	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi)
		RefreshTreeviewItemsIcons(hi, lastIndex);
	hi = TreeView_GetNextSibling(m_hTreeMain, hItem);
	if(hi)
		RefreshTreeviewItemsIcons(hi, lastIndex);
}

static int GetSelectedTVItemId(void){
	HTREEITEM	hi;

	hi = TreeView_GetSelection(m_hTreeMain);
	if(hi){
		return GetTVItemId(hi);
	}
	return -1;
}

static void ShowGroup(HTREEITEM hItem){
	int 		group = GetTVItemId(hItem);
	if(UnlockGroup(group))
		ShowGroupOfNotes(group);
}

static void HideGroup(HTREEITEM hItem){
	int 		group = GetTVItemId(hItem);
	HideGroupOfNotes(group);
}

static void ShowBranch(HTREEITEM hItem, BOOL fGetSibling){
	HTREEITEM		hi;

	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi){
		ShowBranch(hi, TRUE);
	}
	if(fGetSibling){
		hi = TreeView_GetNextSibling(m_hTreeMain, hItem);
		if(hi){
			ShowBranch(hi, fGetSibling);
		}
	}
	ShowGroup(hItem);
}

static void HideBranch(HTREEITEM hItem, BOOL fGetSibling){
	HTREEITEM		hi;

	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi){
		HideBranch(hi, TRUE);
	}
	if(fGetSibling){
		hi = TreeView_GetNextSibling(m_hTreeMain, hItem);
		if(hi){
			HideBranch(hi, fGetSibling);
		}
	}
	HideGroup(hItem);
}

static void CheckGroupVisibility(P_CPENUM pcp){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
			if(pNote->pData->idGroup == pcp->group){
				if(pNote->pData->visible)
					pcp->result |= VISIBLE_ONLY;
				else
					pcp->result |= HIDDEN_ONLY;
			}
		}
		pNote = pNote->next;
	}
}

static void CheckBranchVisibility(HTREEITEM hItem, BOOL fGetSibling){
	HTREEITEM		hi;
	int				id;
	CPENUM			cp;
	LPPNGROUP		ppg;

	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi){
		CheckBranchVisibility(hi, TRUE);
	}
	if(fGetSibling){
		hi = TreeView_GetNextSibling(m_hTreeMain, hItem);
		if(hi){
			CheckBranchVisibility(hi, fGetSibling);
		}
	}
	id = GetTVItemId(hItem);
	if(id >= 0){
		ppg = PNGroupsGroupById(g_PGroups, id);
		if(ppg){
			cp.group = ppg->id;
			cp.result = 0;
			CheckGroupVisibility(&cp);
			m_BranchVisibility |= cp.result;
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DeleteTreeGroup
 Created  : Thu Mar  6 08:45:58 2008
 Modified : Thu Mar  6 08:45:58 2008

 Synopsys : Recursively deletes group
 Input    : hItem - tree view item to delete
            fGetSibling - whether sibling of hItem should be deleted as 
            well
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void DeleteTreeGroup(HTREEITEM hItem, BOOL fGetSibling){
	HTREEITEM	hi;
	int			id;

	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi){
		//continue until there is no child
		DeleteTreeGroup(hi, TRUE);
	}
	if(fGetSibling){
		//delete siblings
		hi = TreeView_GetNextSibling(m_hTreeMain, hItem);
		if(hi){
			DeleteTreeGroup(hi, fGetSibling);
		}
	}
	id = GetTVItemId(hItem);
	if(id >= 0){
		//delete group
		PNGroupsDelete(g_PGroups, id);
		//delete all notes belong to group
		DeleteNotesGroup(id);
		//delete tree view item
		TreeView_DeleteItem(m_hTreeMain, hItem);
	}
}

static void DeleteNotesGroup(int group){
	PMEMNOTE	pNote = MemoryNotes(), pNext;

	while(pNote){
		pNext = pNote->next;
		if(pNote->pData->idGroup == group){
			DeleteToBin(pNote, 0);
		}
		pNote = pNext;
	}
	//update favorites
	FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_FAVORITES);
	UpdateGroupText(GROUP_FAVORITES, m_tItem);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetTVItemId
 Created  : Thu Mar  6 08:43:50 2008
 Modified : Thu Mar  6 08:43:50 2008

 Synopsys : Gets group id of tree view item
 Input    : hItem - tree view item
 Output   : Group id of tree view item
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int GetTVItemId(HTREEITEM hItem){
	TVITEMW		tvi;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	if(SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi)){
		return tvi.lParam;
	}
	return -1;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ShowTreePopUp
 Created  : Wed Feb 13 08:25:33 2008
 Modified : Wed Wed Feb 13 08:25:33 2008

 Synopsys : Shows tree view popup menu
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void ShowTreePopUp(void){
	POINT pt;

	GetCursorPos(&pt);
	//set window foreground for further PostMessageW call 
	SetForegroundWindow(m_hDTree);
	//show popup menu
	TrackPopupMenu(m_hPUMenuTree, TPM_RIGHTALIGN, pt.x, pt.y, 0, m_hDTree, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(m_hDTree, WM_NULL, 0, 0);
}

static void ShowRecyclePopUp(void){
	POINT pt;

	GetCursorPos(&pt);
	//set window foreground for further PostMessageW call 
	SetForegroundWindow(m_hCtrlPanel);
	//show popup menu
	TrackPopupMenu(m_hPUMenuRecycle, TPM_RIGHTALIGN, pt.x, pt.y, 0, m_hCtrlPanel, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(m_hCtrlPanel, WM_NULL, 0, 0);
}

static HTREEITEM GetDropHighLightedItem(int x, int y){
	TVHITTESTINFO		tvh;
	RECT				rc;

	GetWindowRect(m_hTreeMain, &rc);
	MapWindowPoints(HWND_DESKTOP, m_hCtrlPanel, (LPPOINT)&rc, 2);
	tvh.flags = TVHT_ONITEMICON | TVHT_ONITEMLABEL;
	tvh.hItem = NULL;
	tvh.pt.x = x;
	tvh.pt.y = y - rc.top;
	
	return TreeView_HitTest(m_hTreeMain, &tvh);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: IsItemFromChildBranch
 Created  : Thu Mar  6 08:40:28 2008
 Modified : Thu Mar  6 08:40:28 2008

 Synopsys : Checks whether dragged tree view item belongs to one of child 
            branches of highlighted tree view item
 Input    : hDragged - dragged item
            hHighlighted - highlighted item
 Output   : TRUE/FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static BOOL IsItemFromChildBranch(HTREEITEM hDragged, HTREEITEM hHighlighted){
	HTREEITEM	hTemp;

	hTemp = TreeView_GetParent(m_hTreeMain, hHighlighted);
	while(hTemp){
		if(hTemp == hDragged){
			return TRUE;
		}
		hTemp = TreeView_GetParent(m_hTreeMain, hTemp);
	}
	return FALSE;
}

static void SelectedBackItem(wchar_t * lpBuffer){
	int			index;
	LVITEMW		lvi;

	*lpBuffer = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = lpBuffer;
	index = ListView_GetNextItem(m_hListMain, -1, LVNI_SELECTED);
	if(index > -1){
		lvi.iItem = index;
		SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
	}
}

static PMEMNOTE SelectedNote(void){
	int			index;
	LVITEMW		lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;

	index = ListView_GetNextItem(m_hListMain, -1, LVNI_SELECTED);
	if(index > -1){
		lvi.iItem = index;
		SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if(lvi.lParam){
			return (PMEMNOTE)lvi.lParam;
		}
	}
	return NULL;
}

static void RestoreBackupToNote(void){
	LVITEMW		lvi;
	wchar_t		szBuffer[128], szTitle[128], szMessage[256], filespec[MAX_PATH];
	int			index;
	PMEMNOTE	pNote;

	GetPrivateProfileStringW(S_MESSAGES, L"restore_backup", L"Do you want to restore data from selected backup file?", szMessage, 256, g_NotePaths.CurrLanguagePath);
	if(MessageBoxW(g_hCPDialog, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES){
		index = ListView_GetNextItem(m_hListMain, -1, LVNI_ALL | LVNI_SELECTED);
		*szBuffer = '\0';
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = 128;
		lvi.pszText = szBuffer;
		SendMessageW(m_hListMain, LVM_GETITEMTEXTW, index, (LPARAM)&lvi);
		if(wcslen(szBuffer) > 0){
			wcscpy(filespec, g_NotePaths.BackupDir);
			wcscat(filespec, szBuffer);
			wcscat(filespec, BACK_NOTE_EXTENTION);
			wcscpy(szTitle, szBuffer);
			wchar_t		*temp = szBuffer;
			while(*temp){
				if(*temp == '_'){
					*temp = '\0';
					break;
				}
				temp++;
			}
			pNote = MemNoteById(szBuffer);
			if(pNote){
				if(!pNote->pData->visible){
					if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
						ShowNote(pNote);
				}
				ReadRestoreRTFFile((HWND)GetPropW(pNote->hwnd, PH_EDIT), filespec);
				pNote->pFlags->saved = FALSE;
				wcscpy(pNote->pFlags->idBackup, szTitle);
			}
		}
	}
}

static void RestoreNoteFromBackup(void){
	PMEMNOTE	pNote = SelectedNote();

	if(pNote){
		if(!pNote->pData->visible)
			ShowItems();
		SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_RESTORE_BACKUP, 0), 0);
	}
}

static void DeleteSelectedFromBin(void){
	int			count;
	LONG_PTR	*pNotes, *pFree;
	PMEMNOTE	pNote;

	if(MessageBoxW(m_hCtrlPanel, g_Strings.DMessageMultiFromBin, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
		pNotes = SelectedPItems(&count);
		if(pNotes){
			pFree = pNotes;
			//and now we can delete all selected notes from Recycle bin
			for(int i = 0; i < count; i++){
				pNote = (PMEMNOTE)*pNotes++;
				DeleteNoteCompletely(pNote);
			}
			free(pFree);
			SendMessageW(m_hCtrlPanel, PNM_RELOAD, 0, 0);	
			UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTreeMain, TreeView_GetRoot(m_hTreeMain)));
		}
	}
}

static void ToggleItemsProtection(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ToggleNoteProtection(pNote);
		}
		free(pFree);
	}
}
static void ToggleItemsPriority(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ToggleNotePriority(pNote);
		}
		free(pFree);
	}
}

static void ToggleItemsCompleted(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ToggleNoteCompleted(pNote);
		}
		free(pFree);
	}
}

static void RollUnrollItems(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			if(pNote->pRTHandles->rolled)
				UnrollNote(pNote);
			else
				RollNote(pNote);
		}
		free(pFree);
	}
}

static void ToggleLock(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		if(!pNote->pFlags->locked){
			wchar_t			szNewPwrd[256];

			*szNewPwrd = '\0';
			if(ShowLoginDialog(g_hInstance, m_hCtrlPanel, DLG_LOGIN_CREATE, szNewPwrd, LDT_NOTE, pNote) == IDOK){
				pNote->pFlags->locked = true;
				WritePrivateProfileStringW(pNote->pFlags->id, IK_LOCK, szNewPwrd, g_NotePaths.DataFile);
			}
		}
		else{
			if(ShowLoginDialog(g_hInstance, m_hCtrlPanel, DLG_LOGIN_MAIN, NULL, LDT_NOTE, pNote) == IDOK){
				pNote->pFlags->locked = false;
				WritePrivateProfileStringW(pNote->pFlags->id, IK_LOCK, NULL, g_NotePaths.DataFile);
			}
		}
		if(pNote->pData->visible){
			RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
			ShowNoteMarks(pNote->hwnd);
		}
		UpdateNoteItem(pNote);
	}
}

static void SendItemsToMultipleContacts(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			LPPCONTACT		lpc = g_PContacts;
			while(lpc){
				if(lpc->prop.send)
					SendNoteToContact(g_hCPDialog, pNote, lpc->prop.name);
				lpc = lpc->next;
			}
		}
		free(pFree);
	}
	ClearMultipleNames();
}

static void SendItemsToContact(wchar_t * lpContact){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			SendNoteToContact(g_hCPDialog, pNote, lpContact);
		}
		free(pFree);
	}
}

static void TogglePins(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			if(!pNote->pPin){
				AddPin(g_hCPDialog, pNote);
			}
			else{
				RemovePin(pNote);
			}
		}
		free(pFree);
	}
}

static void HideItems(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			if(pNote->pData->visible){
				HideNote(pNote, false);
			}
		}
		free(pFree);
	}
}

static void ShowItems(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
				ShowNote(pNote);
		}
		free(pFree);
	}
}

static void AddItemsToFavorites(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			AddToFavorites(pNote);
		}
		free(pFree);
	}
}

static void PrintItem(void){
	PMEMNOTE	pNote = SelectedNote();
	HWND		hEdit;

	if(pNote){
		if(pNote->pData->visible)
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		else{
			hEdit = GetUnvisibleNoteEdit(pNote);
		}
		if(hEdit){
			PrintEdit(hEdit, pNote->pData->szName);
		}
	}
}

static void SendItemByEmail(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		SendNoteByEmail(pNote);
	}
}

static void SendItemAsAttachment(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;

	if(pNotes){
		pFree = pNotes;
		if(count == 1){
			SendNoteAsAttachment((PMEMNOTE)*pNotes);
		}
		else{
			SendMultipleNotesAsAttachments(pNotes, count);
		}
		free(pFree);
	}

	//PMEMNOTE	pNote = SelectedNote();
	//if(pNote){
		//SendNoteAsAttachment(pNote);
	//}
}

static void SaveItem(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		SavePNote(pNote);
	}
}

static void SaveItemAs(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		SaveNoteAs(pNote, 1);
	}
}

static void RestoreCenterItem(int operation){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		CenterRestorePlacementNote(pNote, operation, FALSE);
	}
}

static void AdjustItem(int type){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		if(pNote->pData->visible){
			if(type == 0)
				SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADJUST_APPEARANCE, 0), 0);
			else
				SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADJUST_SCHEDULE, 0), 0);
		}
		else{
			ADJUST_STRUCT		as;

			as.pNote = pNote;
			as.type = type;
			DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_ADJUST), g_hCPDialog, Adjust_DlgProc, (LPARAM)&as);
			g_hAdjust = NULL;
		}
	}
}

static BOOL RestoreAllFromBin(void){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup == GROUP_RECYCLE){
			RestoreNoteFromBin(pNote, -1);
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), pNote->pData->idGroup);
			UpdateGroupText(pNote->pData->idGroup, m_tItem);
		}
		pNote = pNote->next;
	}
	return TRUE;
}

static BOOL RestoreFromBin(int group){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			RestoreNoteFromBin(pNote, group);
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTreeMain), pNote->pData->idGroup);
			UpdateGroupText(pNote->pData->idGroup, m_tItem);
		}
		free(pFree);
	}
	return TRUE;
	//int			index, count;
	//LVITEMW		lvi;
	
	//ZeroMemory(&lvi, sizeof(lvi));
	//lvi.mask = LVIF_PARAM;

	//index = ListView_GetNextItem(m_hListMain, -1, LVNI_SELECTED);
	//if(!all && index > -1){
		//lvi.iItem = index;
		//SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
		//if(lvi.lParam){
			//RestoreNoteFromBin((PMEMNOTE)lvi.lParam, group);
			//return TRUE;
		//}
	//}
	//else if(all){
		//count = ListView_GetItemCount(m_hListMain);
		//for(int i = 0; i < count; i++){
			//lvi.iItem = i;
			//SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
			//if(lvi.lParam){
				//RestoreNoteFromBin((PMEMNOTE)lvi.lParam, group);
			//}
		//}
		//return TRUE;
	//}

	//return FALSE;
}

static BOOL EmptyBin(void){
	if(MessageBoxW(m_hCtrlPanel, g_Strings.EmptyBinMessage, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
		EmptyRecycleBin();
		return TRUE;
	}
	return FALSE;
}

static BOOL DeleteBackups(void){
	int			index;
	LVITEMW		lvi;
	wchar_t		szBuffer[128], filespec[MAX_PATH];

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = szBuffer;

	index = ListView_GetNextItem(m_hListMain, -1, LVNI_SELECTED);
	if(MessageBoxW(m_hCtrlPanel, g_Strings.DMessageBackup, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
		while(index > -1){
			*szBuffer = '\0';
			lvi.iItem = index;
			SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
			if(wcslen(szBuffer) > 0){
				wcscpy(filespec, g_NotePaths.BackupDir);
				wcscat(filespec, szBuffer);
				wcscat(filespec, BACK_NOTE_EXTENTION);
				DeleteFileW(filespec);
			}
			index = ListView_GetNextItem(m_hListMain, index, LVNI_SELECTED);
		}
		return TRUE;
	}
	return FALSE;
}

static BOOL DeleteItems(void){
	int			count, index, id, group;
	LVITEMW		lvi;
	PMEMNOTE	pNote;
	HTREEITEM	hItem;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;

	index = ListView_GetNextItem(m_hListMain, -1, LVNI_SELECTED);
	count = ListView_GetSelectedCount(m_hListMain);
	if(count > 1){
		if(MessageBoxW(m_hCtrlPanel, g_Strings.DMessageMultiple, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
			hItem = TreeView_GetSelection(m_hTreeMain);
			id = GetTVItemId(hItem);
			while(index > -1){
				lvi.iItem = index;
				SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
				if(lvi.lParam){
					pNote = (PMEMNOTE)lvi.lParam;
					if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
						group = pNote->pData->idGroup;
						DeleteToBin(pNote, -1);
						if(group != id){
							FindItemById(TreeView_GetRoot(m_hTreeMain), group);
							UpdateGroupText(group, m_tItem);
						}
					}
					else if(pNote->pData->idGroup == GROUP_RECYCLE){
						WritePrivateProfileSectionW(pNote->pFlags->id, NULL, g_NotePaths.DataFile);
						DeleteRTFile(pNote->pFlags->id);
					}
				}
				index = ListView_GetNextItem(m_hListMain, index, LVNI_SELECTED);
			}
			if(id != GROUP_RECYCLE){
				UpdateGroupText(id, hItem);
			}
			//update favorites
			if(id != GROUP_FAVORITES){
				FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_FAVORITES);
				UpdateGroupText(GROUP_FAVORITES, m_tItem);
			}
			return TRUE;
		}
	}
	else if(count == 1 && index > -1){
		if(MessageBoxW(m_hCtrlPanel, g_Strings.DMessage, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
			hItem = TreeView_GetSelection(m_hTreeMain);
			id = GetTVItemId(hItem);
			lvi.iItem = index;
			SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
			if(lvi.lParam){
				pNote = (PMEMNOTE)lvi.lParam;
				group = pNote->pData->idGroup;
				if(pNote->pData->idGroup > GROUP_RECYCLE || pNote->pData->idGroup == GROUP_INCOMING){
					DeleteToBin(pNote, -1);
					if(group != id){
						FindItemById(TreeView_GetRoot(m_hTreeMain), group);
						UpdateGroupText(group, m_tItem);
					}
				}
				else if(pNote->pData->idGroup == GROUP_RECYCLE){
					WritePrivateProfileSectionW(pNote->pFlags->id, NULL, g_NotePaths.DataFile);
					DeleteRTFile(pNote->pFlags->id);
				}
				if(id != GROUP_RECYCLE){
					UpdateGroupText(id, hItem);
				}
				//update favorites
				if(id != GROUP_FAVORITES){
					FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_FAVORITES);
					UpdateGroupText(GROUP_FAVORITES, m_tItem);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

static void DefineFavoriteStatus(int index){
	// int				index;
	LVITEMW					lvi;
	PMEMNOTE				pNote;
	wchar_t					szBuffer[256];
	TBBUTTONINFOW			tbi;
	int						size;
	MENUITEMINFOW			mi = {0};
	MITEM					*pmi;
	HMENU					hSub;

	// index = SendMessageW(hList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if(index > -1){
		ZeroMemory(&tbi, sizeof(tbi));
		tbi.cbSize = sizeof(tbi);
		tbi.dwMask = TBIF_IMAGE | TBIF_COMMAND;
		tbi.idCommand = IDM_ADD_TO_FAVORITES;
		// SendMessageW(m_hTbrMain, TB_GETBUTTONINFOW, IDM_ADD_TO_FAVORITES, (LPARAM)&tbi);
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = index;
		SendMessageW(m_hListMain, LVM_GETITEMW, 0, (LPARAM)&lvi);
		pNote = (PMEMNOTE)lvi.lParam;
		if(pNote && pNote->pData->idGroup != GROUP_DIARY){
			size = NELEMS(m_CtrlMenus);
			// GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
			// if(wcslen(szBuffer) > 0){
			if(pNote->pRTHandles->favorite == GROUP_FAVORITES){
				tbi.iImage = 20;
				SetMenuText(IDM_REMOVE_FAVORITES, S_MENU, g_NotePaths.CurrLanguagePath, L"Remove From Favorites", szBuffer);
				SetMIText(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, szBuffer);
				SetMIImageIndex(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, 69);
			}
			else{
				tbi.iImage = 19;
				SetMenuText(IDM_ADD_TO_FAVORITES, S_MENU, g_NotePaths.CurrLanguagePath, L"Add To Favorites", szBuffer);
				SetMIText(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, szBuffer);
				SetMIImageIndex(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, 68);
			}
			SendMessageW(m_hTbrMain, TB_SETBUTTONINFOW, IDM_ADD_TO_FAVORITES, (LPARAM)&tbi);
		}
		hSub = GetSubMenu(m_hPUMenuControl, GetMenuPosition(m_hPUMenuControl, IDM_NOTE_MARKS));
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_DATA | MIIM_ID;
		mi.wID = IDM_LOCK_NOTE;
		GetMenuItemInfoW(hSub, IDM_LOCK_NOTE, FALSE, &mi);
		pmi = (PMITEM)mi.dwItemData;
		if(pmi){
			if(pNote->pFlags->locked){
				SetMenuText(IDM_UNLOCK_NOTE, S_MENU, g_NotePaths.CurrLanguagePath, L"Remove Note Password", szBuffer);
				wcscpy(pmi->szText, szBuffer);
				pmi->xPos = 43;
			}
			else{
				SetMenuText(IDM_LOCK_NOTE, S_MENU, g_NotePaths.CurrLanguagePath, L"Set Note Password", szBuffer);
				wcscpy(pmi->szText, szBuffer);
				pmi->xPos = 42;
			}
		}
		
		mi.wID = IDM_ADD_PIN;
		GetMenuItemInfoW(hSub, IDM_ADD_PIN, FALSE, &mi);
		pmi = (PMITEM)mi.dwItemData;
		if(pmi){
			if(pNote->pPin){
				SetMenuText(IDM_REMOVE_PIN, S_MENU, g_NotePaths.CurrLanguagePath, L"Unpin", szBuffer);
				wcscpy(pmi->szText, szBuffer);
				pmi->xPos = 77;
			}
			else{
				SetMenuText(IDM_ADD_PIN, S_MENU, g_NotePaths.CurrLanguagePath, L"Pin To Window", szBuffer);
				wcscpy(pmi->szText, szBuffer);
				pmi->xPos = 76;
			}
		}
	}
}

// static void ShowTreeViewDropDown(int buttonId, int menuId){
	// RECT		rc;
	// HMENU		hMenu;
	// int			index;

	// index = GetButtonIndexW(m_hTbrTree, buttonId);
	// SendMessageW(m_hTbrTree, TB_GETITEMRECT, index, (LPARAM)&rc);
	// m_hMenuCtrlTemp = m_hPUMenuTree;
	// hMenu = GetSubMenu(m_hMenuCtrlTemp, GetMenuPosition(m_hMenuCtrlTemp, menuId));
	// MapWindowPoints(m_hDTree, HWND_DESKTOP, (LPPOINT)&rc, 2);
	// TrackPopupMenu(hMenu, TPM_LEFTALIGN, rc.left, rc.bottom, 0, m_hDTree, NULL);
	// //remove popup menu when user clicks outside it
	// PostMessageW(m_hCtrlPanel, WM_NULL, 0, 0);
// }

static void ShowButtonDropDown(HMENU hMenuParent, int buttonId, int menuId){
	RECT		rc;
	HMENU		hMenu;
	int			index;

	index = GetButtonIndexW(m_hTbrMain, buttonId);
	SendMessageW(m_hTbrMain, TB_GETITEMRECT, index, (LPARAM)&rc);
	if(hMenuParent == m_hPUMenuControl){
		m_hMenuCtrlTemp = m_hPUMenuControl;
		hMenu = GetSubMenu(m_hMenuCtrlTemp, GetMenuPosition(m_hMenuCtrlTemp, menuId));
	}
	else{
		hMenu = GetSubMenu(hMenuParent, GetMenuPosition(hMenuParent, menuId));
	}
	MapWindowPoints(m_hCtrlPanel, HWND_DESKTOP, (LPPOINT)&rc, 2);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, rc.left, rc.bottom, 0, m_hCtrlPanel, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(m_hCtrlPanel, WM_NULL, 0, 0);
}

static int CountOfBackupCopies(void){
	WIN32_FIND_DATAW		fd;
	wchar_t					filespec[MAX_PATH];
	HANDLE					handle = INVALID_HANDLE_VALUE;
	BOOL					result = TRUE;
	int						count = 0;

	wcscpy(filespec, g_NotePaths.BackupDir);
	wcscat(filespec, L"*");
	wcscat(filespec, BACK_NOTE_EXTENTION);
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			count++;
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
	return count;
}

static void UpdateGroupText(int id, HTREEITEM hItem){
	TVITEMW			tvi;
	wchar_t			szBuffer[256], szCount[16];
	LPPNGROUP		ppg;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_TEXT;
	tvi.hItem = hItem;
	tvi.pszText = szBuffer;
	tvi.cchTextMax = 256;
	SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);

	if(id != GROUP_RECYCLE && id != GROUP_SEARCH_RESULTS && id != GROUP_BACKUP && id != GROUP_DIARY && id != GROUP_FAVORITES && id != GROUP_INCOMING){
		_itow(CountNotesInGroup(id), szCount, 10);
		ppg = PNGroupsGroupById(g_PGroups, id);
		if(ppg){
			wcscpy(szBuffer, ppg->szName);
		}
	}
	else if(id == GROUP_RECYCLE){
		_itow(CountNotesInGroup(id), szCount, 10);
		GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	}
	else if(id == GROUP_SEARCH_RESULTS){
		_itow(m_iSearchCount, szCount, 10);
		GetPrivateProfileStringW(S_CAPTIONS, IK_SEARCH_RESULTS, L"Search results", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	}
	else if(id == GROUP_BACKUP){
		_itow(CountOfBackupCopies(), szCount, 10);
		GetPrivateProfileStringW(S_CAPTIONS, IK_BACKUP_CAPTION, L"Backup", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	}
	else if(id == GROUP_DIARY){
		_itow(CountNotesInGroup(id), szCount, 10);
		GetPrivateProfileStringW(S_CAPTIONS, L"diary", L"Diary", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	}
	else if(id == GROUP_FAVORITES){
		_itow(CountFavorites(), szCount, 10);
		GetPrivateProfileStringW(S_CAPTIONS, IK_FAVORITES, L"Favorites", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	}
	else if(id == GROUP_INCOMING){
		_itow(CountNotesInGroup(id), szCount, 10);
		GetPrivateProfileStringW(S_CAPTIONS, IK_INCOMING, L"Incoming", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	}
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	if(IsGroupLocked(id))
		wcscat(szBuffer, PWRD_PROTECTED_GROUP);
	SendMessageW(m_hTreeMain, TVM_SETITEMW, 0, (LPARAM)&tvi);
	//update "All notes"
	tvi.hItem = TreeView_GetRoot(m_hTreeMain);
	SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
	GetPrivateProfileStringW(S_CAPTIONS, L"group_main", L"All notes", szBuffer, 128, g_NotePaths.CurrLanguagePath);
	_itow(CountNotesAlive(), szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	SendMessageW(m_hTreeMain, TVM_SETITEMW, 0, (LPARAM)&tvi);
}

static void ShowHidePreview(void){
	int		result = GetMenuState(m_hPUMenuControl, IDM_PREVIEW, MF_BYCOMMAND);

	if((result & MF_CHECKED) == MF_CHECKED){
		CheckMenuItem(m_hPUMenuControl, IDM_PREVIEW, MF_UNCHECKED | MF_BYCOMMAND);
		SendMessageW(m_hTbrMain, TB_CHECKBUTTON, IDM_PREVIEW, (LPARAM)MAKELONG(0, 0));
		SendMessageW(m_hHorSplitter, SPM_HIDE_CHILD, 0, CHILD_BOTTOM);
		WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_VISIBLE, L"0", g_NotePaths.INIFile);
	}
	else{
		CheckMenuItem(m_hPUMenuControl, IDM_PREVIEW, MF_CHECKED | MF_BYCOMMAND);
		SendMessageW(m_hTbrMain, TB_CHECKBUTTON, IDM_PREVIEW, (LPARAM)MAKELONG(TBSTATE_CHECKED, 0));
		SendMessageW(m_hHorSplitter, SPM_SHOW_CHILD, 0, CHILD_BOTTOM);
		WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_VISIBLE, L"1", g_NotePaths.INIFile);
	}
}

static BOOL CALLBACK DTree_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, DTree_OnInitDialog);
		HANDLE_MSG (hwnd, WM_SIZE, DTree_OnSize);
		HANDLE_MSG (hwnd, WM_COMMAND, DTree_OnCommand);
		HANDLE_MSG (hwnd, WM_DRAWITEM, DTree_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, DTree_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, DTree_OnInitMenuPopup);

		case WM_NOTIFY:{
			LPNMHDR		lpnm = (LPNMHDR)lParam;
			if(lpnm->code == TBN_DROPDOWN){
				LPNMTOOLBARW	lpnmtbr = (LPNMTOOLBARW)lParam;
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(lpnmtbr->iItem, 0), 0);
				return TBDDRET_TREATPRESSED;
			}
			else{
				return SendMessageW(GetParent(hwnd), WM_NOTIFY, wParam, lParam);
			}
		}
		default: return FALSE;
	}
}

static void DTree_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
	m_tRightClick = TreeView_GetDropHilight(m_hTreeMain);
	if(m_tRightClick == NULL)
		m_tRightClick = m_tSelected;
}

static void DTree_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(m_hMenuCtrlTemp == m_hPUMenuTree)
		DrawMItem(lpDrawItem, m_hBmpCtrlNormal, m_hBmpCtrlGray, CLR_MASK);
}

static void DTree_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	if(m_hMenuCtrlTemp == m_hPUMenuTree)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void DTree_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HTREEITEM			hInserted;

	switch(id){
		case IDM_NEW_GROUP:{
			PNGROUP		pg;
			ZeroMemory(&pg, sizeof(pg));
			pg.id = NEW_GROUP_ID;
			pg.parent = -1;
			pg.crCaption = g_Appearance.crCaption;
			pg.crFont = g_Appearance.crFont;
			wcscpy(pg.szSkin, g_Appearance.szSkin);
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUP), g_hCPDialog, NewGroup_DlgProc, (LPARAM)&pg);
			if(result == IDOK){
				hInserted = InsertGroup(TreeView_GetRoot(m_hTreeMain), &pg, FALSE, TRUE);
				if(hInserted)
					TreeView_EnsureVisible(m_hTreeMain, hInserted);
			}
			break;
		}
		case IDM_NEW_SUBGROUP:{
			PNGROUP		pg;
			ZeroMemory(&pg, sizeof(pg));
			pg.id = NEW_GROUP_ID;
			pg.crCaption = g_Appearance.crCaption;
			pg.crFont = g_Appearance.crFont;
			wcscpy(pg.szSkin, g_Appearance.szSkin);
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUP), g_hCPDialog, NewGroup_DlgProc, (LPARAM)&pg);
			if(result == IDOK){
				if(hwndCtl == m_hTbrTree)
					hInserted = InsertGroup(m_tSelected, &pg, TRUE, TRUE);
				else
					hInserted = InsertGroup(m_tHighlighted, &pg, TRUE, TRUE);
				if(hInserted)
					TreeView_EnsureVisible(m_hTreeMain, hInserted);
			}
			break;
		}
		case IDM_EDIT_GROUP:{
			LPPNGROUP	ppg;
			TVITEMW	tvi;
			wchar_t		szBuffer[128];
			int			result = IDCANCEL;

			ZeroMemory(&tvi, sizeof(tvi));
			tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			if(hwndCtl == m_hTbrTree)
				tvi.hItem = m_tSelected;
			else
				tvi.hItem = m_tHighlighted;
			tvi.cchTextMax = 128;
			tvi.pszText = szBuffer;
			SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
			ppg = PNGroupsGroupById(g_PGroups, tvi.lParam);
			result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUP), g_hCPDialog, NewGroup_DlgProc, (LPARAM)ppg);
			if(result == IDOK){
				int			imageIndex, images = ImageList_GetImageCount(m_hImlTreeNormal);
				wchar_t		szCount[16];

				if(ppg->image + m_gOffset < images)
					imageIndex = ppg->image + m_gOffset;
				else
					imageIndex = m_gOffset;
				tvi.iImage = imageIndex;
				tvi.iSelectedImage = imageIndex;
				_itow(CountNotesInGroup(ppg->id), szCount, 10);
				wcscpy(szBuffer, ppg->szName);
				wcscat(szBuffer, L" (");
				wcscat(szBuffer, szCount);
				wcscat(szBuffer, L")");
				if(IsGroupLocked(ppg->id))
						wcscat(szBuffer, PWRD_PROTECTED_GROUP);
				SendMessageW(m_hTreeMain, TVM_SETITEMW, 0, (LPARAM)&tvi);
				//re-sort items alphabetically
				TreeView_SortChildren(m_hTreeMain, TreeView_GetParent(m_hTreeMain, tvi.hItem), FALSE);
				SaveGroup(ppg);
				UpdateListView();
			}
			break;
		}
		case IDM_DELETE_GROUP:{
			wchar_t		szMessage[768];
			HTREEITEM	hParent;

			wcscpy(szMessage, g_Strings.DeleteGroup1);
			wcscat(szMessage, L"\n");
			wcscat(szMessage, g_Strings.DeleteGroup2);
			// wcscat(szMessage, L"\n");
			// wcscat(szMessage, g_Strings.DeleteGroup3);
			if(MessageBoxW(hwnd, szMessage, g_Strings.DeleteGroup0, MB_YESNO | MB_ICONWARNING) == IDYES){
				if(hwndCtl == m_hTbrTree){
					hParent = TreeView_GetParent(m_hTreeMain, m_tSelected);
					DeleteTreeGroup(m_tSelected, FALSE);
				}
				else{
					hParent = TreeView_GetParent(m_hTreeMain, m_tHighlighted);
					DeleteTreeGroup(m_tHighlighted, FALSE);
				}
				TreeView_SelectItem(m_hTreeMain, hParent);
			}
			break;
		}
		case IDM_SHOW_GROUP:
			if(hwndCtl == m_hTbrTree){
				ShowGroup(m_tSelected);
				EnableTreeMenus(m_tSelected);
			}
			else{
				ShowGroup(m_tHighlighted);
				EnableTreeMenus(m_tHighlighted);
			}
			break;
		case IDM_HIDE_GROUP:
			if(hwndCtl == m_hTbrTree){
				HideGroup(m_tSelected);
				EnableTreeMenus(m_tSelected);
			}
			else{
				HideGroup(m_tHighlighted);
				EnableTreeMenus(m_tHighlighted);
			}
			break;
		case IDM_SHOW_INCLUDE_SG:
			if(hwndCtl == m_hTbrTree){
				ShowBranch(m_tSelected, FALSE);
				EnableTreeMenus(m_tSelected);
			}
			else{
				ShowBranch(m_tHighlighted, FALSE);
				EnableTreeMenus(m_tHighlighted);
			}
			break;
		case IDM_HIDE_INCLUDE_SG:
			if(hwndCtl == m_hTbrTree){
				HideBranch(m_tSelected, FALSE);
				EnableTreeMenus(m_tSelected);
			}
			else{
				HideBranch(m_tHighlighted, FALSE);
				EnableTreeMenus(m_tHighlighted);
			}
			break;
		case IDM_CUSTOMIZE_ICONS:{
			*m_TempLib = '\0';
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CHOOSE_BMP), hwnd, Bitmaps_DlgProc, -1);
			if(result == IDOK){
				if(*m_TempLib != '\0'){
					DeleteFileW(m_ImagesPath);
					CopyFileW(m_TempLib, m_ImagesPath, FALSE);
					FillTreeImageList();
					int		count = ImageList_GetImageCount(m_hImlTreeNormal);
					count--;
					RefreshTreeviewItemsIcons(TreeView_GetRoot(m_hTreeMain), count);
				}
			}
			if(*m_TempLib != '\0'){
				DeleteFileW(m_TempLib);
				*m_TempLib = '\0';
			}
			break;
		}
		case IDM_LOCK_GROUP:{
			wchar_t			szNewPwrd[256];
			LPPNGROUP		ppg;
			TVITEMW			tvi = {0};

			tvi.mask = TVIF_HANDLE | TVIF_PARAM;
			if(hwndCtl == m_hTbrTree)
				tvi.hItem = m_tSelected;
			else
				tvi.hItem = m_tHighlighted;
			SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
			if(tvi.lParam != GROUP_DIARY){
				ppg = PNGroupsGroupById(g_PGroups, tvi.lParam);
				if(ppg){
					*szNewPwrd = '\0';
					if(ShowLoginDialog(g_hInstance, m_hCtrlPanel, DLG_LOGIN_CREATE, szNewPwrd, LDT_GROUP, ppg) == IDOK){
						wcscpy(ppg->szLock, szNewPwrd);
						SaveGroup(ppg);
						UpdateListView();
						EnableTreeMenus(tvi.hItem);
						UpdateGroupText(tvi.lParam, tvi.hItem);
					}
				}
			}
			else{
				*szNewPwrd = '\0';
				if(ShowLoginDialog(g_hInstance, m_hCtrlPanel, DLG_LOGIN_CREATE, szNewPwrd, LDT_DIARY, NULL) == IDOK){
					WritePrivateProfileStringW(S_DIARY, IK_DIARY_LOCK, szNewPwrd, g_NotePaths.INIFile);
					UpdateListView();
					EnableTreeMenus(tvi.hItem);
					UpdateGroupText(tvi.lParam, tvi.hItem);
				}
			}
			break;
		}
		case IDM_UNLOCK_GROUP:{
			LPPNGROUP		ppg;
			TVITEMW			tvi = {0};

			tvi.mask = TVIF_HANDLE | TVIF_PARAM;
			if(hwndCtl == m_hTbrTree)
				tvi.hItem = m_tSelected;
			else
				tvi.hItem = m_tHighlighted;
			SendMessageW(m_hTreeMain, TVM_GETITEMW, 0, (LPARAM)&tvi);
			if(tvi.lParam != GROUP_DIARY){
				ppg = PNGroupsGroupById(g_PGroups, tvi.lParam);
				if(ppg){
					if(ShowLoginDialog(g_hInstance, m_hCtrlPanel, DLG_LOGIN_MAIN, NULL, LDT_GROUP, ppg) == IDOK){
						*ppg->szLock = '\0';
						SaveGroup(ppg);
						UpdateListView();
						EnableTreeMenus(tvi.hItem);
						UpdateGroupText(tvi.lParam, tvi.hItem);
					}
				}
			}
			else{
				if(ShowLoginDialog(g_hInstance, m_hCtrlPanel, DLG_LOGIN_MAIN, NULL, LDT_DIARY, NULL) == IDOK){
					WritePrivateProfileStringW(S_DIARY, IK_DIARY_LOCK, NULL, g_NotePaths.INIFile);
					UpdateListView();
					EnableTreeMenus(tvi.hItem);
					UpdateGroupText(tvi.lParam, tvi.hItem);
				}
			}
			break;
		}
	}
}

static BOOL DTree_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_hTbrTree = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS | TBSTYLE_TRANSPARENT, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, g_hInstance, NULL);
	// m_hTbrTree = GetDlgItem(hwnd, IDC_TBR_TREE);
	m_hTreeMain = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEWW, NULL, WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS| TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, (HMENU)IDC_CTRL_TREEVIEW, g_hInstance, NULL);
	// m_hTreeMain = GetDlgItem(hwnd, IDC_CTRL_TREEVIEW);
	return FALSE;
}
#include <stdio.h>
static void DTree_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	REBARBANDINFOW	band;
	int				y = m_Up;


	// band.fMask = RBBIM_BACKGROUND | RBBIM_SIZE | RBBIM_CHILDSIZE | RBBIM_CHILD | RBBIM_STYLE;
	// SendMessage(m_hRebarTree, RB_GETBANDINFO, 0, (LPARAM)&band);
	// band.cx = cx;
	// SendMessage(m_hRebarTree, RB_SETBANDINFO, 0, (LPARAM)&band);
	// MoveWindow(m_hTbrTree, 0, 0, cx, m_Up, TRUE);
	// MoveWindow(m_hTreeMain, 0, m_Up, cx, cy - m_Up, TRUE);

	// SetWindowPos(m_hRebarTree, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
	if(cx < m_WTreeTbr)
		y *= 2;
	//move and repaint rebar control
	band.cbSize = sizeof(band);
	band.fMask = RBBIM_BACKGROUND | RBBIM_SIZE | RBBIM_CHILDSIZE | RBBIM_CHILD | RBBIM_STYLE | RBBS_VARIABLEHEIGHT;
	SendMessage(m_hRebarTree, RB_GETBANDINFO, 0, (LPARAM)&band);
	band.cx = cx;
	band.cyMinChild = y;
	SendMessage(m_hRebarTree, RB_SETBANDINFO, 0, (LPARAM)&band);
	MoveWindow(m_hTbrTree, 0, 0, cx, y, TRUE);
	MoveWindow(m_hTreeMain, 0, y, cx, cy - y, TRUE);

	// RedrawWindow(m_hRebarTree, NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(m_hTbrTree, NULL, NULL, RDW_INVALIDATE);
	// char s[128];
	// RECT rc;
	// GetWindowRect(m_hRebarTree, &rc);
	// MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	// sprintf(s, "Rebar left = %d, top = %d, width = %d, height = %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	// OutputDebugString(s);
	// GetWindowRect(m_hTbrTree, &rc);
	// MapWindowPoints(HWND_DESKTOP, m_hRebarTree, (LPPOINT)&rc, 2);
	// sprintf(s, "Toolbar left = %d, top = %d, width = %d, height = %d", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	// OutputDebugString(s);
}

static BOOL IsGroupInCollection(int group){
	for(int i = 0; i < m_iGroupsCount; i++){
		if(m_pGroupsArray[i] == group)
			return TRUE;
	}
	return FALSE;
}

static void CollectGroups(HTREEITEM hItem){
	HTREEITEM	hi;

	hi = TreeView_GetChild(m_hTreeMain, hItem);
	if(hi){
		CollectGroups(hi);
	}
	if(!m_pGroupsArray)
		m_pGroupsArray = calloc(1, sizeof(int));
	else
		m_pGroupsArray = realloc(m_pGroupsArray, (m_iGroupsCount + 1) * sizeof(int));
	m_pGroupsArray[m_iGroupsCount] = GetTVItemId(hItem);
	m_iGroupsCount++;
}

static void QuickSearchInGroup(wchar_t * lpString, BOOL fromBin){
	PMEMNOTE		pNote = MemoryNotes();
	BOOL			doGroup;
	LPPNGROUP			pgg;

	pgg = PNGroupsGroupById(g_PGroups, GetSelectedTVItemId());
	if(pgg)
		wcscpy(m_QSGroup, pgg->szName);
	else
		*m_QSGroup = '\0';
	if(m_pSearchResults){
		while(pNote){
			doGroup = FALSE;
			if(pNote->pData->idGroup != GROUP_RECYCLE){
				if(IsGroupInCollection(pNote->pData->idGroup))
					doGroup = TRUE;
			}
			else if(pNote->pData->idGroup == GROUP_RECYCLE && fromBin){
				if(IsGroupInCollection(pNote->pData->idPrevGroup))
					doGroup = TRUE;
			}
			if(doGroup){
				if(_wcsistr(pNote->pData->szName, lpString)){
					if(m_iSearchCount != 0 && (m_iSearchCount % SEARCH_COUNT) == 0){
						m_pSearchResults = realloc(m_pSearchResults, sizeof(PMEMNOTE) * (m_iSearchCount + SEARCH_COUNT));
					}
					m_pSearchResults[m_iSearchCount] = pNote;
					m_iSearchCount++;
				}
			}
			pNote = pNote->next;
		}
	}
	FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_SEARCH_RESULTS);
	TreeView_SelectItem(m_hTreeMain, m_tItem);
	SelectSearchResultsItem();
}

static void QuickSearchAllNotes(wchar_t * lpString, BOOL fromBin){
	PMEMNOTE		pNote = MemoryNotes();

	GetPrivateProfileStringW(S_CAPTIONS, L"group_main", L"All notes", m_QSGroup, 128, g_NotePaths.CurrLanguagePath);
	if(m_pSearchResults){
		while(pNote){
			if(fromBin || (!fromBin && pNote->pData->idGroup != GROUP_RECYCLE)){
				if(_wcsistr(pNote->pData->szName, lpString)){
					if(m_iSearchCount != 0 && (m_iSearchCount % SEARCH_COUNT) == 0){
						m_pSearchResults = realloc(m_pSearchResults, sizeof(PMEMNOTE) * (m_iSearchCount + SEARCH_COUNT));
					}
					m_pSearchResults[m_iSearchCount] = pNote;
					m_iSearchCount++;
				}
			}
			pNote = pNote->next;
		}
	}
	FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_SEARCH_RESULTS);
	TreeView_SelectItem(m_hTreeMain, m_tItem);
	SelectSearchResultsItem();
}

static void QuickSearchRecycleBin(wchar_t * lpString){
	PMEMNOTE		pNote = MemoryNotes();

	GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", m_QSGroup, 128, g_NotePaths.CurrLanguagePath);
	if(m_pSearchResults){
		while(pNote){
			if(pNote->pData->idGroup == GROUP_RECYCLE){
				if(_wcsistr(pNote->pData->szName, lpString)){
					if(m_iSearchCount != 0 && (m_iSearchCount % SEARCH_COUNT) == 0){
						m_pSearchResults = realloc(m_pSearchResults, sizeof(PMEMNOTE) * (m_iSearchCount + SEARCH_COUNT));
					}
					m_pSearchResults[m_iSearchCount] = pNote;
					m_iSearchCount++;
				}
			}
			pNote = pNote->next;
		}
	}
	FindItemById(TreeView_GetRoot(m_hTreeMain), GROUP_SEARCH_RESULTS);
	TreeView_SelectItem(m_hTreeMain, m_tItem);
	SelectSearchResultsItem();
}

static void SelectSearchResultsItem(void){
	NMHDR		nmhdr;

	nmhdr.idFrom = IDC_CTRL_TREEVIEW;
	nmhdr.hwndFrom = m_hTreeMain;
	nmhdr.code = TVN_SELCHANGED;
	SendMessageW(g_hCPDialog, WM_NOTIFY, IDC_CTRL_TREEVIEW, (LPARAM)&nmhdr);
}

static void CreateRebarBackground(HWND hwnd, HWND hToolbar, HBITMAP * phbm){
	HDC				hdcMain, hdc;
	HBITMAP			hbm, hbmOld;
	RECT			rc;
	int				y, cx, cy;

	ImageList_GetIconSize(m_hImlTbrNormal, &cx, &cy);
	// GetClientRect(hToolbar, &rc);
	y = cy + 2 * HIWORD(SendMessageW(m_hTbrMain, TB_GETPADDING, 0, 0));// rc.bottom - rc.top;
	hdcMain = GetDC(hwnd);
	hdc = CreateCompatibleDC(hdcMain);
	hbm = CreateCompatibleBitmap(hdcMain, 1, y);
	SetRect(&rc, 0, 0, 1, y);
	SetBkMode(hdc, TRANSPARENT);
	hbmOld = SelectBitmap(hdc, hbm);
	
	if(g_VSEnabled){
		COLORREF		clr1, clr2;
		clr1 = clr2 = GetSysColor(COLOR_BTNFACE);
		clr1 = ColorAdjustLuma(clr1, 195, FALSE);
		clr2 = ColorAdjustLuma(clr2, -195, FALSE);
		Fill2ColorsRectangle(hdc, &rc, clr1, clr2, GRADIENT_FILL_RECT_V);
	}
	else{
		FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
	}
	SelectBitmap(hdc, hbmOld);
	if(*phbm){
		DeleteBitmap(*phbm);
	}
	*phbm = hbm;
	DeleteDC(hdc);
	ReleaseDC(hwnd, hdcMain);
}

static HWND AddRebar(HWND hParent, HWND hToolbar){
	REBARINFO			rbi;
	REBARBANDINFOW		band;
	HWND				hRebar;

	hRebar = CreateWindowExW(WS_EX_TOOLWINDOW, REBARCLASSNAMEW, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER, 0, 0, 0, 0, hParent, NULL, g_hInstance, NULL);
	ZeroMemory(&rbi, sizeof(rbi));
	SendMessage(hRebar, RB_SETBANDINFO, 0, (LPARAM)&rbi);
	ZeroMemory(&band, sizeof(band));
	band.cbSize = sizeof(band);
	band.fMask = RBBIM_BACKGROUND | RBBIM_SIZE | RBBIM_CHILDSIZE | RBBIM_CHILD | RBBIM_STYLE | RBBS_VARIABLEHEIGHT;
	band.hbmBack = m_RebarBitmap;
	band.cx = 0;
	band.cxMinChild = 0;
	band.cyMinChild = m_Up;
	band.hwndChild = hToolbar;
	SendMessage(hRebar, RB_INSERTBAND, -1, (LPARAM)&band);
	return hRebar;
}

static void DrawToolbarButton(HWND hToolbar, HDC hdc, LPRECT lprc, int state, int id){
	//draw custom toolbar button
	TBBUTTONINFOW		tbi;
	int					x, y, cx, cy;
	COLORREF			clr1, clr2;
	HBRUSH				hBrush;

	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_IMAGE | TBIF_COMMAND;
	tbi.idCommand = id;
	SendMessageW(hToolbar, TB_GETBUTTONINFOW, (WPARAM)id, (LPARAM)&tbi);
	hBrush = CreateSolidBrush(m_clrFrame);

	if((state & CDIS_SELECTED) == 0){
		clr1 = clr2 = m_clrHot;
		clr1 = ColorAdjustLuma(clr1, 195, FALSE);
		clr2 = ColorAdjustLuma(clr2, -195, FALSE);
		Fill2ColorsRectangle(hdc, lprc, clr1, clr2, GRADIENT_FILL_RECT_V);
	}
	else{
		clr1 = clr2 = m_clrSel;
		clr1 = ColorAdjustLuma(clr1, 195, FALSE);
		clr2 = ColorAdjustLuma(clr2, -195, FALSE);
		Fill2ColorsRectangle(hdc, lprc, clr2, clr1, GRADIENT_FILL_RECT_V);
	}

	FrameRect(hdc, lprc, hBrush);
	ImageList_GetIconSize(m_hImlTbrNormal, &cx, &cy);
	x = lprc->left + ((lprc->right - lprc->left) - cx) / 2;
	y = lprc->top + ((lprc->bottom - lprc->top) - cy) / 2;
	ImageList_Draw(m_hImlTbrNormal, tbi.iImage, hdc, x, y, ILD_TRANSPARENT);
	DeleteBrush(hBrush);
}

static BOOL CALLBACK PreviewProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
			HideCaret(hwnd);
			return 0;
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static void FillTreeImageList(void){
	HBITMAP				hBmp;
	HMODULE				hGroups, hIcons;

	TreeView_SetImageList(m_hTreeMain, NULL, TVSIL_NORMAL);
	if(m_hImlTreeNormal)
		ImageList_Destroy(m_hImlTreeNormal);
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_GROUPS));
		m_hImlTreeNormal = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 42, 42);
		ImageList_AddMasked(m_hImlTreeNormal, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		FreeLibrary(hIcons);
	}
	//load groups images
	hGroups = LoadLibraryExW(m_ImagesPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hGroups){
		EnumResourceNames(hGroups, RT_BITMAP, EnumResNameProc, (LPARAM)m_hImlTreeNormal);
		FreeLibrary(hGroups);
		hGroups = NULL;
		m_TreeImagesExist = TRUE;
	}
	else{
		m_TreeImagesExist = FALSE;
	}
	TreeView_SetImageList(m_hTreeMain, m_hImlTreeNormal, TVSIL_NORMAL);
}

static void ReapplyIconNumbers(HWND hList){
	int			count = ListView_GetItemCount(hList);
	LVITEMW		lvi;
	wchar_t		szKey[16];

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	for(int i = 0; i < count; i++){
		lvi.iItem = i;
		lvi.pszText = szKey;
		_itow(i + 1, szKey, 10);
		SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	}
}

static int GetMaxParam(HWND hList){
	int			count = ListView_GetItemCount(hList);
	LVITEMW		lvi;
	int			max = 0;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;
	for(int i = 0; i < count; i++){
		lvi.iItem = i;
		SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if(lvi.lParam > max)
			max = lvi.lParam;
	}
	return max;
}

static void RemoveGroupIcon(HWND hwnd){
	HWND		hList = GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP);
	// HIMAGELIST	iml = ListView_GetImageList(hList, LVSIL_SMALL);
	int			count = ListView_GetItemCount(hList);
	int			index = ListView_GetNextItem(hList, -1, LVNI_ALL | LVNI_SELECTED);
	LVITEMW		lvi;
	int 		lParam;
	
	if(index >= 0){
		//Do not remove image from image list, because it does not shifht remained images and we stay with gap
		// ImageList_Remove(iml, index);
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = index;
		SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		lParam = lvi.lParam;
		ListView_DeleteItem(hList, index);
		ReapplyIconNumbers(hList);
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_STATE;
		if(index == count - 1)
			lvi.iItem = count - 2;
		else
			lvi.iItem = index - 1;
		lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
		lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
		ListView_EnsureVisible(hList, lvi.iItem, FALSE);
		SetFocus(hList);
		if(PathFileExistsW(m_TempLib)){
			HANDLE		hRes;
			hRes = BeginUpdateResourceW(m_TempLib, FALSE);
			if(hRes){
				UpdateResourceW(hRes, MAKEINTRESOURCEW(2), MAKEINTRESOURCEW(lParam), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), NULL, 0);
				EndUpdateResourceW(hRes, FALSE);
			}
		}
	}
}

static void ShowHideColumn(int column, BOOL show){
	if(show){
		m_ColsVisibility[column].width = m_ColsVisibility[column].defWidth;
	}
	else{
		m_ColsVisibility[column].width = 0;
	}
	SendMessageW(m_hListMain, LVM_SETCOLUMNWIDTH, column, m_ColsVisibility[column].width);
	WritePrivateProfileStructW(S_CP_DATA, IK_CP_COLUMNS_WIDTH, m_ColsVisibility, sizeof(COLVISIBLE) * NELEMS(m_ColsVisibility), g_NotePaths.INIFile);
}

static void ChangeButtonImageIndex(int cmd, int index){
	TBBUTTONINFOW		tbi = {0};

	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_COMMAND | TBIF_IMAGE;
	SendMessageW(m_hTbrMain, TB_GETBUTTONINFOW, cmd, (LPARAM)&tbi);
	tbi.iImage = index;
	SendMessageW(m_hTbrMain, TB_SETBUTTONINFOW, cmd, (LPARAM)&tbi);
}

static void AddGroupIcon(HWND hwnd){
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	wchar_t				szBuffer[256];

	GetPrivateProfileStringW(L"captions", L"add_icon", L"Add Icon", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, GROUP_ICONS_FILTER, szBuffer, NULL)){
		HBITMAP		hBmp = LoadImageW(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if(hBmp){
			BITMAP	bm;
			GetObject(hBmp, sizeof(bm), &bm);
			if(bm.bmWidth == 16 && bm.bmHeight == 16){
				//add image to image list and to list vlew
				HWND		hList = GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP);
				HIMAGELIST	iml = ListView_GetImageList(hList, LVSIL_SMALL);
				int			count = ListView_GetItemCount(hList);
				int 		index = ImageList_AddMasked(iml, hBmp, CLR_MASK);
				int			lParam;
				DeleteBitmap(hBmp);
				DeleteObject((void *)CLR_MASK);
				LVITEMW		lvi;
				ZeroMemory(&lvi, sizeof(lvi));
				lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvi.iImage = index;
				lvi.iItem = count;
				lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
				lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
				lParam = GetMaxParam(hList);
				lvi.lParam = lParam + 1;
				SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
				ReapplyIconNumbers(hList);
				ListView_EnsureVisible(hList, count, FALSE);
				SetFocus(hList);
				if(PathFileExistsW(m_TempLib)){
					HANDLE		hRes, hFile;
					LPVOID		lpData = NULL;
					hRes = BeginUpdateResourceW(m_TempLib, FALSE);
					if(hRes){
						hFile = CreateFileW(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						if(hFile != INVALID_HANDLE_VALUE){
							DWORD		dwSize, read;
							
							dwSize = GetFileSize(hFile, NULL);
							dwSize -= 14;
							lpData = malloc(dwSize);
							SetFilePointer(hFile, 14, NULL, FILE_BEGIN);
							ReadFile(hFile, lpData, dwSize, &read, NULL);
							if(dwSize == read){
								UpdateResourceW(hRes, MAKEINTRESOURCEW(2), MAKEINTRESOURCEW(lParam + 1), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), lpData, dwSize);
							}
							CloseHandle(hFile);
						}
						EndUpdateResourceW(hRes, FALSE);
					}
					if(lpData)
						free(lpData);
				}
			}
			else{
				//image should be 16x16 size
				GetPrivateProfileStringW(S_MESSAGES, L"invalid_image_size", L"Invalid image size. Must be 16 x 16.", szBuffer, 256, g_NotePaths.CurrLanguagePath);
				MessageBoxW(hwnd, szBuffer, PROG_NAME, MB_OK | MB_ICONERROR);
				DeleteBitmap(hBmp);
			}
		}
	}
}
