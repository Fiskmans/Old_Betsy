#pragma once

#ifdef _DEBUG
#include <unordered_map>
#endif

#define HWNDPTR void*

//struct POINT;
struct Point
{
	long  x;
	long  y;
};

struct Rect
{
	long    left;
	long    top;
	long    right;
	long    bottom;
};

typedef unsigned __int64 UINT_PTR;
typedef UINT_PTR WPARAM;
typedef __int64 LONG_PTR; 
typedef LONG_PTR LPARAM;

namespace CommonUtilities
{
	class InputHandler
	{
	public:
		enum Key
		{
			Key_0 = 0x30,
			Key_1 = 0x31,
			Key_2 = 0x32,
			Key_3 = 0x33,
			Key_4 = 0x34,
			Key_5 = 0x35,
			Key_6 = 0x36,
			Key_7 = 0x37,
			Key_8 = 0x38,
			Key_9 = 0x39,
			Key_A = 0x41,
			Key_B = 0x42,
			Key_C = 0x43,
			Key_D = 0x44,
			Key_E = 0x45,
			Key_F = 0x46,
			Key_G = 0x47,
			Key_H = 0x48,
			Key_I = 0x49,
			Key_J = 0x4A,
			Key_K = 0x4B,
			Key_L = 0x4C,
			Key_M = 0x4D,
			Key_N = 0x4E,
			Key_O = 0x4F,
			Key_P = 0x50,
			Key_Q = 0x51,
			Key_R = 0x52,
			Key_S = 0x53,
			Key_T = 0x54,
			Key_U = 0x55,
			Key_V = 0x56,
			Key_W = 0x57,
			Key_X = 0x58,
			Key_Y = 0x59,
			Key_Z = 0x5A,
			Key_F1 = 0x70,
			Key_F2 = 0x71,
			Key_F3 = 0x72,
			Key_F4 = 0x73,
			Key_F5 = 0x74,
			Key_F6 = 0x75,
			Key_F7 = 0x76,
			Key_F8 = 0x77,
			Key_F9 = 0x78,
			Key_F10 = 0x79,
			Key_F11 = 0x7A,
			Key_F12 = 0x7B,
			Key_Numpad0 = 0x60,
			Key_Numpad1 = 0x61,
			Key_Numpad2 = 0x62,
			Key_Numpad3 = 0x63,
			Key_Numpad4 = 0x64,
			Key_Numpad5 = 0x65,
			Key_Numpad6 = 0x66,
			Key_Numpad7 = 0x67,
			Key_Numpad8 = 0x68,
			Key_Numpad9 = 0x69,
			Key_Multiply = 0x6A,
			Key_Add = 0x6B,
			Key_Seperator = 0x6C,
			Key_Subtract = 0x6D,
			Key_Decimal = 0x6E,
			Key_Divide = 0x6F,
			Key_Left = 0x25,
			Key_Up = 0x26,
			Key_Right = 0x27,
			Key_Down = 0x28,
			Key_Shift =  0x10, 
			Key_Control = 0x11,
			Key_Alt =  0x12,
			Key_Tab = 0x09,
			Key_Enter = 0x0D,
			Key_Space = 0x20,
			Key_BackSpace = 0x08,
			Key_Insert =  0x2D,
			Key_Delete = 0x2E,
			Key_Home = 0x24,
			Key_End = 0x23,
			Key_CapsLock = 0x14,
			Key_Escape = 0x1B,
			Key_PageUp = 0x21,
			Key_PageDown = 0x22
		};

		enum Mouse
		{
			Mouse_Left = 0x0001,
			Mouse_Right = 0x0002,
			Mouse_Middle = 0x0010,
			Mouse_X1 = 0x0020,
			Mouse_X2 = 0x0040,
			Mouse_Shift = 0x0004,
			Mouse_Control = 0x0008
		};

		InputHandler() = default;
		bool UpdateEvents(unsigned int aMessage, WPARAM aWParam, LPARAM aLParam);
		void FinalizeUpdate();

		bool IsKeyDown(Key aKey) const;
		bool IsKeyUp(Key aKey) const;
		bool IsKeyHit(Key aKey) const;
		bool IsKeyReleased(Key aKey) const;

		bool IsKeyDown(WPARAM aKey) const;
		bool IsKeyUp(WPARAM aKey) const;
		bool IsKeyHit(WPARAM aKey) const;
		bool IsKeyReleased(WPARAM aKey) const;

#ifdef _DEBUG
		std::string GetPressedHistory() const;
		std::string GetDownHistory() const;
#endif

		int GetMouseX() const;
		int GetMouseY() const;
		float GetMouseNomalizedX() const;
		float GetMouseNomalizedY() const;
		Point GetMousePosition() const;

		void SetWindowHandle(HWNDPTR aHandle);
		void SetMousePosition(int aX, int aY);
		void SetMousePosition(V2f aPosition);

		int GetMouseVx() const;
		int GetMouseVy() const;
		Point GetMouseVelocity() const;

		short GetWheelValue() const;
		short GetWheelVel() const;
		short GetWheelVelNormalized() const;

		bool IsMouseDown(Mouse aKey) const;
		bool IsMouseUp(Mouse aKey) const;
		bool IsMouseHit(Mouse aKey) const;
		bool IsMouseReleased(Mouse aKey) const;

		bool IsMouseDown(WPARAM aKey) const;
		bool IsMouseUp(WPARAM aKey) const;
		bool IsMouseHit(WPARAM aKey) const;
		bool IsMouseReleased(WPARAM aKey) const;

	private:
		HWNDPTR myWindowHandle;
		Rect myWindowRect;

		__int64 myNewKeys[4]{ 0, 0, 0, 0 };
		__int64 myOldKeys[4]{ 0, 0, 0, 0 };

		WPARAM myNewMouseState, myOldMouseState;
		int myMouseNewX, myMouseNewY = 0;
		int myMouseOldX, myMouseOldY = 0;
		int myWheelDelta = 0;
		short myWheelValue = 0;
		short myOldWheelValue = 0;


#ifdef _DEBUG
		static std::unordered_map<WPARAM, std::string> ourKeyToStringMap;
#endif
	};
}
