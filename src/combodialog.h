#ifndef __COMBODIALOG_H__
#define __COMBODIALOG_H__

typedef struct {
	HWND		hParent;
	HWND		hButton;
	HWND		hEdit;
} CBCREATION, *P_CBCREATION;

void CreateComboDialog(P_CBCREATION pcr, HINSTANCE hInstance);

#endif	// COMBODIALOG_H__
