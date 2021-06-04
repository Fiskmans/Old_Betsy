#pragma once
#include <pch.h>
#include "InputManager.h"
#include "../GamlaBettan/InputHandler.h"
#include "Message.hpp"
#include "XboxInput.h"
#include "Sprite.h"


InputManager::InputManager()
	: Observer({
			MessageType::InputPauseHit,
			MessageType::InputUnPauseHit,
			MessageType::GameActive,
			MessageType::GameNotActive,
			MessageType::CurrentLevel,
			MessageType::WeaponRecoil
		})
{
	myInputHandler = nullptr;

	myIsPaused = false;
	myMouseIsMoving = false;
	myGameExists = false;

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
	if (myInputHandler->IsKeyHit(myInputHandler->Key_1))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot1Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_2))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot2Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_3))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot3Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_4))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot4Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_5))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot5Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_6))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot6Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_7))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot7Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_8))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot8Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_9))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot9Hit);
	}
	if (myInputHandler->IsKeyHit(myInputHandler->Key_0))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEquipmentSlot10Hit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_W))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputUpHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_W))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputUpIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_W))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputUpReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_S))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputDownHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_S))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputDownIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_S))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputDownReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_A))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputLeftHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_A))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputLeftIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_A))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputLeftReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_D))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightHit);
	}
	if (myInputHandler->IsKeyDown(myInputHandler->Key_D))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightIsDown);
	}
	if (myInputHandler->IsKeyReleased(myInputHandler->Key_D))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightReleased);
	}


	if (myInputHandler->IsKeyHit(myInputHandler->Key_Space))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputJumpHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Down))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputDownHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Left))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputLeftHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Right))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightHit);
	}


	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Up))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputUpReleased);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Down))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputDownReleased);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Left))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputLeftReleased);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Right))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRightReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_R))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputReload);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Escape))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputEscHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_C))
	{
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Space))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputJumpHit);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Shift))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRunHit);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Shift))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputRunReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Control))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputCrouchHit);
	}

	if (myInputHandler->IsKeyReleased(myInputHandler->Key_Control))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputCrouchReleased);
	}

	if (myInputHandler->IsKeyHit(myInputHandler->Key_Enter) || GetAsyncKeyState(VK_RETURN))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputAccept);
	}

	/*if (myInputHandler->IsMouseHit(myInputHandler->Mouse_Left))
	{
		SendMessages(MessageType::InputInteractHit);
	}*/

	if (myInputHandler->IsKeyHit(myInputHandler->Key_I))
	{
		PostMaster::GetInstance().SendMessages(MessageType::InputToggleInventory);
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

	default:
		break;
	}
}