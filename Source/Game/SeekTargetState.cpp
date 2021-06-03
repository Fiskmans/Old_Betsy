#include "pch.h"
#include "SeekTargetState.h"
#include "AIPollingStation.h"
#include "Entity.h"
#include "AIController.h"
#include "GBPhysXKinematicComponent.h"
#include "AnimationComponent.h"
#include "Audio.h"
#include "GamlaBettan\PathFinder.h"

SeekTargetState::SeekTargetState(Entity* aEntity, AIPollingStation* aPollingStation) :
	myEntity(aEntity),
	myPollingStation(aPollingStation)
{
	myUpdatePathfindingTimer = 0;
}

SeekTargetState::~SeekTargetState()
{
}

bool SeekTargetState::CheckCondition(AIStates& aState)
{
	V3F enemyDir = myEntity->GetPosition() - myPollingStation->GetPlayer()->GetPosition();
	float playerDistanceSqr = enemyDir.LengthSqr();

	if (playerDistanceSqr < 30000.f)
	{
		aState = AIStates::Idle;
		return true;
	}

	bool hasTargetPos = false;
	float targetPosDistanceSqr = 0;
	if (myEntity->GetComponent<AIController>()->GetTargetPosition() != V3F())
	{
		hasTargetPos = true;
		targetPosDistanceSqr = myEntity->GetPosition().DistanceSqr(myEntity->GetComponent<AIController>()->GetTargetPosition());
	}

	bool hasTargetEntity = myEntity->GetComponent<AIController>()->GetTargetEntity() != nullptr;

	if (hasTargetPos && targetPosDistanceSqr < 200 * 200)
	{
		aState = AIStates::Idle;
		myEntity->GetComponent<AIController>()->SetTargetPosition(V3F());
		return true;
	}
	else if (!hasTargetPos && !hasTargetEntity && playerDistanceSqr > (700 * 700))
	{
		aState = AIStates::Idle;
		return true;
	}


	aState = AIStates::None;
	return false;
}

bool SeekTargetState::UpdateSeparation(V3F aDirection)
{
	//Check if path is colliding with other enemies and if so change path
	V3F direction = aDirection;

	float threshold = 20000.f;
	float decayCoefficient = 100000000;
	float maxAcceleration = 200000.f;

	float strength = 0;

	bool couldntMoveForward = false;



	for (int i = 0; i < myPollingStation->GetSeekingEnemies().size(); ++i)
	{
		Entity* enemy = myPollingStation->GetSeekingEnemies()[i];

		if (enemy == myEntity)
		{
			continue;
		}

		V3F enemyDir = myEntity->GetPosition() - enemy->GetPosition();
		float distanceSqr = enemyDir.LengthSqr();

		strength = 0;
		if (distanceSqr < threshold)
		{
			strength = min(decayCoefficient / (distanceSqr * distanceSqr), maxAcceleration);
			couldntMoveForward = true;
		}

		if (enemyDir.Length() > 0)
		{
			enemyDir.Normalize();
		}
		direction += enemyDir * strength;
	}


	V3F enemyDir = myEntity->GetPosition() - myTargetPos;
	float distanceSqr = enemyDir.LengthSqr();

	strength = 0;
	if (distanceSqr < threshold)
	{
		strength = min(decayCoefficient / (distanceSqr * distanceSqr), maxAcceleration);
		couldntMoveForward = true;
	}

	if (enemyDir.Length() > 0)
	{
		enemyDir.Normalize();
	}
	direction += enemyDir * strength;

	if (couldntMoveForward)
	{
		myTargetPos = myEntity->GetPosition() + direction.GetNormalized() * 100.f;
		return false;
	}
	return true;
}

void SeekTargetState::UpdatePathfinder()
{
	//TODO: makes the same check again if should seek to player or target position. maybe optimize to just one check.
	if (V3F(myPollingStation->GetPlayer()->GetPosition() - myEntity->GetPosition()).LengthSqr() < (1000 * 1000))
	{
		myTargetPos = myPollingStation->GetPlayer()->GetPosition();
	}
	else if (myEntity->GetComponent<AIController>()->GetTargetPosition() != V3F())
	{
		myTargetPos = myEntity->GetComponent<AIController>()->GetTargetPosition();
	}
	else if (myEntity->GetComponent<AIController>()->GetTargetEntity() != nullptr)
	{
		myTargetPos = myEntity->GetComponent<AIController>()->GetTargetEntity()->GetPosition();
	}
	//----------------------------------------------------------------------------

	V3F from = PathFinder::GetInstance().Floorify(myEntity->GetPosition());
	V3F to = PathFinder::GetInstance().Floorify(myTargetPos);

	myWayPoints = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), myTargetPos);
	
	if (myWayPoints.size() == 0)
	{
		//std::cout << "Couldnt find path on navmesh from: x " << from.y << " To: " << to.y << std::endl;
	}

	if (myWayPoints.size() > 0)
	{
		if (!UpdateSeparation(V3F(myWayPoints[0] - myEntity->GetPosition()).GetNormalized()))
		{
			myWayPoints = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), myTargetPos);
		}
	}
}

void SeekTargetState::UpdateMovement()
{
	if (myWayPoints.size() > 0)
	{
		while (V3F(myEntity->GetPosition()).DistanceSqr(myWayPoints[0]) < 100)
		{
			myWayPoints.erase(myWayPoints.begin());
			if (myWayPoints.size() == 0)
			{
				break;
			}
		}

		if (myWayPoints.size() > 0)
		{
			V3F direction = myWayPoints[0] - myEntity->GetPosition();
			myEntity->GetComponent<GBPhysXKinematicComponent>()->SetTargetDirection(direction.GetNormalized());
		}

		/*for (int i = 0; i < myWayPoints.size(); ++i)
		{
			if (i < myWayPoints.size() - 1)
			{
				DebugDrawer::GetInstance().DrawArrow(myWayPoints[i], myWayPoints[i + 1]);
			}
		}*/

	}
	else
	{
		myEntity->GetComponent<GBPhysXKinematicComponent>()->SetTargetDirection(V3F());
	}
}

void SeekTargetState::Update(float aDeltaTime)
{
	if (!myIsStunlocked)
	{
		myUpdatePathfindingTimer -= aDeltaTime;

		if (myUpdatePathfindingTimer <= 0)
		{
			myUpdatePathfindingTimer = 0.4f;

			UpdatePathfinder();
		}

		UpdateMovement();

		if (myEntity->GetComponent<GBPhysXKinematicComponent>()->GetDeltaMovement().LengthSqr() > 10)
		{
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Walking);
		}
		else
		{
			//TODO: byt till stående idle
			//myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle, false, false);
		}
	}
}

void SeekTargetState::OnEnter()
{
	myPollingStation->AddSeekingEnemy(myEntity);
	myEntity->GetComponent<GBPhysXKinematicComponent>()->Walk();
	myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Walking);
}

void SeekTargetState::OnExit()
{
	myWayPoints.clear();
	myEntity->GetComponent<GBPhysXKinematicComponent>()->SetTargetDirection(V3F());
	myPollingStation->RemoveSeekingEnemy(myEntity);
}

void SeekTargetState::SetStunlocked(bool aIsStunned)
{
	myIsStunlocked = aIsStunned;
}
