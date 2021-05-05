#pragma once
#include <pch.h>
#include "InputManager.h"
#include "../GamlaBettan/InputHandler.h"
#include "Message.hpp"
#include "XboxInput.h"
#include "PauseState.h"
#ifdef _DEBUG
#include "../GamlaBettan/DebugTools.h"
#endif // _DEBUG
#include "Sprite.h"


InputManager::InputManager()
{
	myInputHandler = nullptr;

	myIsPaused = false;
	myMouseIsMoving = false;
	myGameExists = false;

	myLevel = 0;

	myCurrentMousePosition = { 0 };

	mySecrets.emplace_back("DISCO", MessageType::EnableDiscoMode);
	mySecrets.emplace_back("HASTFAN", MessageType::EnableHastfan);
	mySecrets.emplace_back("SUPERTINYWINDOW", MessageType::SuperTinyWindow, true);
	mySecrets.emplace_back("REALLYSUPERSMALLWINDOW", MessageType::WindowSmallerThanAMouse, true);
	mySecrets.emplace_back("RESETWINDOW", MessageType::PleaseResetTheWindowThanks, true);
	mySecrets.emplace_back("HEEEJ", MessageType::GiveMeAdam, true);
}

InputManager::~InputManager()
{
	myInputHandler = nullptr;

	UnSubscribeToMessage(MessageType::InputPauseHit);
	UnSubscribeToMessage(MessageType::InputUnPauseHit);
	UnSubscribeToMessage(MessageType::GameActive);
	UnSubscribeToMessage(MessageType::GameNotActive);
	UnSubscribeToMessage(MessageType::CurrentLevel);
	UnSubscribeToMessage(MessageType::WeaponRecoil);
}

void InputManager::Init(CommonUtilities::InputHandler* aInputHandler)
{
	myInputHandler = aInputHandler;

	SubscribeToMessage(MessageType::InputPauseHit);
	SubscribeToMessage(MessageType::InputUnPauseHit);
	SubscribeToMessage(MessageType::GameActive);
	SubscribeToMessage(MessageType::GameNotActive);
	SubscribeToMessage(MessageType::CurrentLevel);
	SubscribeToMessage(MessageType::WeaponRecoil);
}

void InputManager::Update()
{
	if (myInputHandler->IsKeyHit(myInputHandler->Key_1))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot1Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_2))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot2Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_3))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot3Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_4))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot4Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_5))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot5Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_6))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot6Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_7))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot7Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_8))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot8Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_9))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot9Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_0))
	{
		Publisher::SendMessages(MessageType::InputEquipmentSlot10Hit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_W))
	{
		Publisher::SendMessages(MessageType::InputUpHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_W))
	{
		Publisher::SendMessages(MessageType::InputUpIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_W))
	{
		Publisher::SendMessages(MessageType::InputUpReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_S))
	{
		Publisher::SendMessages(MessageType::InputDownHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_S))
	{
		Publisher::SendMessages(MessageType::InputDownIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_S))
	{
		Publisher::SendMessages(MessageType::InputDownReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_A))
	{
		Publisher::SendMessages(MessageType::InputLeftHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_A))
	{
		Publisher::SendMessages(MessageType::InputLeftIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_A))
	{
		Publisher::SendMessages(MessageType::InputLeftReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_D))
	{
		Publisher::SendMessages(MessageType::InputRightHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_D))
	{
		Publisher::SendMessages(MessageType::InputRightIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_D))
	{
		Publisher::SendMessages(MessageType::InputRightReleased);
	}


	if (myInputHandler->IsKeyHit(myInputHandler->Key_Space))
	{
		Publisher::SendMessages(MessageType::InputJumpHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Down))
	{
		Publisher::SendMessages(MessageType::InputDownHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Left))
	{
		Publisher::SendMessages(MessageType::InputLeftHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Right))
	{
		Publisher::SendMessages(MessageType::InputRightHit);
	}


	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Up))
	{
		Publisher::SendMessages(MessageType::InputUpReleased);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Down))
	{
		Publisher::SendMessages(MessageType::InputDownReleased);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Left))
	{
		Publisher::SendMessages(MessageType::InputLeftReleased);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Right))
	{
		Publisher::SendMessages(MessageType::InputRightReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_R))
	{
		Publisher::SendMessages(MessageType::InputReload);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Escape))
	{
		Publisher::SendMessages(MessageType::InputEscHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_C))
	{
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Space))
	{
		Publisher::SendMessages(MessageType::InputJumpHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Shift))
	{
		Publisher::SendMessages(MessageType::InputRunHit);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Shift))
	{
		Publisher::SendMessages(MessageType::InputRunReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Control))
	{
		Publisher::SendMessages(MessageType::InputCrouchHit);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Control))
	{
		Publisher::SendMessages(MessageType::InputCrouchReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Enter) || GetAsyncKeyState(VK_RETURN))
	{
		Publisher::SendMessages(MessageType::InputAccept);
	}

	/*if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Left))
	{
		SendMessages(MessageType::InputInteractHit);
	}*/

	if (myInputHandler->IsKeyHit(myInputHandler->Key_I))
	{
		SendMessages(MessageType::InputToggleInventory);
	}

	if (myInputHandler->IsKeyHit(CU::InputHandler::Key_Escape))
	{
		if (myGameExists)
		{
			Message msg;

			if (myIsPaused)
			{
				msg.myMessageType = MessageType::PopState;
				msg.myBool = false;
				Publisher::SendMessages(MessageType::InputUnPauseHit);
			}
			else
			{
				msg.myData = new PauseState;
				static_cast<PauseState*>(msg.myData)->Init();
				msg.myMessageType = MessageType::PushState;
				Publisher::SendMessages(MessageType::InputPauseHit);
			}

			PostMaster::GetInstance()->SendMessages(msg);
		}
	}

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


		Message message;
		message.myFloatValue = static_cast<float>(myInputHandler->GetMouseNomalizedX());
		message.myFloatValue2 = static_cast<float>(myInputHandler->GetMouseNomalizedY());
		message.myMessageType = MessageType::InputMouseMoved;
		Publisher::SendMessages(message);

		myMouseIsMoving = true;
	}
	else if (myMouseIsMoving)
	{
		myMouseIsMoving = false;

		Message message;
		message.myMessageType = MessageType::InputMouseStopedMoving;
		Publisher::SendMessages(message);
	}


	if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Left))
	{
		Message message;
		message.myMessageType = MessageType::InputLeftMouseHit;
		message.myFloatValue = static_cast<float>(myInputHandler->GetMousePosition().x) / Sprite::ourWindowSize.x;
		message.myFloatValue2 = static_cast<float>(myInputHandler->GetMousePosition().y) / Sprite::ourWindowSize.y;

		Publisher::SendMessages(message);
	}

	if (myInputHandler->IsMouseDown(myInputHandler->Mouse_Left))
	{
		Message message;
		message.myMessageType = MessageType::InputLeftMouseDown;
		message.myFloatValue = static_cast<float>(myInputHandler->GetMousePosition().x) / Sprite::ourWindowSize.x;
		message.myFloatValue2 = static_cast<float>(myInputHandler->GetMousePosition().y) / Sprite::ourWindowSize.y;

		Publisher::SendMessages(message);
	}

	if (myInputHandler->IsMouseReleased(myInputHandler->Mouse_Left))
	{
		Message message;
		message.myMessageType = MessageType::InputLeftMouseReleased;
		message.myFloatValue = static_cast<float>(myInputHandler->GetMousePosition().x) / Sprite::ourWindowSize.x;
		message.myFloatValue2 = static_cast<float>(myInputHandler->GetMousePosition().y) / Sprite::ourWindowSize.y;

		Publisher::SendMessages(message);
	}

	if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Right))
	{
		Publisher::SendMessages(MessageType::InputRightMouseHit);
	}

	if (myInputHandler->IsMouseDown(myInputHandler->Mouse_Right))
	{
		//Publisher::SendMessages(MessageType::InputRightMouseDown);
	}

	if (myInputHandler->IsMouseReleased(myInputHandler->Mouse_Right))
	{
		Publisher::SendMessages(MessageType::InputRightMouseReleased);
	}

	if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Middle))
	{
		Publisher::SendMessages(MessageType::InputMiddleMouseHit);
	}

	if (myInputHandler->IsMouseDown(myInputHandler->Mouse_Middle))
	{
		Publisher::SendMessages(MessageType::InputMiddleMouseDown);
	}

	short vel = myInputHandler->GetWheelVelNormalized();
	if (vel != 0)
	{
		for (size_t i = 0; i < std::abs(vel); i++)
		{
			Publisher::SendMessages(vel < 0 ? MessageType::InputScrollDown : MessageType::InputScrollUp);
		}
	}

#ifndef _RETAIL
	if (myInputHandler->IsKeyHit(myInputHandler->Key_F2))
	{
		const int assetGym = 1;
		static int lastLvl = 0;

		Message mess;
		mess.myMessageType = MessageType::ChangeLevel;
		mess.myIntValue = (myLevel == assetGym ? lastLvl : assetGym);

		lastLvl = myLevel;

		Publisher::SendMessages(mess);

	}
#endif // !_RETAIL

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

void InputManager::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::InputPauseHit:
		myIsPaused = true;
		break;

	case MessageType::InputUnPauseHit:
		myIsPaused = false;
		break;

	case MessageType::GameActive:
		myGameExists = true;
		break;

	case MessageType::GameNotActive:
		myGameExists = myIsPaused;
		break;

	case MessageType::CurrentLevel:
		myLevel = aMessage.myIntValue;
		break;

	case MessageType::WeaponRecoil:
		Point recoilOffset = myInputHandler->GetMousePosition();
		recoilOffset.y += aMessage.myIntValue;
		recoilOffset.x += aMessage.myIntValue2;
		myInputHandler->SetMousePosition(recoilOffset.x, recoilOffset.y);
		break;

	default:
		break;
	}
}