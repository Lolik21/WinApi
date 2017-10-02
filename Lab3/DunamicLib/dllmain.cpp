// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//WCHAR str[10];
		//_itow_s(_getpid(), str, 10, 10);
		//MessageBox(NULL, L"Atached to PROCESS!!", str, MB_ICONEXCLAMATION);
		//break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

