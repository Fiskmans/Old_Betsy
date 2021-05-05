#pragma once
#include "Component.h"
#include "Macros.h"
#include "MetricValue.h"
#include "Publisher.hpp"

class Life : public Component, public Publisher
{
public:
	Life();
	virtual ~Life();

	virtual void Init(Entity* aEntity) override;
	virtual void Init(Entity* aEntity, int aLifeAmount);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	virtual void Collided(int aDamageValue) override;

	void TakeDamage(int aAmount, V3F aDirection = V3F());
	void HealInPercentage(int aHealPercentage);
	void MakeInvinciable(float aTime);
	void SetNewMaxLife(int aLife);

	float GetLifePercentage() const;

	const int& GetLife() const;
	const int& GetMaxLife() const;
#if USEIMGUI
	static bool ourShowHealthBar;
#endif

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	float myRecoveryTimer;
	const static int ourDeaultMaxLife;
	int myMaxLife;
	int myLife;
	float myInvincibilityTimer = 0;
	GAMEMETRIC(bool, myIsImmortal, PLAYERISIMMORTAL, false);
};

