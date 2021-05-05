#include "pch.h"
#include "VideoState.h"
#include "Video.h"
#include "ModelLoader.h"
#include "GraphicEngine.h"
#include "SpriteRenderer.h"
#include <Xinput.h>

VideoState::VideoState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop)
{
	myVideo = nullptr;
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
}

VideoState::~VideoState()
{
	Deactivate();
}

void VideoState::Update(const float aDeltaTime)
{
	if (myVideo != nullptr)
	{	
		myVideo->Update(myVideoInitData.myDeviceContex,aDeltaTime);
		if (myVideo->HasVideoEnded() == true)
		{
			Destroy(true);
		}
	}
}

void VideoState::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputAccept)
	{
		Destroy(true);
	}
	/*if (aMessage.myMessageType == MessageType::InputControllerButtonHit)
	{
		if (aMessage.aIntValue == XINPUT_GAMEPAD_START)
		{
			Destroy(true);
		}
		else if (aMessage.aIntValue == XINPUT_GAMEPAD_A)
		{
			Destroy(true);
		}
	}*/
}

bool VideoState::Init(ModelLoader* aModelLoader, SpriteFactory* aSpritefactory, const char* aPath, bool aShouldLoop, ID3D11DeviceContext* aDeviceContext)
{
	myVideoInitData.myDeviceContex = aDeviceContext;
	myVideoInitData.myModelLoader = aModelLoader;
	myVideoInitData.myPath = aPath;
	myVideoInitData.myShouldLoop = aShouldLoop;
	myVideoInitData.mySpritefactory = aSpritefactory;
	
	return true;
}

void VideoState::StartVideo()
{
	if (myVideo == nullptr)
	{
		myVideo = new Video();
		myVideo->Init(myVideoInitData.myPath, myVideoInitData.myModelLoader->myDevice, myVideoInitData.mySpritefactory);
		myVideo->Play(myVideoInitData.myShouldLoop);
	}
}

void VideoState::Render(CGraphicsEngine* aGraphicsEngine)
{
	if (myVideo != nullptr)
	{
		std::vector<SpriteInstance*> sprites;
		sprites.push_back(myVideo->GetSpriteInstance());
		aGraphicsEngine->RenderMovie(sprites);
	}

}

void VideoState::Activate()
{
	PostMaster::GetInstance()->Subscribe(MessageType::InputAccept, this);

	Destroy(false);
	StartVideo();
}

void VideoState::Deactivate()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::InputAccept, this);
	Destroy(false);
}

void VideoState::Unload()
{
}

void VideoState::SetVideoType(VideoType aType)
{
	myVideoType = aType;
}

void VideoState::Destroy(bool aShouldPop)
{
	if (myVideo != nullptr)
	{
		myVideo->Stop();
		myVideo->Destroy();
		delete myVideo;
		myVideo = nullptr;
	}
	if (aShouldPop == true)
	{
		Message message;
		message.myMessageType = MessageType::PopState;
		message.myBool = true;
		Publisher::SendMessages(message);
	}
}
