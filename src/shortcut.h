#ifndef __SHORTCUT_H__
#define __SHORTCUT_H__

HRESULT CreateShortcut(LPCTSTR pszLink,  LPCTSTR pszFile,  LPCTSTR pszDir,  LPCTSTR pszArgs,  int iShowCmd,  LPCTSTR pszIconPath,  int iIcon,  LPCTSTR pszDescription,  WORD wHotkey);

#endif	// SHORTCUT_H__
