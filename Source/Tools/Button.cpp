#include <pch.h>
#include "Button.h"
#include "GamlaBettan\Scene.h"
#include "GamlaBettan\SpriteInstance.h"
#include "GamlaBettan\SpriteFactory.h"
#include "PostMaster.hpp"

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
	myBounds = { 0,0,0,0 };
	myPosition = { 0,0 };
	myScreenSize = { 1600.f, 900.f };
	myIsEnabled = true;
}

Button::~Button()
{
	if (myIsEnabled)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
	}
}

void Button::SetOnPressedFunction(const std::function<void(void)> aOnPressed)
{
	myOnPressed = aOnPressed;
}

void Button::TriggerOnPressed()
{
	if (myOnPressed)
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

void Button::SetHovered()
{
	if (myState != ButtonState::Hover)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Hover)]);
	}
	myState = ButtonState::Hover;
}

void Button::SetPressed()
{
	if (myState != ButtonState::Pressed)
	{
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
		Scene::GetInstance().AddToScene(mySprites[CAST(int, ButtonState::Pressed)]);
	}
	myState = ButtonState::Pressed;
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

bool Button::Init(const std::string& aFolderPath, const std::string& aButtonName, const CommonUtilities::Vector2<float>& aPosition,
	const V2f& aHitBoxSize, SpriteFactory* aSpriteFactory)
{
	myPosition = aPosition;

	std::string path = aFolderPath + "\\" + aButtonName;
	mySprites[ButtonState::Normal] = aSpriteFactory->CreateSprite(path + " button off.dds");
	mySprites[ButtonState::Normal]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Normal]->SetPosition(myPosition);
	mySprites[ButtonState::Normal]->SetDepth(0.5f);
	Scene::GetInstance().AddToScene(mySprites[ButtonState::Normal]);

	mySprites[ButtonState::Hover] = aSpriteFactory->CreateSprite(path + " button hover.dds");
	mySprites[ButtonState::Hover]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Hover]->SetPosition(myPosition);
	mySprites[ButtonState::Hover]->SetDepth(0.5f);

	mySprites[ButtonState::Pressed] = aSpriteFactory->CreateSprite(path + " button on.dds");
	mySprites[ButtonState::Pressed]->SetPivot(V2f(0.5f, 0.5f));
	mySprites[ButtonState::Pressed]->SetPosition(myPosition);
	mySprites[ButtonState::Pressed]->SetDepth(0.5f);

	mySize = aHitBoxSize;

	SetupBounds();

	return true;
}

bool Button::IsHovered()
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

void Button::Disable()
{
	if (myIsEnabled)
	{
		myIsEnabled = false;
		Scene::GetInstance().RemoveFromScene(mySprites[CAST(int, myState)]);
	}
}

void Button::Enable()
{
	if(!myIsEnabled);
	{
		myIsEnabled = true;
		Scene::GetInstance().AddToScene(mySprites[CAST(int, myState)]);
	}
}

void Button::RecieveMessage(const Message& aMessage)
{
	if (!myIsEnabled)
	{
		return;
	}

	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		if (!IsPressed())
		{
			if (CheckHover(*reinterpret_cast<const V2f*>(aMessage.myData)))
			{
				SetHovered();
			}
			else
			{
				SetToNormal();
			}
		}

	}
	else if (aMessage.myMessageType == MessageType::InputLeftMouseReleased)
	{
		PERFORMANCETAG("MouseReleasedInButton");
		if (IsPressed())
		{
			SetHovered();
			TriggerOnPressed();
		}
	}
	else if (aMessage.myMessageType == MessageType::InputLeftMouseHit)
	{
		PERFORMANCETAG("MouseHitInButton");
		if (IsHovered())
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
void Button::SetupBounds()
{
	CommonUtilities::Vector2<float> imageSize;

	imageSize = mySprites[ButtonState::Normal]->GetSizeOnScreen();

	myBounds.x = myPosition.x - (imageSize.x * mySize.x) / 2;
	myBounds.y = myPosition.y - (imageSize.y * mySize.y) / 2;
	myBounds.z = myPosition.x + (imageSize.x * mySize.x) / 2;
	myBounds.w = myPosition.y + (imageSize.y * mySize.y) / 2;
}
