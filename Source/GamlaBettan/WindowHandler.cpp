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
Window::WindowData WindowHandler::myWindowData;

WindowHandler::WindowHandler() :
	myWindowHandle(NULL),
	myTaskbarButtonCreatedMessageId(0),
	myWindowStyle(0)
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

void WindowHandler::SubscribeToMessages()
{
	PostMaster::GetInstance()->Subscribe(MessageType::ResizeWindow,this);
	PostMaster::GetInstance()->Subscribe(MessageType::SuperTinyWindow, this);
	PostMaster::GetInstance()->Subscribe(MessageType::WindowSmallerThanAMouse, this);
	PostMaster::GetInstance()->Subscribe(MessageType::PleaseResetTheWindowThanks, this);

}

void WindowHandler::UnSubscrideToMessages()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::ResizeWindow, this);
	PostMaster::GetInstance()->Subscribe(MessageType::SuperTinyWindow, this);
	PostMaster::GetInstance()->Subscribe(MessageType::WindowSmallerThanAMouse, this);
	PostMaster::GetInstance()->Subscribe(MessageType::PleaseResetTheWindowThanks, this);
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
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(aLParam);
		windowHandler = reinterpret_cast<WindowHandler*>(createStruct->lpCreateParams);
	}
	else if (myFrameWork && aUMsg == WM_SIZE)
	{
		myFrameWork->Resize(aHWND);

		RECT rect;
		GetWindowRect(static_cast<HWND>(aHWND), &rect);

		myWindowData.myX = CAST(unsigned short, rect.left);
		myWindowData.myY = CAST(unsigned short, rect.top);
		myWindowData.myWidth = CAST(unsigned short, rect.right - rect.left);
		myWindowData.myHeight = CAST(unsigned short, rect.bottom - rect.top);
	}

#if USEIMGUI
	if (ImGui_ImplWin32_WndProcHandler(aHWND, aUMsg, aWParam, aLParam))
	{
		return TRUE;
	}
#endif // !_RETAIL


	myInputHandler.UpdateEvents(aUMsg, aWParam, aLParam); 

#ifdef _DEBUG
#if ESCAPEQUITSGAME
	if (myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key_Escape))
	{
		PostQuitMessage(0);
		return 0;
	}
#endif
#endif // DEBUG

	if (!hwndIsSet)
	{
		myInputHandler.SetWindowHandle(aHWND);
	}

	return DefWindowProc(aHWND, aUMsg, aWParam, aLParam);
}

bool WindowHandler::Init(Window::WindowData aWindowData, DirectX11Framework* aFramework)
{
	if (aFramework)
	{
		WNDCLASSW windowClass = {};
		windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = WindowHandler::WinProc;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"Gamla Bettan";
		RegisterClassW(&windowClass);
		myWindowStyle = WS_POPUP | WS_VISIBLE;

#if USEFULLSCREEN
		myWindowStyle |= WS_MAXIMIZE | WS_OVERLAPPED;
		aWindowData.myX = 0;
		aWindowData.myY = 0;
		aWindowData.myWidth = CAST(unsigned short, GetSystemMetrics(SM_CXSCREEN));
		aWindowData.myHeight = CAST(unsigned short,  GetSystemMetrics(SM_CYSCREEN));
#else
		myWindowStyle |= WS_OVERLAPPEDWINDOW;
#endif
		Sprite::ourWindowSize = V2F(aWindowData.myWidth, aWindowData.myHeight);

		myWindowHandle = CreateWindowW(L"Gamla Bettan", L"MoonView Mountain", myWindowStyle,
										aWindowData.myX, aWindowData.myY, aWindowData.myWidth, aWindowData.myHeight, 
										nullptr, nullptr, nullptr, this);

		myTaskbarButtonCreatedMessageId = RegisterWindowMessage(L"TaskbarButtonCreated");

		ChangeWindowMessageFilterEx(myWindowHandle, myTaskbarButtonCreatedMessageId, MSGFLT_ALLOW, NULL);
	}


	myWindowData = aWindowData;
	myFrameWork = aFramework;
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

unsigned short WindowHandler::GetWidth()
{
	return myWindowData.myWidth;
}

unsigned short WindowHandler::GetHeight()
{
	return myWindowData.myHeight;
}

void WindowHandler::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::ResizeWindow:
		SetWindowPos(myWindowHandle, 0, 0, 0, aMessage.myIntValue, aMessage.myIntValue2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
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
