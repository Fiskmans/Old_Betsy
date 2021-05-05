#include "pch.h"
#include "InteractImage.h"
#include "SpriteFactory.h"
#include "Scene.h"
#include "SpriteInstance.h"

void InteractImage::PreInit(Scene* aScene, SpriteFactory* aFactory)
{
	myScene = aScene;
	myFactory = aFactory;
}

void InteractImage::SetImagePath(const std::string& aFilePath)
{
	myFilePath = aFilePath;
}

void InteractImage::InteractWith(Item* aHeldItem)
{
	Activate();
}

void InteractImage::Init(Entity* aEntity)
{
	myEntity = aEntity;
	SubscribeToMyMessages();
	myIsActive = false;
}

void InteractImage::Update(const float aDeltaTime)
{
}

void InteractImage::Reset()
{
	UnSubscribeToMyMessages();
	if (myIsActive)
	{
		Deactivate();
	}
}

void InteractImage::Activate()
{
	SubscribeToMessage(MessageType::InputLeftMouseHit);
	myIsActive = true;
	mySprite = myFactory->CreateSprite(myFilePath);
	mySprite->SetPivot(V2F(0.5f, 0.5f));
	mySprite->SetPosition(V2F(0.5f, 0.5f));
	myScene->AddSprite(mySprite);
	PostMaster::GetInstance()->SendMessages(MessageType::LockPlayer);
}

void InteractImage::Deactivate()
{
	UnSubscribeToMessage(MessageType::InputLeftMouseHit);
	myIsActive = false;
	myScene->RemoveSprite(mySprite);
	SAFE_DELETE(mySprite);
	PostMaster::GetInstance()->SendMessages(MessageType::UnlockPlayer);
}


void InteractImage::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::InputLeftMouseHit:
		if (myIsActive)
		{
			Deactivate();
		}
		break;
	default:
		break;
	}
}
