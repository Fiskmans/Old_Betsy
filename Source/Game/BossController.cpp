#include "pch.h"
#include "BossController.h"
#include "AnimationComponent.h"
#include "Random.h"
#include <DataStructs.h>
BossController::BossController() :
	myTarget(nullptr),
	myCurrentState(BossState::Idle)
{
	myIdleTimer = 2.0f;
	myUpdatePathfindingTimer = 0;
	myTeleportTimer = 12.0f;
	myTeleportDiveTimer = 2.0f;
	myTeleportWaitTimer = 2.0f;
	myTeleportEmergeTimer = 2.7f;
	myRangedAttackTimer = 2.0f;
	myMeleeAttackTimer = 2.0f;
	myRangedAttackAnimationTimer = 2.0f;
	myMeleeAttackAnimationTimer = 1.0f;
	myPhase = 1;
	myCurrentTeleportTarget = 0;
	testBool = true;
	mySummonAnimationTimer = 5.0f;
	mySummonTimer = 2.0f;
	myPhase2Duration = 60.0f;
	myTimerVisualDamageTimer = 1.0f;
}

BossController::~BossController()
{
}


void BossController::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myEntity->GetComponent<AnimationComponent>()->SetShouldUseEntityMessage(false);

	myPossibleTeleportLocations[0] = V3F(-9009.0f, 225.0f, -6737.0f);
	myPossibleTeleportLocations[1] = V3F(-8242.0f, 306.0f, -5451.0f);
	myPossibleTeleportLocations[2] = V3F(-8930.0f, 225.0f, -5443.0f);
	myPossibleTeleportLocations[3] = V3F(-9313.0f, 225.0f, -6087.0f);
	myPossibleTeleportLocations[4] = V3F(-10112.0f, 225.0f, -6489.0f);
	myPossibleTeleportLocations[5] = V3F(-9557.0f, 225.0f, -7152.0f);
	myPossibleTeleportLocations[6] = V3F(-9100.0f, 225.0f, -7945.0f);
	myPossibleTeleportLocations[7] = V3F(-8285.0f, 306.0f, -7934.0f);
	myPossibleTeleportLocations[8] = V3F(-8185.0f, 320.0f, -6688.0f);
	myPossibleTeleportLocations[9] = V3F(0.0f, 0.0f, 0.0f);
}

void BossController::SetTarget(Entity* aEntity)
{
	myTarget = aEntity;
}

void BossController::Update(const float aDeltaTime)
{
	if (myPhase == 1)
	{
		if (myLifeComponent->GetLifePercentage() < 0.1f)
		{
			myLifeComponent->SetNewMaxLife(60);
			myLifeComponent->HealInPercentage(100.0f);
			myCurrentTeleportTarget = 8;
			myTeleportTimer = 0;
			myPhase = 2;
			myCollisionComponent->Disable();
			// HÄR JONAS KAN DU BYTA TILL PHAS 2 mYSIK

			Message message;
			message.myMessageType = MessageType::BossStage2Started;
			SendMessages(message);
		}
		V3F currentPos = myEntity->GetPosition();
		V3F myTargetPos = myTarget->GetPosition();
		V3F dir = currentPos - myTargetPos;

		switch (myCurrentState)
		{

		case BossController::BossState::Idle:
			myIdleTimer -= aDeltaTime;
			if (myIdleTimer <= 0.0f)
			{
				myIdleTimer = 2.0f;
				SetState(BossState::HuntTarget);
			}
			return;

			break;

		case BossController::BossState::HuntTarget:

			myTeleportTimer -= aDeltaTime;
			myRangedAttackTimer -= aDeltaTime;
			myMeleeAttackTimer -= aDeltaTime;
			//myUpdatePathfindingTimer -= aDeltaTime;
			myAnimationTimer -= aDeltaTime;
			//CHECK ANIMATIONLOCK
			if (myAnimationTimer > 0)
			{
				//ANIMATION
				return;
			}

			//CHECK TELEPORT
			if (myTeleportTimer <= 0.0f)
			{
				//TELEPORT
				myTeleportTimer = 12.0f;
				Message spawnAnimationMessage;
				spawnAnimationMessage.myMessageType = MessageType::SpawnAbility;
				spawnAnimationMessage.aFloatValue = dir.x;
				spawnAnimationMessage.aFloatValue2 = dir.z;
				spawnAnimationMessage.aIntValue = 83;
				spawnAnimationMessage.myData = myEntity;

				SendMessages(spawnAnimationMessage);

				SetState(BossState::TeleportStart);
				return;
			}

			//CHECK RANGED ATTACK
			if (myRangedAttackTimer <= 0)
			{
				if (dir.LengthSqr() < myRangedAttackRangeSqrd)
				{
					SetState(BossState::RangedAttack);

					//RANGED ATTACK
					std::cout << "Boss Ranged Attack" << std::endl;
					myRangedAttackTimer = 5.0f;

					V3F msgDir = myEntity->GetForward();
					msgDir = msgDir.GetNormalized();

					Message attackMessage;
					attackMessage.myMessageType = MessageType::SpawnAbility;
					attackMessage.aFloatValue = msgDir.x;
					attackMessage.aFloatValue2 = msgDir.z;
					attackMessage.aIntValue = 81;
					attackMessage.myData = myEntity;

					SendMessages(attackMessage);

					//myEntity->GetComponent<Movement3D>()->SetAnimationPauseTimer(myEntity->GetComponent<CharacterInstance>()->GetAbilityCooldown(82) - 0.1f);
					return;
				}
			}

			//CHECK MELEE ATTACK
			if (myMeleeAttackTimer <= 0)
			{
				if (dir.LengthSqr() < myMeleeAttackRangeSqrd)
				{
					SetState(BossState::MeleeAttack);
					//MELEE ATTACK
					std::cout << "Boss Melee Attack" << std::endl;
					myMeleeAttackTimer = 2.0f;

					V3F msgDir = myEntity->GetForward();
					msgDir = msgDir.GetNormalized();

					Message attackMessage;
					attackMessage.myMessageType = MessageType::SpawnAbility;
					attackMessage.aFloatValue = msgDir.x;
					attackMessage.aFloatValue2 = msgDir.z;
					attackMessage.aIntValue = 82;
					attackMessage.myData = myEntity;

					SendMessages(attackMessage);

					//myAnimationTimer = 1.0f;
					//myEntity->GetComponent<Movement3D>()->SetAnimationPauseTimer(myEntity->GetComponent<CharacterInstance>()->GetAbilityCooldown(82) - 0.1f);

					return;
				}
			}

			//MOVE
			myMovement3D->FaceTowards(myTargetPos);
			myMovement3D->SetTargetPositionOnNavmeshAtPosition(myTargetPos);

			break;

		case BossController::BossState::TeleportStart:
			myTeleportDiveTimer -= aDeltaTime;
			if (myTeleportDiveTimer < 0.05)
			{
				float yPos = LERP(myTargetPos.y, myTargetPos.y - 500, (0.05f - myTeleportDiveTimer) / 0.05f);
				myEntity->SetPosition(V3F(currentPos.x, yPos, currentPos.z));
			}
			if (myTeleportDiveTimer <= 0.0f)
			{
				myTeleportDiveTimer = 2.0f;
				SetState(BossState::TeleportWait);
			}
			break;
		case BossController::BossState::TeleportWait:
			myTeleportWaitTimer -= aDeltaTime;
			if (myTeleportWaitTimer <= 0.0f)
			{
				myTeleportWaitTimer = 3.0f;
				if (myCurrentTeleportTarget != 8)
				{
					myCurrentTeleportTarget = Tools::RandomRange(0, 7);
				}
				myEntity->SetPosition(myPossibleTeleportLocations[myCurrentTeleportTarget]);

				Message spawnAnimationMessage;
				spawnAnimationMessage.myMessageType = MessageType::SpawnAbility;
				spawnAnimationMessage.aFloatValue = dir.x;
				spawnAnimationMessage.aFloatValue2 = dir.z;
				spawnAnimationMessage.aIntValue = 83;
				spawnAnimationMessage.myData = myEntity;

				SendMessages(spawnAnimationMessage);

				myEntity->SetPosition(myEntity->GetPosition() - V3F(0.0f, 500.0f, 0.0f));
				SetState(BossState::TeleportEmerge);
			}
			break;
		case BossController::BossState::TeleportEmerge:
		{
			myTeleportEmergeTimer -= aDeltaTime;
			if (myTeleportEmergeTimer < 2.9f && myTeleportEmergeTimer > 1.4f)
			{
				float animTime = 2.9f - myTeleportEmergeTimer;
				float yPos = LERP(myPossibleTeleportLocations[myCurrentTeleportTarget].y - 500, myPossibleTeleportLocations[myCurrentTeleportTarget].y, animTime / 1.5f);
				myEntity->SetPosition(V3F(currentPos.x, yPos, currentPos.z));
			}
			if (myTeleportEmergeTimer <= 0.0f)
			{
				myEntity->SetPosition(myPossibleTeleportLocations[myCurrentTeleportTarget]);
				myTeleportEmergeTimer = 3.0f;
				SetState(BossState::HuntTarget);
			}
			break;
		}
		case BossController::BossState::RangedAttack:
			myRangedAttackAnimationTimer -= aDeltaTime;
			if (myRangedAttackAnimationTimer <= 0.0f)
			{
				myRangedAttackAnimationTimer = 2.0f;
				SetState(BossState::HuntTarget);
			}
			break;
		case BossController::BossState::MeleeAttack:
			myMeleeAttackAnimationTimer -= aDeltaTime;
			if (myMeleeAttackAnimationTimer <= 0.0f)
			{
				myMeleeAttackAnimationTimer = 1.0f;
				SetState(BossState::HuntTarget);
			}
			break;
		case BossController::BossState::Death:
			//IM DEAAAD
			return;

			break;
		default:
			return;
			break;
		}
	}
	else if (myPhase == 2)
	{
		myPhase2Duration -= aDeltaTime;
		myTimerVisualDamageTimer -= aDeltaTime;
		if (myTimerVisualDamageTimer <= 0.0f)
		{
			myLifeComponent->TakeDamage(1);
			myTimerVisualDamageTimer = 1.0f;
		}
		if (myLifeComponent->GetLifePercentage() < 0.02f || myPhase2Duration <= 0.0f)
		{
			myCurrentTeleportTarget = 9;
			myTeleportTimer = 0;
			myPhase = 3;
		}
		V3F currentPos = myEntity->GetPosition();
		V3F myTargetPos = myTarget->GetPosition();
		V3F dir = currentPos - myTargetPos;

		switch (myCurrentState)
		{

		case BossController::BossState::Idle:
			myIdleTimer -= aDeltaTime;
			if (myIdleTimer <= 0.0f)
			{
				myIdleTimer = 2.0f;
				SetState(BossState::HuntTarget);
			}
			return;

			break;

		case BossController::BossState::HuntTarget:

			myRangedAttackTimer -= aDeltaTime;
			mySummonTimer -= aDeltaTime;
			myAnimationTimer -= aDeltaTime;
			//CHECK ANIMATIONLOCK
			if (myAnimationTimer > 0)
			{
				//ANIMATION
				return;
			}

			//CHECK TELEPORT
			if (myTeleportTimer <= 0.0f)
			{
				//TELEPORT OUT AND CHANGE PHASE TO 3
				myTeleportTimer = 999.0f;
				Message spawnAnimationMessage;
				spawnAnimationMessage.myMessageType = MessageType::SpawnAbility;
				spawnAnimationMessage.aFloatValue = dir.x;
				spawnAnimationMessage.aFloatValue2 = dir.z;
				spawnAnimationMessage.aIntValue = 83;
				spawnAnimationMessage.myData = myEntity;

				SendMessages(spawnAnimationMessage);

				SetState(BossState::TeleportStart);
				return;
			}

			//CHECK RANGED ATTACK
			if (myRangedAttackTimer <= 0)
			{
				if (dir.LengthSqr() < myRangedAttackRangeSqrd)
				{
					SetState(BossState::RangedAttack);

					//RANGED ATTACK
					std::cout << "Boss Ranged Attack" << std::endl;
					myRangedAttackTimer = 4.0f;

					Message attackMessage;
					attackMessage.myMessageType = MessageType::SpawnAbilityAt;
					attackMessage.aFloatValue = myTargetPos.x;
					attackMessage.aIntValue2 = myTargetPos.y;
					attackMessage.aFloatValue2 = myTargetPos.z;
					attackMessage.aIntValue = 81;
					attackMessage.myData = myEntity;

					SendMessages(attackMessage);

					//myEntity->GetComponent<Movement3D>()->SetAnimationPauseTimer(myEntity->GetComponent<CharacterInstance>()->GetAbilityCooldown(82) - 0.1f);
					return;
				}
			}

			//CHECK MELEE ATTACK
			if (mySummonTimer <= 0)
			{

				SetState(BossState::SummonMinnions);
				//MELEE ATTACK
				std::cout << "Boss Summon" << std::endl;
				mySummonTimer = 14.0f;

				EnemyInstanceBuffer bufferBob;

				bufferBob.enemyID = 5;

				for (int i = 0; i < 8; i++)
				{
					V3F pos = myPossibleTeleportLocations[i];
					bufferBob.position[0] = pos.x;
					bufferBob.position[1] = pos.y;
					bufferBob.position[2] = pos.z;

					Message spawnMessage;
					spawnMessage.myMessageType = MessageType::SpawnEnemy;
					spawnMessage.aFloatValue = dir.x;
					spawnMessage.aFloatValue2 = dir.z;
					spawnMessage.aIntValue = 82;
					spawnMessage.myData = &bufferBob;

					SendMessages(spawnMessage);
				}

				return;

			}

			//MOVE
			myMovement3D->FaceTowards(myTargetPos);

			break;

		case BossController::BossState::TeleportStart:
			myTeleportDiveTimer -= aDeltaTime;
			if (myTeleportDiveTimer < 0.05)
			{
				float yPos = LERP(myTargetPos.y, myTargetPos.y - 500, (0.05f - myTeleportDiveTimer) / 0.05f);
				myEntity->SetPosition(V3F(currentPos.x, yPos, currentPos.z));
			}
			if (myTeleportDiveTimer <= 0.0f)
			{
				myTeleportDiveTimer = 2.0f;
				SetState(BossState::TeleportWait);
			}
			break;
		case BossController::BossState::TeleportWait:
			myTeleportWaitTimer -= aDeltaTime;
			if (myTeleportWaitTimer <= 0.0f)
			{
				myTeleportWaitTimer = 3.0f;
				if (myCurrentTeleportTarget != 8)
				{
					myCurrentTeleportTarget = Tools::RandomRange(0, 7);
				}
				myEntity->SetPosition(myPossibleTeleportLocations[myCurrentTeleportTarget]);

				Message spawnAnimationMessage;
				spawnAnimationMessage.myMessageType = MessageType::SpawnAbility;
				spawnAnimationMessage.aFloatValue = dir.x;
				spawnAnimationMessage.aFloatValue2 = dir.z;
				spawnAnimationMessage.aIntValue = 83;
				spawnAnimationMessage.myData = myEntity;

				SendMessages(spawnAnimationMessage);

				myEntity->SetPosition(myEntity->GetPosition() - V3F(0.0f, 500.0f, 0.0f));
				SetState(BossState::TeleportEmerge);
			}
			break;
		case BossController::BossState::TeleportEmerge:
		{
			myTeleportEmergeTimer -= aDeltaTime;
			if (myTeleportEmergeTimer < 2.9f && myTeleportEmergeTimer > 1.4f)
			{
				float animTime = 2.9f - myTeleportEmergeTimer;
				float yPos = LERP(myPossibleTeleportLocations[myCurrentTeleportTarget].y - 500, myPossibleTeleportLocations[myCurrentTeleportTarget].y, animTime / 1.5f);
				myEntity->SetPosition(V3F(currentPos.x, yPos, currentPos.z));
			}
			if (myTeleportEmergeTimer <= 0.0f)
			{
				myEntity->SetPosition(myPossibleTeleportLocations[myCurrentTeleportTarget]);
				myTeleportEmergeTimer = 3.0f;
				SetState(BossState::HuntTarget);
			}
			break;
		}
		case BossController::BossState::RangedAttack:
			myRangedAttackAnimationTimer -= aDeltaTime;
			if (myRangedAttackAnimationTimer <= 0.0f)
			{
				myRangedAttackAnimationTimer = 2.0f;
				SetState(BossState::HuntTarget);
			}
			break;
		case BossController::BossState::MeleeAttack:
			myMeleeAttackAnimationTimer -= aDeltaTime;
			if (myMeleeAttackAnimationTimer <= 0.0f)
			{
				myMeleeAttackAnimationTimer = 1.0f;
				SetState(BossState::HuntTarget);
			}
			break;

		case BossController::BossState::SummonMinnions:
			mySummonAnimationTimer -= aDeltaTime;
			if (mySummonAnimationTimer <= 0.0f)
			{
				mySummonAnimationTimer = 5.0f;
				SetState(BossState::Idle);
			}
			break;
		case BossController::BossState::Death:
			//IM DEAAAD
			return;

			break;
		default:
			return;
			break;
		}
	}
	else
	{
		if (myEntity->GetIsAlive())
		{
			Message hideHealthBarMessage;
			hideHealthBarMessage.myMessageType = MessageType::BossHideHealthBar;
			SendMessages(hideHealthBarMessage);

			myEntity->SetIsAlive(false);
			Message spawnPhase3Message;
			spawnPhase3Message.myMessageType = MessageType::BossDemonSpawn;
			SendMessages(spawnPhase3Message);
			// GO TO PHASE 3
		}
	}
}

void BossController::Reset()
{
	myEntity = nullptr;
	myTarget = nullptr;
	myCurrentState = BossState::Idle;
	myUpdatePathfindingTimer = 0;
}

Entity* BossController::GetTarget()
{
	return myTarget;
}

void BossController::Collided()
{
}

void BossController::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
}

void BossController::SetState(BossState aState)
{
	switch (aState)
	{
	case BossController::BossState::Idle:
		std::cout << "Idle" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myAnimationComponent->SetState(AnimationComponent::States::Idle);
		break;
	case BossController::BossState::HuntTarget:
		std::cout << "Hunt" << std::endl;
		//myEntity->GetComponent<Movement3D>()->ClearMovementTargetPoints();
		myAnimationComponent->SetState(AnimationComponent::States::Walking);
		break;
	case BossController::BossState::RangedAttack:
		std::cout << "Ranged Attack" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myMovement3D->FaceTowards(myTarget->GetPosition());
		myAnimationComponent->SetState(AnimationComponent::States::Attack5);
		break;
	case BossController::BossState::MeleeAttack:
		std::cout << "Melee Attack" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myMovement3D->FaceTowards(myTarget->GetPosition());
		myAnimationComponent->SetState(AnimationComponent::States::Attack1);
		break;
	case BossController::BossState::TeleportStart:
		std::cout << "Teleport Start" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myAnimationComponent->SetState(AnimationComponent::States::Attack3);
		break;
	case BossController::BossState::TeleportWait:
		std::cout << "Teleport Wait" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myEntity->SetPosition(myEntity->GetPosition() - V3F(0.0f, 500.0f, 0.0f));
		myAnimationComponent->SetState(AnimationComponent::States::Idle);
		break;
	case BossController::BossState::TeleportEmerge:
		std::cout << "Teleport Emerge" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myAnimationComponent->SetState(AnimationComponent::States::Attack4);
		break;
	case BossController::BossState::SummonMinnions:
		std::cout << "SUMMON" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myAnimationComponent->SetState(AnimationComponent::States::Attack2);
		break;
	case BossController::BossState::Death:
		std::cout << "Death" << std::endl;
		myMovement3D->ClearMovementTargetPoints();
		myAnimationComponent->SetState(AnimationComponent::States::Dying);
		break;
	default:
		break;
	}
	myCurrentState = aState;
}

void BossController::SetSavedVariables()
{
	myCharacterInstance = myEntity->GetComponent<CharacterInstance>();
	myRangedAttackRangeSqrd = myCharacterInstance->GetAbilityRangeSqrd(myCharacterInstance->GetCharacterStats()->rangedAttack);
	myMeleeAttackRangeSqrd = myCharacterInstance->GetAbilityRangeSqrd(myCharacterInstance->GetCharacterStats()->basicAttack);
	myLifeComponent = myEntity->GetComponent<Life>();
	myMovement3D = myEntity->GetComponent<Movement3D>();
	myAnimationComponent = myEntity->GetComponent<AnimationComponent>();
	myCollisionComponent = myEntity->GetComponent<Collision>();
}




void BossController::OnAttach()
{
}

void BossController::OnDetach()
{
}

void BossController::OnKillMe()
{
}