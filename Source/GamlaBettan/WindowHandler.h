#pragma once
#include <Windows.h>
#include "InputHandler.h"
#include "WindowData.h"
#include "Observer.hpp"


class DirectX11Framework;


class WindowHandler : public Observer
{
public:
	WindowHandler();
	~WindowHandler();

	void SubscribeToMessages();
	void UnSubscrideToMessages();

	static LRESULT CALLBACK WinProc(_In_ HWND aHWND, _In_ UINT aUMsg, _In_ WPARAM aWParam, _In_ LPARAM aLParam);
	bool Init(Window::WindowData aWindowData, DirectX11Framework* aFramework);
	HWND GetWindowHandle();
	CommonUtilities::InputHandler& GetInputHandler();

	unsigned short GetWidth();
	unsigned short GetHeight();

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;

private:
	HWND myWindowHandle;
	static Window::WindowData myWindowData;
	static CommonUtilities::InputHandler myInputHandler;
	static DirectX11Framework* myFrameWork;
	UINT myTaskbarButtonCreatedMessageId;
	DWORD myWindowStyle;


};

