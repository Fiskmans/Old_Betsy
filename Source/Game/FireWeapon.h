#pragma once
#include "Component.h"
#include <unordered_map>

enum class TriggerStatus
{
	Pressed,
	Down,
	Released,
};

struct FireInfo
{
	V3F recoilVelocity;
	std::vector<V2F> BulletDirectionOffset;
};

enum class WeaponType
{
	None,
	Mp5,
	Glock
};

struct WeaponData
{
	bool myIsAutomatic = true;
	float myFirerate = 0;
	unsigned int myClipSize = 0;
	unsigned int myTotalAmmoLimit = 0;

	bool myTotalAmmoUnlimited = false;

	unsigned int myCurrentClipSize = 0;
	unsigned int myTotalAmmoAmount = 0;
};

class FireWeapon :
	public Component
{
public:
	FireWeapon();
	virtual ~FireWeapon();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void AddWeapon(WeaponType aType, WeaponData someData);
	void SwitchWeapon(WeaponType aType);

	void PressTrigger();
	void ReleaseTrigger();

	bool Fire(FireInfo& aFireInfo);
	bool Reload();
	bool CanReload();

	void GiveAmmo(int aAmount);

	int GetCurrentClipSize();
	int GetTotalAmmo();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:

	void InternalFire();

	TriggerStatus myTrigger;
	float myFireRateTimer;

	std::unordered_map<WeaponType, WeaponData> myWeapons;
	WeaponType myEquipedWeapon;

	GAMEMETRIC(bool, myTotalAmmoIsUnlimited, UNLIMITEDAMMO, true);
	GAMEMETRIC(bool, myClipIsUnlimited, UNLIMITEDCLIP, false);
};
