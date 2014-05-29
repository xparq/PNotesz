#ifndef __CCTreeView_H__
#define __CCTreeView_H__

typedef struct tagCTVNCHECKCHANGE{
	NMHDR		nmhdr;
	HTREEITEM	hItem;
	BOOL		fChecked;
}CTVNCHECKCHANGE, *LPCTVNCHECKCHANGE;

#define	CTVN_CHECKSTATECHANGED		WM_APP + 1

void CTreeView_Subclass(HWND hTree);
BOOL CTreeView_IsCheckBoxItem(HWND hTree, HTREEITEM hItem);
BOOL CTreeView_IsLabelClicked(HWND hTree, LPTVHITTESTINFO lptvh);
BOOL CTreeView_IsCheckboxClicked(HWND hTree, LPTVHITTESTINFO lptvh);
BOOL CTreeView_ToggleCheckState(HWND hTree, HTREEITEM hItem);
void CTreeView_SetEnable(HWND hTree, HTREEITEM hItem, BOOL fEnable);
BOOL CTreeView_IsItemEnabled(HWND hTree, HTREEITEM hItem);
BOOL CTreeView_SetCheckBoxState(HWND hTree, HTREEITEM hItem, BOOL fCheck);
BOOL CTreeView_GetCheckBoxState(HWND hTree, HTREEITEM hItem);
LRESULT CTreeView_OnCustomDraw(HWND hParent, HWND hwnd, LPNMTVCUSTOMDRAW pNMTVCD);
void CTreeView_OnCheckStateChanged(HWND hwnd, HTREEITEM hItem, BOOL fChecked);

#endif	// CCTreeView_H__
