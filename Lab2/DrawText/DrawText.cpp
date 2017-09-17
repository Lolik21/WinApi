
#include "stdafx.h"
#include "DrawText.h"

#define IDM_MENU_IMAGE_OPEN 0



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegWindowClass(HINSTANCE, LPWSTR);

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{

	LPWSTR ClassName = L"mainClass";

	if (!RegWindowClass(hInstance, ClassName))
		return -1;

	const int WindowH = 730;
	const int WindowW = 1280;

	RECT scr_rect;
	GetWindowRect(GetDesktopWindow(), &scr_rect);
	int WindowCenterX = (scr_rect.right / 2) - (WindowW / 2);
	int WindowCenterY = (scr_rect.bottom / 2) - (WindowH / 2);



	HWND hWnd;
	hWnd = CreateWindow(ClassName, L"Text", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		WindowCenterX, WindowCenterY, WindowW, WindowH, NULL, NULL, hInstance, NULL);

	if (!hWnd) return -2;

	MSG msg = { 0 };
	int Getter = 0;
	while ((Getter = GetMessage(&msg, NULL, 0, 0) != 0))
	{
		if (Getter == -1) return -1;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

ATOM RegWindowClass(HINSTANCE hInst, LPWSTR lpClassName)
{
	WNDCLASS WindowClass = { 0 };
	WindowClass.lpfnWndProc = WndProc;
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.hInstance = hInst;
	WindowClass.lpszClassName = lpClassName;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.hIcon = LoadIcon(NULL, IDI_SHIELD);
	WindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	return RegisterClass(&WindowClass);
}

const int COLUMS_COUNT = 3;
const int MIN_HEIGHT = 20;
int RectCount;
RECT Rectangles[1000] = {0};
LPWSTR Text[1000] = {0};

void MyDrawText(RECT Rectangle)
{

}

void PrintTable(HWND hWnd)
{
	RECT ClientRectangle;
	GetClientRect(hWnd, &ClientRectangle);
	int Width = ClientRectangle.right / COLUMS_COUNT;
	int OstHeight = ClientRectangle.bottom;
	int PerHeight = 0;

	HDC hDC;
	PAINTSTRUCT ps; 
	COLORREF colorText = RGB(255, 0, 0);

	hDC = BeginPaint(hWnd, &ps); 
	SetTextColor(hDC, colorText);

	int Left = 0;
	int K = 0;

	while (PerHeight < OstHeight)
	{
		Left = 0;
		int Height = MIN_HEIGHT;
		for (int i = 0; i < COLUMS_COUNT; i++)
		{
			Rectangles[K].left = Left;
			Rectangles[K].top = PerHeight;
			Rectangles[K].right = Left + Width;
			Rectangles[K].bottom = PerHeight + Height;
			Rectangle(hDC, Rectangles[K].left, Rectangles[K].top, Rectangles[K].right, Rectangles[K].bottom);
			K = K + 1;
			Left = Left + Width;
		}
		PerHeight = PerHeight + Height;
	}
	RectCount = K;

	if (Text[2] != NULL) 
		DrawText(hDC, Text[2], -1, &Rectangles[2], DT_WORDBREAK);
	EndPaint(hWnd, &ps);
}

int SelectedRECT = -1;
LPWSTR text;

int GetRECT(WORD xPos, WORD yPos)
{
	for (int i = 0; i < RectCount; i++)
	{
		if (xPos > Rectangles[i].left  && xPos < Rectangles[i].right)
		{
			if (yPos > Rectangles[i].top  && yPos < Rectangles[i].bottom)
			{
				return i;
			}
		}
	}
	return -1;
}

void WriteLetter(WPARAM wParam)
{
	if (SelectedRECT != -1)
	{
		if (Text[SelectedRECT] == NULL)
		{
			text = (LPWSTR)GlobalAlloc(GPTR, 1000 * sizeof(TCHAR));
			Text[SelectedRECT] = text;
		}
		int size = wcsnlen(Text[SelectedRECT],1000);
		if (size <= 1000)
			Text[SelectedRECT][size] = (char)wParam;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	int size = 0;
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		WORD xPos, yPos;
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		int K = GetRECT(xPos, yPos);
		SelectedRECT = K;

	}
	case WM_CHAR:
		if (isalpha(wParam))
			WriteLetter(wParam);

		
	case WM_PAINT:
		PrintTable(hWnd);

		break;
	case WM_DESTROY:
		//for (int i = 0; i < RectCount; i++)
		//{
		//	free(Text[i]);
		//}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}