#pragma once
#include "Secret.h"

namespace CommonUtilities
{
	class InputHandler;
}

class InputManager
{
public:

	InputManager();
	~InputManager();

	void Init(CommonUtilities::InputHandler* aInputHandler);
	void Update();

	float GetMouseNomalizedX() const;
	float GetMouseNomalizedY() const;

	//Only for debug
	inline CommonUtilities::InputHandler* GetInputHandler() const;

private:
	
	CommonUtilities::InputHandler* myInputHandler;

	CommonUtilities::Vector2<float> myCurrentMousePosition;

	bool myIsPaused;
	bool myMouseIsMoving;

	std::vector<Secret> mySecrets;
};

inline CommonUtilities::InputHandler* InputManager::GetInputHandler() const
{
	return myInputHandler;
}