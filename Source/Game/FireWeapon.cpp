#include "pch.h"
#include "FireWeapon.h"

FireWeapon::FireWeapon()
{
	myTrigger = TriggerStatus::Released;
	
	myFireRateTimer = 0;
}

FireWeapon::~FireWeapon()
{
}

void FireWeapon::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myEquipedWeapon = WeaponType::Mp5;
}

void FireWeapon::Update(const float aDeltaTime)
{
	if (myFireRateTimer > 0)
	{
		myFireRateTimer -= aDeltaTime;
	}
}

void FireWeapon::Reset()
{
}

void FireWeapon::AddWeapon(WeaponType aType, WeaponData someData)
{
	if (myWeapons.find(aType) == myWeapons.end())
	{
		someData.myCurrentClipSize = someData.myClipSize;
		someData.myTotalAmmoAmount = someData.myTotalAmmoLimit;

		myWeapons[aType] = someData;
	}
}

void FireWeapon::SwitchWeapon(WeaponType aType)
{
	myEquipedWeapon = aType;
	myFireRateTimer = 0;
}

void FireWeapon::PressTrigger()
{
	if (myTrigger == TriggerStatus::Released)
	{
		myTrigger = TriggerStatus::Pressed;
	}
}

void FireWeapon::ReleaseTrigger()
{
	myTrigger = TriggerStatus::Released;
}

bool FireWeapon::Fire(FireInfo& aFireInfo)
{

	if (myEquipedWeapon == WeaponType::None)
	{
		return false;
	}

	WeaponData weapon = myWeapons[myEquipedWeapon];

	if (myFireRateTimer <= 0 && weapon.myCurrentClipSize > 0)
	{
		if (myTrigger == TriggerStatus::Pressed)
		{
			myTrigger = TriggerStatus::Down;
			InternalFire();
		}
		else if (myTrigger == TriggerStatus::Down && weapon.myIsAutomatic)
		{
			InternalFire();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	//TODO: Lägg till en offset riktning för skott om man t ex höftskjuter eller springer och skjuter
	//aFireInfo.BulletDirectionOffset.push_back(V2F(random, random));

	//TODO: Lägg till recoilVelocity
	//aFireInfo.recoilVelocity = ?

	return true;
}

bool FireWeapon::Reload()
{
	if (myEquipedWeapon == WeaponType::None)
	{
		return false;
	}

	WeaponData& weapon = myWeapons[myEquipedWeapon];

	if (myTotalAmmoIsUnlimited || myWeapons[myEquipedWeapon].myTotalAmmoUnlimited)
	{
		weapon.myCurrentClipSize = weapon.myClipSize;
		return true;
	}

	int ammoToLoad = weapon.myClipSize - weapon.myCurrentClipSize;
	
	if (weapon.myTotalAmmoAmount <= 0 || ammoToLoad <= 0)
	{
		return false;
	}
	else if (weapon.myTotalAmmoAmount < ammoToLoad)
	{
		weapon.myCurrentClipSize = weapon.myTotalAmmoAmount;
		weapon.myTotalAmmoAmount = 0;
		return true;
	}
	else
	{
		weapon.myCurrentClipSize = weapon.myClipSize;
		weapon.myTotalAmmoAmount -= ammoToLoad;
		return true;
	}
}

bool FireWeapon::CanReload()
{
	if (myEquipedWeapon == WeaponType::None)
	{
		return false;
	}

	if (myTotalAmmoIsUnlimited || myWeapons[myEquipedWeapon].myTotalAmmoUnlimited)
	{
		return true;
	}

	int ammoToLoad = myWeapons[myEquipedWeapon].myClipSize - myWeapons[myEquipedWeapon].myCurrentClipSize;

	if (myWeapons[myEquipedWeapon].myTotalAmmoAmount <= 0 || ammoToLoad <= 0)
	{
		return false;
	}

	return true;
}

void FireWeapon::GiveAmmo(int aAmount)
{
	if (myWeapons[WeaponType::Mp5].myTotalAmmoAmount + aAmount < myWeapons[WeaponType::Mp5].myTotalAmmoLimit)
	{
		myWeapons[WeaponType::Mp5].myTotalAmmoAmount += aAmount;
	}
	else
	{
		myWeapons[WeaponType::Mp5].myTotalAmmoAmount = myWeapons[WeaponType::Mp5].myTotalAmmoLimit;
	}
}

int FireWeapon::GetCurrentClipSize()
{
	if (myEquipedWeapon == WeaponType::None)
	{
		return 0;
	}

	return myWeapons[myEquipedWeapon].myCurrentClipSize;
}

int FireWeapon::GetTotalAmmo()
{
	if (myEquipedWeapon == WeaponType::None)
	{
		return 0;
	}

	return myWeapons[myEquipedWeapon].myTotalAmmoAmount;
}

void FireWeapon::OnAttach()
{
}

void FireWeapon::OnDetach()
{
}

void FireWeapon::OnKillMe()
{
}

void FireWeapon::InternalFire()
{
	myFireRateTimer = myWeapons[myEquipedWeapon].myFirerate;
	myTrigger == TriggerStatus::Down;

	if (!myClipIsUnlimited )
	{
		myWeapons[myEquipedWeapon].myCurrentClipSize -= 1;
	}

	if (myWeapons[myEquipedWeapon].myCurrentClipSize < 0)
	{
		myWeapons[myEquipedWeapon].myCurrentClipSize = 0;
	}
}
