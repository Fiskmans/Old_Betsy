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
			index = static_cast<int>(static_cast<int>(aWParam) / CHAR_BIT);
			myNewKeys[index] |= BIT(static_cast<int>(aWParam) % CHAR_BIT);
			break;
		case WM_KEYUP:   
			index = static_cast<int>(static_cast<int>(aWParam) / CHAR_BIT);
			myNewKeys[index] &= ~BIT(static_cast<int>(aWParam) % CHAR_BIT);
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

	bool InputHandler::IsKeyDown(const Key aKey) const
	{
#if USEIMGUI
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			return false;
		}
#endif
		return !!(myNewKeys[static_cast<size_t>(aKey) / CHAR_BIT] & BIT(static_cast<size_t>(aKey) % CHAR_BIT));
	}
	
	bool InputHandler::IsKeyUp(const Key aKey) const
	{
#if USEIMGUI
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			return false;
		}
#endif
		return !(myNewKeys[static_cast<size_t>(aKey) / CHAR_BIT] & BIT(static_cast<size_t>(aKey) % CHAR_BIT));
	}
	
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
		return IsKeyDown(aKey) && !(myOldKeys[static_cast<size_t>(aKey) / CHAR_BIT] & BIT(static_cast<size_t>(aKey) % CHAR_BIT));
	}
	
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
		return !IsKeyDown(aKey) && !!(myOldKeys[static_cast<size_t>(aKey) / CHAR_BIT] & BIT(static_cast<size_t>(aKey) % CHAR_BIT));
	}
	
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

}
