#ifndef __PRINT_H__
#define __PRINT_H__

PAGESETUPDLGW			g_pSetup;
PRINTDLGW				g_pPdlg;

void InitPrintMembers(void);
void SetPaper(void);
void PrintEdit(HWND hEdit, wchar_t * lpDocName);

#endif	// PRINT_H__
