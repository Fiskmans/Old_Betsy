#include "pch.h"
#include "AcceptInteraction.h"
#include "Button.h"
#include "SpriteFactory.h"
#include "TextFactory.h"
#include "TextInstance.h"
#include "AssetManager.h"
#include "GamlaBettan\Scene.h"

AcceptInteraction::AcceptInteraction():
	myInteractionActive(false)
{
	myAcceptButton = nullptr;
	myCancelButton = nullptr;
}

AcceptInteraction::~AcceptInteraction()
{
}


bool AcceptInteraction::UpdateAcceptScreen()
{
	if (myInteractionActive)
	{
		if (myAcceptButton && myAcceptButton->IsPressed())
		{
			EndInteraction();

			return true;
		}
		else if (myCancelButton && myCancelButton->IsPressed())
		{
			EndInteraction();

			return false;
		}
	}

	return false;
}

void AcceptInteraction::SetScenePtr(Scene* aScene)
{
	myScene = aScene;
}

void AcceptInteraction::SetTextFactory(TextFactory* aTextFactory)
{
	myTextFactory = aTextFactory;
}

void AcceptInteraction::InitializeInteraction(std::string aTextToDisplay, bool aAcceptOption, bool aCancelOption)
{

	if (!myScene)
	{
		assert(false);
	}
	else
	{
		myInteractionActive = true;

		if (aAcceptOption)
		{
			myAcceptButton = new Button();
			myAcceptButton->Init("Data/UI/UIButtons/Sleep_Normal.dds", "Data/UI/UIButtons/Sleep_Hover.dds", "Data/UI/UIButtons/Sleep_Pressed.dds", "Data/UI/UIButtons/Sleep_Disabled.dds", V2F(0.5f, 0.3f));
			myAcceptButton->SetScenePtr(myScene);

			myScene->AddSprite(myAcceptButton->GetCurrentSprite());
		}
		if (aCancelOption)
		{
			myCancelButton = new Button();
			myCancelButton->Init("Data/UI/UIButtons/Close_Normal.dds", "Data/UI/UIButtons/Close_Hover.dds", "Data/UI/UIButtons/Close_Pressed.dds", "Data/UI/UIButtons/Close_Disabled.dds", V2F(0.5f, 0.35f));
			myCancelButton->SetScenePtr(myScene);

			myScene->AddSprite(myCancelButton->GetCurrentSprite());
		}

		myText = AssetManager::GetInstance().GetFont("MoonviewDialogue.spritefont").InstansiateText();
		myText->SetText(aTextToDisplay);
		myText->SetPivot({0.5, 0.5});
		myText->SetPosition({0.5, 0.2});

		myScene->AddText(myText);

		Message message;
		message.myMessageType = MessageType::LockPlayer;
		SendMessages(message);
	}
}

void AcceptInteraction::EndInteraction()
{
	myInteractionActive = false;

	if (myAcceptButton)
	{
		myScene->RemoveSprite(myAcceptButton->GetCurrentSprite());
		delete myAcceptButton;
		myAcceptButton = nullptr;
	}
	if (myCancelButton)
	{
		myScene->RemoveSprite(myCancelButton->GetCurrentSprite());
		delete myCancelButton;
		myCancelButton = nullptr;
	}

	myScene->RemoveText(myText);
	delete myText;
	myText = nullptr;

	Message message;
	message.myMessageType = MessageType::UnlockPlayer;
	SendMessages(message);
}