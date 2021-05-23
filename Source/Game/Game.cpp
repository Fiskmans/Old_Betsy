#include "pch.h"
#include "Game.h"
#include "GameState.h"
#include "SpriteRenderer.h"
#include "InputManager.h"
#include "PostMaster.hpp"
#include <assert.h>
#include "MainMenuState.h"
#include <DirectX11Framework.h>

Game::Game() :
	myInputManager(nullptr)
{
}

Game::~Game()
{
	SAFE_DELETE(myInputManager);
}

bool Game::Init(WindowHandler* aWindowHandler, CommonUtilities::InputHandler* aInputHandler, LightLoader* aLightLoader, SpriteFactory* aSpriteFactory,
	DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myInputManager = new InputManager();
	myInputManager->Init(aInputHandler);
	myStateStack.Init();
	Message message;
	message.myMessageType = MessageType::PushState;

	MainMenuState* menu = new MainMenuState();
	if (menu->Init(myInputManager, aSpriteFactory, aLightLoader, aWindowHandler, aFramework, aAudioManager, aSpriteRenderer) == false)
	{
		//TODO: PROPER DELETE OF DATA
		delete menu;
		return false;
	}
	menu->SetMain(true);
	message.myData = menu;
	PostMaster::GetInstance()->SendMessages(message);

#if !SKIPSPLASHSCREEN
	message.myMessageType = MessageType::PushState;
	VideoState* video = new VideoState();
	if (video->Init(aModelLoader, aSpriteFactory, "Data\\Cinematics\\logos.mp4", false, aFramework->GetContext()) == false)
	{
		//TODO: PROPER DELETE OF DATA
		delete video;
		return false;
	}
	video->SetMain(true);
	message.myData = video;
	PostMaster::GetInstance()->SendMessages(message);
#endif // !SKIPSPLASHSCREEN

	return !!aInputHandler;
}


bool Game::Run(CGraphicsEngine* aEngine,float aDeltaTime)
{
	//while (ShowCursor(false) >= 0);
	{
		PERFORMANCETAG("Input");
		myInputManager->Update();
	}

	{
		PERFORMANCETAG("Update")
		myTooFastLimit += aDeltaTime;
		if (myTooFastLimit > 0.000001f)
		{
			myStateStack.Update(myTooFastLimit, 0);
			myTooFastLimit = 0.f;
		}
	}
	{
		PERFORMANCETAG("Render");
		myStateStack.Render(0, aEngine);
	}
	{
		PERFORMANCETAG("Message handling");
		myStateStack.HandleMessages();
	}
	return !myStateStack.IsEmpty();
}


