#include "pch.h"
#include "House.h"
#include "TimeHandler.h"

void House::InteractWith(Item* aHeldItem)
{
	if (myTimeOfDay < 18)
	{
		myAcceptInteraction.InitializeInteraction("It's too early to sleep", false, true);
	}
	else
	{
		myAcceptInteraction.InitializeInteraction("Go to sleep?", true, true);
	}
}

void House::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::UpdateTime)
	{
		myTimeOfDay = aMessage.myIntValue;
	}
}

void House::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myTimeOfDay = 0;

	SubscribeToMessage(MessageType::UpdateTime);
}

void House::Update(const float aDeltaTime)
{
	if (myAcceptInteraction.UpdateAcceptScreen())
	{
		TimeHandler::GetInstance().NextDay();
	}
}

void House::Reset()
{
	UnSubscribeToMessage(MessageType::UpdateTime);
}

void House::OnAttach()
{
}

void House::OnDetach()
{
}

void House::OnKillMe()
{
}

void House::SetScenePtr(Scene* aScene)
{
	myAcceptInteraction.SetScenePtr(aScene);
}

void House::SetTextFactory(TextFactory* aTextFactory)
{
	myAcceptInteraction.SetTextFactory(aTextFactory);
}
