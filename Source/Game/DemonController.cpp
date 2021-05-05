#include "pch.h"
#include "DemonController.h"
#include "AnimationComponent.h"
#include "Random.h"

DemonController::DemonController() :
	myTarget(nullptr),
	myCurrentState(DemonState::Idle)
{

}

DemonController::~DemonController()
{
}


void DemonController::Init(Entity* aEntity)
{
	myEntity = aEntity;

	myIdleTimer = 7.0f;
	myComboCounter = 1;

	//combo 1 & 3
	myMeleeSingleAttack1AnimationTimer = 2.02f;
	myMeleeSingleAttack2AnimationTimer = 2.02f;
	myMeleeDoubleAttackAnimationTimer = 2.9f;
	//Combo 2
	myRangedAttackAnimationTimer = 11.6f;
	myRangedCastTimer = 0.125f;
	//COMBO 4
	myBeamAttackAnimationTimer = 11.6f;
	myBeamStartTargetRotation = 0.0f;
	myBeamEndTargetRotation = 0.0f;
	mySavedCurrentRotation = -1.5707f;
	mySavedTargetRotation = -1.5707f;
	mySavedDefaultRotation = -1.5707f;
	myBeamSpawnReady = true;
	myDeathAnimationTimer = 3.75f;

	myEntity->GetComponent<AnimationComponent>()->SetShouldUseEntityMessage(false);

	float minZ = -5200.0f;
	float maxZ = -8200.0f;

	float minX = -8100.0f;
	float maxX = -10300.0f;

	float xYdiff = -8700;

	float y = 306.0f;

	for (int z = -5200; z > -8100; z -= 200)
	{
		for (int x = -8100; x >= -10300; x -= 200)
		{
			if (x < -8700)
			{
				y = 225.0f;
				if (x < -9301)
				{
					if (z > -5998.0f || z < -7100.0f)
					{
						continue;
					}
				}
			}
			else
			{
				if (z < -5602.0f && z > -7598.0f)
				{
					continue;
				}
			}
			myPossibleAoeLocations.push_back(V3F(x, y, z));
		}
	}
}

void DemonController::SetTarget(Entity* aEntity)
{
	myTarget = aEntity;
}

void DemonController::Update(const float aDeltaTime)
{
	V3F currentPos = myEntity->GetPosition();
	V3F myTargetPos = myTarget->GetPosition();
	V3F dir = currentPos - myTargetPos;

	if (myLifeComponent->GetLifePercentage() <= 0.0f)
	{
		if (myCurrentState != DemonState::Death)
		{
			SetState(DemonState::Death);
		}
	}

	switch (myCurrentState)
	{

	case DemonController::DemonState::Idle:
		myIdleTimer -= aDeltaTime;

		if (myIdleTimer < 3.0f)
		{
			float currentTime = 3.0f - myIdleTimer;
			if (currentTime < 0.8)
			{
				float yRot = LERP(mySavedCurrentRotation, mySavedDefaultRotation, currentTime / 0.8f);
				myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
			}
		}

		if (myIdleTimer <= 0.0f)
		{
			myEntity->SetRotation(V3F(0.0f, mySavedDefaultRotation, 0.0f));
			mySavedCurrentRotation = mySavedDefaultRotation;
			myIdleTimer = 3.0f;
			switch (myComboCounter)
			{
			case 1:
				SetState(DemonState::MeleeSingle1Attack);
				break;
			case 2:
				SetState(DemonState::RangedAttack);
				break;
			case 3:
				SetState(DemonState::MeleeSingle1Attack);
				break;
			case 4:
				SetState(DemonState::BeamAttack);
				break;
			default:
				break;
			}

		}
		return;

		break;

	case DemonController::DemonState::MeleeSingle1Attack:
	{

		myMeleeSingleAttack1AnimationTimer -= aDeltaTime;

		float currentTime = 2.02f - myMeleeSingleAttack1AnimationTimer;
		if (currentTime < 0.8f)
		{
			float yRot = LERP(mySavedDefaultRotation, mySavedTargetRotation, (currentTime) / 0.8f);
			myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
		}

		if (myMeleeSingleAttack1AnimationTimer <= 0.0f)
		{
			myEntity->SetRotation(V3F(0.0f, mySavedTargetRotation, 0.0f));
			mySavedCurrentRotation = mySavedTargetRotation;
			myMeleeSingleAttack1AnimationTimer = 2.02f;
			SetState(DemonState::MeleeSingle2Attack);
		}
	}
	break;

	case DemonController::DemonState::MeleeSingle2Attack:
	{

		myMeleeSingleAttack2AnimationTimer -= aDeltaTime;

		float currentTime = 2.02f - myMeleeSingleAttack2AnimationTimer;
		if (currentTime < 0.8f)
		{
			float yRot = LERP(mySavedCurrentRotation, mySavedTargetRotation, (currentTime) / 0.8f);
			myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
		}

		if (myMeleeSingleAttack2AnimationTimer <= 0.0f)
		{
			myEntity->SetRotation(V3F(0.0f, mySavedTargetRotation, 0.0f));
			mySavedCurrentRotation = mySavedTargetRotation;
			myMeleeSingleAttack2AnimationTimer = 2.02f;
			SetState(DemonState::MeleeDoubleAttack);
		}
	}
	break;

	case DemonController::DemonState::MeleeDoubleAttack:
	{

		myMeleeDoubleAttackAnimationTimer -= aDeltaTime;

		float currentTime = 2.9f - myMeleeDoubleAttackAnimationTimer;
		if (currentTime < 0.8f)
		{
			float yRot = LERP(mySavedCurrentRotation, mySavedTargetRotation, (currentTime) / 0.8f);
			myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
		}

		if (myMeleeDoubleAttackAnimationTimer <= 0.0f)
		{
			myEntity->SetRotation(V3F(0.0f, mySavedTargetRotation, 0.0f));
			mySavedCurrentRotation = mySavedTargetRotation;
			myMeleeDoubleAttackAnimationTimer = 2.9f;
			SetState(DemonState::Idle);
		}
	}
	break;
	case DemonController::DemonState::RangedAttack:
		myRangedAttackAnimationTimer -= aDeltaTime;
		myRangedCastTimer -= aDeltaTime;

		if (myRangedCastTimer <= 0.0f)
		{
			// CREATE EXPLOSION ABILITY
			int index = Tools::RandomRange((int)0, (int)myPossibleAoeLocations.size() + 7);
			V3F pos;
			if (index > myPossibleAoeLocations.size() - 1)
			{
				pos = myTarget->GetPosition();
			}
			else
			{
				pos = myPossibleAoeLocations[index];
			}
			Message attackMessage;
			attackMessage.myMessageType = MessageType::SpawnAbilityAt;
			attackMessage.aFloatValue = pos.x;
			attackMessage.aIntValue2 = pos.y;
			attackMessage.aFloatValue2 = pos.z;
			attackMessage.aIntValue = 81;
			attackMessage.myData = myEntity;

			SendMessages(attackMessage);

			SYSINFO("EXPUROTIONNNS");
			myRangedCastTimer = 0.125f;
		}

		if (myRangedAttackAnimationTimer <= 0.0f)
		{
			myRangedAttackAnimationTimer = 11.6f;
			SetState(DemonState::Idle);
		}
		break;
	case DemonController::DemonState::BeamAttack:
	{

		myBeamAttackAnimationTimer -= aDeltaTime;
		float currentTime = 11.6f - myBeamAttackAnimationTimer;
		float yRot = 0.0f;
		if (currentTime < 1.5f)
		{

		}
		else if (currentTime < 4.0f)
		{
			if (myBeamStartTargetRotation == 0.0f)
			{
				yRot = LERP(-1.5725845f, myBeamStartTargetRotation, (currentTime - 1.5f) / 2.5f);
			}
			else
			{
				yRot = LERP(-1.5725845f, myBeamStartTargetRotation, (currentTime - 1.5f) / 2.5f);
			}
			myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
		}
		else if (currentTime < 9.0f)
		{
			if (myBeamSpawnReady)
			{
				myBeamSpawnReady = false;
				V3F pos = myEntity->GetPosition();
				Message beamAttackMessage;
				beamAttackMessage.myMessageType = MessageType::SpawnAbilityAt;
				beamAttackMessage.aFloatValue = pos.x;
				beamAttackMessage.aIntValue2 = pos.y;
				beamAttackMessage.aFloatValue2 = pos.z;
				beamAttackMessage.aIntValue = 86;
				beamAttackMessage.myData = myEntity;

				//Beam ljud

				SendMessages(beamAttackMessage);
			}


			yRot = LERP(myBeamStartTargetRotation, myBeamEndTargetRotation, (currentTime - 4.0f) / 5.0f);
			myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
			V3F targetpos = myTarget->GetPosition();
			V2F targetPos2D = V2F(targetpos.x, targetpos.z);
			V2F myPos2D = V2F(myEntity->GetPosition().x, myEntity->GetPosition().z);
			V2F direct = targetPos2D - myPos2D;
			float targetRot = atan2f(direct.x, direct.y);

			if (abs(targetRot - yRot) < 0.2)
			{
				myTarget->GetComponent<Life>()->TakeDamage(20, V3F(direct.x, 0.0f, direct.y));
			}
		}
		else
		{
			yRot = LERP(myBeamEndTargetRotation, -1.5725845f, (currentTime - 9.0f) / 2.6f);
			myEntity->SetRotation(V3F(0.0f, yRot, 0.0f));
		}

		if (myBeamAttackAnimationTimer <= 0.0f)
		{
			myBeamAttackAnimationTimer = 11.6f;
			myEntity->SetRotation(V3F(0.0f, -1.5725845f, 0.0f));
			SetState(DemonState::Idle);
			myBeamSpawnReady = true;
		}
		break;
	}
	case DemonController::DemonState::Death:
		//IM DEAAAD
		myDeathAnimationTimer -= aDeltaTime;
		if (myDeathAnimationTimer <= 0.0f)
		{
			myDeathAnimationTimer = 100.0f;
			Message changeLevelMessage;
			changeLevelMessage.myMessageType = MessageType::ChangeLevel;
			changeLevelMessage.aIntValue = 9;
			SendMessages(changeLevelMessage);
		}
		return;

		break;
	default:
		return;
		break;
	}

	// WHEN DEAD ???? ------------------------------------------------

	if (!myEntity->GetIsAlive())
	{
		Message hideHealthBarMessage;
		hideHealthBarMessage.myMessageType = MessageType::BossHideHealthBar;
		SendMessages(hideHealthBarMessage);

		// GO TO END CINEMATIC
	}
}

void DemonController::Reset()
{
	myEntity = nullptr;
	myTarget = nullptr;
	myCurrentState = DemonState::Idle;
	myUpdatePathfindingTimer = 0;
}

Entity* DemonController::GetTarget()
{
	return myTarget;
}

void DemonController::Collided()
{
}

void DemonController::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
}

void DemonController::SetState(DemonState aState)
{
	switch (aState)
	{
	case DemonController::DemonState::Idle:
	{

		std::cout << "Idle" << std::endl;
		//CHANGE COMBO COUNTER
		switch (myComboCounter)
		{
		case 1:
			myComboCounter = 2;
			break;
		case 2:
			myComboCounter = 3;
			break;
		case 3:
			myComboCounter = 4;
			break;
		case 4:
			myComboCounter = 1;
			break;
		default:
			break;
		}
		myAnimationComponent->SetState(AnimationComponent::States::Idle);
	}
	break;
	case DemonController::DemonState::MeleeSingle1Attack:
	{
		std::cout << "Melee Single Attack 1" << std::endl;
		V3F targetPos = V3F(myTarget->GetPosition().x, myEntity->GetPosition().y, myTarget->GetPosition().z);

		//myMovement3D->FaceTowards(targetPos);
		V3F targetRotPos = myTarget->GetPosition();
		V2F targetPos2D = V2F(targetRotPos.x, targetRotPos.z);
		V2F myPos2D = V2F(myEntity->GetPosition().x, myEntity->GetPosition().z);
		V2F dir = targetPos2D - myPos2D;
		mySavedTargetRotation = atan2f(dir.x, dir.y);

		targetPos.y = myTarget->GetPosition().y + 1.0f;

		Message attackMessage;
		attackMessage.myMessageType = MessageType::SpawnAbilityAt;
		attackMessage.aFloatValue = targetPos.x;
		attackMessage.aIntValue2 = targetPos.y;
		attackMessage.aFloatValue2 = targetPos.z;
		attackMessage.aIntValue = 84;
		attackMessage.myData = myEntity;

		SendMessages(attackMessage);

		if (targetPos.x < -9200.0f || targetPos.z > -6000.0f || targetPos.z < -7400.0f)
		{
			myAnimationComponent->SetState(AnimationComponent::States::Attack1);
		}
		else
		{
			myAnimationComponent->SetState(AnimationComponent::States::Attack6);
		}
	}
	break;
	case DemonController::DemonState::MeleeSingle2Attack:
	{
		std::cout << "Melee Single Attack 2" << std::endl;
		V3F targetPos = V3F(myTarget->GetPosition().x, myEntity->GetPosition().y, myTarget->GetPosition().z);
		//myMovement3D->FaceTowards(targetPos);
		V3F targetRotPos = myTarget->GetPosition();
		V2F targetPos2D = V2F(targetRotPos.x, targetRotPos.z);
		V2F myPos2D = V2F(myEntity->GetPosition().x, myEntity->GetPosition().z);
		V2F dir = targetPos2D - myPos2D;
		mySavedTargetRotation = atan2f(dir.x, dir.y);

		targetPos.y = myTarget->GetPosition().y + 1.0f;

		Message attackMessage;
		attackMessage.myMessageType = MessageType::SpawnAbilityAt;
		attackMessage.aFloatValue = targetPos.x;
		attackMessage.aIntValue2 = targetPos.y;
		attackMessage.aFloatValue2 = targetPos.z;
		attackMessage.aIntValue = 84;
		attackMessage.myData = myEntity;

		SendMessages(attackMessage);

		if (targetPos.x < -9200.0f || targetPos.z > -6000.0f || targetPos.z < -7400.0f)
		{
			myAnimationComponent->SetState(AnimationComponent::States::Attack1);
		}
		else
		{
			myAnimationComponent->SetState(AnimationComponent::States::Attack6);
		}
	}
	break;
	case DemonController::DemonState::MeleeDoubleAttack:
	{
		std::cout << "Melee Attack" << std::endl;
		V3F targetPos = V3F(myTarget->GetPosition().x, myEntity->GetPosition().y, myTarget->GetPosition().z);
		//myMovement3D->FaceTowards(targetPos);
		V3F targetRotPos = myTarget->GetPosition();
		V2F targetPos2D = V2F(targetRotPos.x, targetRotPos.z);
		V2F myPos2D = V2F(myEntity->GetPosition().x, myEntity->GetPosition().z);
		V2F dir = targetPos2D - myPos2D;
		mySavedTargetRotation = atan2f(dir.x, dir.y);

		targetPos.y = myTarget->GetPosition().y + 1.0f;

		Message attackMessage;
		attackMessage.myMessageType = MessageType::SpawnAbilityAt;
		attackMessage.aFloatValue = targetPos.x;
		attackMessage.aIntValue2 = targetPos.y;
		attackMessage.aFloatValue2 = targetPos.z;
		attackMessage.aIntValue = 85;
		attackMessage.myData = myEntity;

		SendMessages(attackMessage);

		if (targetPos.x < -9200.0f || targetPos.z > -6000.0f || targetPos.z < -7400.0f)
		{
			myAnimationComponent->SetState(AnimationComponent::States::Attack2);
		}
		else
		{
			myAnimationComponent->SetState(AnimationComponent::States::Attack5);
		}
	}
	break;
	case DemonController::DemonState::RangedAttack:
		std::cout << "Ranged Attack" << std::endl;
		myAnimationComponent->SetState(AnimationComponent::States::Attack4);
		break;
	case DemonController::DemonState::BeamAttack:
		std::cout << "Beam Attack" << std::endl;
		if (myTarget->GetPosition().z > myEntity->GetPosition().z)
		{
			myBeamStartTargetRotation = 0.0f;
			myBeamEndTargetRotation = -2.52f;
		}
		else
		{
			myBeamStartTargetRotation = -3.14516f;
			myBeamEndTargetRotation = -0.62f;
		}
		myAnimationComponent->SetState(AnimationComponent::States::Attack3);
		break;
	case DemonController::DemonState::Death:
	{
		myAnimationComponent->SetState(AnimationComponent::States::Dying);

		Message message;
		message.myMessageType = MessageType::BossDied;
		SendMessages(message);

		break;
	}
	default:
		break;
	}
	myCurrentState = aState;
}

void DemonController::SetSavedVariables()
{
	myCharacterInstance = myEntity->GetComponent<CharacterInstance>();
	myLifeComponent = myEntity->GetComponent<Life>();
	myMovement3D = myEntity->GetComponent<Movement3D>();
	myAnimationComponent = myEntity->GetComponent<AnimationComponent>();
}




void DemonController::OnAttach()
{
}

void DemonController::OnDetach()
{
}

void DemonController::OnKillMe()
{
}