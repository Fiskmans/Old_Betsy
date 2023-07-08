#ifndef TOOLS_WINDOWMANAGER_H
#define TOOLS_WINDOWMANAGER_H

#include "tools/Singleton.h"
#include "tools/MathVector.h"
#include "tools/Event.h"

#define NOMINMAX
#include <WinSock2.h>
#include <windows.h>

namespace engine
{
	class WindowManager : public fisk::tools::Singleton<WindowManager>
	{
	public:
		WindowManager();
		~WindowManager();

		static LRESULT WndProc(HWND aHWND, UINT aUMsg, WPARAM aWParam, LPARAM aLParam);

		bool OpenWindow(tools::V2ui aSize);

		void Update();

		HWND GetWindowHandle();

		tools::V2ui GetSize();

		fisk::tools::Event<tools::V2ui> ResolutionChanged;

	private:
		HWND myWindowHandle;
		tools::V2ui mySize;

		UINT myTaskbarButtonCreatedMessageId;
		DWORD myWindowStyle;
	};

}

#endif