#pragma once
#include <pch.h>
#include "StateStack.h"
#include "BaseState.h"
#include "PostMaster.hpp"

void* StateStack::ourCurrentStateIdentifier = nullptr;

StateStack::StateStack()
{
	myStates = new std::vector<BaseState*>();
	myStates->reserve(10);
	myShouldPopMajorAtEndOfFrame = false;
}

void StateStack::Init()
{
	SubscribeToMessage(MessageType::PopState);
	SubscribeToMessage(MessageType::PushState);
}

void StateStack::Update(const float aDeltaTime, int aStateToUpdateDeph)
{
	--aStateToUpdateDeph;
	if (myStates->size() > 0)
	{
		int stateToUpdateIndex = static_cast<int>(myStates->size()) + aStateToUpdateDeph;
		if (stateToUpdateIndex >= 0)
		{			
			BaseState* state = myStates->at(myStates->size() + aStateToUpdateDeph);
			if (state->IsUpdateThroughEnabled() == true)
			{
				Update(aDeltaTime, aStateToUpdateDeph);
			}
			state->Update(aDeltaTime);
		}

		if (myShouldPopMajorAtEndOfFrame)
		{
			StateMessage message;
			message.myCommand = eCommands::ePopMajor;
			myMessageQueue.emplace(message);
			myShouldPopMajorAtEndOfFrame = false;
		}
	}
}

void StateStack::Render(int aStateToRenderDeph, CGraphicsEngine* aGraphicsEngine)
{
	--aStateToRenderDeph;
	if (myStates->size() > 0)
	{
		int stateToUpdateIndex = static_cast<int>(myStates->size()) + aStateToRenderDeph;
		if (stateToUpdateIndex >= 0)
		{
			BaseState* state = myStates->at(myStates->size() + aStateToRenderDeph);
			if (state->IsDrawThroughEnabled() == true)
			{
				Render(aStateToRenderDeph, aGraphicsEngine);
			}
			ourCurrentStateIdentifier = state;
			state->Render(aGraphicsEngine);
		}
	}
}

bool StateStack::IsEmpty()
{
	if (myStates->size() > 0)
	{
		return false;
	}
	return true;
}

void StateStack::ActivateStates(int aActivationDepth)
{
	--aActivationDepth;
	if (myStates->size() > 0)
	{
		int stateToUpdateIndex = static_cast<int>(myStates->size()) + aActivationDepth;
		if (stateToUpdateIndex >= 0)
		{
			BaseState* state = myStates->at(myStates->size() + aActivationDepth);
			if (state->IsUpdateThroughEnabled() == true)
			{
				ActivateStates(aActivationDepth);
			}
			state->Activate();
		}
	}
}

void StateStack::RecieveMessage(const Message& aMessage)
{
	StateMessage message;
	if (aMessage.myMessageType == MessageType::PopState)
	{
		if (aMessage.myBool == false)
		{
			message.myCommand = eCommands::ePopMinor;
			myMessageQueue.emplace(message);
		}
		else
		{
			message.myCommand = eCommands::ePopMajor;
			myMessageQueue.emplace(message);
		}
	}
	else if (aMessage.myMessageType == MessageType::PushState)
	{
		message.myCommand = eCommands::ePush;
		message.myState = CAST(BaseState*, aMessage.myData);
		myMessageQueue.emplace(message);
	}
}
void StateStack::HandleMessages()
{	
	while ((myMessageQueue.size() > 0)==true)
	{
		switch (myMessageQueue.front().myCommand)
		{
		case eCommands::ePopMajor:
		{
			PopMajor();
			ActivateStates(0);
			break;
		}
		case eCommands::ePopMinor:
		{
			PopMinor();
			ActivateStates(0);
			break;
		}
		case eCommands::ePush:
		{
			if (myMessageQueue.front().myState != nullptr)
			{
				if (myMessageQueue.front().myState->IsUpdateThroughEnabled() == false)
				{
					for (size_t i = 0; i < myStates->size(); i++)
					{
						(*myStates)[i]->Deactivate();
					}
				}
				myStates->push_back(myMessageQueue.front().myState);
				myStates->back()->Activate();
			}
			break;
		}
		}
		myMessageQueue.pop();
	}
}

void StateStack::PopMajor()
{
	bool isMain = myStates->back()->IsMain();

	Pop();
	if (isMain == true)
	{
		return;
	}
	else
	{
		myShouldPopMajorAtEndOfFrame = true;
	}
}
void StateStack::PopMinor()
{
	if (myStates->size() > 0)
	{
		if (myStates->back()->IsMain() == false)
		{
			Pop();
		}
	}
}
void StateStack::Pop()
{
	myStates->back()->Deactivate();
	myStates->back()->Unload();

	if (myStates->back()->myShouldDeleteOnPop)
	{
		delete myStates->back();
	}

	myStates->back() = nullptr;
	myStates->pop_back();
}

StateStack::~StateStack()
{
	UnSubscribeToMessage(MessageType::PopState);
	UnSubscribeToMessage(MessageType::PushState);
	while (myStates->size() > 0)
	{
		Pop();
	}

	delete myStates;
	myStates = nullptr;
}