#pragma once
#include "Component.h"
#include "AbilityData.h"
#include "CharacterData.h"
#include <vector>

enum class AbilityColliderType
{
	Box,
	Sphere,
	Cone,
	Capsule,
	None,
	Count
};

enum class AbilityBehaviour
{
	Follow,
	Rotate,
	None,
	Count
};

class AbilityInstance :
	public Component
{
public:
	AbilityInstance();
	virtual ~AbilityInstance();

	virtual void Init(Entity* aEntity) override;
	void Init(Entity* aEntity, Ability* aAbility, Entity* aCastingEntity, Stats* aStats);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	bool CanDamageEntity(Entity* aEntity);
	int GetAbilityID();
	int GetRageCost();
	Ability* GetAbility();
	Entity* GetCastingEntity();
	
	void SetIsColliderActive(bool aVal);
	bool GetIsColliderActive();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	Entity* myCastingEntity;
	Ability myAbility;
	AbilityColliderType myColliderType;

	float myTickRateTimer;
	float myCastPointTimer = 0.1f;
	std::vector<int> myDamagedEntities;
	bool myIsColliderActive;
};

