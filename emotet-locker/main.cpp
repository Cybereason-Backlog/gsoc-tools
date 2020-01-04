
//  Copyright 2020 Cybereason, Inc.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this softwareand associated documentation files(the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense,
//  and /or sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions :
//
//  The above copyright noticeand this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include <fileapi.h>

#define LETTERS_NUM 26
#define MAX_MUTANTS_NUM (LETTERS_NUM * 2)

#define _LOG_PRINT(fmt, ...) _tprintf(fmt, __VA_ARGS__)

#ifdef _DEBUG
#define _DEBUG_LOG_PRINT(fmt, ...)  _tprintf(fmt, __VA_ARGS__)
#else
#define _DEBUG_LOG_PRINT(fmt, ...)
#endif


static BOOL _GetSerialNumber(TCHAR DriveLetter, LPDWORD lpVolumeSerianNumber) {
	TCHAR szRootPath[4];
	szRootPath[0] = DriveLetter;
	szRootPath[1] = ':';
	szRootPath[2] = '\\';
	szRootPath[3] = '\0';

	return GetVolumeInformation(szRootPath, NULL, 0,
		lpVolumeSerianNumber, NULL, NULL, NULL, 0);
}

static BOOL _IsPhysicalDrives(TCHAR DriveLetter) {
	TCHAR szRootPath[4];
	szRootPath[0] = DriveLetter;
	szRootPath[1] = ':';
	szRootPath[2] = '\\';
	szRootPath[3] = '\0';

	UINT DriveType = GetDriveType(szRootPath);
	return DriveType == DRIVE_REMOVABLE
		|| DriveType == DRIVE_FIXED
		|| DriveType == DRIVE_CDROM
		|| DriveType == DRIVE_RAMDISK;
}


static HANDLE _CreateMutex(TCHAR* pszName) {

	SetLastError(0);
	HANDLE h = CreateMutex(NULL, TRUE, pszName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_LOG_PRINT(L"  mutex already exists\n");
		CloseHandle(h);
		return NULL;
	}

	if (h != NULL) {
		DWORD result = WaitForSingleObject(h, 5 * 1000);
		if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED) {
			_LOG_PRINT(L"  grabbed successfully \n");
		}
		else {
			_LOG_PRINT(L"  failed to grab until timeout. error = %d \n", result);
		}
	}
	else {
		_LOG_PRINT(L"  failed to grab. error = %d \n", GetLastError());
	}
	return h;
}

static HANDLE* emotet2019SepVerFunc(LPVOID param) {
	TCHAR szName[32];
	HANDLE* lphMutants;
	lphMutants = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HANDLE) * MAX_MUTANTS_NUM);
	if (lphMutants == NULL) {
		_LOG_PRINT(L"Memory allocation error. exist.\n");
		return NULL;
	}

	int i, j;
	DWORD Serial = 0;
	DWORD Drives = GetLogicalDrives();

	for (i = 0, j = 0; i < LETTERS_NUM; i++) {
		if (((1 << i) & Drives)
			&& _GetSerialNumber((TCHAR)('A' + i), &Serial)
			&& _IsPhysicalDrives((TCHAR)('A' + i))) {
			_snwprintf_s(szName, sizeof(szName) - 1, L"Global\\M%08X", Serial);
			_LOG_PRINT(L"[%c] Grabbing mutex '%s'\n", (TCHAR)('A' + i), szName);
			lphMutants[j++] = _CreateMutex(szName);
			_snwprintf_s(szName, sizeof(szName) - 1, L"Global\\I%08X", Serial);
			_LOG_PRINT(L"[%c] Grabbing mutex '%s'\n", (TCHAR)('A' + i), szName);
			lphMutants[j++] = _CreateMutex(szName);
		}
	}
	return lphMutants;
}

DWORD WINAPI threadProc(LPVOID param) {
	HANDLE* lphMutants = emotet2019SepVerFunc(param);

	if (lphMutants != NULL) {
#if _DEBUG
		Sleep(60 * 1000);
#else
		Sleep(INFINITE);
#endif
	}

	if (lphMutants != NULL) {
		for (int i = 0; i < MAX_MUTANTS_NUM; i++) {
			if (lphMutants[i] != NULL) {
				ReleaseMutex(lphMutants[i]);
				CloseHandle(lphMutants[i]);
			}
		}
		HeapFree(GetProcessHeap(), 0, lphMutants);
	}
	return 0;
}

int main(int argc, TCHAR* argv[]) {

	HANDLE hThread = CreateThread(NULL, 0, threadProc, (LPVOID)0, 0, NULL);
	if (hThread != NULL) {
		WaitForSingleObject(hThread, INFINITE);
	}
	else {
		_LOG_PRINT(L"Failed to create thread. exist.\n");
	}
	return 0;
}