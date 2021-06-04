#include "pch.h"
#include "WindowHandler.h"
#include "DirectX11Framework.h"
#include "PostMaster.hpp"

#include "Sprite.h"

#if USEIMGUI
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // _RETAIL




CommonUtilities::InputHandler WindowHandler::myInputHandler;
DirectX11Framework* WindowHandler::myFrameWork = nullptr;

WindowHandler::WindowHandler() :
	myWindowHandle(NULL),
	myTaskbarButtonCreatedMessageId(0),
	myWindowStyle(0),
	Observer({
			MessageType::ResizeWindow,
			MessageType::SuperTinyWindow,
			MessageType::WindowSmallerThanAMouse,
			MessageType::PleaseResetTheWindowThanks
		})
{
}


WindowHandler::~WindowHandler()
{
	if (CloseWindow(myWindowHandle) != TRUE)
	{
		SYSERROR("Could not close window");
	}
	Logger::Shutdown();
}

LRESULT CALLBACK WindowHandler::WinProc(_In_ HWND aHWND, _In_ UINT aUMsg, _In_ WPARAM aWParam, _In_ LPARAM aLParam)
{
	static WindowHandler* windowHandler = nullptr;
	static bool hwndIsSet = false;


	if (windowHandler)
	{
		if (aUMsg == windowHandler->myTaskbarButtonCreatedMessageId)
		{
			Logger::SetupIcons(aHWND);
		}
	}
	static bool isClosing = false;
	if (aUMsg == WM_DESTROY || aUMsg == WM_CLOSE)
	{
		isClosing = true;
		PostQuitMessage(0);
		return 0;
	}
	if (isClosing)
	{
		return 0;
	}

	if (aUMsg == WM_CREATE)
	{
		assert(!windowHandler);
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(aLParam);
		windowHandler = reinterpret_cast<WindowHandler*>(createStruct->lpCreateParams);
	}
	else if (myFrameWork && aUMsg == WM_SIZE)
	{
		assert(windowHandler);

		myFrameWork->Resize(aHWND);

		RECT rect;
		GetWindowRect(static_cast<HWND>(aHWND), &rect);

		windowHandler->mySize.x = rect.right - rect.left;
		windowHandler->mySize.y = rect.bottom - rect.top;
	}

#if USEIMGUI
	if (ImGui_ImplWin32_WndProcHandler(aHWND, aUMsg, aWParam, aLParam))
	{
		return TRUE;
	}
#endif // !_RETAIL


	myInputHandler.UpdateEvents(aUMsg, aWParam, aLParam); 

	if (!hwndIsSet)
	{
		myInputHandler.SetWindowHandle(aHWND);
	}

	return DefWindowProc(aHWND, aUMsg, aWParam, aLParam);
}

bool WindowHandler::OpenWindow()
{
	return OpenWindow({
			static_cast<unsigned int>(GetSystemMetrics(SM_CXSCREEN)),
			static_cast<unsigned int>(GetSystemMetrics(SM_CYSCREEN))
		});
}

bool WindowHandler::OpenWindow(V2ui aSize)
{
	assert(!myWindowHandle);

	mySize = aSize;

	WNDCLASSW windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowHandler::WinProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"Gamla Bettan";
	RegisterClassW(&windowClass);
	myWindowStyle = WS_POPUP | WS_VISIBLE;

	myWindowStyle |= WS_MAXIMIZE | WS_OVERLAPPED;

	myWindowHandle = CreateWindowW(L"Gamla Bettan", L"MoonView Mountain", myWindowStyle,
									0 , 0, mySize.x, mySize.y,
									nullptr, nullptr, nullptr, this);

	myTaskbarButtonCreatedMessageId = RegisterWindowMessage(L"TaskbarButtonCreated");

	ChangeWindowMessageFilterEx(myWindowHandle, myTaskbarButtonCreatedMessageId, MSGFLT_ALLOW, NULL);

	return true;
}

HWND WindowHandler::GetWindowHandle()
{
	return myWindowHandle;
}

CommonUtilities::InputHandler& WindowHandler::GetInputHandler()
{
	return myInputHandler;
}

V2ui WindowHandler::GetSize()
{
	return mySize;
}

void WindowHandler::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::ResizeWindow:
	{
		const V2ui& data = *reinterpret_cast<const V2ui*>(aMessage.myData);
		SetWindowPos(myWindowHandle, 0, 0, 0, data.x, data.y, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
		break;
	case MessageType::SuperTinyWindow:
		SetWindowPos(myWindowHandle, 0, 0, 0, 256, 144, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		break;
	case MessageType::WindowSmallerThanAMouse:
		SetWindowPos(myWindowHandle, 0, 0, 0, 16, 9, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		break;
	case MessageType::PleaseResetTheWindowThanks:
		SetWindowPos(myWindowHandle, 0, 0, 0, 1280, 720, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
		break;
	}
}
