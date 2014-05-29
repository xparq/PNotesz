// Copyright (C) 2010 Andrey Gruber (aka lamer)

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

#include <stdio.h>

#include "backuprestore.h"
#include "enums.h"
#include "shared.h"
#include "encryption.h"
#include "stringconstants.h"

BOOL DoFullBackup(wchar_t * lpBackFile){

	FILE				*fb, *fi;
	WIN32_FIND_DATA		wfd;
	int					c;
	HANDLE				hFile;
	BOOL				result = TRUE, fDoEncryption = FALSE;
	char				szPath[MAX_PATH], szTemp[MAX_PATH], szFile[MAX_PATH];
	wchar_t				wszCurrentHash[256], wszWPath[MAX_PATH], wszTempPath[MAX_PATH];
	
	fb = _wfopen(lpBackFile, L"wb");
	if(!fb)
		return FALSE;
	
	if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
		GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", wszCurrentHash, 256, g_NotePaths.INIFile);
		fDoEncryption = TRUE;
	}

	putc('\2', fb);
	fi = _wfopen(g_NotePaths.DataFile, L"rb");
	if(!fi){
		fclose(fb);
		return FALSE;
	}
	c = getc(fi);
	while(c != EOF){
		putc(c, fb);
		c = getc(fi);
	}
	fclose(fi);
	WideCharToMultiByte(CP_ACP, 0, g_NotePaths.DataDir, -1, szTemp, MAX_PATH, NULL, NULL);
	strcpy(szFile, szTemp);
	strcat(szTemp, "*.pnote");
	hFile = FindFirstFile(szTemp, &wfd);
	while(hFile != INVALID_HANDLE_VALUE && result){
		strcpy(szPath, szFile);
		strcat(szPath, wfd.cFileName);
		// set temp file name to 0
		*wszTempPath = '\0';
		// check whether file should be decrypted first
		if(fDoEncryption){
			// convert path to wide characters
			MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszWPath, MAX_PATH);
			// get temp file name
			GetNoteTempFileName(wszTempPath);
			// decrypt file
			CryptDecryptFile(wszWPath, wszTempPath, wszCurrentHash);
			// use created temp file as source
			WideCharToMultiByte(CP_ACP, 0, wszTempPath, -1, szPath, MAX_PATH, NULL, NULL);
		}
		putc('\3', fb);
		fputs(wfd.cFileName, fb);
		putc('\4', fb);
		fi = fopen(szPath, "rb");
		if(!fi){
			fclose(fb);
			return FALSE;
		}
		c = getc(fi);
		while(c != EOF){
			putc(c, fb);
			c = getc(fi);
		}
		fclose(fi);
		// remove temp file if there is any
		if(wszTempPath){
			DeleteFileW(wszTempPath);
		}
		result = FindNextFile(hFile, &wfd);
	}
	fclose(fb);
	
	return TRUE;
}

BOOL RestoreFullBackUp(wchar_t * lpBackFile){
	FILE				*fb, *fo;
	WIN32_FIND_DATA		wfd;
	int					c;
	HANDLE				hFile;
	BOOL				result = TRUE;
	char				szPath[MAX_PATH], szFile[MAX_PATH], szTemp[260], szDelete[MAX_PATH], *pc;
	wchar_t				wszCurrentHash[256], wszWPath[MAX_PATH], wszTempPath[MAX_PATH];
	BOOL				fDoEncryption = FALSE;

	// check whether notes should be encrypted after job
	if(IsBitOn(g_NoteSettings.reserved1, SB1_STORE_ENCRYPTED)){
		// get current hash
		GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", wszCurrentHash, 256, g_NotePaths.INIFile);
		fDoEncryption = TRUE;
	}

	WideCharToMultiByte(CP_ACP, 0, g_NotePaths.DataDir, -1, szFile, MAX_PATH, NULL, NULL);
	fb = _wfopen(lpBackFile, L"rb");
	if(!fb)
		return FALSE;
	strcpy(szPath, szFile);
	strcat(szPath, "*.pnote");
	hFile = FindFirstFile(szPath, &wfd);
	while(hFile != INVALID_HANDLE_VALUE && result){
		strcpy(szDelete, szFile);
		strcat(szDelete, wfd.cFileName);
		DeleteFile(szDelete);
		result = FindNextFile(hFile, &wfd);
	}
	c = getc(fb);
	if(c == '\2'){
		fo = _wfopen(g_NotePaths.DataFile, L"wb");
		if(!fo){
			fclose(fb);
			return FALSE;
		}
		c = getc(fb);
		while(c != '\3' && c != EOF){
			putc(c, fo);
			c = getc(fb);
		}
		fclose(fo);
	}
	while(c != EOF){
		memset(szTemp, '\0', 260);
		pc = szTemp;
		c = getc(fb);
		while(c != 4 && c != EOF){
			*pc++ = c;
			c = getc(fb);
		}
		strcpy(szPath, szFile);
		strcat(szPath, szTemp);
		fo = fopen(szPath, "wb");
		if(!fo){
			fclose(fb);
			return FALSE;
		}
		if(c == EOF)
			break;
		c = getc(fb);
		while(c != 3 && c != EOF){
			putc(c, fo);
			c = getc(fb);
		}
		fclose(fo);
		// check whether note should be encrypted again
		if(fDoEncryption){
			// convert created file name to wide characters
			MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszWPath, MAX_PATH);
			// get temp file name
			GetNoteTempFileName(wszTempPath);
			// encrypt note using current hash
			CryptEncryptFile(wszWPath, wszTempPath, wszCurrentHash);
			// copy encrypted file back
			CopyFileW(wszTempPath, wszWPath, FALSE);
			// delete temp file
			DeleteFileW(wszTempPath);
		}
	}
	fclose(fb);
	return TRUE;
}
