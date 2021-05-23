#include "pch.h"
#include "MainMenuState.h"
#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include "video.h"
#include <Xinput.h>
#include "OptionState.h"
#include "LevelSelectState.h"
#include "AssetManager.h"

template<typename>
struct array_size;
template<typename T, size_t N>
struct array_size<std::array<T, N> > {
	static size_t const size = N;
};
#define SIZEOFARRAY(arg) array_size<decltype(arg)>::size


MainMenuState::MainMenuState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	myStateInitData{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }
{
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myIntroHasPlayed = false;
	myGameTitleImage = nullptr;
	myMousePointer = nullptr;
	myShouldRemoveVideo = true;
}

MainMenuState::~MainMenuState()
{
	Deactivate();

	delete myMousePointer;
	delete myGameTitleImage;

	WIPE(*this);
}

void MainMenuState::Update(const float aDeltaTime)
{
	if (myGameStateToStart)
	{
		myGameStateToStart->PreSetup(aDeltaTime);
	}

#if DIRECTTOGAME
	static bool first = true;

	if (first)
	{
		myPlayButton.TriggerOnPressed();
		first = false;
	}
#endif
}

void MainMenuState::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		myMousePointer->SetPosition(CommonUtilities::Vector2<float>(aMessage.myFloatValue, aMessage.myFloatValue2));
	}
}

bool MainMenuState::Init(InputManager* aInputManager, SpriteFactory* aSpritefactory,
	LightLoader* aLightLoader, WindowHandler* aWindowHandler, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myIsMain = true;

	if (!myMousePointer)
	{
		myMousePointer = aSpritefactory->CreateSprite("ui/mouse.dds");
	}

	myStateInitData.myFrameWork = aFramework;
	myStateInitData.myInputManager = aInputManager;
	myStateInitData.myLightLoader = aLightLoader;
	myStateInitData.mySpriteFactory = aSpritefactory;
	myStateInitData.myWindowHandler = aWindowHandler;
	myStateInitData.myAudioManager = aAudioManager;
	myStateInitData.mySpriteRenderer = aSpriteRenderer;

	InitLayout(aSpritefactory);

	return true;
}

void MainMenuState::Render(CGraphicsEngine* aGraphicsEngine)
{
	std::vector<SpriteInstance*> sprites;
	sprites.push_back(myGameTitleImage);

	sprites.push_back(myPlayButton.GetCurrentSprite());
	sprites.push_back(myExitButton.GetCurrentSprite());
	sprites.push_back(myCreditButton.GetCurrentSprite());
	//sprites.push_back(myOptionsButton.GetCurrentSprite());

	sprites.push_back(myMousePointer);

	aGraphicsEngine->RenderMovie(sprites);
}

void MainMenuState::Activate()
{
	PostMaster::GetInstance()->Subscribe(MessageType::InputMouseMoved, this);
	myExitButton.Subscribe();
	myPlayButton.Subscribe();
	myCreditButton.Subscribe();
	//myOptionsButton.Subscribe();

	myIsActive = true;
	myShouldRemoveVideo = true;

	Message message;
	message.myMessageType = MessageType::MainMenuStateActivated;
	SendMessages(message);
}

void MainMenuState::Deactivate()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::InputMouseMoved, this);
	myExitButton.Unsubscribe();
	myPlayButton.Unsubscribe();
	myCreditButton.Unsubscribe();
	//myOptionsButton.Unsubscribe();
	myIsActive = false;
}

void MainMenuState::Unload()
{
}

void MainMenuState::InitLayout(SpriteFactory* aSpritefactory)
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("menu/MainMenuLayout.json").GetAsJSON();

	std::string imagesPath = root["ImagesPath"].Get<std::string>();

	myGameTitleImage = aSpritefactory->CreateSprite(imagesPath + "\\" + root["GameTitleImage"]["name"].Get<std::string>());
	myGameTitleImage->SetPosition({ root["GameTitleImage"]["PosX"].Get<float>(),  root["GameTitleImage"]["PosY"].Get<float>() });
	myGameTitleImage->SetUVMinMaxInTexels(V2F(0, 0), V2F(1920.f, 1080.f));
	myGameTitleImage->SetSize(V2F(1, 1));


	myPlayButton.Init(imagesPath, root["StartButton"]["name"].Get<std::string>(), { root["StartButton"]["PosX"].Get<float>(),  root["StartButton"]["PosY"].Get<float>() }, V2F(0.545f, 1.f), aSpritefactory);
	myExitButton.Init(imagesPath, root["ExitButton"]["name"].Get<std::string>(), { root["ExitButton"]["PosX"].Get<float>(),  root["ExitButton"]["PosY"].Get<float>() }, V2F(0.545f, 1.f), aSpritefactory);
	myCreditButton.Init(imagesPath, root["CreditsButton"]["name"].Get<std::string>(), { root["CreditsButton"]["PosX"].Get<float>(),  root["CreditsButton"]["PosY"].Get<float>() }, V2F(0.545f, 1.f), aSpritefactory);
	//myOptionsButton.Init(imagesPath, mainMenuDoc["OptionsButton"]["name"].GetString(), { mainMenuDoc["OptionsButton"]["PosX"].GetFloat(),  mainMenuDoc["OptionsButton"]["PosY"].GetFloat() }, V2F(0.545f, 1.f), aSpritefactory);

	
	if (!myGameStateToStart)
	{
		myGameStateToStart = CreateGameState(0);
	}

	myPlayButton.SetOnPressedFunction([this]() -> void
	{
		Message message;
		message.myMessageType = MessageType::PushState;
		if (myGameStateToStart)
		{
			message.myData = myGameStateToStart;
			myGameStateToStart = nullptr;
		}
		else
		{
			message.myData = CreateGameState(0);
		}

		Publisher::SendMessages(message);
#if PLAYINTRO

		message.myMessageType = MessageType::PushState;
		VideoState* video = new VideoState();
		if (video->Init(myStateInitData.myModelLoader, myStateInitData.mySpriteFactory, "Data\\Cinematics\\Intro.mp4", false, myStateInitData.myFrameWork->GetContext()) == false)
		{
			//TODO: PROPER DELETE OF DATA
			delete video;
			return;
		}
		video->SetMain(true);
		message.myData = video;
		Publisher::SendMessages(message);

		Publisher::SendMessages(MessageType::PlayIntro);

		myIntroHasPlayed = true;
#endif
	});

	myExitButton.SetOnPressedFunction([this]() -> void
	{
		Message message;
		message.myMessageType = MessageType::PopState;
		message.myBool = true;
		Publisher::SendMessages(message);
	});
}

GameState* MainMenuState::CreateGameState(const int& aStartLevel)
{
	GameState* state = new GameState();

	if (state->Init(myStateInitData.myWindowHandler, myStateInitData.myInputManager, 
		myStateInitData.mySpriteFactory, myStateInitData.myLightLoader, myStateInitData.myFrameWork, myStateInitData.myAudioManager, myStateInitData.mySpriteRenderer) == false)
	{
		//TODO: PROPER DELETE OF DATA
		delete state;
		return nullptr;
	}
	state->SetMain(true);
	state->LoadLevel(aStartLevel);

	return state;
}
