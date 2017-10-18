// Threads.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Threads.h"

#define LINES_TOTASK 10
#define THREADS_COUNT 1
#define MAX_LOADSTRING 100
#define MSG_SORT_THREAD_CREATED L"Sort threads created!\r\n"
#define MSG_DEVIDER_THREAD_CREATED L"Devider created!\r\n"
#define MSG_FILE_OPENED L"File opened. Path : \r\n"
#define MSG_TASKS_CREATED L"Tasks for threads created!\r\n"
#define MSG_TASK_COUNT L"Task count : "
#define MSG_SORT_THREAD_T_F L"%d finished task\r\n"
#define MSG_SORT_THREAD_F L"%d terminated\r\n"
#define MSG_DEVIDER_F L"Devider terminated!\r\n"

HINSTANCE hInst;
HWND hMain;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hEdit;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

class Line
{
public:
	int offcet;
	int size;
};
class Task
{
public:
	Line* Lines[LINES_TOTASK];
	bool IsDone;
	bool IsTerminate;
	int SortedInd;
};

Task* DeviderArr[THREADS_COUNT];
HANDLE DeviderArrMutex;
HANDLE TextBoxMutex;
std::vector<Task*> TaskQuery;

char* FileView;
LPWSTR FileName = NULL;
DWORD FileSize = 0;


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
	WaitForSingleObject(TextBoxMutex, INFINITE);

	HWND hwndOutput = hEdit;

	int outLength = GetWindowTextLength(hwndOutput) + wcslen(newText) + 1;

	TCHAR * buf = (TCHAR *)GlobalAlloc(GPTR, outLength * sizeof(TCHAR));
	if (!buf) return;

	GetWindowText(hwndOutput, buf, outLength);

	_tcscat_s(buf, outLength, newText);

	SetWindowText(hwndOutput, buf);

	GlobalFree(buf);

	ReleaseMutex(TextBoxMutex);
}

void RedrawEditBox(HWND hWnd)
{
	RECT wndRECT;
	GetWindowRect(hWnd, &wndRECT);
	SetWindowPos(hEdit, HWND_TOP, 0, 0, wndRECT.right, wndRECT.bottom, SWP_SHOWWINDOW);	
}

void PrintAddedTask(int i)
{
	wchar_t buff[256];
	swprintf_s(buff, L"Devider added task to ID : %d \r\n", i);
	AppendText(hMain, buff);
}

bool AddToDeviderArr(Task* AddingTask, int i)
{
	WaitForSingleObject(DeviderArrMutex, INFINITE);
	if (DeviderArr[i] == NULL)
	{
		DeviderArr[i] = AddingTask;
	}
	else
	{
		if (DeviderArr[i]->IsDone == true)
		{
			DeviderArr[i] = AddingTask;
		}
		else
		{
			ReleaseMutex(DeviderArrMutex);
			return false;
		}
	}
	PrintAddedTask(i);
	ReleaseMutex(DeviderArrMutex);
	return true;
}

DWORD WINAPI DeviderThreadProc(LPVOID lpParam) {
	size_t i = 0;
	int k = 0;
	
	while (k < TaskQuery.size())
	{
		while (!AddToDeviderArr(TaskQuery[k],i)) Sleep(10);
		k++;
		i++;
		if (i >= THREADS_COUNT) i = 0;
	}
	AppendText(hMain, MSG_DEVIDER_F);
	ExitThread(0);
}

Task* GetTask(int Identifier)
{
	WaitForSingleObject(DeviderArrMutex, INFINITE);
	Task* RezTask = DeviderArr[Identifier];
	ReleaseMutex(DeviderArrMutex);
	return RezTask;
}

void SetTaskComplite(int Identifier)
{
	WaitForSingleObject(DeviderArrMutex, INFINITE);
	DeviderArr[Identifier]->IsDone = true;
	ReleaseMutex(DeviderArrMutex);
}
DWORD WINAPI SortThreadProc(LPVOID lpParam) {
	int Identifier = (int)lpParam;
	bool IsTerminate = false;
	while (!IsTerminate)
	{
		Task* ThreadTask = GetTask(Identifier);
		if (ThreadTask != NULL && ThreadTask->IsTerminate == true)
		{
			IsTerminate = true;
		}
		else
		{
			if (ThreadTask != NULL && ThreadTask->IsDone == false)
			{
				SetTaskComplite(Identifier);
				wchar_t mybuff[256];
				swprintf_s(mybuff, MSG_SORT_THREAD_T_F, GetThreadId(GetCurrentThread()));
				AppendText(hMain, mybuff);
			}
		}		
	}
	wchar_t buff[256];
	swprintf_s(buff, MSG_SORT_THREAD_F, GetThreadId(GetCurrentThread()));
	AppendText(hMain, buff);
	ExitThread(0);
}

bool MapFile(LPWSTR FileName)
{
	HANDLE hFile = CreateFile(FileName, FILE_READ_DATA, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL)
	{
		return false;
	}
	FileSize = GetFileSize(hFile, NULL);
	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap == NULL)
	{
		return false;
	}
	FileView = (char*)MapViewOfFileEx(hMap, FILE_MAP_READ, 0, 0, 0, NULL);
	if (FileView == NULL)
	{
		return false;
	}
	CloseHandle(hFile);
	UnmapViewOfFile(hMap);
	return true;
}

void OpenSourceFile(HWND hWnd)
{
	OPENFILENAME ofn = { 0 };
	WCHAR szDirect[MAX_PATH] = { 0 };
	WCHAR szFileName[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szDirect;
	*(ofn.lpstrFile) = NULL;
	ofn.nMaxFile = sizeof(szDirect) / sizeof(WCHAR);
	ofn.lpstrFilter = L"Text files(*.txt)\0 * .txt\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileName;
	*(ofn.lpstrFileTitle) = NULL;
	ofn.nMaxFileTitle = sizeof(szFileName) / sizeof(WCHAR);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER;
	if (GetOpenFileName(&ofn) == TRUE)
	{
		FileName = (TCHAR*)GlobalAlloc(GPTR, (wcslen(ofn.lpstrFile) + 1)*2);
		for (int i = 0; i < wcslen(ofn.lpstrFile)+1; i++)
		{
			FileName[i] = ofn.lpstrFile[i];
		}
		AppendText(hWnd, MSG_FILE_OPENED);
		AppendText(hWnd, FileName);
		AppendText(hWnd, L"\r\n");
	}
}

Task* GetNewTask()
{
	Task *NewTask = new Task();
	NewTask->IsDone = false;
	NewTask->IsTerminate = false;
	NewTask->SortedInd = 0;
	return NewTask;
}

void PrepareTasks()
{
	DWORD i = 0;
	int LineSize = 0;
	int LinesFound = 0;
	Task *NewTask = GetNewTask();
	while (i < FileSize)
	{
		while (i < FileSize && FileView[i] != '\n')
		{
			LineSize++;
			i++;
		} 
		if (LineSize != 0)
		{
			Line *NewLine = new Line();
			NewLine->offcet = i - LineSize;
			LineSize++;
			NewLine->size = LineSize;
			NewTask->Lines[LinesFound] = NewLine;
			LinesFound++;
			if (LinesFound == LINES_TOTASK)
			{
				TaskQuery.insert(TaskQuery.end(), NewTask);
				NewTask = GetNewTask();
				LinesFound = 0;
			}
			else
			{
				if (i >= FileSize)
				{
					TaskQuery.insert(TaskQuery.end(), NewTask);
				}
			}
			LineSize = 0;		
		}	
		i++;
	}
}

void ConcatinateRezult()
{
	// TODO
}

HANDLE CreateMyFile()
{
	// TODO 
	return NULL;
}

void PrintTaskCount(HWND hWnd)
{
	AppendText(hWnd, MSG_TASK_COUNT);
	wchar_t m_reportFileName[10];
	swprintf_s(m_reportFileName, L"%d\r\n", TaskQuery.size());
	AppendText(hWnd, m_reportFileName);
}

void OnStartPerform(HWND hWnd)
{
	if (FileName == NULL) return;
	if (!MapFile(FileName)) return;
	PrepareTasks();
	AppendText(hWnd, MSG_TASKS_CREATED);
	PrintTaskCount(hWnd);
	DeviderArrMutex = CreateMutex(NULL, FALSE, NULL);
	HANDLE hThreads[THREADS_COUNT];

	for (int i = 0; i < THREADS_COUNT; i++)
	{
		hThreads[i] = CreateThread(NULL, 0, &SortThreadProc, (LPVOID)i, 0, NULL);
	}
	AppendText(hWnd, MSG_SORT_THREAD_CREATED);
	
	HANDLE Devider = CreateThread(NULL, 0, &DeviderThreadProc, 0, 0, NULL);
	AppendText(hWnd, MSG_DEVIDER_THREAD_CREATED);

	//WaitForSingleObject(Devider, INFINITE);
	//WaitForMultipleObjects(THREADS_COUNT, hThreads, TRUE, INFINITE);
	//ConcatinateRezult();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		TextBoxMutex = CreateMutex(NULL, FALSE, NULL);
		hMain = hWnd;
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_CHOUSE_FILE:
			OpenSourceFile(hWnd);
			break;
		case IDM_START:
			OnStartPerform(hWnd);
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
		if (FileName != NULL) GlobalFree(FileName);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

