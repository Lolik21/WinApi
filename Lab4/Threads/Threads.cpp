// Threads.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Threads.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hEdit;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_THREADS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_THREADS));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THREADS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_THREADS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 640, 320, nullptr, nullptr, hInstance, nullptr);
	HWND hEditBox = CreateWindowExW(NULL, L"EDIT", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE | ES_OEMCONVERT,
		20, 20, 100, 100, hWnd, NULL, hInst, NULL);
	hEdit = hEditBox;

	HFONT Font = CreateFontW(12, 0, 0, 0, 0, 0, 0, 0,
		RUSSIAN_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console");
	SendMessageW(hEdit, WM_SETFONT, (WPARAM)Font, 0);


	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void AppendText(const HWND &hwnd, LPCWSTR newText)
{
	HWND hwndOutput = hEdit;
	DWORD StartPos, EndPos;
	SendMessageW(hwndOutput, EM_GETSEL, (WPARAM)&StartPos, (WPARAM)&EndPos);
	int outLength = GetWindowTextLength(hwndOutput);
	SendMessageW(hwndOutput, EM_SETSEL, outLength, outLength);
	SendMessageW(hwndOutput, EM_REPLACESEL, TRUE, (LPARAM)newText);
	SendMessageW(hwndOutput, EM_SETSEL, StartPos, EndPos);
}

void RedrawEditBox(HWND hWnd)
{
	RECT wndRECT;
	GetWindowRect(hWnd, &wndRECT);
	SetWindowPos(hEdit, HWND_TOP, 0, 0, wndRECT.right, wndRECT.bottom, SWP_SHOWWINDOW);
	AppendText(hWnd, L"ффф\r\n");
	
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_CHOUSE_FILE:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		RedrawEditBox(hWnd);
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

