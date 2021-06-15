#pragma once
#include <pch.h>
#include "InputManager.h"
#include "../GamlaBettan/InputHandler.h"
#include "Message.hpp"
#include "XboxInput.h"
#include "Sprite.h"


InputManager::InputManager()
{
	myInputHandler = nullptr;

	myIsPaused = false;
	myMouseIsMoving = false;

	myCurrentMousePosition = { 0 };

	mySecrets.emplace_back("SUPERTINYWINDOW", MessageType::SuperTinyWindow, true);
	mySecrets.emplace_back("REALLYSUPERSMALLWINDOW", MessageType::WindowSmallerThanAMouse, true);
	mySecrets.emplace_back("RESETWINDOW", MessageType::PleaseResetTheWindowThanks, true);
}

InputManager::~InputManager()
{
	myInputHandler = nullptr;
}

void InputManager::Init(CommonUtilities::InputHandler* aInputHandler)
{
	myInputHandler = aInputHandler;
}

void InputManager::Update()
{

	for (char i = 'A'; i < 'Z'; i++)
	{
		if (myInputHandler->IsKeyHit(i))
		{
			for (auto& sec : mySecrets)
			{
				sec.Input(i);
			}
		}
	}

	//TODO: Check if mouse moved before sending without making the mouse lag.
	if (myCurrentMousePosition.x < static_cast<float>(myInputHandler->GetMouseNomalizedX()) || myCurrentMousePosition.x > static_cast<float>(myInputHandler->GetMouseNomalizedX()) ||
		myCurrentMousePosition.y < static_cast<float>(myInputHandler->GetMouseNomalizedY()) || myCurrentMousePosition.y > static_cast<float>(myInputHandler->GetMouseNomalizedY()))
	{
		myCurrentMousePosition.x = static_cast<float>(myInputHandler->GetMouseNomalizedX());
		myCurrentMousePosition.y = static_cast<float>(myInputHandler->GetMouseNomalizedY());

		V2f data{
			myInputHandler->GetMouseNomalizedX(),
			myInputHandler->GetMouseNomalizedY()
		};

		PostMaster::GetInstance().SendMessages(MessageType::InputMouseMoved, &data);

		myMouseIsMoving = true;
	}
	else if (myMouseIsMoving)
	{
		myMouseIsMoving = false;

		Message message;
		message.myMessageType = MessageType::InputMouseStopedMoving;
		PostMaster::GetInstance().SendMessages(message);
	}


	if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Left))
	{
		V2f data{
			myInputHandler->GetMouseNomalizedX(),
			myInputHandler->GetMouseNomalizedY()
		};

		PostMaster::GetInstance().SendMessages(MessageType::InputLeftMouseHit, &data);
	}

	if (myInputHandler->IsMouseDown(myInputHandler->Mouse_Left))
	{
		V2f data{
			myInputHandler->GetMouseNomalizedX(),
			myInputHandler->GetMouseNomalizedY()
		};

		PostMaster::GetInstance().SendMessages(MessageType::InputLeftMouseDown, &data);
	}

	if (myInputHandler->IsMouseReleased(myInputHandler->Mouse_Left))
	{
		V2f data{
			myInputHandler->GetMouseNomalizedX(),
			myInputHandler->GetMouseNomalizedY()
		};

		PostMaster::GetInstance().SendMessages(MessageType::InputLeftMouseReleased, &data);
	}

	if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Right))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightMouseHit);
	}

	if (myInputHandler->IsMouseDown(myInputHandler->Mouse_Right))
	{
		//PostMaster::GetInstance().SendMessages(MessageType::InputRightMouseDown);
	}

	if (myInputHandler->IsMouseReleased(myInputHandler->Mouse_Right))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightMouseReleased);
	}

	if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Middle))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputMiddleMouseHit);
	}

	if (myInputHandler->IsMouseDown(myInputHandler->Mouse_Middle))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputMiddleMouseDown);
	}

	short vel = myInputHandler->GetWheelVelNormalized();
	if (vel != 0)
	{
		for (size_t i = 0; i < std::abs(vel); i++)
		{
			PostMaster::GetInstance().SendMessages(vel < 0 ? MessageType::InputScrollDown : MessageType::InputScrollUp);
		}
	}

	myInputHandler->FinalizeUpdate();
}


float InputManager::GetMouseNomalizedX() const
{
	if (myInputHandler != nullptr)
	{
		return myInputHandler->GetMouseNomalizedX();
	}
	return 0;
}

float InputManager::GetMouseNomalizedY() const
{
	if (myInputHandler != nullptr)
	{
		return myInputHandler->GetMouseNomalizedY();
	}
	return 0;
}