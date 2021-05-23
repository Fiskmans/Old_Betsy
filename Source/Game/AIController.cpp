#include "pch.h"
#include "AIController.h"
#include "Entity.h"
#include "ComponentLake.h"
#include "DebugDrawer.h"
#include "Random.h"

#include <PathFinder.h>

#include "AIBaseState.h"
#include "IdleState.h"
#include "AlertedState.h"
#include "SeekTargetState.h"
#include "AttackState.h"
#include "SpawningState.h"
#include "AIPollingStation.h"

#include "GBPhysXKinematicComponent.h"
#include "AnimationComponent.h"
#include "Audio.h"

#include "Sphere.hpp"

AIController::AIController() :
	myCurrentState(AIStates::Idle),
	myTargetPosition(V3F()),
	myTargetEntity(nullptr),
	myTookDamage(false)
{
	myUpdatePathfindingTimer = 0;
	myIsAnimLocked = false;
}

AIController::~AIController()
{
}

void AIController::Init(Entity* aEntity)
{
	myEntity = aEntity;

	SubscribeToMessage(MessageType::NewLevelLoaded);
	SubscribeToMessage(MessageType::NewDay);
}

void AIController::AIInit(AIPollingStation* aPollingStation, AIStates aStartingState, int aCharacterID, bool aSkipAlerted)
{
	myPollingStation = aPollingStation;
	myCurrentState = aStartingState;
	myCharacterID = aCharacterID;

	IdleState* idleState = new IdleState(myEntity, aPollingStation);
	idleState->myShouldSkipAlerted = aSkipAlerted;
	myStates[AIStates::Idle] = idleState;
	SeekTargetState* seekState = new SeekTargetState(myEntity, aPollingStation);
	myStates[AIStates::SeekTarget] = seekState;

	myStates[myCurrentState]->OnEnter();

	animalSoundTimer = Tools::RandomRange(3, 10);
}

AIPollingStation* AIController::GetPollingStation()
{
	return myPollingStation;
}

void AIController::SetTargetPosition(const V3F& aTargetPosition)
{
	myTargetPosition = aTargetPosition;
}

V3F AIController::GetTargetPosition()
{
	return myTargetPosition;
}

void AIController::SetTargetEntity(Entity* aEntity)
{
	myTargetEntity = aEntity;
}

Entity* AIController::GetTargetEntity()
{
	return myTargetEntity;
}

int AIController::GetCharacterID()
{
	return myCharacterID;
}


void AIController::Update(const float aDeltaTime)
{
	AIStates state;
	if (myStates[myCurrentState]->CheckCondition(state))
	{
		SwitchState(state);
	}

	//DebugDrawer::GetInstance().DrawCross(myTargetPosition, 10.f);


	myStates[myCurrentState]->Update(aDeltaTime);


	animalSoundTimer -= aDeltaTime;

	if (animalSoundTimer <= 0)
	{
		animalSoundTimer = Tools::RandomRange(5, 15);
		myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::AnimalSound);
	}
}

void AIController::Reset()
{
	myCurrentState = AIStates::Idle;
	myTargetEntity = nullptr;
	myTargetPosition = V3F();
	myIsDead = false;

	for (auto i : myStates)
	{
		delete i.second;
		i.second = nullptr;
	}
	UnSubscribeToMessage(MessageType::NewLevelLoaded);
	UnSubscribeToMessage(MessageType::NewDay);
}

void AIController::Collided()
{
}

void AIController::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::CuttingAnimationFinnished)
	{
		if (myCharacterID == 2)
		{
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
		}
	}
}

void AIController::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::NewLevelLoaded)
	{
		GBPhysXKinematicComponent* comp = myEntity->GetComponent<GBPhysXKinematicComponent>();
		if (comp)
		{
			comp->SetUpdateMovement(true);
		}
		else
		{
			SYSERROR("AIController wont move", "");
		}
	}
	else if (aMessage.myMessageType == MessageType::NewDay)
	{
		if (myCharacterID == 2)
		{
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
		}
	}
}

void AIController::SwitchState(AIStates aState)
{
	if (aState != myCurrentState && !myIsDead)
	{
		myStates[myCurrentState]->OnExit();

		myCurrentState = aState;

		myStates[myCurrentState]->OnEnter();
	}
	else if (myIsDead)
	{
		myPollingStation->RemoveSeekingEnemy(myEntity);
	}
}

void AIController::OnDisable()
{
	myPollingStation->RemoveSeekingEnemy(myEntity);
	myIsDead = true;
}

void AIController::OnAttach()
{

}

void AIController::OnDetach()
{

}

void AIController::OnKillMe()
{

}