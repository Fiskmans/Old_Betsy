#include <pch.h>
#include "XboxInput.h"
#include <wtypes.h>
#include <XInput.h>


namespace CommonUtilities
{
	XboxInput::XboxInput(int aPlayerNumber)
	{
		myControllerNumber = aPlayerNumber;
		myDeadZoneY = 0.05f;
		myDeadZoneX = 0.07f;

		Vibrate();
	}


	XboxInput::XboxInput()
	{

	}

	XboxInput::~XboxInput()
	{
	}

	bool XboxInput::GetKeyDown(int aButton)
	{

		if ((myControllerState.Gamepad.wButtons & aButton) > 0)
		{
			return true;
		}
		return false;
	}

	bool XboxInput::GetKeyPressed(int aButton)
	{

		if ((myPreviusControllerState.Gamepad.wButtons & aButton) == 0
			&& (myControllerState.Gamepad.wButtons & aButton) > 0)
		{
			return true;
		}
		return false;

	}

	bool XboxInput::GetKeyReleased(int aButton)
	{

		if ((myPreviusControllerState.Gamepad.wButtons & aButton) > 0
			&& (myControllerState.Gamepad.wButtons & aButton) == 0)
		{
			return true;
		}
		return false;

	}

	bool XboxInput::GetIsLeftTriggerDown()
	{
		float TriggerAmount = GetLeftTriggerAmount();

		if (TriggerAmount > 0)
		{
			return true;
		}
		return false;
	}

	bool XboxInput::GetIsRightTriggerDown()
	{
		float TriggerAmount = GetRightTriggerAmount();

		if (TriggerAmount > 0)
		{
			return true;
		}
		return false;
	}

	XINPUT_STATE XboxInput::GetState()
	{

		ZeroMemory(&myControllerState, sizeof(XINPUT_STATE));

		// Get the state

		XInputGetState(myControllerNumber, &myControllerState);

		return myControllerState;
	}

	bool XboxInput::IsConnected()
	{

		ZeroMemory(&myControllerState, sizeof(XINPUT_STATE));

		// Get the state
		DWORD Result = XInputGetState(myControllerNumber, &myControllerState);

		//DL_PRINT("[XboxInput.cpp IsConnected()] Connected Result: %d", Result);

		if (Result == ERROR_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void XboxInput::Vibrate(float leftVal /*= 0*/, float rightVal /*= 0*/)
	{

		XINPUT_VIBRATION Vibration;

		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

		// Set the Vibration Values
		Vibration.wLeftMotorSpeed = static_cast<WORD>(leftVal * 65535);
		Vibration.wRightMotorSpeed = static_cast<WORD>(rightVal * 65535);

		XInputSetState(myControllerNumber, &Vibration);
	}

	void XboxInput::Update()
	{
		myPreviusControllerState = myControllerState;
		myControllerState = GetState();
	}

	bool XboxInput::CheckIfInDeadZone()
	{
		return false;
	}

	float XboxInput::GetLeftTriggerAmount()
	{
		BYTE Trigger = GetState().Gamepad.bLeftTrigger;

		if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			return Trigger / 255.0f;
		}

		return 0.0f; // Trigger was not pressed
	}

	float XboxInput::GetRightTriggerAmount()
	{

		BYTE Trigger = GetState().Gamepad.bRightTrigger;

		if (Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			return Trigger / 255.0f;
		}

		return 0.0f; // Trigger was not pressed
	}

	bool XboxInput::GetRightTriggerReleased()
	{
		if (myPreviusControllerState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			&& GetRightTriggerAmount() == 0)
		{
			return true;
		}
		return false;
	}

	bool XboxInput::GetLeftTriggerReleased()
	{
		if (myPreviusControllerState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			&& GetLeftTriggerAmount() == 0)
		{
			return true;
		}
		return false;
	}


	void XboxInput::SetLeftVibration(float aAmount /*= 1.f*/)
	{
		Vibrate(aAmount, 0);
	}

	void XboxInput::SetRightVibration(float aAmount /*= 1.f*/)
	{
		Vibrate(0, aAmount);
	}

	void XboxInput::SetFullControllerVibration(float aAmount /*= 1.f*/)
	{
		Vibrate(aAmount, aAmount);
	}

	void XboxInput::QuitVibration()
	{
		Vibrate(0, 0);
	}

	Vector2<float> XboxInput::GetLeftStick_Pos()
	{

		Vector2<float> tempPosVectorLeft = Vector2<float>(0.f, 0.f);
		short sX = myControllerState.Gamepad.sThumbLX;
		short sY = myControllerState.Gamepad.sThumbLY;

		tempPosVectorLeft.x = (static_cast<float>(sX) / 32768.0f);
		tempPosVectorLeft.y = (static_cast<float>(sY) / 32768.0f);

		return tempPosVectorLeft;
	}


	bool XboxInput::LeftStickInDeadzone()
	{
		// Obtain the X & Y axes of the stick
		short sX = myControllerState.Gamepad.sThumbLX;
		short sY = myControllerState.Gamepad.sThumbLY;

		// X axis is outside of deadzone
		if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			return false;
		}

		// Y axis is outside of deadzone
		if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			return false;
		}
		// One (or both axes) axis is inside of deadzone
		return true;

	}

	bool XboxInput::RightStickInDeadzone()
	{
		// Obtain the X & Y axes of the stick
		short sX = myControllerState.Gamepad.sThumbRX;
		short sY = myControllerState.Gamepad.sThumbRY;

		// X axis is outside of deadzone
		if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			return false;
		}

		// Y axis is outside of deadzone
		if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			return false;
		}
		// One (or both axes) axis is inside of deadzone
		return true;
	}

	Vector2<float> XboxInput::GetRightStick_Pos()
	{
		Vector2<float> tempPosVectorRight = Vector2<float>(0.f, 0.f);

		short sX = myControllerState.Gamepad.sThumbRX;
		short sY = myControllerState.Gamepad.sThumbRY;

		tempPosVectorRight.x = (static_cast<float>(sX) / 32768.0f);
		tempPosVectorRight.y = (static_cast<float>(sY) / 32768.0f);
		return tempPosVectorRight;
	}
}
