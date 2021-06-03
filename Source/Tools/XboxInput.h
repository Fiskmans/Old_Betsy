#pragma once

#include <Xinput.h>

//#pragma comment(lib, "XInput.lib")
#pragma comment(lib, "XInput9_1_0.lib")

namespace CommonUtilities
{
	class XboxInput
	{
	public:
		XboxInput();
		XboxInput(int aPlayerNumber);
		~XboxInput();

		bool GetKeyDown(int aButton);
		bool GetKeyPressed(int aButton);
		bool GetKeyReleased(int aButton);
		bool GetIsLeftTriggerDown();
		bool GetIsRightTriggerDown();

		XINPUT_STATE GetState();
		bool IsConnected();
		void Vibrate(float leftVal = 0, float rightVal = 0);
		void Update();
		bool CheckIfInDeadZone();
		// Controll Checking
		V2F GetRightStick_Pos();
		V2F GetLeftStick_Pos();
		bool LeftStickInDeadzone();
		bool RightStickInDeadzone();

		float GetLeftTriggerAmount();
		float GetRightTriggerAmount();
		bool GetRightTriggerReleased();
		bool GetLeftTriggerReleased();

		void SetLeftVibration(float aAmount = 1.f);
		void SetRightVibration(float aAmount = 1.f);
		void SetFullControllerVibration(float aAmount = 1.f);
		void QuitVibration();
	private:
		float myDeadZoneX;
		float myDeadZoneY;
		float myLeftVibrate;
		float myRightVibrate;
		XINPUT_STATE myControllerState;
		XINPUT_STATE myPreviusControllerState;
		int myControllerNumber;

	};
};
