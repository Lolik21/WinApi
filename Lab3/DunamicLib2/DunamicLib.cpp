// DunamicLib.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "DunamicLib.h";

#define MAX_READ 256

int fMatchCheck(char *mainstr, int mainstrLen, char *checkstr, int checkstrLen)
{
	/*
	Проверка наличия подстроки в строке.
	При этом под "строкой" подразумевается
	просто последовательность байт.
	*/
	BOOL fmcret = TRUE;
	int x, y;

	for (x = 0; x<mainstrLen; x++) {
		fmcret = TRUE;

		for (y = 0; y<checkstrLen; y++) {
			if (checkstr[y] != mainstr[x + y]) {
				fmcret = FALSE;
				break;
			}
		}

		if (fmcret)
			return x + checkstrLen;
	}
	return -1;
}

BOOL DoRtlAdjustPrivilege()
{
	/*
	Важная функция. Получаем привилегии дебаггера.
	Именно это позволит нам получить нужную информацию
	о доступности памяти.
	*/
#define SE_DEBUG_PRIVILEGE 20L
#define AdjustCurrentProcess 0
	BOOL bPrev = FALSE;
	LONG(WINAPI *RtlAdjustPrivilege)(DWORD, BOOL, INT, PBOOL);
	*(FARPROC *)&RtlAdjustPrivilege = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) return FALSE;
	RtlAdjustPrivilege(SE_DEBUG_PRIVILEGE, TRUE, AdjustCurrentProcess, &bPrev);
	return TRUE;
}

bool __stdcall FindString(LPSTR Find, LPSTR Replace,int PID)
{
	/*** VARIABLES ***/
	HANDLE hProc;

	MEMORY_BASIC_INFORMATION mbi;
	SYSTEM_INFO msi;
	LPWSTR error = NULL;
	ZeroMemory(&mbi, sizeof(mbi));
	GetSystemInfo(&msi);

	/*
	Получаем информацию о памяти в текущей системе.
	*/

	DWORD dwRead = 0;

	char *lpData = (char *)GlobalAlloc(GMEM_FIXED, MAX_READ);
	char *lpOrig = Find; // что ищем
	char *lpReplacement = Replace; // на что меняем

	int at;
	/*****************/

	if (!lpData)
		return -1;

	ZeroMemory(lpData, MAX_READ);

	// открываем процесс

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (hProc == NULL) return -1;

	if (DoRtlAdjustPrivilege()) {
		/*
		Привилегии отладчика для работы с памятью.
		*/

		for (LPBYTE lpAddress = (LPBYTE)msi.lpMinimumApplicationAddress;
			lpAddress <= (LPBYTE)msi.lpMaximumApplicationAddress;
			lpAddress += mbi.RegionSize) {
			/*
			Этот цикл отвечает как раз за то, что наша программа не совершит
			лишних действий. Память в Windows в процессе делится на "регионы".
			У каждого региона свой уровень доступа: к какому-то доступ запрещен,
			какой-то можно только прочитать. Нам нужны регионы доступные для записи.
			Это позволит в разы ускорить работу поиска по памяти и избежать ошибок
			записи в память. Именно так работает ArtMoney.
			*/

			if (VirtualQueryEx(hProc, lpAddress, &mbi, sizeof(mbi))) {
				/*
				Узнаем о текущем регионе памяти.
				*/

				if ((mbi.Protect & PAGE_READWRITE) || (mbi.Protect & PAGE_WRITECOPY)) {
					/*
					Если он доступен для записи, работаем с ним.
					*/
					LPBYTE Border = lpAddress + mbi.RegionSize;
					BOOL IsFound;
					while (lpAddress<Border)
					{
						/*
						Проходим по адресам указателей в памяти чужого процесса от начала, до конца региона
						и проверяем, не в них ли строка поиска.
						*/
						IsFound = FALSE;

						dwRead = 0;
						if (ReadProcessMemory(hProc, (LPCVOID)lpAddress, lpData, MAX_READ, &dwRead) == TRUE) {
							/*
							Читаем по 128 байт из памяти чужого процесса от начала
							и проверяем, не в них ли строка поиска.
							*/

							if (fMatchCheck(lpData, dwRead, lpOrig, strlen(lpOrig)) != -1) {
								/*Нашли, сообщим об успехе и поменяем в чужом процессе искомую строку на нашу.*/

								at = fMatchCheck(lpData, dwRead, lpOrig, strlen(lpOrig));

								IsFound = TRUE;

								at -= strlen(lpOrig);

								int k = 0;

								int Bord = at + strlen(lpOrig);

								for (at; (at < Bord)
									&& k <= strlen(lpReplacement); at++)
								{
									lpData[at] = lpReplacement[k];
									k++;
								}

								if (WriteProcessMemory(hProc, (LPVOID)lpAddress, lpData, dwRead, &dwRead)) {
									MessageBox(NULL, L"Sucess", L"Replacment done", MB_ICONWARNING);
								}

							} // if match
						}// read mem
						if (!IsFound) lpAddress += 128;
					} // for addr
				} // protect if
			}// virtual query
		} // main for
	}
	else error = L"Unable to get Debug privileges";

	if (error != NULL)
	{
		MessageBox(NULL, error, L"Error!", MB_ICONWARNING);
	}

	// // // // //
	// Cleanup
	if (hProc)
		CloseHandle(hProc);
	if (lpData)
		GlobalFree(lpData);
	///////////////

	return true;
}



