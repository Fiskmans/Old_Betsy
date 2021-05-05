#include <pch.h>
#include "ClipBoardHelper.h"

void Tools::WriteToClipboard(const std::string& aString)
{
	HGLOBAL hMem(GlobalAlloc(GMEM_MOVEABLE, aString.length()+1));
	if (hMem != NULL)
	{
		memcpy(GlobalLock(hMem), aString.c_str(), aString.length()+1);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
		GlobalFree(hMem);
	}
}

std::string Tools::ReadFromClipboard()
{
	if (OpenClipboard(NULL) == FALSE)
	{
		return "";
	};
	char* data = (char*)GetClipboardData(CF_TEXT);
	CloseClipboard();
	if (data)
	{
		return data;
	}
	return "";
}
