#include <pch.h>
#include "Console.hpp"

namespace CommonUtilities
{
	HANDLE GetConsoleHandle()
	{
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		return handle;
	}

	void SetCursorPosition(short aX, short aY, HANDLE aHandle)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		GetConsoleScreenBufferInfo(aHandle, &csbInfo);
		csbInfo.dwCursorPosition.X = aX;
		csbInfo.dwCursorPosition.Y = aY;
		SetConsoleCursorPosition(aHandle, csbInfo.dwCursorPosition);
	}

	short GetWindowWidth(HANDLE aHandle)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		GetConsoleScreenBufferInfo(aHandle, &csbInfo);
		return csbInfo.srWindow.Right - csbInfo.srWindow.Left + 1;
	}

	short GetWindowHeight(HANDLE aHandle)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		GetConsoleScreenBufferInfo(aHandle, &csbInfo);
		return csbInfo.srWindow.Bottom - csbInfo.srWindow.Top + 1;
	}


	void SetColor(unsigned short aColor, HANDLE aHandle)
	{
		SetConsoleTextAttribute(aHandle, aColor);
	}
}