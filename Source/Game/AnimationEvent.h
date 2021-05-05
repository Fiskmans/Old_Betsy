#pragma once
#include "Event.h"
#include "ObjectPool.hpp"
#include "Entity.h"

enum class AnimationEventState
{
	Spawn = 0,
	Loop = 1,
	End = 2,
	Count = 3
};

class AnimationEvent :
	public Event
{
public:
	AnimationEvent();
	~AnimationEvent();

	// Inherited via Event
	virtual void Init() override;
	virtual void Update(float aDeltaTime) override;
	virtual void StartEvent() override;
	virtual void IdleEvent() override;
	virtual void EndEvent() override;
	virtual void CleanUpEvent() override;

	void SetUp(std::string& aFbxPath, float aStartDuration, float aLoopDuration, float aEndDuration, V3F& aPos, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aObjectPool);
private:
	AnimationEventState myState;
	float myEventTimer;
	Entity* myEntity;
	std::string myFbxPath;
	float myStartDuration;
	float myLoopDuration;
	float myEndDuration;

	std::vector<Entity*>* myAnimationEntityVectorPtr = nullptr;
	CommonUtilities::ObjectPool<Entity>* myEntityPool = nullptr;
	V3F myPos;
	bool myIsRunning = false;
};

