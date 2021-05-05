#pragma once

#include <Windows.h>

namespace CommonUtilities
{
	HANDLE GetConsoleHandle();
	short GetWindowWidth(HANDLE aHandle);
	short GetWindowHeight(HANDLE aHandle);
	void SetCursorPosition(short aX, short aY, HANDLE aHandle);
	void SetColor(unsigned short aColor, HANDLE aHandle);
}