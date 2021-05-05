#pragma once

enum class EntityMessage
{
	StartWalking,
	StopWalking,
	StartDying,
	DeathAnimationOver,
	StartAttacking,
	StopAttacking,
	WasHit,
	StartFalling,
	TookDamage,

	SpawnAnimationFinnished,
	UnequipAnimationFinnished,
	EquipAnimationFinnished,
	UseAnimationFinnished,
	ActionAnimationFinnished,
	InteractAnimationFinnished,
	CuttingAnimationFinnished,

	Count
};