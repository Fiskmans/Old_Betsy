// Launcher.cpp : Defines the entry point for the application.

#include <pch.h>
#include <DbgHelp.h>
#include "DirectXTK\Inc\ScreenGrab.h"

int Run();

std::wstring CreateMiniDump(EXCEPTION_POINTERS* aExceptionPointers);
LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aExceptionP);
LONG CALLBACK UnhandledHandler(EXCEPTION_POINTERS* e);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
#if COPYDLLS
#ifdef _DEBUG
	system("SetUpDebugDLLs.bat");
#else
	system("SetUpReleaseDLLs.bat");
#endif
#endif

	//To stop warnings
	hInstance = hInstance;
	hPrevInstance = hPrevInstance;
	lpCmdLine = lpCmdLine;
	nShowCmd = nShowCmd;

	SetUnhandledExceptionFilter(UnhandledHandler);

	__try
	{
		return Run();
	}
	__except (ExceptionFilterFunction(GetExceptionInformation()))
	{
		return EXIT_FAILURE;
	}
}
ID3D11DeviceContext* GetAndOrSetContext(ID3D11DeviceContext* aContext = nullptr, bool aReallyShouldSet = false);
ID3D11Resource* GetAndOrSetResource(ID3D11Resource* aResource = nullptr, bool aReallyShouldSet = false);

void ScreenShot(const wchar_t* aFilepath)
{
	ID3D11Resource* res = GetAndOrSetResource();
	ID3D11DeviceContext* cont = GetAndOrSetContext();
	if (res && cont)
	{
		DirectX::SaveDDSTextureToFile(cont, res, aFilepath);
	}
}

std::wstring CreateMiniDump(EXCEPTION_POINTERS* aExceptionPointers)
{
#ifndef _RETAIL 
	HMODULE hDbgHelp = LoadLibraryA("dbghelp");
	if (hDbgHelp == nullptr)
	{
		return L"Could not dump";
	}
	auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (pMiniDumpWriteDump == nullptr)
	{
		return L"Could not dump";
	}

	char name[MAX_PATH];
	{
		GetModuleFileNameA(GetModuleHandleA(0), name, MAX_PATH);

		//wsprintfA(nameEnd - strlen(".exe"),"_%02d-%02d_%02d%02d.dmp",t.wMonth, t.wDay, t.wHour, t.wMinute);
	}
	std::filesystem::path path = name;
	SYSTEMTIME t;
	GetLocalTime(&t);

	std::filesystem::path outPath;
	std::filesystem::path::iterator it = path.begin();
	while (it != path.end())
	{
		std::filesystem::path::iterator next(it);
		++next;
		if (next != path.end())
		{
			outPath /= *it;
		}
		it++;
	}
	it--;
	std::filesystem::path file = *it;

	char days[7 * 4] =
		"Mon\0"
		"Tue\0"
		"Wed\0"
		"Thu\0"
		"Fri\0"
		"Wtf\0"
		"Nop";
	std::string DumpName = file.filename().string().substr(0, file.filename().string().size() - 4) + "_" + std::string(days + ((__int64)t.wDayOfWeek - 1) * 4) + "_" + std::to_string(t.wHour) + "_" + std::to_string(t.wMinute);

	outPath /= "crashdumps";
	outPath /= DumpName;
	system(("mkdir \"" + outPath.string() + "\"").c_str());
	std::filesystem::path dumpPath = outPath;
	dumpPath /= "dump.dmp";


	HANDLE hFile = CreateFileA((dumpPath.string()).c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return L"Could not dump";
	}

	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = aExceptionPointers;
	exceptionInfo.ClientPointers = FALSE;

	BOOL dumped = pMiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
		aExceptionPointers ? &exceptionInfo : nullptr,
		nullptr,
		nullptr);

	CloseHandle(hFile);

	std::filesystem::path screenshotPath = outPath;
	screenshotPath /= "screenshot.dds";
	ScreenShot(screenshotPath.wstring().c_str());

	system(("copy *.pdb " + outPath.string()).c_str());

	return L"Crash file written to: " + outPath.wstring();
#else
	return L"The program encountered a fatal error.";
#endif // !_RETAIL
}

LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aExceptionP)
{
	int result = MessageBoxW(NULL, L"Do you want a crashdump?", L"It all came tumbling down", MB_ICONEXCLAMATION | MB_YESNO);
	//std::count << result << std::endl;

	MessageBoxA(NULL, std::to_string(aExceptionP->ExceptionRecord->ExceptionCode).c_str(), "Error code", MB_ICONEXCLAMATION | MB_OK);
	if(result == IDYES);
	{
		MessageBoxW(NULL, CreateMiniDump(aExceptionP).c_str(), L"Chashdump Result", MB_ICONEXCLAMATION | MB_OK);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG CALLBACK UnhandledHandler(EXCEPTION_POINTERS* e)
{
	std::wstring stopWarningMe = CreateMiniDump(e);
	return EXCEPTION_CONTINUE_SEARCH;
}
