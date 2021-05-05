#pragma once
#include "Observer.hpp"
#include <vector>
#include <queue>

class BaseState;
class CGraphicsEngine;
class StateStack: public Observer
{
public:
	StateStack();
	~StateStack();
	void Init();
	void Update(const float aDeltaTime, int aStateToUpdateDeph);
	void Render(int aStateToRenderDeph, CGraphicsEngine* aGraphicsEngine);
	void HandleMessages();
	bool IsEmpty();

	static void* ourCurrentStateIdentifier;
private:
	enum class eCommands
	{
		ePush,
		ePopMajor,
		ePopMinor
	};
	virtual void RecieveMessage(const Message& aMessage) override;

	void PopMajor();
	void PopMinor();
	void Pop();
	void ActivateStates(int aActivationDepth);
	std::vector<BaseState*>* myStates;

	struct StateMessage
	{
		eCommands myCommand;
		BaseState* myState = nullptr;
	};
	std::queue<StateMessage> myMessageQueue;

	bool myShouldPopMajorAtEndOfFrame;
};

