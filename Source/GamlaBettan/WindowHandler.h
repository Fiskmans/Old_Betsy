#pragma once
#include "InputHandler.h"
#include "CommonUtilities\Singleton.hpp"

class DirectX11Framework;

class WindowHandler : public CommonUtilities::Singleton<WindowHandler>, public Observer
{
public:
	WindowHandler();
	~WindowHandler();

	static LRESULT WinProc(HWND aHWND, UINT aUMsg, WPARAM aWParam, LPARAM aLParam);

	bool OpenWindow();
	bool OpenWindow(V2ui aSize);

	HWND GetWindowHandle();
	CommonUtilities::InputHandler& GetInputHandler();

	V2ui GetSize();

	virtual void RecieveMessage(const Message& aMessage) override;

private:
	HWND myWindowHandle;
	V2ui mySize;

	static CommonUtilities::InputHandler myInputHandler;
	static DirectX11Framework* myFrameWork;
	UINT myTaskbarButtonCreatedMessageId;
	DWORD myWindowStyle;


};

