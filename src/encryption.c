// Copyright (C) 2009 Andrey Gruber (aka lamer)

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

#define WIN32_LEAN_AND_MEAN  /* speed up compilations */

#include <windows.h>
#include <windowsx.h>
#include <wincrypt.h>
#include <winuser.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>

#define KEYLENGTH  			0x00800000
#define ENCRYPT_ALGORITHM 	CALG_RC4 
#define ENCRYPT_BLOCK_SIZE 	8 
#define	KEY_SET				"KEY_SET"

BOOL CryptEncryptFile(wchar_t * lpszSrcFile, wchar_t * lpszDestFile, wchar_t * lpszPwrd){
	// Declare and initialize local variables.
    BOOL 			fReturn = FALSE;
    HANDLE 			hSourceFile = INVALID_HANDLE_VALUE;
    HANDLE 			hDestinationFile = INVALID_HANDLE_VALUE; 

    HCRYPTPROV 		hCryptProv = 0; 
    HCRYPTKEY 		hKey = 0; 
    HCRYPTHASH 		hHash = 0; 

    PBYTE 			pbBuffer = NULL; 
    DWORD 			dwBlockLen; 
    DWORD 			dwBufferLen; 
    DWORD 			dwCount; 

	char			*pszSourceFile = NULL, *pszDestinationFile = NULL, *pszPassword = NULL;

	pszSourceFile = calloc(wcslen(lpszSrcFile) + 1, sizeof(char));
	if(!pszSourceFile)
		goto _CEF_Exit;
	if(wcstombs(pszSourceFile, lpszSrcFile, wcslen(lpszSrcFile)) == -1)
		goto _CEF_Exit;
	pszDestinationFile = calloc(wcslen(lpszDestFile) + 1, sizeof(char));
	if(!pszDestinationFile)
		goto _CEF_Exit;
	if(wcstombs(pszDestinationFile, lpszDestFile, wcslen(lpszDestFile)) == -1)
		goto _CEF_Exit;
	pszPassword = calloc(wcslen(lpszPwrd) + 1, sizeof(char));
	if(!pszPassword)
		goto _CEF_Exit;
	if(wcstombs(pszPassword, lpszPwrd, wcslen(lpszPwrd)) == -1)
		goto _CEF_Exit;
	// Open the source file. 
    hSourceFile = CreateFile(pszSourceFile, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hSourceFile == INVALID_HANDLE_VALUE)
		goto _CEF_Exit;
	// Open the destination file. 
    hDestinationFile = CreateFile(pszDestinationFile, FILE_WRITE_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDestinationFile == INVALID_HANDLE_VALUE)
		goto _CEF_Exit;
	// Get the handle to the default provider. 
    if(CryptAcquireContext(&hCryptProv, KEY_SET, MS_ENHANCED_PROV, PROV_RSA_FULL, 0) == 0)
		if(CryptAcquireContext(&hCryptProv, KEY_SET, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET) == 0)
			goto _CEF_Exit;
    // The file will be encrypted with a session key derived from a password.
    // The session key will be recreated when the file is 
    // decrypted only if the password used to create the key is available. 

    // Create a hash object. 
	if(CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash) == 0)
		goto _CEF_Exit;
	// Hash the password. 
    if(CryptHashData(hHash, (BYTE *)pszPassword, lstrlen(pszPassword), 0) == 0)
		goto _CEF_Exit;
	// Derive a session key from the hash object. 
    if(CryptDeriveKey(hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey) == 0)
		goto _CEF_Exit;
	//---------------------------------------------------------------
    // The session key is now ready. If it is not a key derived from 
    // a  password, the session key encrypted with the private key 
    // has been written to the destination file.
     
    //---------------------------------------------------------------
    // Determine the number of bytes to encrypt at a time. 
    // This must be a multiple of ENCRYPT_BLOCK_SIZE.
    // ENCRYPT_BLOCK_SIZE is set by a #define statement.
    dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE; 
	
	// Determine the block size. If a block cipher is used, 
    // it must have room for an extra block. 
    if(ENCRYPT_BLOCK_SIZE > 1) 
    {
        dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE; 
    }
    else 
    {
        dwBufferLen = dwBlockLen; 
    }
	// Allocate memory. 
    if((pbBuffer = (BYTE *)malloc(dwBufferLen)) == 0)
		goto _CEF_Exit;
	// In a do loop, encrypt the source file, 
    // and write to the source file. 
    BOOL fEOF = FALSE;
    do 
    { 
        // Read up to dwBlockLen bytes from the source file. 
        if(!ReadFile(hSourceFile, pbBuffer, dwBlockLen, &dwCount, NULL))
			goto _CEF_Exit;
        if(dwCount < dwBlockLen)
            fEOF = TRUE;
        // Encrypt data. 
        if(!CryptEncrypt(hKey, 0, fEOF, 0, pbBuffer, &dwCount, dwBufferLen))
			goto _CEF_Exit;
        // Write the encrypted data to the destination file. 
        if(!WriteFile(hDestinationFile, pbBuffer, dwCount, &dwCount, NULL))
			goto _CEF_Exit;
        // End the do loop when the last block of the source file 
        // has been read, encrypted, and written to the destination file.
    } while(!fEOF);

    fReturn = TRUE;

_CEF_Exit:
	// Close files.
    if(hSourceFile)
        CloseHandle(hSourceFile);
    if(hDestinationFile)
        CloseHandle(hDestinationFile);
    // Free memory. 
	if(pszSourceFile)
		free(pszSourceFile);
	if(pszDestinationFile)
		free(pszDestinationFile);
	if(pszPassword)
		free(pszPassword);
    if(pbBuffer) 
        free(pbBuffer); 
    // Release the hash object. 
    if(hHash) 
    {
        if(!(CryptDestroyHash(hHash)))
			;
        hHash = 0;
    }
    // Release the session key. 
    if(hKey)
    {
        if(!(CryptDestroyKey(hKey)))
			;
    }
    // Release the provider handle. 
    if(hCryptProv)
    {
        if(!(CryptReleaseContext(hCryptProv, 0)))
			;
    }
	return fReturn;
}

BOOL CryptDecryptFile(wchar_t * lpszSrcFile, wchar_t * lpszDestFile, wchar_t * lpszPwrd){
	// Declare and initialize local variables.
    BOOL fReturn = FALSE;
    HANDLE hSourceFile = INVALID_HANDLE_VALUE;
    HANDLE hDestinationFile = INVALID_HANDLE_VALUE; 

    HCRYPTKEY hKey = 0; 
    HCRYPTHASH hHash = 0; 
    HCRYPTPROV hCryptProv = 0; 

    DWORD dwCount;
    PBYTE pbBuffer = NULL; 
    DWORD dwBlockLen; 
    DWORD dwBufferLen; 

	char			*pszSourceFile = NULL, *pszDestinationFile = NULL, *pszPassword = NULL;

	pszSourceFile = calloc(wcslen(lpszSrcFile) + 1, sizeof(char));
	if(!pszSourceFile)
		goto _CDF_Exit;
	if(wcstombs(pszSourceFile, lpszSrcFile, wcslen(lpszSrcFile)) == -1)
		goto _CDF_Exit;
	pszDestinationFile = calloc(wcslen(lpszDestFile) + 1, sizeof(char));
	if(!pszDestinationFile)
		goto _CDF_Exit;
	if(wcstombs(pszDestinationFile, lpszDestFile, wcslen(lpszDestFile)) == -1)
		goto _CDF_Exit;
	pszPassword = calloc(wcslen(lpszPwrd) + 1, sizeof(char));
	if(!pszPassword)
		goto _CDF_Exit;
	if(wcstombs(pszPassword, lpszPwrd, wcslen(lpszPwrd)) == -1)
		goto _CDF_Exit;
	// Open the source file. 
    hSourceFile = CreateFile(pszSourceFile, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hSourceFile == INVALID_HANDLE_VALUE)
		goto _CDF_Exit;
	// Open the destination file. 
    hDestinationFile = CreateFile(pszDestinationFile, FILE_WRITE_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDestinationFile == INVALID_HANDLE_VALUE)
		goto _CDF_Exit;
	// Get the handle to the default provider. 
    if(CryptAcquireContext(&hCryptProv, KEY_SET, MS_ENHANCED_PROV, PROV_RSA_FULL, 0) == 0)
		if(CryptAcquireContext(&hCryptProv, KEY_SET, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET) == 0)
			goto _CDF_Exit;
    // Decrypt the file with a session key derived from a password. 
	// Create a hash object. 
    if(!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash))
		goto _CDF_Exit;
	// Hash in the password data. 
    if(!CryptHashData(hHash, (BYTE *)pszPassword, lstrlen(pszPassword), 0))
		goto _CDF_Exit;
	// Derive a session key from the hash object. 
	if(!CryptDeriveKey(hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey))
		goto _CDF_Exit;

	// The decryption key is now available, either having been 
    // imported from a BLOB read in from the source file or having 
    // been created by using the password. This point in the program 
    // is not reached if the decryption key is not available.
     
    //---------------------------------------------------------------
    // Determine the number of bytes to decrypt at a time. 
    // This must be a multiple of ENCRYPT_BLOCK_SIZE. 

    dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE; 
    dwBufferLen = dwBlockLen;

	// Allocate memory for the file read buffer. 
	if(!(pbBuffer = (PBYTE)malloc(dwBufferLen)))
		goto _CDF_Exit;
	
	// Decrypt the source file, and write to the destination file. 
    BOOL fEOF = FALSE;
    do
    {
        // Read up to dwBlockLen bytes from the source file. 
        if(!ReadFile(hSourceFile, pbBuffer, dwBlockLen, &dwCount, NULL))
			goto _CDF_Exit;
        if(dwCount < dwBlockLen)
            fEOF = TRUE;
        // Decrypt the block of data. 
        if(!CryptDecrypt(hKey, 0, fEOF, 0, pbBuffer, &dwCount))
			goto _CDF_Exit;
        // Write the decrypted data to the destination file. 
        if(!WriteFile(hDestinationFile, pbBuffer, dwCount, &dwCount, NULL))
			goto _CDF_Exit;
        // End the do loop when the last block of the source file 
        // has been read, encrypted, and written to the destination file.
    }while(!fEOF);
	fReturn = TRUE;

_CDF_Exit:
	// Close files.
    if(hSourceFile)
        CloseHandle(hSourceFile);
    if(hDestinationFile)
        CloseHandle(hDestinationFile);
    // Free memory. 
	if(pszSourceFile)
		free(pszSourceFile);
	if(pszDestinationFile)
		free(pszDestinationFile);
	if(pszPassword)
		free(pszPassword);
    if(pbBuffer) 
        free(pbBuffer);
	// Release the hash object. 
    if(hHash) 
    {
        if(!(CryptDestroyHash(hHash)))
			;
        hHash = 0;
    }
    // Release the session key. 
    if(hKey)
    {
        if(!(CryptDestroyKey(hKey)))
			;
    } 
    // Release the provider handle. 
    if(hCryptProv)
    {
        if(!(CryptReleaseContext(hCryptProv, 0)))
			;
    } 
	return fReturn;
}

void CreateHashString(wchar_t * lpszDest, const wchar_t * lpszSrc){
	HCRYPTPROV		hProv;
	HCRYPTHASH		hHash;
	char			*pData;
	char			*pbHash;
	char			*pbHashSize;
	DWORD			dwHashLen = sizeof(DWORD);
	char			*pbString, buffer[4];
	int				result;

	*lpszDest = '\0';
	pData = calloc(wcslen(lpszSrc) + 1, sizeof(char));
	if(pData){
		if(wcstombs(pData, lpszSrc, wcslen(lpszSrc)) != -1){
			result = CryptAcquireContext(&hProv, KEY_SET, NULL, PROV_RSA_FULL, 0);
			if(!result){
				if(GetLastError() == NTE_BAD_KEYSET)
					result = CryptAcquireContext(&hProv, KEY_SET, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
			}
			if(result){
				if(CryptCreateHash(hProv, ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD5, 0, 0, &hHash)){
					if(CryptHashData(hHash, (const unsigned char *)pData, strlen(pData) + 1, 0)){
						pbHashSize = calloc(dwHashLen, sizeof(char));
						if(pbHashSize){
							if(CryptGetHashParam(hHash, HP_HASHSIZE, (unsigned char *)pbHashSize, &dwHashLen, 0)){
								free(pbHashSize);
								if(CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0)){
									pbHash = calloc(dwHashLen, sizeof(char));
									if(pbHash){
										if(CryptGetHashParam(hHash, HP_HASHVAL, (unsigned char *)pbHash, &dwHashLen, 0)){
											pbString = calloc(2 * dwHashLen, sizeof(char));
											if(pbString){
												for(int i = 0 ; i < dwHashLen ; i++){
													sprintf(buffer, "%02hhX", pbHash[i]);
													strcat(pbString, buffer);
											   	}
												mbstowcs(lpszDest, pbString, 2 * dwHashLen);
												if(wcslen(lpszDest) > 2 * dwHashLen)
													lpszDest[2 * dwHashLen] = '\0';
												free(pbString);
											}
										}
									}
								}
							}
						}
					}
					CryptDestroyHash(hHash);
				}
				CryptReleaseContext(hProv, 0);
			}
		}
		free(pData);
	}
}


