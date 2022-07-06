
#include "engine/graphics/WindowManager.h"
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderManager.h"
#include "engine/GameEngine.h"

#include "logger/Logger.h"

#include "tools/Stopwatch.h"

#include <cassert>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine
{
	WindowManager::WindowManager()
		: myWindowHandle(NULL)
		, mySize(0, 0)
		, myTaskbarButtonCreatedMessageId(0)
		, myWindowStyle(0)
	{
	}

	WindowManager::~WindowManager()
	{
		if (CloseWindow(myWindowHandle) != TRUE)
		{
			LOG_SYS_ERROR("Could not close window");
		}
	}

	LRESULT CALLBACK WindowManager::WndProc(_In_ HWND aHWND, _In_ UINT aUMsg, _In_ WPARAM aWParam, _In_ LPARAM aLParam)
	{

		if (aUMsg == WindowManager::GetInstance().myTaskbarButtonCreatedMessageId)
		{
			logger::SetupIcons(aHWND);
		}

		if (aUMsg == WM_SIZE)
		{
			GetInstance().mySize = tools::V2ui(LOWORD(aLParam), HIWORD(aLParam));
			GetInstance().ResolutionChanged.Fire(GetInstance().mySize);
			RenderManager::GetInstance().Resize(WindowManager::GetInstance().mySize);
		}

		LRESULT imguiResult = ImGui_ImplWin32_WndProcHandler(aHWND, aUMsg, aWParam, aLParam);
		if (imguiResult != NULL)
		{
			return imguiResult;
		}

		//InputManager::GetInstance().UpdateEvents(aHWND, aUMsg, aWParam, aLParam);

		return DefWindowProcW(aHWND, aUMsg, aWParam, aLParam);
	}

	bool WindowManager::OpenWindow(tools::V2ui aSize)
	{
		LOG_SYS_INFO("Opening window");
		tools::Stopwatch watch;
		{
			tools::TimedScope scopeTimer(watch);

			assert(!myWindowHandle);

			tools::V2ui size = aSize;
			if (size == tools::V2ui(0, 0))
			{
				size = tools::V2ui(
					static_cast<unsigned int>(GetSystemMetrics(SM_CXSCREEN)),
					static_cast<unsigned int>(GetSystemMetrics(SM_CYSCREEN))
				);

				LOG_SYS_INFO("No window size specified opening with screen size: [w:" + std::to_string(size[0]) + " h:" + std::to_string(size[1]) + "]");
			}

			mySize = size;

			WNDCLASSW windowClass = {};
			windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
			windowClass.lpfnWndProc = WindowManager::WndProc;
			windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
			windowClass.lpszClassName = L"OldBetsy";
			if (RegisterClassW(&windowClass) == 0)
			{
				LOG_SYS_ERROR("Failed to create window class");
				return false;
			}

			myWindowStyle = WS_POPUP | WS_VISIBLE;

			myWindowStyle |= WS_MAXIMIZE | WS_OVERLAPPED;


			myWindowHandle = CreateWindowExW(
				0,
				L"OldBetsy",
				engine::GameEngine::GetInstance().GetCurrentGame().Name().c_str(),
				myWindowStyle,
				0, 0, mySize[0], mySize[1],
				NULL,
				NULL,
				NULL,
				this);

			if (myWindowHandle == NULL)
			{
				LOG_SYS_ERROR("Failed to create window");
				return false;
			}

			ShowWindow(myWindowHandle, SW_SHOW);

			myTaskbarButtonCreatedMessageId = RegisterWindowMessageW(L"TaskbarButtonCreated");

			if (myTaskbarButtonCreatedMessageId == 0)
			{
				LOG_SYS_ERROR("Failed to register event");
				return false;
			}

			if (ChangeWindowMessageFilterEx(myWindowHandle, myTaskbarButtonCreatedMessageId, MSGFLT_ALLOW, NULL) != TRUE)
			{
				LOG_SYS_ERROR("Failed to set window filter");
				return false;
			}
		}
		LOG_SYS_INFO("Window opened in " + std::to_string(watch.Read()) + " seconds");

		return true;
	}

	void WindowManager::Update()
	{
		MSG windowMessage;
		WIPE(windowMessage);

		PERFORMANCETAG("Winmessage Parsing");
		while (PeekMessage(&windowMessage, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);

			if (windowMessage.message == WM_QUIT || windowMessage.message == WM_DESTROY)
			{
				engine::GameEngine::GetInstance().GetCurrentGame().Exit();
			}
		}
	}

	HWND WindowManager::GetWindowHandle()
	{
		return myWindowHandle;
	}

	tools::V2ui WindowManager::GetSize()
	{
		return mySize;
	}
}