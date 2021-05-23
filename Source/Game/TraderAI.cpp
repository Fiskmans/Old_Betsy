#include "pch.h"
#include "TraderAI.h"
#include "AnimationComponent.h"
#include "TimeHandler.h"
#include "GBPhysXKinematicComponent.h"


TraderAI::TraderAI() :
	myCurrentState(TraderState::None),
	myStateBeforeTrade(TraderState::Enter),
	myStateBerforeGoodbye(TraderState::Idle),
	myDistanceToPlayerOrTargetpointBeforeSettingUpShop(100.0f),
	myDistanceToSpawnBeforeDespawn(100.0f),
	myTimeToIdleBeforeLeaving(60.0f)
{
	myEntity = nullptr;
}

TraderAI::~TraderAI()
{
}

void TraderAI::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myTimeToIdleBeforeLeaving = 60.0f;
	myStateBeforeTrade =TraderState::Enter;
	myStateBerforeGoodbye = TraderState::Idle;
}

void TraderAI::AIInit(AIPollingStation* aPollingStation, TraderState aStartingState)
{
	myTimeToIdleBeforeLeaving = 60.0f;
	myPollingStation = aPollingStation;
	SwitchState(aStartingState);
}

void TraderAI::SwitchState(TraderState aState)
{
	if (myCurrentState != aState)
	{
		switch (aState)
		{
		case TraderState::Enter:
			myPollingStation->AddSeekingEnemy(myEntity);
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Walking);
			myMovementWayPoints = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), myTargetPosition);
			myStateBeforeTrade = TraderState::Enter;
			break;
		case TraderState::Hello:
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Action);
			myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::Hello);
			break;
		case TraderState::Idle:
			myStateBeforeTrade = TraderState::Idle;
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
			break;
		case TraderState::Trading:
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
			break;
		case TraderState::Goodbye:
			myStateBerforeGoodbye = myCurrentState;
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Interact);
			myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::Goodbye);
			break;
		case TraderState::Leaving:
			myStateBeforeTrade = TraderState::Leaving;
			myMovementWayPoints = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), myDespawnPosition);
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Walking);
			break;
		case TraderState::Despawn:
			myEntity->SetIsAlive(false);
			break;
		default:
			break;
		}
	}
	myCurrentState = aState;
}

void TraderAI::StartTrading()
{
	PostMaster::GetInstance()->SendMessages(MessageType::LockPlayer);
	TimeHandler::GetInstance().PauseTime();
	myEntity->GetComponent<GBPhysXKinematicComponent>()->StopMoving();
	SwitchState(TraderState::Trading);
}

void TraderAI::StopTrading()
{
	PostMaster::GetInstance()->SendMessages(MessageType::UnlockPlayer);
	TimeHandler::GetInstance().ResumeTime();
	/*if (myStateBeforeTrade == TraderState::Enter || myStateBeforeTrade == TraderState::Leaving)
	{
		myEntity->GetComponent<GBPhysXKinematicComponent>()->Walk();
	}*/
	SwitchState(TraderState::Goodbye);
}

void TraderAI::SetTargetPosition(const V3F& aTargetPosition)
{
	myTargetPosition = aTargetPosition;
}

V3F TraderAI::GetTargetPosition()
{
	return myTargetPosition;
}

void TraderAI::SetTargetEntity(Entity* aEntity)
{
	myTargetEntity = aEntity;
}

Entity* TraderAI::GetTargetEntity()
{
	return myTargetEntity;
}

AIPollingStation* TraderAI::GetPollingStation()
{
	return myPollingStation;
}

void TraderAI::Update(const float aDeltaTime)
{
	V3F direction = V3F();
	V3F tar = V3F();
	V3F cur = myEntity->GetPosition();;
	cur.y = 0;

	if (myMovementWayPoints.size() > 0)
	{
		tar = myMovementWayPoints[0];
		tar.y = 0;
		direction = tar - cur;

		if (direction.LengthSqr() <= 100.0f)
		{
			myMovementWayPoints.erase(myMovementWayPoints.begin());
		}
	}

	switch (myCurrentState)
	{
	case TraderState::Enter:
		//WALK TOWARDS TARGETPOINT / PLAYER
		myEntity->GetComponent<GBPhysXKinematicComponent>()->SetTargetDirection(direction.GetNormalized());
		if ((myEntity->GetPosition() - myTargetPosition).LengthSqr() <= myDistanceToPlayerOrTargetpointBeforeSettingUpShop)
		{
			SwitchState(TraderState::Hello);
			myEntity->GetComponent<GBPhysXKinematicComponent>()->StopMoving();
		}
		break;

	case TraderState::Idle:
		
		myTimeToIdleBeforeLeaving -= aDeltaTime;
		if (myTimeToIdleBeforeLeaving <= 0.0f)
		{
			SwitchState(TraderState::Goodbye);
		}
		break;

	case TraderState::Leaving:
		myEntity->GetComponent<GBPhysXKinematicComponent>()->SetTargetDirection(direction.GetNormalized());
		if ((cur - myDespawnPosition).LengthSqr() <= myDistanceToSpawnBeforeDespawn)
		{
			myPollingStation->RemoveSeekingEnemy(myEntity);
			SwitchState(TraderState::Despawn);
		}
		break;
	default:
		break;
	}
}

void TraderAI::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::ActionAnimationFinnished)
	{
		SwitchState(TraderState::Idle);
	}

	if (aMessage == EntityMessage::InteractAnimationFinnished)
	{
		switch (myStateBerforeGoodbye)
		{		
		case TraderState::Idle:
			myEntity->GetComponent<GBPhysXKinematicComponent>()->Walk();
			SwitchState(TraderState::Leaving);
			break;
		case TraderState::Trading:
			SwitchState(myStateBeforeTrade);
			break;
		case TraderState::Leaving:
			myEntity->GetComponent<GBPhysXKinematicComponent>()->Walk();
			SwitchState(TraderState::Leaving);
			break;
		default:
			break;
		}	
	}
}

void TraderAI::RecieveMessage(const Message& aMessage)
{
}

void TraderAI::SetDespawnPosition(const V3F& aDespawnPosition)
{
	myDespawnPosition = aDespawnPosition;
}

V3F TraderAI::GetDespawnPosition()
{
	return myDespawnPosition;
}

void TraderAI::Reset()
{
}

void TraderAI::OnDisable()
{
}

void TraderAI::OnAttach()
{
}

void TraderAI::OnDetach()
{
}

void TraderAI::OnKillMe()
{
}

void TraderAI::Collided()
{
}
