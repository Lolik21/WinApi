// AAAATest.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

char szText[] = "Hello world.",
szTitle[] = "InformationMyMy";

int main()
{
	while (TRUE)
		MessageBoxA(NULL, szText, szTitle, MB_ICONINFORMATION);
	return EXIT_SUCCESS;
}