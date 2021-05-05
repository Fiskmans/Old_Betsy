#include "pch.h"
#include "Audio.h"
#include "AudioManager.h"
#include "Wwise_IDs.h"

#include "Entity.h"
#include "Camera.h"
#include "FollowCamera.h"

LMVector V3FTooLMV(V3F aVector)
{
	LMVector returnVec;

	returnVec.x = aVector.x;
	returnVec.y = aVector.y;
	returnVec.z = aVector.z;

	return returnVec;
}

V3F LMVTooV3F(LMVector aVector)
{
	V3F returnVec;

	returnVec.x = aVector.x;
	returnVec.y = aVector.y;
	returnVec.z = aVector.z;

	return returnVec;
}

Audio::Audio() :
	myEntity(nullptr),
	myAudioManager(nullptr)
{
}

Audio::~Audio()
{
	Reset();
}

void Audio::PreInit(AudioManager* aAudioManager)
{
	myAudioManager = aAudioManager;

	for (auto id : myEventIDs)
	{
		id = 0;
	}
}

void Audio::Init(Entity* aEntity)
{
	myEntity = aEntity;

	if (myAudioManager)
	{
		myAudioManager->RegisterGameObject(myEntity->GetEntityID());

		if (myEntity->GetEntityType() == EntityType::Camera)
		{
			myAudioManager->SetDefaultListener(myEntity->GetEntityID());
		}
	}
}

void Audio::Update(const float aDeltaTime)
{
	if (myAudioManager == nullptr)
	{
		return;
	}

	if (myEntity->GetEntityType() == EntityType::Camera)
	{
		Camera* camera = myEntity->GetComponent<FollowCamera>()->GetCamera();
		myAudioManager->UpdateObjectTransform(V3FTooLMV(camera->GetPosition()), V3FTooLMV(camera->GetForward()), V3FTooLMV(camera->GetUp()), myEntity->GetEntityID());
	}
	else
	{
		myAudioManager->UpdateObjectTransform(V3FTooLMV(myEntity->GetPosition()), V3FTooLMV(myEntity->GetForward()), V3FTooLMV(myEntity->GetUp()), myEntity->GetEntityID());
	}
}

void Audio::Reset()
{
	if (myAudioManager)
	{
		PostAudioEvent(AudioEvent::LoopStop);
		myAudioManager->UnRegisterGameObject(myEntity->GetEntityID());
	}
}

void Audio::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::StartDying)
	{
		//Stop all sounds??
	}
}

void Audio::InternalInit(const AudioComponentType& aType, bool aIsStatic)
{
	//InitAudioEngineIDs(aEmitterType);

	myAudioComponentType = aType;

	switch (aType)
	{
		case AudioComponentType::None:
		{
			ONETIMEWARNING("EmitterType None!", "This Component wont play any audio");
			return;
		}
		break;
		case AudioComponentType::Player:
		{
			myEventIDs[static_cast<int>(AudioEvent::Footstep)] = AK::EVENTS::PLAYER_FOOTSTEP;
			myEventIDs[static_cast<int>(AudioEvent::GettingWater)] = AK::EVENTS::GETTING_WATER;
		}
		break;
		case AudioComponentType::GrowSpot:
		{
			myEventIDs[static_cast<int>(AudioEvent::Plow)] = AK::EVENTS::PLOW_GROUND;
			myEventIDs[static_cast<int>(AudioEvent::Plant)] = AK::EVENTS::PLANT_PLANT;
			myEventIDs[static_cast<int>(AudioEvent::Water)] = AK::EVENTS::WATER_PLANT;
			myEventIDs[static_cast<int>(AudioEvent::HarvestPlant)] = AK::EVENTS::HARVEST_PLANT;
		}
		break;
		case AudioComponentType::River:
		{
			myEventIDs[static_cast<int>(AudioEvent::LoopPlay)] = AK::EVENTS::RIVER_PLAY;
			myEventIDs[static_cast<int>(AudioEvent::LoopStop)] = AK::EVENTS::RIVER_STOP;
		}
		break;
		case AudioComponentType::FirePlace:
		{
			myEventIDs[static_cast<int>(AudioEvent::LoopPlay)] = AK::EVENTS::FIREPLACE_START;
			myEventIDs[static_cast<int>(AudioEvent::LoopStop)] = AK::EVENTS::FIREPLACE_STOP;
		}
		break;
		case AudioComponentType::Hen:
		{
			myEventIDs[static_cast<int>(AudioEvent::AnimalSound)] = AK::EVENTS::HEN_SOUND;
		}
		break;
		case AudioComponentType::Sheep:
		{
			myEventIDs[static_cast<int>(AudioEvent::AnimalSound)] = AK::EVENTS::SHEEP_SOUND;
		}
		break;
	}
}

void Audio::PostAudioEvent(AudioEvent aAudioEvent)
{
#if USEAUDIO
	int eventIndex = static_cast<int>(aAudioEvent);

	if (myEventIDs[eventIndex] != 0)
	{
		myAudioManager->PostEvent(myEventIDs[eventIndex], myEntity->GetEntityID());
	}
#endif
}

void Audio::OnAttach()
{
}

void Audio::OnDetach()
{
}

void Audio::OnKillMe()
{
}