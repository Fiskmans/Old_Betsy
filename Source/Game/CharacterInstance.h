#pragma once
#include "Component.h"
#include "CharacterData.h"
#include "Publisher.hpp"

class AbilityData;

class CharacterInstance : public Publisher, public Component
#ifndef  _RETAIL
	, public Observer
#endif 

{
public:
	CharacterInstance();
	virtual ~CharacterInstance();

	virtual void Init(Entity* aEntity) override;
	void Init(Entity* aEntity, Stats* aCharacterStats);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	Stats* GetCharacterStats();
	const Stats* GetCharacterStats() const;
	Entity* GetParentEntity();

	void RecieveMessage(const Message& aMessage)
#ifndef  _RETAIL
		override
#endif
;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	Stats* myStats;
};