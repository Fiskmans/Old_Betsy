#include "pch.h"
#include "MenuSlider.h"
#include "SpriteFactory.h"
#include "SpriteInstance.h"
#include "Scene.h"

MenuSlider::MenuSlider()
{
}

MenuSlider::~MenuSlider()
{
	UnSubscribeToMessage(MessageType::InputLeftMouseDown);
}

void MenuSlider::Init(SpriteFactory* aSpriteFactory, const V2F& aPos, const int& aMaxValue, MessageType aMessageToSendNewValue, const int& aStartValue)
{
	mySpriteFactory = aSpriteFactory;

	myPos = aPos;
	myMaxValue = aMaxValue;
	myCurrentValue = aStartValue;

	myBackground = mySpriteFactory->CreateSprite("Data/UI/MenuSlider/MenuSlider_Background.dds");
	mySlider = mySpriteFactory->CreateSprite("Data/UI/MenuSlider/MenuSlider_Slider.dds");
	myForeground = mySpriteFactory->CreateSprite("Data/UI/MenuSlider/MenuSlider_Foreground.dds");


	myBackground->SetPosition(myPos - mySlider->GetSizeOnScreen() / 2.f);
	mySlider->SetPosition(myPos - mySlider->GetSizeOnScreen() / 2.f);
	myForeground->SetPosition(myPos - mySlider->GetSizeOnScreen() / 2.f);

	myHitboxMin = myPos - mySlider->GetSizeOnScreen() / 2.f;
	myHitboxMax = myPos + mySlider->GetSizeOnScreen() / 2.f;

	myMessageType = aMessageToSendNewValue;

	SubscribeToMessage(MessageType::InputLeftMouseDown);
}

void MenuSlider::AddToScene(Scene* aScene)
{
	aScene->AddSprite(myBackground);
	aScene->AddSprite(mySlider);
	aScene->AddSprite(myForeground);
}

void MenuSlider::RemoveFromScene(Scene* aScene)
{
	aScene->RemoveSprite(myBackground);
	aScene->RemoveSprite(mySlider);
	aScene->RemoveSprite(myForeground);
}

void MenuSlider::SetCurrentValue(int aValue)
{
	myCurrentValue = aValue;

	mySlider->SetScale(V2F(static_cast<float>(myCurrentValue) / static_cast<float>(myMaxValue), 1.f));
}

SpriteInstance* MenuSlider::GetBackgroundSprite()
{
	return myBackground;
}

SpriteInstance* MenuSlider::GetSliderSprite()
{
	return mySlider;
}

SpriteInstance* MenuSlider::GetForegroundSprite()
{
	return myForeground;
}

void MenuSlider::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputLeftMouseDown)
	{
		if (aMessage.myFloatValue > myHitboxMin.x &&
			aMessage.myFloatValue < myHitboxMax.x &&
			aMessage.myFloatValue2 > myHitboxMin.y &&
			aMessage.myFloatValue2 < myHitboxMax.y)
		{
			float size = (myHitboxMax.x - myHitboxMin.x);
			float mousePos = (aMessage.myFloatValue - myHitboxMin.x);
			float mousePosPercent = mousePos / size;

			mySlider->SetScale(V2F(mousePosPercent, 1.f));

			myCurrentValue = mousePosPercent * 100;

			Message message;
			message.myMessageType = myMessageType;
			message.myIntValue = myCurrentValue;
			SendMessages(message);
		}
	}
}

void MenuSlider::SetPivot(const V2F& aPivot)
{
	mySlider->SetPivot(aPivot);
	//Do position stuff here

	if (myForeground)
	{
		myForeground->SetPivot(aPivot);
	}
	if (myBackground)
	{
		myBackground->SetPivot(aPivot);
	}
}