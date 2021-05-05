#include "pch.h"
#include "PauseState.h"
#include "InputHandler.h"
#include "PostMaster.hpp"
#include "GraphicEngine.h"
#include "SpriteInstance.h"
#include <Xinput.h>
#include "AudioManager.h"
#include "MenuSlider.h"

PauseState::PauseState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop)
{
	myDrawThrough = true;
	myMousePointer = nullptr;
	myFadeBackgroundSprite = nullptr;
	myFadeSprite = nullptr;
	myUpdateThrough = false;
	myIsMain = false;
}

PauseState::~PauseState()
{
	SAFE_DELETE(myFadeSprite);
	SAFE_DELETE(myMousePointer);
}

void PauseState::Update(const float aDeltaTime)
{
	if (myResumeButton.IsPressed())
	{
		PostMaster::GetInstance()->SendMessages(MessageType::InputUnPauseHit);

		Message msg;
		msg.myMessageType = MessageType::PopState;
		msg.myBool = false;
		PostMaster::GetInstance()->SendMessages(msg);

	}

	if (myMainMenuButton.IsPressed())
	{
		PostMaster::GetInstance()->SendMessages(MessageType::InputUnPauseHit);

		Message msg;
		msg.myMessageType = MessageType::PopState;
		msg.myBool = true;
		PostMaster::GetInstance()->SendMessages(msg);
	}
}

bool PauseState::Init()
{
	V2F pos = { 0.5, 0.4f };
	myResumeButton.Init("Data/UI/UIButtons", "Resume", pos, V2F(0.545f, 1.f));
	pos.y += 0.1f;
	myMainMenuButton.Init("Data/UI/UIButtons", "Main Menu", pos, V2F(0.545f, 1.f));

	SpriteFactory* spriteFactory = myResumeButton.GetSpriteFactory();

	myFadeBackgroundSprite = spriteFactory->CreateSprite("Data/UI/fadeScreen.dds");
	myFadeBackgroundSprite->SetScale(V2F(1000.0f, 1000.0f));
	myFadeBackgroundSprite->SetPivot(V2F(0.5f, 0.5f));
	myFadeBackgroundSprite->SetPosition(V2F(0.5f, 0.5f));
	myFadeBackgroundSprite->SetColor(V4F(1.0f, 1.0f, 1.0f, 0.7f));
	myFadeSprite = spriteFactory->CreateSprite("Data/UI/UIButtons/Pause Menu Background.dds");
	//myFadeSprite->SetUVMinMaxInTexels(V2F(0, 0), V2F(1920, 1080));
	//myFadeSprite->SetSize(V2F(1, 1));
	myFadeSprite->SetPivot(V2F(0.5f, 0.0f));
	myFadeSprite->SetPosition(0.5f, 0.0f);



	myMousePointer = spriteFactory->CreateSprite("Data/UI/mouse.dds");

	myMasterVolumeSlider = new MenuSlider();
	pos.y += 0.1f;
	myMasterVolumeSlider->Init(spriteFactory, pos, 100, MessageType::SetMasterVolume);

	SubscribeToMessage(MessageType::CurrentMasterVolume);

	return true;
}

void PauseState::Render(CGraphicsEngine* aGraphicsEngine)
{
	std::vector<SpriteInstance*> sprites;

	sprites.push_back(myFadeBackgroundSprite);
	sprites.push_back(myFadeSprite);
	sprites.push_back(myMainMenuButton.GetCurrentSprite());
	sprites.push_back(myResumeButton.GetCurrentSprite());
	sprites.push_back(myMasterVolumeSlider->GetBackgroundSprite());
	sprites.push_back(myMasterVolumeSlider->GetSliderSprite());
	sprites.push_back(myMasterVolumeSlider->GetForegroundSprite());
	sprites.push_back(myMousePointer);

	aGraphicsEngine->RenderMovie(sprites);
}

void PauseState::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		myMousePointer->SetPosition(CommonUtilities::Vector2<float>(aMessage.myFloatValue, aMessage.myFloatValue2));
	}
	else if (aMessage.myMessageType == MessageType::CurrentMasterVolume)
	{
		myMasterVolumeSlider->SetCurrentValue(aMessage.myIntValue);
	}
}

void PauseState::Activate()
{
	myResumeButton.Subscribe();
	myMainMenuButton.Subscribe();

	PostMaster::GetInstance()->Subscribe(MessageType::InputMouseMoved, this);
}

void PauseState::Deactivate()
{
	myResumeButton.Unsubscribe();
	myMainMenuButton.Unsubscribe();

	PostMaster::GetInstance()->UnSubscribe(MessageType::InputMouseMoved, this);
	UnSubscribeToMessage(MessageType::CurrentMasterVolume);
}

void PauseState::Unload()
{
}