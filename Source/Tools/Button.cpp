#include <pch.h>
#include "Button.h"
#include "..\\GamlaBettan\SpriteInstance.h"
#include "..\\GamlaBettan\SpriteFactory.h"
#include "PostMaster.hpp"
SpriteFactory* Button::ourSpriteFactoryPtr = nullptr;

Button::Button()
{
	myState = ButtonState::Normal;
	myOnPressed = nullptr;
	mySprites[ButtonState::Normal] = nullptr;
	mySprites[ButtonState::Hover] = nullptr;
	mySprites[ButtonState::Pressed] = nullptr;
	mySprites[ButtonState::Disabled] = nullptr;
	myBounds = { 0,0,0,0 };
	myPosition = { 0,0 };
	myScreenSize = { 1600.f, 900.f };
	myIsListening = false;
	myScenePtr = nullptr;
}

Button::~Button()
{
	Unsubscribe();

	//for (int i = 0; i < mySprites.size(); ++i)
	//{
	//	if (mySprites[i])
	//	{
	//		mySprites[i] = nullptr;
	//	}
	//}
	//SAFE_DELETE(mySprites[ButtonState::Active]);
	//SAFE_DELETE(mySprites[ButtonState::Inactive]);
	//SAFE_DELETE(mySprites[ButtonState::Pressed]);
}

void Button::SetOnPressedFunction(const std::function<void(void)> aOnPressed)
{
	myOnPressed = aOnPressed;
}

void Button::TriggerOnPressed()
{
	if (myOnPressed != nullptr)
	{
		myOnPressed();
	}
}

SpriteInstance* Button::GetCurrentSprite()
{
	if (mySprites[myState] != nullptr)
	{
		return mySprites[myState];
	}
	return nullptr;
}

void Button::SetActive()
{
	if (myScenePtr != nullptr)
	{
		if (myState != ButtonState::Hover)
		{
			myScenePtr->RemoveSprite(mySprites[CAST(int, myState)]);
			myScenePtr->AddSprite(mySprites[CAST(int, ButtonState::Hover)]);
		}
	}
	myState = ButtonState::Hover;
	
	Message message;
	message.myMessageType = MessageType::MenuButtonActive;
	PostMaster::GetInstance()->SendMessages(message);
}

void Button::SetPressed()
{
	if (myScenePtr != nullptr)
	{
		if (myState != ButtonState::Pressed)
		{
			myScenePtr->RemoveSprite(mySprites[CAST(int, myState)]);
			myScenePtr->AddSprite(mySprites[CAST(int, ButtonState::Pressed)]);
		}
	}
	myState = ButtonState::Pressed;

	Message message;
	message.myMessageType = MessageType::MenuButtonHit;
	PostMaster::GetInstance()->SendMessages(message);
}

void Button::SetToNormal()
{
	if (myScenePtr != nullptr)
	{
		if (myState != ButtonState::Normal)
		{
			myScenePtr->RemoveSprite(mySprites[CAST(int, myState)]);
			myScenePtr->AddSprite(mySprites[CAST(int, ButtonState::Normal)]);
		}
	}
	myState = ButtonState::Normal;
}

void Button::ActuallyEnable()
{
	if (!myIsListeningToClick)
	{
		myScenePtr->RemoveSprite(mySprites[CAST(int, ButtonState::Disabled)]);
		myScenePtr->AddSprite(mySprites[CAST(int, ButtonState::Normal)]);
		myIsListeningToClick = true;
		SubscribeToMessage(MessageType::InputLeftMouseHit);
		SubscribeToMessage(MessageType::InputMouseMoved);
		myState = ButtonState::Normal;
	}
}

void Button::ActuallyDisable()
{
	if (myIsListeningToClick)
	{
		myScenePtr->RemoveSprite(mySprites[CAST(int, myState)]);
		myScenePtr->AddSprite(mySprites[CAST(int, ButtonState::Disabled)]);
		myIsListeningToClick = false;
		UnSubscribeToMessage(MessageType::InputLeftMouseHit);
		UnSubscribeToMessage(MessageType::InputMouseMoved);
		myState = ButtonState::Disabled;
	}
}

bool Button::CheckHover(float aMouseX, float aMouseY)
{
	if (aMouseX >= myBounds.x && aMouseX <= myBounds.z)
	{
		if (aMouseY >= myBounds.y && aMouseY <= myBounds.w)
		{
			return true;
		}
	}
	return false;
}

bool Button::Init(const std::string& aNormalPath, const std::string& aHoveredPath, const std::string& aPressedPath, const std::string& aDisabledPath, const CommonUtilities::Vector2<float>& aPosition,
	const V2F& aHitBoxSize, SpriteFactory* aSpriteFactory)
{
	myPosition = aPosition;
	myState = ButtonState::Normal;

	mySprites[ButtonState::Normal] = aSpriteFactory->CreateSprite(aNormalPath);
	mySprites[ButtonState::Normal]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Normal]->SetPosition(myPosition);

	mySprites[ButtonState::Hover] = aSpriteFactory->CreateSprite(aHoveredPath);
	mySprites[ButtonState::Hover]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Hover]->SetPosition(myPosition);

	mySprites[ButtonState::Pressed] = aSpriteFactory->CreateSprite(aPressedPath);
	mySprites[ButtonState::Pressed]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Pressed]->SetPosition(myPosition);

	mySprites[ButtonState::Disabled] = aSpriteFactory->CreateSprite(aDisabledPath);
	mySprites[ButtonState::Disabled]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Disabled]->SetPosition(myPosition);

	mySize = aHitBoxSize;

	SetupBounds();

	Subscribe();

	return true;
}

bool Button::Init(const std::string& aFolderPath, const std::string& aButtonName, const CommonUtilities::Vector2<float>& aPosition,
	const V2F& aHitBoxSize, SpriteFactory* aSpriteFactory)
{
	myPosition = aPosition;

	std::string path = aFolderPath + "\\" + aButtonName;
	mySprites[ButtonState::Normal] = aSpriteFactory->CreateSprite(path + " button off.dds");
	mySprites[ButtonState::Normal]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Normal]->SetPosition(myPosition);

	mySprites[ButtonState::Hover] = aSpriteFactory->CreateSprite(path + " button hover.dds");
	mySprites[ButtonState::Hover]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Hover]->SetPosition(myPosition);

	mySprites[ButtonState::Pressed] = aSpriteFactory->CreateSprite(path + " button on.dds");
	mySprites[ButtonState::Pressed]->SetPivot(V2F(0.5f, 0.5f));
	mySprites[ButtonState::Pressed]->SetPosition(myPosition);

	mySize = aHitBoxSize;

	SetupBounds();

	Subscribe();

	return true;
}

bool Button::IsActive()
{
	if (myState == ButtonState::Hover)
	{
		return true;
	}
	return false;
}

bool Button::IsPressed()
{
	if (myState == ButtonState::Pressed)
	{
		return true;
	}
	return false;
}

void Button::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		if (IsPressed() == false)
		{
			if (CheckHover(aMessage.myFloatValue, aMessage.myFloatValue2) == true && !IsActive())
			{
				SetActive();
			}
			else if (!CheckHover(aMessage.myFloatValue, aMessage.myFloatValue2))
			{
				SetToNormal();
			}
		}

	}
	else if (aMessage.myMessageType == MessageType::InputLeftMouseReleased)
	{
		PERFORMANCETAG("MouseReleasedInButton");
		if (IsPressed() == true)
		{
			SetActive();
			TriggerOnPressed();
		}
	}
	else if (aMessage.myMessageType == MessageType::InputLeftMouseHit)
	{
		PERFORMANCETAG("MouseHitInButton");
		if (IsActive() == true)
		{
			SetPressed();
		}
	}
	else if (aMessage.myMessageType == MessageType::WindowResize)
	{
		myScreenSize = V2F(CAST(float, aMessage.myIntValue), CAST(float, aMessage.myIntValue2));
		SetupBounds();

	}
}

void Button::Subscribe()
{
	if (!myIsListening)
	{
		SubscribeToMessage(MessageType::InputLeftMouseReleased);
		SubscribeToMessage(MessageType::WindowResize);
		if (!myIsListeningToClick)
		{
			myIsListeningToClick = true;
			SubscribeToMessage(MessageType::InputMouseMoved);
			SubscribeToMessage(MessageType::InputLeftMouseHit);
		}
		myIsListening = true;
	}
}

void Button::Unsubscribe()
{
	if (myIsListening)
	{
		UnSubscribeToMessage(MessageType::InputLeftMouseReleased);
		UnSubscribeToMessage(MessageType::WindowResize);

		if (myIsListeningToClick)
		{
			myIsListeningToClick = false;
			UnSubscribeToMessage(MessageType::InputMouseMoved);
			UnSubscribeToMessage(MessageType::InputLeftMouseHit);
		}
		myIsListening = false;
	}
}

void Button::SetSpriteFactory(SpriteFactory* aSpriteFactory)
{
	ourSpriteFactoryPtr = aSpriteFactory;
}

void Button::SetScenePtr(Scene* aScenePtr)
{
	myScenePtr = aScenePtr;
}

void Button::RemoveAllSprites()
{
	if (myScenePtr != nullptr)
	{
		myScenePtr->RemoveSprite(mySprites[ButtonState::Hover]);
		myScenePtr->RemoveSprite(mySprites[ButtonState::Normal]);
		myScenePtr->RemoveSprite(mySprites[ButtonState::Pressed]);
		myScenePtr->RemoveSprite(mySprites[ButtonState::Disabled]);
	}
}

SpriteFactory* Button::GetSpriteFactory()
{
	return ourSpriteFactoryPtr;
}

void Button::SetupBounds()
{
	CommonUtilities::Vector2<float> imageSize;

	imageSize = mySprites[ButtonState::Normal]->GetSizeOnScreen();

	myBounds.x = myPosition.x - (imageSize.x * mySize.x) / 2;
	myBounds.y = myPosition.y - (imageSize.y * mySize.y) / 2;
	myBounds.z = myPosition.x + (imageSize.x * mySize.x) / 2;
	myBounds.w = myPosition.y + (imageSize.y * mySize.y) / 2;
}
