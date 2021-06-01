#include "pch.h"
#include "AnimationEvent.h"
#include "TimeHandler.h"
#include "Entity.h"
#include "AnimationComponent.h"
#include "ComponentLake.h"

AnimationEvent::AnimationEvent() :
	myEventTimer(0.0f),
	myEntity(nullptr),
	myFbxPath("Birb/Birb.fbx"),
	myStartDuration(0.0f),
	myLoopDuration(0.0f),
	myEndDuration(0.0f)
{
	myPos = V3F();
	myIsRunning = false;
}

AnimationEvent::~AnimationEvent()
{
}

void AnimationEvent::Init()
{
	AnimationEventState myState = AnimationEventState::Spawn;
	myEventTimer = 0.0f;
	myPos = V3F();
	myIsRunning = false;
}

void AnimationEvent::Update(float aDeltaTime)
{
	if (myIsRunning)
	{
		myEventTimer += aDeltaTime;
		switch (myState)
		{
		case AnimationEventState::Spawn:
			if (myEventTimer >= myStartDuration)
			{
				IdleEvent();
			}
			break;
		case AnimationEventState::Loop:
			if (myEventTimer >= myLoopDuration)
			{
				EndEvent();
			}
			break;
		case AnimationEventState::End:
			if (myEventTimer >= myEndDuration)
			{
				CleanUpEvent();
			}
			break;
		default:
			SYSERROR("Animation Event with wrong event state", std::to_string(static_cast<int>(myState)));
			break;
		}
	}
}

void AnimationEvent::StartEvent()
{
	//spawn my entity with mesh

	myEntity = myEntityPool->Retrieve();
	myEntity->Init(EntityType::EnvironmentDynamic, 99999);
	myEntity->SetRotation(V3F());
	myEntity->AddComponent<Mesh>()->Init(myEntity);
	myEntity->GetComponent<Mesh>()->SetUpModel(myFbxPath);
	myEntity->AddComponent<AnimationComponent>()->Init(myEntity);
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Walking);
	myEventTimer = 0.0f;
	myEntity->Spawn(myPos);
	myAnimationEntityVectorPtr->push_back(myEntity);
	myIsRunning = true;
}

void AnimationEvent::IdleEvent()
{
	//change animation to idle
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
	myState = AnimationEventState::Loop;
	myEventTimer = 0.0f;
}

void AnimationEvent::EndEvent()
{
	//change animation to death
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Action);
	myState = AnimationEventState::End;
	myEventTimer = 0.0f;
}

void AnimationEvent::CleanUpEvent()
{
	//delete my mesh
	if (myEntity)
	{
		myEntity->SetIsAlive(false);
		myEntity = nullptr;
	}
	myIsRunning = false;
	myEventTimer = 0.0f;
}

void AnimationEvent::SetUp(std::string& aFbxPath, float aStartDuration, float aLoopDuration, float aEndDuration, V3F& aPos, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aObjectPool)
{
	myFbxPath = aFbxPath;
	myStartDuration = aStartDuration;
	myLoopDuration = aLoopDuration;
	myEndDuration = aEndDuration;
	myPos = aPos;
	myAnimationEntityVectorPtr = aEntityVector;
	myEntityPool = aObjectPool;
}
