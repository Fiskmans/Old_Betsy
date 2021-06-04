#include "pch.h"
#include "InputHandler.h"
#include <Windowsx.h>

namespace CommonUtilities
{

	bool InputHandler::UpdateEvents(const unsigned int aMessage, const WPARAM aWParam, LPARAM aLParam)
	{
		int index;

		switch (aMessage)
		{

		case WM_MOUSEWHEEL:
			myWheelValue += GET_WHEEL_DELTA_WPARAM(aWParam);
			myWheelDelta = GET_WHEEL_DELTA_WPARAM(aWParam);
			break;

		case WM_MOUSEHOVER:
		case WM_MOUSEMOVE:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_LBUTTONUP:
			myNewMouseState = aWParam;
			myMouseNewX = GET_X_LPARAM(aLParam);
			myMouseNewY = GET_Y_LPARAM(aLParam);

			break;

		case WM_KEYDOWN:
			index = static_cast<int>(std::floor<int>(static_cast<int>(aWParam) / 64));
			myNewKeys[index] |= 1LL << (static_cast<int>(aWParam) % 64);
			break;
		case WM_KEYUP:   
			index = static_cast<int>(std::floor<int>(static_cast<int>(aWParam) / 64));
			myNewKeys[index] &= ~(1LL << (static_cast<int>(aWParam) % 64));
			break;
		default:
			return false;
		}

		return true;
	}

	void InputHandler::FinalizeUpdate()
	{
		myOldKeys[0] = myNewKeys[0];
		myOldKeys[1] = myNewKeys[1];
		myOldKeys[2] = myNewKeys[2];
		myOldKeys[3] = myNewKeys[3];

		myOldMouseState = myNewMouseState;
		myMouseOldX = myMouseNewX;
		myMouseOldY = myMouseNewY;
		myWheelDelta = 0;
	}

	/**
	 * \brief Has partial support as defined by the CommonUtilities::Key:: enum
	 */
	bool InputHandler::IsKeyDown(const Key aKey) const
	{
#if USEIMGUI
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			return false;
		}
#endif
		const int index = static_cast<int>(std::floor(aKey / 64));
		return (myNewKeys[index] & (1LL << (aKey % 64)));
	}
	
	/**
	 * \brief Has partial support as defined by the CommonUtilities::Key:: enum
	 */
	bool InputHandler::IsKeyUp(const Key aKey) const
	{
#if USEIMGUI
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			return false;
		}
#endif
		const int index = static_cast<int>(std::floor(aKey / 64));
		return !(myNewKeys[index] & (1LL << (aKey % 64)));
	}
	
	/**
	 * \brief Has partial support as defined by the CommonUtilities::Key:: enum
	 */
	bool InputHandler::IsKeyHit(const Key aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				return false;
			}
		}
#endif
		const int index = static_cast<int>(std::floor(aKey / 64));
		return (myNewKeys[index] & (1LL << (aKey % 64))) && !(myOldKeys[index] & (1LL << (aKey % 64)));
	}
	
	/**
	* \brief Has partial support as defined by the CommonUtilities::Key:: enum
	*/
	bool InputHandler::IsKeyReleased(const Key aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				return false;
			}
		}
#endif
		const int index = static_cast<int>(std::floor(aKey / 64));
		return !(myNewKeys[index] & (1LL << (aKey % 64))) && (myOldKeys[index] & (1LL << (aKey % 64)));
	}
	
	/**
	 * \brief Has full support for all VK_* as defined in the <winuser> header
	 * \param aKey A VK_* macro found in the <winuser> header. Use char literals for a-z and 0-9
	 */
	bool InputHandler::IsKeyDown(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				return false;
			}
		}
#endif
		const int index = static_cast<int>(std::floor(aKey / 64));
		return (myNewKeys[index] & (1LL << (aKey % 64)));
	}
	
	/**
	 * \brief Has full support for all VK_* as defined in the <winuser> header
	 * \param aKey A VK_* macro found in the <winuser> header. Use char literals for a-z and 0-9
	 */
	bool InputHandler::IsKeyUp(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				return false;
			}
		}
#endif
		const int index = static_cast<int>(std::floor(aKey / 64));
		return !(myNewKeys[index] & (1LL << (aKey % 64)));
	} 
	
	/**
	 * \brief Has full support for all VK_* as defined in the <winuser> header
	 * \param aKey A VK_* macro found in the <winuser> header. Use char literals for a-z and 0-9
	 */
	bool InputHandler::IsKeyHit(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				return false;
			}
		}
#endif
		const int index =static_cast<int>(std::floor(aKey / 64));
		return (myNewKeys[index] & (1LL << (aKey % 64))) && !(myOldKeys[index] & (1LL << (aKey % 64)));
	}
	
	/**
	 * \brief Has full support for all VK_* as defined in the <winuser> header
	 * \param aKey A VK_* macro found in the <winuser> header. Use char literals for a-z and 0-9
	 */
	bool InputHandler::IsKeyReleased(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				return false;
			}
		}
#endif
		const int index =static_cast<int>(std::floor(aKey / 64));
		return !(myNewKeys[index] & (1LL << (aKey % 64))) && (myOldKeys[index] & (1LL << (aKey % 64)));
	}

#ifdef _DEBUG
	std::string InputHandler::GetPressedHistory() const
	{
		static std::string string = "";
		
		for (auto value : ourKeyToStringMap)
		{
			if (IsKeyHit(value.first))
				string += "Hit: " + value.second + " ";
			if (IsKeyReleased(value.first))
				string += "Released: " + value.second + " ";
		}

		return string;
	}

	std::string InputHandler::GetDownHistory() const
	{
		static std::vector<std::string> history;

		std::string string = "";

		for (auto value : ourKeyToStringMap)
		{
			if (IsKeyDown(value.first) && std::find(history.begin(), history.end(), value.second) == history.end())
			{
				history.push_back(value.second);
			}

			if (IsKeyUp(value.first))
			{
				auto iter = std::find(history.begin(), history.end(), value.second);
				if (iter != history.end())
					history.erase(iter);
			}
		}

		for (auto s : history)
			string += s + " ";

		return string;
	}
#endif

	int InputHandler::GetMouseX() const
	{
		return myMouseNewX;
	}

	int InputHandler::GetMouseY() const
	{
		return myMouseNewY;
	}

	float InputHandler::GetMouseNomalizedX() const
	{
		return static_cast<float>(myMouseNewX) / static_cast<float>(myWindowRect.right - myWindowRect.left);
	}

	float InputHandler::GetMouseNomalizedY() const
	{
		return static_cast<float>(myMouseNewY) / static_cast<float>(myWindowRect.bottom - myWindowRect.top);
	}

	Point InputHandler::GetMousePosition() const
	{
		return { myMouseNewX, myMouseNewY };
	}

	void InputHandler::SetWindowHandle(HWNDPTR aHandle)
	{
		myWindowHandle = aHandle;
		RECT rect;
		GetWindowRect(static_cast<HWND>(myWindowHandle), &rect);

		myWindowRect = { rect.left, rect.top, rect.right, rect.bottom };
	}

	void InputHandler::SetMousePosition(int aX, int aY)
	{
		myMouseOldX = myMouseNewX;
		myMouseOldX = myMouseNewX;
		POINT p = { myMouseNewX = aX, myMouseNewY = aY };
		ClientToScreen((HWND)myWindowHandle, &p);
		SetCursorPos(p.x, p.y);
	}

	void InputHandler::SetMousePosition(V2f	aPosition)
	{
		SetMousePosition(LERP(myWindowRect.left, myWindowRect.right, aPosition.x), LERP(myWindowRect.top, myWindowRect.bottom, aPosition.y));
	}

	int InputHandler::GetMouseVx() const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return 0;
			}
		}
#endif
		return myMouseNewX - myMouseOldX;
	}
	int InputHandler::GetMouseVy() const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return 0;
			}
		}
#endif
		return myMouseNewY - myMouseOldY;
	}

	Point InputHandler::GetMouseVelocity() const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return {0,0};
			}
		}
#endif
		return { GetMouseVx(), GetMouseVy() };
	}

	short InputHandler::GetWheelValue() const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		return myWheelValue;
	}
	short InputHandler::GetWheelVel() const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return 0;
			}
		}
#endif
		return myWheelDelta;
	}
	short InputHandler::GetWheelVelNormalized() const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		return myWheelDelta / WHEEL_DELTA;
	}

	bool InputHandler::IsMouseDown(const Mouse aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		const WPARAM key = static_cast<WPARAM>(aKey);
		return (myNewMouseState & key) == key;
	}
	bool InputHandler::IsMouseUp(const Mouse aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		const WPARAM key = static_cast<WPARAM>(aKey);
		return (myNewMouseState & key) != key;
	}
	bool InputHandler::IsMouseHit(const Mouse aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		const UINT_PTR key = static_cast<WPARAM>(aKey);
		return (myNewMouseState & key) == key && (myOldMouseState & key) != key;
	}
	bool InputHandler::IsMouseReleased(const Mouse aKey) const
	{
		const auto key = static_cast<WPARAM>(aKey);
		return (myNewMouseState & key) != key && (myOldMouseState & key) == key;
	}

	bool InputHandler::IsMouseDown(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		return (myNewMouseState & aKey) == aKey;
	}
	bool InputHandler::IsMouseUp(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		return (myNewMouseState & aKey) != aKey;
	}
	bool InputHandler::IsMouseHit(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		return (myNewMouseState & aKey) == aKey && (myOldMouseState & aKey) != aKey;
	}
	bool InputHandler::IsMouseReleased(const WPARAM aKey) const
	{
#if USEIMGUI
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				return false;
			}
		}
#endif
		return (myNewMouseState & aKey) != aKey && (myOldMouseState & aKey) == aKey;
	}

#ifdef _DEBUG
	std::unordered_map<WPARAM, std::string> InputHandler::ourKeyToStringMap =
	{
	{ Key_0        , "0"},
	{ Key_1		   , "1"},
	{ Key_2		   , "2"},
	{ Key_3		   , "3"},
	{ Key_4		   , "4"},
	{ Key_5		   , "5"},
	{ Key_6		   , "6"},
	{ Key_7		   , "7"},
	{ Key_8		   , "8"},
	{ Key_9		   , "9"},
	{ Key_A		   , "A"},
	{ Key_B		   , "B"},
	{ Key_C		   , "C"},
	{ Key_D		   , "D"},
	{ Key_E		   , "E"},
	{ Key_F		   , "F"},
	{ Key_G		   , "G"},
	{ Key_H		   , "H"},
	{ Key_I		   , "I"},
	{ Key_J		   , "J"},
	{ Key_K		   , "K"},
	{ Key_L		   , "L"},
	{ Key_M		   , "M"},
	{ Key_N		   , "N"},
	{ Key_O		   , "O"},
	{ Key_P		   , "P"},
	{ Key_Q		   , "Q"},
	{ Key_R		   , "R"},
	{ Key_S		   , "S"},
	{ Key_T		   , "T"},
	{ Key_U		   , "U"},
	{ Key_V		   , "V"},
	{ Key_W		   , "W"},
	{ Key_X		   , "X"},
	{ Key_Y		   , "Y"},
	{ Key_Z		   , "Z"},
	{ Key_F1	   , "F1"},
	{ Key_F2	   , "F2"},
	{ Key_F3	   , "F3"},
	{ Key_F4	   , "F4"},
	{ Key_F5	   , "F5"},
	{ Key_F6	   , "F6"},
	{ Key_F7	   , "F7"},
	{ Key_F8	   , "F8"},
	{ Key_F9	   , "F9"},
	{ Key_F10	   , "F10"},
	{ Key_F11	   , "F11"},
	{ Key_F12	   , "F12"},
	{ Key_Numpad0  , "Numpad0"},
	{ Key_Numpad1  , "Numpad1"},
	{ Key_Numpad2  , "Numpad2"},
	{ Key_Numpad3  , "Numpad3"},
	{ Key_Numpad4  , "Numpad4"},
	{ Key_Numpad5  , "Numpad5"},
	{ Key_Numpad6  , "Numpad6"},
	{ Key_Numpad7  , "Numpad7"},
	{ Key_Numpad8  , "Numpad8"},
	{ Key_Numpad9  , "Numpad9"},
	{ Key_Multiply , "Multiply"},
	{ Key_Add	   , "Add"},
	{ Key_Seperator, "Seperator"},
	{ Key_Subtract , "Subtract"},
	{ Key_Decimal  , "Decimal"},
	{ Key_Divide   , "Divide"},
	{ Key_Left	   , "Left"},
	{ Key_Up	   , "Up"},
	{ Key_Right	   , "Right"},
	{ Key_Down	   , "Down"},
	{ Key_Shift	   , "Shift"},
	{ Key_Control  , "Control"},
	{ Key_Alt	   , "Alt"},
	{ Key_Tab	   , "Tab"},
	{ Key_Enter	   , "Enter"},
	{ Key_Space	   , "Space"},
	{ Key_BackSpace, "BackSpace"},
	{ Key_Insert   , "Insert"},
	{ Key_Delete   , "Delete"},
	{ Key_Home	   , "Home"},
	{ Key_End	   , "End"},
	{ Key_CapsLock , "CapsLock"},
	{ Key_Escape   , "Escape"},
	{ Key_PageUp   , "PageUp"},
	{ Key_PageDown , "PageDown"}
	};
#endif
}
