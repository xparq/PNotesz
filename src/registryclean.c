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

#include "registryclean.h"

/*-@@+@@------------------------------------------------------------------
 Procedure: CleanRegMUICache
 Created  : Thu Aug  9 17:59:15 2007
 Modified : Thu Aug  9 17:59:15 2007

 Synopsys : Cleans MUICache
 Input    : Program name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CleanRegMUICache(const char * lpcProgName){

	HKEY		hKey;			//main key
	LONG		result;			//result
	DWORD		index = 0;		//index
	DWORD 		cb = 512;		//buffer size
	char		szValue[512];	//buffer

	//open main key
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache", 0, KEY_ALL_ACCESS, &hKey);
	if(result == ERROR_SUCCESS){
		__try{
			//loop through values
			result = RegEnumValue(hKey, index, szValue, &cb, NULL, NULL, NULL, NULL);
			while(result == ERROR_SUCCESS){
				//if there is our program name in value name - delete this value
				if(_stristr(szValue, lpcProgName)){
					RegDeleteValue(hKey, szValue);
					break;
				}
				//increase index
				index++;
				//reset size, since it has been changed in previous function call
				cb = 512;
				//loop
				result = RegEnumValue(hKey, index, szValue, &cb, NULL, NULL, NULL, NULL);
			}
		}
		__finally{
			//close main key
			RegCloseKey(hKey);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CleanRegMRU
 Created  : Sat Aug 18 11:10:06 2007
 Modified : Sat Aug 18 11:10:06 2007

 Synopsys : Cleans MRU
 Input    : Program name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CleanRegMRU(const char * lpcProgName){
	HKEY		hKey;						//main key
	LONG		result;						//result
	DWORD		index = 0;					//index
	DWORD		cb = 512, cbValue = 512;	//buffers sizes
	char		szValue[512];				//buffer
	char		szBuffer[512];				//buffer

	//open main key
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU", 0, KEY_ALL_ACCESS, &hKey);
	if(result == ERROR_SUCCESS){
		__try{
			//loop through values
			result = RegEnumValue(hKey, index, szValue, &cb, NULL, NULL, NULL, NULL);
			while(result == ERROR_SUCCESS){
				//increase index
				index++;
				//get value's data
				if(RegQueryValueEx(hKey, szValue, NULL, NULL, (unsigned char *)szBuffer, &cbValue) == ERROR_SUCCESS){
					if(_stristr(szBuffer, lpcProgName)){
						//if value's data contains our program name - delete this value
						RegDeleteValue(hKey, szValue);
						//return index back
						index--;
					}
				}
				//reset sizes, since they are changed in previous functions calls
				cbValue = 512;
				cb = 512;
				//loop
				result = RegEnumValue(hKey, index, szValue, &cb, NULL, NULL, NULL, NULL);
			}
		}
		__finally{
			//close main key
			RegCloseKey(hKey);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CleanRegOpenWithList
 Created  : Sat Aug 18 15:30:37 2007
 Modified : Sat Aug 18 15:30:37 2007

 Synopsys : Cleans OpenWithList
 Input    : Program name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CleanRegOpenWithList(const char * lpcProgName){

	HKEY		hKey;								//main key
	HKEY		hKeyExt;							//extention key
	HKEY		hKeyOpen;							//extension subkey
	LONG		result, resultExt, resultOpen;		//results
	DWORD		index = 0, indexExt, indexOpen;		//indexes
	DWORD		cb = 512, cbO = 512; 				//buffers sizes
	DWORD		cbValueExt = 512, cbValueOpen;		//buffers sizes
	char		szValue[512], szValueExt[512];		//buffers
	char		szValueOpen[512], szBuffer[512];	//buffers

	//open FileExts key
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts", 0, KEY_ALL_ACCESS, &hKey);
	if(result == ERROR_SUCCESS){
		__try{
			//check each extension
			result = RegEnumKeyEx(hKey, index, szValue, &cb, NULL, NULL, NULL, NULL);
			while(result == ERROR_SUCCESS){
				//open extension key
				if(RegOpenKeyEx(hKey, szValue, 0, KEY_ALL_ACCESS, &hKeyExt) == ERROR_SUCCESS){
					__try{
						indexExt = 0;
						//check each subkey of extension; usually includes OpenWithList and/or OpenWithProgids
						resultExt = RegEnumKeyEx(hKeyExt, indexExt, szValueExt, &cbValueExt, NULL, NULL, NULL, NULL);
						while(resultExt == ERROR_SUCCESS){
							//increase index
							indexExt++;
							//open each subkey
							if(RegOpenKeyEx(hKeyExt, szValueExt, 0, KEY_ALL_ACCESS, &hKeyOpen) == ERROR_SUCCESS){
								__try{
									indexOpen = 0;
									//check all subkey values
									resultOpen = RegEnumValue(hKeyOpen, indexOpen, szValueOpen, &cbValueOpen, NULL, NULL, NULL, NULL);
									while(resultOpen == ERROR_SUCCESS){
										//increase index
										indexOpen++;
										//get value's data
										if(RegQueryValueEx(hKeyOpen, szValueOpen, NULL, NULL, (unsigned char *)szBuffer, &cbO) == ERROR_SUCCESS){
											if(_stristr(szBuffer, lpcProgName)){
												//if there is our program name in value's data - delete this value
												RegDeleteValue(hKeyOpen, szValueOpen);
												//empty buffer
												szBuffer[0] = '\0';
												//return index one position back, in order not to miss the next value
												indexOpen--;
											}
										}
										//restore default sizes, since they are changed in previous functions calls
										cbO = 512;
										cbValueOpen = 512;
										//loop
										resultOpen = RegEnumValue(hKeyOpen, indexOpen, szValueOpen, &cbValueOpen, NULL, NULL, NULL, NULL);
									}
								}
								__finally{
									//close subkey
									RegCloseKey(hKeyOpen);
								}
							}
							//restore default size, since it is changed in previous functions calls
							cbValueExt = 512;
							//loop
							resultExt = RegEnumKeyEx(hKeyExt, indexExt, szValueExt, &cbValueExt, NULL, NULL, NULL, NULL);
						}
					}
					__finally{
						//close extension subkey
						RegCloseKey(hKeyExt);
					}
				}
				//increase index
				index++;
				//restore default size, since it is changed in previous functions calls
				cb = 512;
				//loop
				result = RegEnumKeyEx(hKey, index, szValue, &cb, NULL, NULL, NULL, NULL);
			}
		}
		__finally{
			//close main key
			RegCloseKey(hKey);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CleanRegOpenSaveMRU
 Created  : Sat Aug 18 15:31:23 2007
 Modified : Sat Aug 18 15:31:23 2007

 Synopsys : Cleans OpenSaveMRU
 Input    : Program name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CleanRegOpenSaveMRU(const char * lpcProgName){
	
	HKEY		hKey;							//main key
	HKEY		hKeyExt;						//extention key
	LONG		result, resultExt;				//results
	DWORD		index = 0, indexExt = 0;		//indexes
	DWORD		cb = 512, cbO = 512; 			//buffers sizes
	DWORD		cbValueExt = 512;				//buffers sizes
	char		szValue[512], szValueExt[512];	//buffers
	char		szBuffer[512];					//buffers

	//open ComDlg32\OpenSaveMRU key
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSaveMRU", 0, KEY_ALL_ACCESS, &hKey);
	if(result == ERROR_SUCCESS){
		__try{
			//check each extension
			resultExt = RegEnumKeyEx(hKey, indexExt, szValueExt, &cbValueExt, NULL, NULL, NULL, NULL);
			while(resultExt == ERROR_SUCCESS){
				//open extension key
				if(RegOpenKeyEx(hKey, szValueExt, 0, KEY_ALL_ACCESS, &hKeyExt) == ERROR_SUCCESS){
					__try{
						index = 0;
						//loop through each subkey
						result = RegEnumValue(hKeyExt, index, szValue, &cb, NULL, NULL, NULL, NULL);
						while(result == ERROR_SUCCESS){
							//increase index
							index++;
							//get value's data
							if(RegQueryValueEx(hKeyExt, szValue, NULL, NULL, (unsigned char *)szBuffer, &cbO) == ERROR_SUCCESS){
								if(_stristr(szBuffer, lpcProgName)){
									//if there is our program name in value's data - delete this value
									RegDeleteValue(hKeyExt, szValue);
									//empty buffer
									szBuffer[0] = '\0';
									//return index one position back, in order not to miss the next value
									index--;
								}
							}
							//restore default sizes, since they are changed in previous functions calls
							cbO = 512;
							cb = 512;
							//loop
							result = RegEnumValue(hKeyExt, index, szValue, &cb, NULL, NULL, NULL, NULL);
						}
					}
					__finally{
						//close extension subkey
						RegCloseKey(hKeyExt);
					}
				}
				//increase index
				indexExt++;
				//restore default size, since it is changed in previous functions calls
				cbValueExt = 512;
				//loop
				resultExt = RegEnumKeyEx(hKey, indexExt, szValueExt, &cbValueExt, NULL, NULL, NULL, NULL);
			}
		}
		__finally{
			//close main key
			RegCloseKey(hKey);
		}
	}
}
