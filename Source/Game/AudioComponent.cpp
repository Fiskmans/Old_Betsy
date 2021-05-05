#include "pch.pch"
#include "Audio.h"
#include "AudioInstance.h"
#include "AudioManager.h"
#include "Entity.h"

Audio::Audio()
{
}

Audio::~Audio()
{
}

void Audio::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void Audio::Update(const float aDeltaTime)
{
	myAudioPosition = myEntity->GetPosition();
}

void Audio::Reset()
{
}

void Audio::PlayAudio(std::string aSound, bool isOneShot)
{
	myAudioPosition = myEntity->GetPosition();

	if (isOneShot)
	{
		AudioManager::GetInstance()->PlaySound(aSound, myAudioPosition);
		return;
	}
}
