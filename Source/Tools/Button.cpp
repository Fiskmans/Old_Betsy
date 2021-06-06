#include <pch.h>
#include "Button.h"
#include "GamlaBettan\Scene.h"
#include "GamlaBettan\SpriteInstance.h"
#include "GamlaBettan\SpriteFactory.h"
#include "PostMaster.hpp"

SpriteFactory* Button::ourSpriteFactoryPtr = nullptr;

Button::Button() 
	: Observer(
		{
			MessageType::InputLeftMouseHit,
			MessageType::InputLeftMouseReleased,
			MessageType::InputMouseMoved,
			MessageType::ResizeWindow
		})
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
}

Button::~Button()
{
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
	

	if (myState != ButtonState::Hover)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Hover)]);
	}
	myState = ButtonState::Hover;
	
	PostMaster::GetInstance().SendMessages(MessageType::MenuButtonActive);
}

void Button::SetPressed()
{
	if (myState != ButtonState::Pressed)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Pressed)]);
	}
	myState = ButtonState::Pressed;

	PostMaster::GetInstance().SendMessages(MessageType::MenuButtonHit);
}

void Button::SetToNormal()
{
	if (myState != ButtonState::Normal)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Normal)]);
	}
	myState = ButtonState::Normal;
}

void Button::ActuallyEnable()
{
	if (!myIsListeningToClick)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, ButtonState::Disabled)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Normal)]);
		myIsListeningToClick = true;
		myState = ButtonState::Normal;
	}
}

void Button::ActuallyDisable()
{
	if (myIsListeningToClick)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Disabled)]);
		myIsListeningToClick = false;
		myState = ButtonState::Disabled;
	}
}

bool Button::CheckHover(const V2f& aPosition)
{
	if (aPosition.x >= myBounds.x && aPosition.x <= myBounds.z)
	{
		if (aPosition.y >= myBounds.y && aPosition.y <= myBounds.w)
		{
			return true;
		}
	}
	return false;
}

bool Button::Init(const std::string& aNormalPath, const std::string& aHoveredPath, const std::string& aPressedPath, const std::string& aDisabledPath, const CommonUtilities::Vector2<float>& aPosition,
	const V2f& aHitBoxSize, SpriteFactory* aSpriteFactory)
{
	myPosition = aPosition;
	myState = ButtonState::Normal;

	mySprites[ButtonState::Normal] = aSpriteFactory->CreateSprite(aNormalPath);
	mySprites[ButtonState::Normal]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Normal]->SetPosition(myPosition);

	mySprites[ButtonState::Hover] = aSpriteFactory->CreateSprite(aHoveredPath);
	mySprites[ButtonState::Hover]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Hover]->SetPosition(myPosition);

	mySprites[ButtonState::Pressed] = aSpriteFactory->CreateSprite(aPressedPath);
	mySprites[ButtonState::Pressed]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Pressed]->SetPosition(myPosition);

	mySprites[ButtonState::Disabled] = aSpriteFactory->CreateSprite(aDisabledPath);
	mySprites[ButtonState::Disabled]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Disabled]->SetPosition(myPosition);

	mySize = aHitBoxSize;

	SetupBounds();

	return true;
}

bool Button::Init(const std::string& aFolderPath, const std::string& aButtonName, const CommonUtilities::Vector2<float>& aPosition,
	const V2f& aHitBoxSize, SpriteFactory* aSpriteFactory)
{
	myPosition = aPosition;

	std::string path = aFolderPath + "\\" + aButtonName;
	mySprites[ButtonState::Normal] = aSpriteFactory->CreateSprite(path + " button off.dds");
	mySprites[ButtonState::Normal]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Normal]->SetPosition(myPosition);

	mySprites[ButtonState::Hover] = aSpriteFactory->CreateSprite(path + " button hover.dds");
	mySprites[ButtonState::Hover]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Hover]->SetPosition(myPosition);

	mySprites[ButtonState::Pressed] = aSpriteFactory->CreateSprite(path + " button on.dds");
	mySprites[ButtonState::Pressed]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Pressed]->SetPosition(myPosition);

	mySize = aHitBoxSize;

	SetupBounds();

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
			if (CheckHover(*reinterpret_cast<const V2f*>(aMessage.myData)))
			{
				if (!IsActive())
				{
					SetActive();
				}
			}
			else
			{
				if (IsActive())
				{
					SetToNormal();
				}
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
		const V2ui& source = *reinterpret_cast<const V2ui*>(aMessage.myData);
		myScreenSize =
			V2f{
				static_cast<float>(source.x),
				static_cast<float>(source.y)
		};
		SetupBounds();

	}
}

void Button::SetSpriteFactory(SpriteFactory* aSpriteFactory)
{
	ourSpriteFactoryPtr = aSpriteFactory;
}

void Button::RemoveAllSprites()
{
	Scene::GetInstance().RemoveFromScene(mySprites[ButtonState::Hover]);
	Scene::GetInstance().RemoveFromScene(mySprites[ButtonState::Normal]);
	Scene::GetInstance().RemoveFromScene(mySprites[ButtonState::Pressed]);
	Scene::GetInstance().RemoveFromScene(mySprites[ButtonState::Disabled]);
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
