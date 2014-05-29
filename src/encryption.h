#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__

void CreateHashString(wchar_t * lpszDest, const wchar_t * lpszSrc);
BOOL CryptEncryptFile(wchar_t * lpszSrcFile, wchar_t * lpszDestFile, wchar_t * lpszPwrd);
BOOL CryptDecryptFile(wchar_t * lpszSrcFile, wchar_t * lpszDestFile, wchar_t * lpszPwrd);
#endif	// ENCRYPTION_H__
