#pragma once

class Entity;

enum class AIStates
{
	None,
	Spawning,
	Idle,
	Alerted,
	SeekTarget,
	Attack,
};

class AIBaseState
{
public:

	AIBaseState();
	~AIBaseState();

	virtual bool CheckCondition(AIStates& aState) = 0;
	virtual void Update(float aDeltaTime) = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
private:
};