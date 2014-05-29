#ifndef __ABOUT_H__
#define __ABOUT_H__

#define IDD_ABOUT  				21234
#define	IDC_SHOW_LICENSE		21236
#define	IDC_EMAIL				21237
#define	IDC_EDI_CREDITS			21238
#define	IDC_ST_ABOUT_STRING		21239
#define	IDC_ST_ABOUT_ICON		21240
#define	IDC_ST_CREDITS_1		21241
#define	IDC_ST_CREDITS_2		21242
#define	IDC_ST_PNGLIB			21243
#define	IDC_PNGLIB_ADDRESS		21244
#define	IDC_ST_HUNSPELL			21245
#define	IDC_HUNSPELL_ADDRESS	21216
void CreateAboutDialog(HWND hwnd, HINSTANCE hInstance, HICON hIcon, wchar_t * lpLangFile);

HWND			g_hAbout;

#endif	// ABOUT_H__
