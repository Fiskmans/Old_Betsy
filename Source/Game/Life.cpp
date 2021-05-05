#include "pch.h"
#include "Life.h"
#include "Entity.h"
#include "Mesh.h"
#include "PostMaster.hpp"
#include "Audio.h"
#include "AIController.h"
#include "Movement3D.h"
#include "AnimationComponent.h"
#include "DataStructs.h"
#include "GBPhysXKinematicComponent.h"
#include "Collision.h"
#include "PlayerController.h"

const int Life::ourDeaultMaxLife = 10;
#if USEIMGUI
bool Life::ourShowHealthBar = false;
#include <DebugDrawer.h>
#endif // USEIMGUI



Life::Life() :
	myLife(ourDeaultMaxLife),
	myMaxLife(ourDeaultMaxLife)
{
}

Life::~Life()
{
}

void Life::Init(Entity* aEntity)
{
	myEntity = aEntity;
}

void Life::Init(Entity* aEntity, int aLifeAmount)
{
	myEntity = aEntity;
	myLife = aLifeAmount;
	myMaxLife = aLifeAmount;
}

void Life::Update(const float aDeltaTime)
{
	if (myEntity->GetEntityType() == EntityType::Player)
	{
		if (myLife < myMaxLife)
		{
			myRecoveryTimer -= aDeltaTime;
			if (myRecoveryTimer < 0.f)
			{
				myLife++;
				myRecoveryTimer = 3.f;
				Message updateUILifeMessage;
				updateUILifeMessage.myMessageType = MessageType::SetPlayerHP;
				updateUILifeMessage.myIntValue = myLife;
				SendMessages(updateUILifeMessage);
			}
#if USEIMGUI
			if (ourShowHealthBar)
			{
				DebugDrawer::GetInstance().DrawProgress(myEntity->GetPosition() + V3F(0, 20, 0), 20, myRecoveryTimer / 8.f);
			}
#endif
		}
	}

	myInvincibilityTimer -= aDeltaTime;
	if (myInvincibilityTimer < 0.0)
	{
		myInvincibilityTimer = 0.0;
#if USEIMGUI
		if (ourShowHealthBar)
		{
			DebugDrawer::GetInstance().DrawProgress(myEntity->GetPosition() + V3F(0, 14, 0), 20, myInvincibilityTimer * 2);
		}
#endif
	}
#if USEIMGUI
	if (ourShowHealthBar)
	{
		DebugDrawer::GetInstance().DrawProgress(myEntity->GetPosition() + V3F(0, 10, 0), 20, GetLifePercentage());
	}
#endif
}

void Life::Reset()
{
	myLife = myMaxLife;
	myInvincibilityTimer = 0.0f;
}

void Life::Collided(int aDamageValue)
{
	TakeDamage(aDamageValue);
}

void Life::TakeDamage(int aAmount, V3F aDirection)
{
	if (myInvincibilityTimer > 0)
	{
		return;
	}
	if (myIsImmortal && myEntity->GetEntityType() == EntityType::Player)
	{
		return;
	}

	myEntity->SendEntityMessage(EntityMessage::TookDamage);


	if (aAmount > myLife)
	{
		myLife = 0;
	}
	else
	{
		myLife -= aAmount;
	}

	myRecoveryTimer = 8.f;
	if (myEntity->GetEntityType() == EntityType::Player)
	{
		myInvincibilityTimer = 0.5f;

		//myEntity->GetComponent<Audio>()->PlayAudioEvent(AudioEvent::Hit);
	}

	if (myLife < 1)
	{
		myLife = 0;


		if (myEntity->GetEntityType() == EntityType::Player)
		{
			myInvincibilityTimer += 3.0f;

			Message playerDyingMessage;
			playerDyingMessage.myMessageType = MessageType::PlayerDying;
			SendMessages(playerDyingMessage);

			myEntity->SendEntityMessage(EntityMessage::StartDying);

			myEntity->GetComponent<PlayerController>()->SetInputBlock(true);


			Message fadeOutMessage;
			fadeOutMessage.myMessageType = MessageType::FadeOut;
			fadeOutMessage.myBool = false;
			fadeOutMessage.myIntValue = 1;
			SendMessages(fadeOutMessage);

			SYSINFO("Player died!");
			//PostMaster::GetInstance()->SendMessages(MessageType::PlayerDied);
		}
		else if (myEntity->GetEntityType() == EntityType::Enemy)
		{
			Message message;
			message.myMessageType = MessageType::EnemyDied;
			message.myData = myEntity;
			SendMessages(message);
			myEntity->SendEntityMessage(EntityMessage::StartDying);
			myEntity->GetComponent<Collision>()->Disable();
			myEntity->GetComponent<Mesh>()->SetShouldBeDrawnThroughWalls(false);
			myEntity->GetComponent<AIController>()->Disable();
			myEntity->Kill();
			myInvincibilityTimer += 20.0f;
		}
		else if (myEntity->GetEntityType() == EntityType::EnvironmentDestructible)
		{
			myEntity->SetIsAlive(false);
			SYSINFO("Destructible destroyed");
		}
		else
		{
			myEntity->SetIsAlive(false);
		}
	}
	else
	{
		if (myEntity->GetEntityType() == EntityType::Player)
		{
			Message updateUILifeMessage;
			updateUILifeMessage.myMessageType = MessageType::SetPlayerHP;
			updateUILifeMessage.myIntValue = myLife;
			SendMessages(updateUILifeMessage);
		}
	}

}

void Life::HealInPercentage(int aHealPercentage)
{
	float heal = CAST(float, aHealPercentage);
	heal *= 0.01f;
	heal *= myMaxLife;

	if ((myLife + heal) > myMaxLife)
	{
		myLife = myMaxLife;
	}
	else
	{
		myLife += CAST(int, heal);
	}

	if (myEntity->GetEntityType() == EntityType::Player)
	{
		Message updateUILifeMessage;
		updateUILifeMessage.myMessageType = MessageType::SetPlayerHP;
		updateUILifeMessage.myIntValue = myLife;
		SendMessages(updateUILifeMessage);
	}
}

void Life::MakeInvinciable(float aTime)
{
	myInvincibilityTimer = MAX(aTime, myInvincibilityTimer);
}

void Life::SetNewMaxLife(int aLife)
{
	myMaxLife = aLife;
}

float Life::GetLifePercentage() const
{
	if (myLife <= 0.0f)
	{
		return 0.0f;
	}
	return static_cast<float>(myLife) / static_cast<float>(myMaxLife);
}

const int& Life::GetLife() const
{
	return myLife;
}

const int& Life::GetMaxLife() const
{
	return myMaxLife;
}


void Life::OnAttach()
{
}

void Life::OnDetach()
{
}

void Life::OnKillMe()
{
}
