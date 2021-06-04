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

bool Game::Init(CommonUtilities::InputHandler* aInputHandler, LightLoader* aLightLoader, SpriteFactory* aSpriteFactory,
	DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myInputManager = new InputManager();
	myInputManager->Init(aInputHandler);
	myStateStack.Init();

	MainMenuState* menu = new MainMenuState();
	if (menu->Init(myInputManager, aSpriteFactory, aLightLoader, aFramework, aAudioManager, aSpriteRenderer) == false)
	{
		delete menu;
		return false;
	}
	menu->SetMain(true);

	PostMaster::GetInstance().SendMessages(MessageType::PushState, menu);

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
		if (myTooFastLimit > 0.0001f)
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


