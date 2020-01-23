
// Copyright (C) 2020 Cybereason, Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.

#include <tchar.h>
#include <stdio.h>
#include <Windows.h>
#include <fileapi.h>

#define LETTERS_NUM 26
#define MAX_MUTANTS_NUM (LETTERS_NUM * 4)

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
			_snwprintf_s(szName, sizeof(szName) - 1, L"Global\\M%X", Serial);
			_LOG_PRINT(L"[%c] Grabbing mutex '%s'\n", (TCHAR)('A' + i), szName);
			lphMutants[j++] = _CreateMutex(szName);
			_snwprintf_s(szName, sizeof(szName) - 1, L"Global\\I%X", Serial);
			_LOG_PRINT(L"[%c] Grabbing mutex '%s'\n", (TCHAR)('A' + i), szName);
			lphMutants[j++] = _CreateMutex(szName);
			_snwprintf_s(szName, sizeof(szName) - 1, L"M%X", Serial);
			_LOG_PRINT(L"[%c] Grabbing mutex '%s'\n", (TCHAR)('A' + i), szName);
			lphMutants[j++] = _CreateMutex(szName);
			_snwprintf_s(szName, sizeof(szName) - 1, L"I%X", Serial);
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
