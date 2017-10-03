// Lab3Dll.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Lab3Dll.h"
#include "DunamicLib.h"

#define MAX_LOADSTRING 100
#define IDM_LOAD_STATIC_DDL 10001
#define IDM_LOAD_DYNAMIC_DDL 10002
#define IDM_LOAD_TOPID 10003
#define PATH_TO_DLL L"C:\\Users\\Ilya\\Documents\\GitHub\\WinApi\\WinApi\\Lab3\\DunamicLib\\Debug\\DunamicLib.dll"


// Глобальные переменные:
HINSTANCE hInst;
HWND ghWnd;
LPWSTR szTitle;
LPWSTR szWindowClass;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance, LPWSTR szWindowClass);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    DlgStringsFunc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    DlgPIDFunc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{

	szTitle = L"DDL Libs";
	szWindowClass = L"MyClass";
	MyRegisterClass(hInstance, szWindowClass);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance, LPWSTR szWindowClass)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_WARNING);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	HMENU hMenu = CreateMenu();
	HMENU hPopMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hPopMenu, L"Dll");
	AppendMenu(hPopMenu, MF_STRING, IDM_LOAD_STATIC_DDL, L"Call Static");
	AppendMenu(hPopMenu, MF_STRING, IDM_LOAD_DYNAMIC_DDL, L"Call Dynamic");
	AppendMenu(hPopMenu, MF_STRING, IDM_LOAD_TOPID, L"Load to PID");

	SetMenu(hWnd, hMenu);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
 
LPSTR Find = (LPSTR)GlobalAlloc(GPTR, 100 * sizeof(CHAR));
LPSTR Replace = (LPSTR)GlobalAlloc(GPTR, 100 * sizeof(CHAR));
int PID = 0;

bool GetStrings(HWND hWnd)
{
	DialogBoxW(hInst, L"IDD_STRINGBOX", hWnd, DlgStringsFunc);
	if (Find[0] == 0 || Replace[0] == 0) return false;
	return true;
}


void PerformDunamicCall(HWND hWnd,LPWSTR DllPath)
{
//	HWND hWnd = ghWnd;
	HMODULE hModule = LoadLibrary(DllPath);
	typedef bool __stdcall TFindStr(LPSTR,LPSTR,int);
	TFindStr* pFindStr;
	pFindStr = (TFindStr*)GetProcAddress(hModule, "FindString");

	if (GetStrings(hWnd))
	{
		DialogBoxW(hInst, MAKEINTRESOURCE(IDD_NUMBERBOX), hWnd, DlgPIDFunc);

		if (PID != 0)
		{
			bool B = pFindStr(Find, Replace, PID);
		}
	}	
	FreeLibrary(hModule);
}

void PerformStaticCall(HWND hWnd)
{
	if (GetStrings(hWnd))
	{
		DialogBoxW(hInst, MAKEINTRESOURCE(IDD_NUMBERBOX), hWnd, DlgPIDFunc);

		if (PID != 0)
		{
			bool B = FindString(Find, Replace, PID);
		}
		
	}
}

BOOL CALLBACK DlgPIDFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPWSTR Number;
	switch (message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			Number = (LPWSTR)GlobalAlloc(GPTR, 10 * sizeof(TCHAR));
			if (GetDlgItemText(hWnd, IDC_EDIT_NUMBER, Number, 6))
			{
				PID = _wtoi(Number);
			}
			GlobalFree(Number);
		case IDCANCEL:
			EndDialog(hWnd, wParam);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CALLBACK DlgStringsFunc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		SetDlgItemText(hWnd, IDC_EDIT_ENTER_STRING, L"Find");
		SetDlgItemText(hWnd, IDC_EDIT_ENTER_STRING2, L"Replace");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (!GetDlgItemTextA(hWnd, IDC_EDIT_ENTER_STRING, Find, 100))
			{
				Find[0] = 0;
			}
			if (!GetDlgItemTextA(hWnd, IDC_EDIT_ENTER_STRING2, Replace, 100))
			{
				Replace[0] = 0;
			}
		case IDCANCEL:
			EndDialog(hWnd, wParam);
			return TRUE;
		}
	}
	return FALSE;
}

void LoadDllToProcess(HWND hWnd)
{
	DialogBoxW(hInst, MAKEINTRESOURCE(IDD_NUMBERBOX), hWnd, DlgPIDFunc);
	
	if (PID != 0)
	{
		char fullPath[MAX_PATH] = "C:\\Users\\NotePad.by\\Documents\\GitHub\\WinApi\\Lab3\\DunamicLib2\\Debug\\DunamicLib.dll";
		LPVOID libAddr, llParam;

		HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
		if (hProc == NULL)
		{
			MessageBox(hWnd, L"Unable to open process!", L"Error!!", 
				MB_ICONERROR);
			return;
		}
		
		libAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		if (libAddr == NULL)
		{
			MessageBox(hWnd, L"Unable to find LoadLibraryW in kernel32.dll!", L"Error!!", 
				MB_ICONERROR);
			return;
		}

		llParam = (LPVOID)VirtualAllocEx(hProc, NULL, strlen(fullPath)+1,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (llParam == NULL)
		{
			MessageBox(hWnd, L"Unable to allocate virual memory in remote process!", L"Error!!", 
				MB_ICONERROR);
			return;
		}

		int n = WriteProcessMemory(hProc, llParam, fullPath, strlen(fullPath) + 1, NULL);
		if (n == NULL)
		{
			MessageBox(hWnd, L"There was no bytes written to the process's address!", L"Error!!",
				MB_ICONERROR);
			return;
		}
		HANDLE threadID  = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)libAddr, (LPVOID)llParam, NULL, NULL);
		if (threadID == NULL)
		{
			MessageBox(hWnd, L"The remote thread could not be created!", L"Error!!",
				MB_ICONERROR);
			return;
		}



		CloseHandle(hProc);
	}
	
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	switch (message)
	{
	case WM_CREATE:
		ghWnd = hWnd;
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_LOAD_STATIC_DDL:
			PerformStaticCall(hWnd);
			break;
		case IDM_LOAD_DYNAMIC_DDL:
			PerformDunamicCall(hWnd,PATH_TO_DLL);
			break;
		case IDM_LOAD_TOPID:
			LoadDllToProcess(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
