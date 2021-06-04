#include "pch.h"
#include "AudioManager.h"
#include <AK/SoundEngine/Common/AkTypes.h>
#include "WwiseFramework.h"
#include "Wwise_IDs.h"
#include "../Precompiled/Macros.h"

#define BANKNAME_INIT "Init.bnk"
#define BANKNAME_SFX "InGame.bnk"
#define BANKNAME_MUSIC "Music.bnk"

AudioManager::AudioManager():
	myWwiseFramework(nullptr),
	myListenerID(NULL),
	Observer({
			MessageType::StartLoading,
			MessageType::UnloadLevel,
			MessageType::InputPauseHit,
			MessageType::SetMasterVolume,
			MessageType::StartInGameAudio,
			MessageType::MenuButtonActive,
			MessageType::MenuButtonHit,
			MessageType::DialogueStarted,
			MessageType::ChangedItem,
			MessageType::NewDay,
			MessageType::UpdateTime,
			MessageType::RespawnTrader,
			MessageType::PlayIntro,
			MessageType::SpawnItem,
			MessageType::MainMenuStateActivated
		})
{
}

AudioManager::~AudioManager()
{
}

void AudioManager::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::StartInGameAudio:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::INTRO_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_MORNING_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::MUSIC1_STOP, my2DMasterObjectID);

		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_MORNING_PLAY, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_PLAY, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::MUSIC1_PLAY, my2DMasterObjectID);
	}
	break;
	case MessageType::UnloadLevel:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_STOP, my2DMasterObjectID);
	}
	break;
	case MessageType::InputPauseHit:
	{
		PostMaster::GetInstance().SendMessages(MessageType::CurrentMasterVolume,&myMasterVolume);
	}
	break;
	case MessageType::SetMasterVolume:
	{
		SetMasterVolume(*reinterpret_cast<const int*>(aMessage.myData));
	}
	break;
	case MessageType::MenuButtonActive:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::BUTTON_HOVER, my2DMasterObjectID);
	}
	break;
	case MessageType::MenuButtonHit:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::BUTTON_KLICK, my2DMasterObjectID);
	}
	break;
	case MessageType::DialogueStarted:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::INTERACT, my2DMasterObjectID);
	}
	break;
	case MessageType::ChangedItem:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::CHANGE_ITEM, my2DMasterObjectID);
	}
	break;
	case MessageType::NewDay:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_NIGHT_STOP, my2DMasterObjectID);

		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_MORNING_PLAY, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_PLAY, my2DMasterObjectID);

		myWwiseFramework->PostEvent(AK::EVENTS::MUSIC1_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::MUSIC1_PLAY, my2DMasterObjectID);

		myDayTime = 0;
	}
	break;
	case MessageType::UpdateTime:
	{
		const MessageStructs::UpdateTimeData& data = *reinterpret_cast<const MessageStructs::UpdateTimeData*>(aMessage.myData);
		if (data.myHour == 19 && myDayTime != 19)
		{
			myDayTime = 19;

			myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_STOP, my2DMasterObjectID);
			myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_NIGHT_STOP, my2DMasterObjectID);

			myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_NIGHT_PLAY, my2DMasterObjectID);
			myWwiseFramework->PostEvent(AK::EVENTS::MUSIC1_STOP, my2DMasterObjectID);
		}
	}
	break;
	case MessageType::RespawnTrader:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::TRADER_ENTERED, my2DMasterObjectID);
	}
	break;
	case MessageType::PlayIntro:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::INTRO_PLAY, my2DMasterObjectID);
	}
	break;
	case MessageType::SpawnItem:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::ADD_ITEM, my2DMasterObjectID);
	}
	break;
	case MessageType::MainMenuStateActivated:
	{
		myWwiseFramework->PostEvent(AK::EVENTS::INTRO_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_MORNING_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_WIND_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::MUSIC1_STOP, my2DMasterObjectID);
		myWwiseFramework->PostEvent(AK::EVENTS::AMBIENCE_NIGHT_STOP, my2DMasterObjectID);
	}
	break;
	}
}

void AudioManager::Init()
{
	myWwiseFramework = new WwiseFramework();
	myWwiseFramework->Init();

	myWwiseFramework->LoadBank(BANKNAME_INIT);
	myWwiseFramework->LoadBank(BANKNAME_SFX);
	myWwiseFramework->LoadBank(BANKNAME_MUSIC);
}

void AudioManager::ShutDown()
{	
	myWwiseFramework->Terminate();
}

void AudioManager::Update(float aDeltaTime)
{
	myWwiseFramework->ProcessAudio();
}

void AudioManager::SetMasterVolume(float aVolume)
{
	//TODO: implement better way to set master bus volume
	myMasterVolume = aVolume;
	myWwiseFramework->SetGameParameterValue(AK::GAME_PARAMETERS::MASTER_VOLUME, aVolume,  AK_INVALID_GAME_OBJECT, 0.5f);
}

void AudioManager::PostEvent(const AudioEventID& aEvent, const AudioObjectID& aObjectID)
{
	if (!USEAUDIO)
	{
		return;
	}

	if (aObjectID == -1)
	{
		myWwiseFramework->PostEvent(aEvent, my2DMasterObjectID);
	}
	else
	{
		myWwiseFramework->PostEvent(aEvent, aObjectID);
	}
}

void AudioManager::SetGameParameterValue(const AudioGameParameterID& aParameter, const float& aValue, const AudioObjectID& aObjectID, const int& aTimeMS)
{
	myWwiseFramework->SetGameParameterValue(aParameter, aValue, aObjectID, aTimeMS);
}

void AudioManager::RegisterGameObject(const int& aObjectID)
{
	myWwiseFramework->RegisterObject(aObjectID);
}

void AudioManager::UnRegisterGameObject(const int& aObjectID)
{
	myWwiseFramework->UnRegisterObject(aObjectID);
}

void AudioManager::Register2DMasterObject(const AudioObjectID& anID)
{
	my2DMasterObjectID = anID;
	myWwiseFramework->RegisterObject(my2DMasterObjectID);
}

void AudioManager::SetDefaultListener(const int& aObjectID)
{
	myWwiseFramework->SetDefaultListener(aObjectID);
	myListenerID = aObjectID;
}

LMVector AudioManager::GetObjectPosition(const AudioObjectID& aObjectID)
{
	AkTransform transform = myWwiseFramework->GetObjectPosition(aObjectID);
	
	LMVector pos;
	pos.x = transform.Position().X;
	pos.y = transform.Position().Y;
	pos.z = transform.Position().Z;

	return pos;
}

LMVector AudioManager::GetListenerPosition()
{
	return GetObjectPosition(myListenerID);
}

int AudioManager::GetListenerID()
{
	return myListenerID;
}

void AudioManager::UpdateObjectTransform(const LMVector& aPosition, const LMVector& aForward, const LMVector& aTop, const int& aObjectID)
{
	AkGameObjectID objectID = aObjectID;

	AkVector position = {aPosition.x, aPosition.y, aPosition.z};
	AkVector forward = { aForward.x, aForward.y, aForward.z };
	AkVector top = { aTop.x, aTop.y, aTop.z };

	AkTransform transform;
	transform.Set(position, forward, top);

	myWwiseFramework->SetObjectTransform(transform, objectID);
}