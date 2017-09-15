


#include "stdafx.h"
#include "MiniPaint.h"

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

	const int WindowH = 720;
	const int WindowW = 1280;

	RECT scr_rect;
	GetWindowRect(GetDesktopWindow(), &scr_rect);
	int WindowCenterX = (scr_rect.right / 2) - (WindowW / 2);
	int WindowCenterY = (scr_rect.bottom / 2) - (WindowH / 2);

	HMENU hMenu = CreateMenu();
	HMENU hPopMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hPopMenu, L"File");
	AppendMenu(hPopMenu, MF_STRING, IDM_MENU_IMAGE_OPEN, L" Chouse Image");

	HWND hWnd;
	hWnd = CreateWindow(ClassName, L"Sprites", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		WindowCenterX, WindowCenterY, WindowW, WindowH, NULL, hMenu, hInstance, NULL);

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

RECT Sprite = { 10,10,200,200 };
HBRUSH MySpriteBrush = CreateSolidBrush(RGB(255, 0, 0));;

void BorderJump(RECT *Sprite, RECT rect)
{
	const int N = 30;
	if (Sprite->left < 0)
	{
		Sprite->left = Sprite->left + N;
		Sprite->right = Sprite->right + N;
	}
	if (Sprite->top < 0)
	{
		Sprite->bottom = Sprite->bottom + N;
		Sprite->top = Sprite->top + N;
	}
	if (Sprite->right > rect.right)
	{
		Sprite->left = Sprite->left - N;
		Sprite->right = Sprite->right - N;
	}
	if (Sprite->bottom > rect.bottom)
	{
		Sprite->bottom = Sprite->bottom - N;
		Sprite->top = Sprite->top - N;
	}

}

void PrintFigure(HWND hWnd)
{
	PAINTSTRUCT ps;
	RECT rect;

	COLORREF color = RGB(255, 255, 0);
	HBRUSH Brush = CreateSolidBrush(color);
	HDC hDC = BeginPaint(hWnd, &ps);
	HPEN hpenOld = (HPEN)SelectObject(hDC, GetStockObject(DC_PEN));
	HBRUSH hbrushOld = (HBRUSH)SelectObject(hDC, MySpriteBrush);

	GetClientRect(hWnd, &rect);
	BorderJump(&Sprite, rect);

	Ellipse(hDC, Sprite.left, Sprite.top, Sprite.right, Sprite.bottom);

	//	FillRect(hDC, &Sprite, MySpriteBrush);

	SelectObject(hDC, hpenOld);
	SelectObject(hDC, hbrushOld);
	EndPaint(hWnd, &ps);

}

void ArrowsControl(HWND hWnd, WPARAM wParam, int VKdx)
{
	switch (wParam)
	{
	case VK_UP:
		Sprite.bottom = Sprite.bottom - VKdx;
		Sprite.top = Sprite.top - VKdx;
		break;
	case VK_DOWN:
		Sprite.bottom = Sprite.bottom + VKdx;
		Sprite.top = Sprite.top + VKdx;
		break;
	case VK_RIGHT:
		Sprite.left = Sprite.left + VKdx;
		Sprite.right = Sprite.right + VKdx;
		break;
	case VK_LEFT:
		Sprite.left = Sprite.left - VKdx;
		Sprite.right = Sprite.right - VKdx;
		break;
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

HBITMAP SpriteBitmap;

void OpenFile(HWND hWnd)
{
	OPENFILENAME ofn = { 0 };
	WCHAR szDirect[MAX_PATH] = { 0 };
	WCHAR szFileName[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szDirect;
	*(ofn.lpstrFile) = NULL;
	ofn.nMaxFile = sizeof(szDirect) / sizeof(WCHAR);
	ofn.lpstrFilter = L"Bitmap files(*.bmp)\0 * .bmp\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = szFileName;
	*(ofn.lpstrFileTitle) = NULL;
	ofn.nMaxFileTitle = sizeof(szFileName) / sizeof(WCHAR);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER;

	if (GetOpenFileName(&ofn) == TRUE)
	{

		SpriteBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		MySpriteBrush = CreatePatternBrush(SpriteBitmap);
		InvalidateRect(hWnd, NULL, TRUE);
	}

}

bool IsShitDown;

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	int VKdx = 4;
	int Weeldx;

	switch (message)
	{

	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_MENU_IMAGE_OPEN:
			OpenFile(hWnd);
			break;
		}

		break;
	case WM_MOUSEWHEEL:
		Weeldx = GET_WHEEL_DELTA_WPARAM(wParam) / 10;
		if (IsShitDown)
		{
			Sprite.left = Sprite.left + Weeldx;
			Sprite.right = Sprite.right + Weeldx;
		}
		else
		{
			Sprite.bottom = Sprite.bottom + Weeldx;
			Sprite.top = Sprite.top + Weeldx;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYDOWN:
		ArrowsControl(hWnd, wParam, VKdx);
		switch (wParam)
		{
		case VK_SHIFT:
			IsShitDown = true;
			break;

		}break;
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SHIFT:
			IsShitDown = false;
			break;

		}break;
	case WM_PAINT:
		PrintFigure(hWnd);

		break;
	case WM_DESTROY:
		DeleteObject(SpriteBitmap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}