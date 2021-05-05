#pragma once

enum class EntityType
{
	Player,
	Enemy,
	Projectile,
	EnvironmentStatic,
	EnvironmentDynamic,
	EnvironmentDestructible,
	EnvironmentInteractable,
	TriggerBox,
	BreakableContainer,
	UnbreakableContainer,
	LevelGate,
	Ability,
	AudioOneShot,
	Camera,
	Count,
	None
};