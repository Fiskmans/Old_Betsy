#pragma once

#include "Component.h"
#include <vector>
#include <array>
#include "Observer.hpp"

class AudioManager;
class Entity;

typedef unsigned long AudioEventID;

enum class AudioEvent
{
	Plow,
	Plant,
	Water,
	HarvestPlant,
	AnimalSound,
	GettingWater,

	LoopPlay,
	LoopStop,

	Footstep,

	Hello,
	Goodbye,

	Count
};

enum class AudioComponentType
{
	None,
	Listener,
	Player,
	GrowSpot,
	River,
	FirePlace,
	Hen,
	Sheep,
	Trader,
};

class Audio : public Component
{
public:

	Audio();
	~Audio();

	void PreInit(AudioManager* aAudioManager/*, unsigned int aEntityID*/);
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void InternalInit(const AudioComponentType& aEmitterType, bool aIsStatic = false);

	void PostAudioEvent(AudioEvent aAudioEvent);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:

	AudioManager* myAudioManager;
	Entity* myEntity;

	std::array<AudioEventID, static_cast<int>(AudioEvent::Count)> myEventIDs;
	AudioComponentType myAudioComponentType;
};