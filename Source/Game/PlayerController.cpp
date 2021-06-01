#include "pch.h"
#include "PlayerController.h"
#include "Entity.h"
#include "Camera.h"
#include "Movement3D.h"
#include "SlabRay.h"
#include "AABB3D.hpp"
#include "Octree.h"
#include "Life.h"
#include "Audio.h"
#include "CharacterInstance.h"
#include "GBPhysXKinematicComponent.h"
#include "FollowCamera.h"
#include "FireWeapon.h"
#include "GBPhysX.h"
#include "AnimationComponent.h"
#include "Mesh.h"
#include <Model.h>
#include "Random.h"
#include "Inventory.h"
#include "GrowthSpot.h"
#include "AIController.h"
#include "TraderAI.h"
#include "Trader.h"
#include "Plane.hpp"
#include "Growable.h"
#include "PlantLoader.h"
#include "WaterSource.h"
#include "House.h"
#include "WildPlant.h"
#include "ChickenCoop.h"

#include "ParticleActivatable.h"

#include <PathFinder.h>
#if _DEBUG
#include <DebugTools.h>
#include <DebugDrawer.h>
#endif // _DEBUG


#pragma warning(push, 1)
#pragma warning(disable : 26451 26812)
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#pragma warning(pop)

#define MARKERUPTIME .5f


PlayerController::PlayerController() :
	myOctree(nullptr),
	myMouseRay(nullptr),
	myCurrentMousedOverEntity(nullptr),
	myCurrentMousePos(V2F(0.5f, 0.5f)),
	myFollowCameraPtr(nullptr),
	myLookAtInteractable(nullptr),
	myClickedInteractable(nullptr)
{
	myMouseRay = new SlabRay(V3F(0.0f, 0.0f, 0.0f), V3F(0.0f, 0.0f, 0.0f));
}

PlayerController::~PlayerController()
{
	UnsubscribeToMessages();
	SAFE_DELETE(myMouseRay);

	WIPE(*this);
}

void PlayerController::Init(Entity* aEntity)
{
	myEntity = aEntity;
	SYSERROR("Playercontroller Inited with Init with no inventoryPtr, inventory wont scale");
}

void PlayerController::InternalInit(GBPhysX* aGBPhysX)
{
	myGBPhysX = aGBPhysX;

	CommonUtilities::Vector3<float> normal = { 0.0f, 1.0f, 0.0f };
	myMapPlane.InitWithPointAndNormal(CommonUtilities::Vector3<float>(0.0f, -0.5f, 0.0f), normal);

	ResetControlerInput();

	//myInputMovementState = InputMovementState::Idle;
	SubscribeToMessages();


}

void PlayerController::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	if (aMessage == EntityMessage::ActionAnimationFinnished)
	{
		myIsDoingAction = false;
	}
}

void PlayerController::Update(const float aDeltaTime)
{
	if (myInputIsBlocked)
	{
		return;
	}

	*myMouseRay = GetRayFromMousePos(V2F(myCurrentMousePos.x, myCurrentMousePos.y));

	if (!myShouldBlockEverything)
	{
		HandleCharacterMovement();
		HandleInputStates();

		CheckForInteractable();

		if (myClickedInteractable)
		{
			if (CheckInteractDistance(myClickedInteractable))
			{
				Interact(myClickedInteractable);
				myClickedInteractable = nullptr;
				myLookAtInteractable = nullptr;
			}
		}
	}

	if (myCuttingSheepEvent.timer > 0)
	{
		myCuttingSheepEvent.timer -= aDeltaTime;

		if (myCuttingSheepEvent.timer <= 0 && myCuttingSheepEvent.SheepEntity)
		{
			myCuttingSheepEvent.SheepEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Cutting);

			/*Inventory* invent = myEntity->GetComponent<Inventory>();

			Item wool = Item();
			wool.myItemId = PreCalculatedItemIds::Wool;
			wool.myMaxAmount = 100;
			wool.myAmount = 10;
			invent->Add(wool);*/

			ItemCreationStruct struc;
			struc.myItemId = PreCalculatedItemIds::Wool.ID;
			struc.myPosition = myEntity->GetPosition();
			struc.myAmount = 10;
			Message mess;
			mess.myData = &struc;
			mess.myMessageType = MessageType::SpawnItem;
			PostMaster::GetInstance()->SendMessages(mess);

			myCuttingSheepEvent.timer = 0;
			myCuttingSheepEvent.SheepEntity = nullptr;
		}
	}
}

void PlayerController::Reset()
{
	UnsubscribeToMessages();
}

void PlayerController::ResetControlerInput()
{
}

void PlayerController::RecieveMessage(const Message& aMessage)
{

	if (aMessage.myMessageType == MessageType::InventoryToggled)
	{
		myInputIsBlocked = aMessage.myBool;
		return;
	}
	else if (aMessage.myMessageType == MessageType::DialogueStarted)
	{
		myInputIsBlocked = true;
		myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
		return;
	}
	else if (aMessage.myMessageType == MessageType::DialogueOver)
	{
		myInputIsBlocked = false;
		return;
	}

	if (myInputIsBlocked)
	{
		return;
	}

	if (aMessage.myMessageType == MessageType::LockPlayer)
	{
		myShouldBlockEverything = true;
		myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
		return;
	}

	if (aMessage.myMessageType == MessageType::UnlockPlayer)
	{
		myShouldBlockEverything = false;
		return;
	}

	if (aMessage.myMessageType == MessageType::StartInGameAudio)
	{
		myShouldBlockEverything = false;
	}

	switch (aMessage.myMessageType)
	{
	case MessageType::InputInteractHit:
	{
		/*if (myLookAtInteractable)
		{
			Mesh* mesh = myLookAtInteractable->GetComponent<Mesh>();
			if (mesh)
			{
				ModelInstance* inst = mesh->GetModelInstance();
				if (inst)
				{
					inst->Interact();
					SYSINFO("Interacted with: " + inst->GetFriendlyName());
				}
			}
			if (myLookAtInteractable->GetEntityType() == EntityType::EnvironmentInteractable)
			{
				Message mess;
				mess.myMessageType = MessageType::EntityInteractedWith;
				mess.myData = myLookAtInteractable;
				SendMessages(mess);
			}
		}*/
	}
	break;

	case MessageType::FadeInComplete:
	{
		myCanMove = true;
		break;
	}
	case MessageType::InputMouseMoved:
	{

		myCurrentMousePos = V2F(aMessage.myFloatValue, aMessage.myFloatValue2);

		*myMouseRay = GetRayFromMousePos(V2F(myCurrentMousePos.x, myCurrentMousePos.y));
#if _DEBUG
		DebugTools::LastKnownMouseRay = myMouseRay;
#endif

		break;
	}
	case MessageType::InputMouseStopedMoving:
	{
		myMouseMovement = { 0.f, 0.f };
		break;
	}
	case MessageType::InputLeftMouseDown:
	{
		break;
	}
	case MessageType::InputLeftMouseReleased:
	{
		break;
	}
	case MessageType::InputLeftMouseHit:
	{
		if (!myShouldBlockEverything)
		{
			if (!myIsDoingAction)
			{
				myClickedInteractable = nullptr;
				if (!CheckInteractDistance(myLookAtInteractable))
				{
					V3F pos;
					if (myLookAtInteractable)
					{
						myClickedInteractable = myLookAtInteractable;

						pos = myLookAtInteractable->myInteractPoint;
					}

					if (pos == V3F())
					{
						pos = PathFinder::GetInstance().FindPoint(*myMouseRay);
					}

					V3F pos2 = PathFinder::GetInstance().FindPoint(*myMouseRay);

					if (pos != V3F())
					{
						myWayPoints = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), pos);
					}

					if (myWayPoints.size() == 0)
					{
						if (pos == V3F() || pos2 == V3F())
						{
							CommonUtilities::Plane<float> p;
							p.InitWithPointAndNormal(V3F(), V3F(0, 1, 0));
							float trash;
							V3F center = myMouseRay->FindIntersection(p, trash);
							V3F last = center;

							for (size_t i = 0; i < 200; i++)
							{
								V3F offset = V3F(0, 0, i * 5);
								offset.RotateY(i * 0.5f);
								V3F found = PathFinder::GetInstance().Floorify(center + offset);
								DebugDrawer::GetInstance().DrawArrow(last, center + offset);
								last = center + offset;
								if (found != V3F())
								{
									DebugDrawer::GetInstance().DrawCross(found, 50);
									pos = found;
									break;
								}
							}
						}
						if (pos != V3F())
						{
							V3F at = PathFinder::GetInstance().Floorify(myEntity->GetPosition());
							if (at == V3F())
							{
								V3F source = myEntity->GetPosition();
								for (size_t i = 0; i < 200; i++)
								{
									V3F offset = V3F(0, 0, i * 1);
									offset.RotateY(i * 0.5f);
									V3F found = PathFinder::GetInstance().Floorify(source + offset);
									if (found != V3F())
									{
										myEntity->SetPosition(found);
										break;
									}
								}
							}
							myWayPoints = PathFinder::GetInstance().FindPath(myEntity->GetPosition(), pos);
						}
					}
				}
				else
				{
					Interact(myLookAtInteractable);
					myLookAtInteractable = nullptr;
				}
			}
		}
		break;
	}
	case MessageType::InputRightMouseHit:
	{
		break;
	}
	case MessageType::InputRightMouseReleased:
	{
		break;
	}
	case MessageType::InputJumpHit:
	{
		break;
	}
	case MessageType::InputUpIsDown:
	{

		break;
	}
	case MessageType::InputDownIsDown:
	{

		break;
	}
	case MessageType::InputRightIsDown:
	{

		break;
	}
	case MessageType::InputLeftIsDown:
	{

		break;
	}
	case MessageType::WindowResize:
	{
		break;
	}
	case MessageType::NewOctreeCreated:
	{
		myOctree = CAST(Octree*, aMessage.myData);
		break;
	}
	case MessageType::WateringPlant:
	{
		myIsDoingAction = true;
		myAction = Action::Watering;

		Item* item = myEntity->GetComponent<Inventory>()->GetSelectedItem();

		if (item && item->myCharges <= 0)
		{
			Item emptyWatercan = Item();
			emptyWatercan.myItemId = PreCalculatedItemIds::WateringCan;
			emptyWatercan.myCharges = 0;
			myEntity->GetComponent<Inventory>()->Replace(*item, emptyWatercan, false);
		}

		break;
	}
	case MessageType::HoeingGround:
	{
		myIsDoingAction = true;
		myAction = Action::Hoeing;
		break;
	}
	case MessageType::PlantingSeed:
	{
		myIsDoingAction = true;
		myAction = Action::Planting;
		break;
	}
	case MessageType::HarvestingPlant:
	{
		myIsDoingAction = true;
		myAction = Action::Harvesting;
		break;
	}
	case MessageType::LeavingHen:
	{
		myIsDoingAction = true;
		myAction = Action::LeavingHen;
		break;
	}
	case MessageType::FillingWaterbucket:
	{
		myIsDoingAction = true;
		myAction = Action::GettingWater;

		Item* item = myEntity->GetComponent<Inventory>()->GetSelectedItem();

		if (item)
		{
			Item filledWatercan = Item();
			filledWatercan.myItemId = PreCalculatedItemIds::WateringCanFilled;
			filledWatercan.myCharges = 8;
			myEntity->GetComponent<Inventory>()->Replace(*item, filledWatercan, false);
		}

		myEntity->GetComponent<Audio>()->PostAudioEvent(AudioEvent::GettingWater);

		break;
	}
	case MessageType::NewDay:
	{
		myEntity->MoveToSpawn();
		Movement3D* move = myEntity->GetComponent<Movement3D>();
		if (move)
		{
			move->ClearMovementTargetPoints();
		}
		break;
	}
	default:
		break;
	}

}

void PlayerController::SetFollowCameraPtr(FollowCamera* aFollowCam)
{
	myFollowCameraPtr = aFollowCam;
}

void PlayerController::SetShouldBlockEverything(bool aShouldBlockEverything)
{
	myShouldBlockEverything = aShouldBlockEverything;
}

void PlayerController::SetSpawnPoint(V3F aSpawnPoint)
{
	mySpawnPoint = aSpawnPoint;
}

void PlayerController::StopMoving()
{
	myEntity->GetComponent<Movement3D>()->ClearMovementTargetPoints();
	myWayPoints.clear();
}

void PlayerController::OnAttach()
{
}

void PlayerController::OnDetach()
{
}

void PlayerController::OnKillMe()
{
}

bool PlayerController::Interact(Entity* aEntity)
{
	if (aEntity->GetEntityType() == EntityType::EnvironmentInteractable)
	{
		Message mess;
		mess.myMessageType = MessageType::EntityInteractedWith;
		mess.myData = aEntity;
		SendMessages(mess);

		Inventory* invent = myEntity->GetComponent<Inventory>();
		if (invent)
		{
			aEntity->InteractWith(invent->GetSelectedItem());
			myEntity->GetComponent<Movement3D>()->ClearMovementTargetPoints();
			myWayPoints.clear();

			V3F point;
			if (aEntity->GetComponent<WaterSource>() || aEntity->GetComponent<House>())
			{
				point = aEntity->myInteractPoint;
			}
			else
			{
				point = aEntity->GetPosition();
			}
			point.y = 0;

			myEntity->GetComponent<Movement3D>()->SetTargetDirectionToPoint(point);

			return true;
		}

	}
	else if (aEntity->GetEntityType() == EntityType::Enemy)
	{
		Inventory* invent = myEntity->GetComponent<Inventory>();
		if (aEntity->GetComponent<TraderAI>() != nullptr)
		{
			aEntity->GetComponent<Trader>()->InteractWith(invent->GetSelectedItem());
			aEntity->GetComponent<TraderAI>()->StartTrading();

			myEntity->GetComponent<Movement3D>()->ClearMovementTargetPoints();
			myWayPoints.clear();
			myEntity->GetComponent<Movement3D>()->SetTargetDirectionToPoint(aEntity->GetPosition());

			return true;
		}
		int id = aEntity->GetComponent<AIController>()->GetCharacterID();

		if (id == 2 && invent && invent->GetSelectedItem() && invent->GetSelectedItem()->myItemId == PreCalculatedItemIds::Scissor)
		{
			if (myCuttingSheepEvent.SheepEntity && myCuttingSheepEvent.timer > 0)
			{
				return false;
			}

			myIsDoingAction = true;
			myAction = Action::CuttingWool;

			myEntity->GetComponent<Movement3D>()->ClearMovementTargetPoints();
			myWayPoints.clear();
			myEntity->GetComponent<Movement3D>()->SetTargetDirectionToPoint(aEntity->GetPosition());
			aEntity->GetComponent<ParticleActivatable>()->ActivateParticle(3);


			myCuttingSheepEvent.SheepEntity = aEntity;
			myCuttingSheepEvent.timer = 3;

			return true;
		}
		else
		{
			Inventory* invent = myEntity->GetComponent<Inventory>();
			if (invent && invent->GetSelectedItem())
			{
				aEntity->InteractWith(invent->GetSelectedItem());
				myEntity->GetComponent<Movement3D>()->ClearMovementTargetPoints();
				myWayPoints.clear();
				myEntity->GetComponent<Movement3D>()->SetTargetDirectionToPoint(aEntity->GetPosition());
				return true;
			}
		}
	}
	return false;
}

bool PlayerController::CheckInteractDistance(Entity* aEntity)
{
	if (aEntity)
	{
		V3F pos = aEntity->myInteractPoint;

		float distanceSqr;
		if (pos != V3F())
		{
			distanceSqr = V3F(pos - myEntity->GetPosition()).LengthSqr();
		}
		else
		{
			distanceSqr = V3F(aEntity->GetPosition() - myEntity->GetPosition()).LengthSqr();
		}

		if (aEntity->GetComponent<WaterSource>() != nullptr)
		{
			if (distanceSqr < 40000)
			{
				return true;
			}
		}
		else if (distanceSqr < 20000)
		{
			return true;
		}
	}

	return false;
}



void PlayerController::HandleInputStates()
{
	Item* item = myEntity->GetComponent<Inventory>()->GetSelectedItem();
	int id = CAST(int, EquipmentID::None);

	if (item)
	{
		if (GrowthSpot::CanBePlanted(item->myItemId.ID))
		{
			id = CAST(int, EquipmentID::Seeds);
		}
		else
		{
			if (PreCalculatedItemIds::Hoe == item->myItemId)
			{
				id = CAST(int, EquipmentID::Hoe);
			}
			else if (PreCalculatedItemIds::WateringCanFilled == item->myItemId || PreCalculatedItemIds::WateringCan == item->myItemId)
			{
				id = CAST(int, EquipmentID::WaterBucket);
			}
			else if (PreCalculatedItemIds::Scissor == item->myItemId)
			{
				id = CAST(int, EquipmentID::Scissor);
			}
			else if (PreCalculatedItemIds::Basket == item->myItemId)
			{
				id = CAST(int, EquipmentID::Basket);
			}
			else if (PreCalculatedItemIds::Chicken == item->myItemId)
			{
				id = CAST(int, EquipmentID::Chicken);
			}
			else if (PreCalculatedItemIds::None == item->myItemId)
			{
				id = CAST(int, EquipmentID::None);
			}
		}
	}

	if (myIsDoingAction)
	{
		myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Action);
	}
	else
	{
		if (myEntity->GetComponent<Movement3D>()->IsMoving())
		{
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Walking);
		}
		else
		{
			myEntity->GetComponent<AnimationComponent>()->SetState(AnimationComponent::States::Idle);
		}
	}
}

void PlayerController::HandleCharacterMovement()
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
			myWayPoints[0].y = 0;
			myEntity->GetComponent<Movement3D>()->AddMovementTargetPoint(myWayPoints[0], true);
		}
	}
}

void PlayerController::SubscribeToMessages()
{
	SubscribeToMessage(MessageType::WindowResize);
	SubscribeToMessage(MessageType::NewOctreeCreated);

	SubscribeToMessage(MessageType::InputLeftMouseHit);
	SubscribeToMessage(MessageType::InputLeftMouseDown);
	SubscribeToMessage(MessageType::InputLeftMouseReleased);
	SubscribeToMessage(MessageType::InputRightMouseHit);
	SubscribeToMessage(MessageType::InputRightMouseReleased);
	SubscribeToMessage(MessageType::InputMouseMoved);
	SubscribeToMessage(MessageType::InputMouseStopedMoving);
	SubscribeToMessage(MessageType::InputMiddleMouseDown);
	SubscribeToMessage(MessageType::InputMiddleMouseHit);
	SubscribeToMessage(MessageType::FadeInComplete);

	SubscribeToMessage(MessageType::InputUpHit);
	SubscribeToMessage(MessageType::InputUpIsDown);
	SubscribeToMessage(MessageType::InputUpReleased);
	SubscribeToMessage(MessageType::InputDownHit);
	SubscribeToMessage(MessageType::InputDownIsDown);
	SubscribeToMessage(MessageType::InputDownReleased);
	SubscribeToMessage(MessageType::InputRightHit);
	SubscribeToMessage(MessageType::InputRightIsDown);
	SubscribeToMessage(MessageType::InputRightReleased);
	SubscribeToMessage(MessageType::InputLeftHit);
	SubscribeToMessage(MessageType::InputLeftIsDown);
	SubscribeToMessage(MessageType::InputLeftReleased);

	SubscribeToMessage(MessageType::InputInteractHit);

	SubscribeToMessage(MessageType::HoeingGround);
	SubscribeToMessage(MessageType::WateringPlant);
	SubscribeToMessage(MessageType::PlantingSeed);
	SubscribeToMessage(MessageType::HarvestingPlant);
	SubscribeToMessage(MessageType::FillingWaterbucket);
	SubscribeToMessage(MessageType::LeavingHen);

	SubscribeToMessage(MessageType::InventoryToggled);
	SubscribeToMessage(MessageType::DialogueStarted);
	SubscribeToMessage(MessageType::DialogueOver);

	SubscribeToMessage(MessageType::NewDay);

	SubscribeToMessage(MessageType::LockPlayer);
	SubscribeToMessage(MessageType::UnlockPlayer);

	SubscribeToMessage(MessageType::StartInGameAudio);

}

void PlayerController::UnsubscribeToMessages()
{
	UnSubscribeToMessage(MessageType::InputRightMouseHit);
	UnSubscribeToMessage(MessageType::InputLeftMouseHit);
	UnSubscribeToMessage(MessageType::InputLeftMouseDown);
	UnSubscribeToMessage(MessageType::InputMouseMoved);
	UnSubscribeToMessage(MessageType::InputMiddleMouseDown);
	UnSubscribeToMessage(MessageType::InputLeftMouseReleased);
	UnSubscribeToMessage(MessageType::InputMiddleMouseHit);
	UnSubscribeToMessage(MessageType::WindowResize);
	UnSubscribeToMessage(MessageType::NewOctreeCreated);

	UnSubscribeToMessage(MessageType::InputUpHit);
	UnSubscribeToMessage(MessageType::InputUpIsDown);
	UnSubscribeToMessage(MessageType::InputUpReleased);
	UnSubscribeToMessage(MessageType::InputDownHit);
	UnSubscribeToMessage(MessageType::InputDownIsDown);
	UnSubscribeToMessage(MessageType::InputDownReleased);
	UnSubscribeToMessage(MessageType::InputRightHit);
	UnSubscribeToMessage(MessageType::InputRightIsDown);
	UnSubscribeToMessage(MessageType::InputRightReleased);
	UnSubscribeToMessage(MessageType::InputLeftHit);
	UnSubscribeToMessage(MessageType::InputLeftIsDown);
	UnSubscribeToMessage(MessageType::InputLeftReleased);

	UnSubscribeToMessage(MessageType::InputInteractHit);

	UnSubscribeToMessage(MessageType::HoeingGround);
	UnSubscribeToMessage(MessageType::WateringPlant);
	UnSubscribeToMessage(MessageType::PlantingSeed);
	UnSubscribeToMessage(MessageType::HarvestingPlant);
	UnSubscribeToMessage(MessageType::FillingWaterbucket);
	UnSubscribeToMessage(MessageType::LeavingHen);

	UnSubscribeToMessage(MessageType::InventoryToggled);
	UnSubscribeToMessage(MessageType::DialogueStarted);
	UnSubscribeToMessage(MessageType::DialogueOver);

	UnSubscribeToMessage(MessageType::NewDay);

	UnSubscribeToMessage(MessageType::LockPlayer);
	UnSubscribeToMessage(MessageType::UnlockPlayer);
	UnSubscribeToMessage(MessageType::StartInGameAudio);
}

V3F PlayerController::Unproject(const V3F& aViewportPoint)
{
	Camera* cam = myFollowCameraPtr->GetCamera();
	auto view = cam->GetTransform();
	auto invViev = CommonUtilities::Matrix4x4<float>::GetRealInverse(view);

	auto invProjectionMatrix = CommonUtilities::Matrix4x4<float>::GetRealInverse(CommonUtilities::Matrix4x4<float>::GetRealInverse(view) * cam->GetProjection(false));
	V4F result =
	{
		(aViewportPoint.x - .5f) * 2.f,
		(aViewportPoint.y - .5f) * -2.f,
		aViewportPoint.z,
		1
	};

	result = result * invProjectionMatrix;

	result /= result.w;

	return V3F(result.x, result.y, result.z);
}

SlabRay PlayerController::GetRayFromMousePos(V2F aMousePos)
{
	V3F rayA = Unproject({ aMousePos.x, aMousePos.y, 0.f });
	//V3F rayA = Unproject({ 0.5f, 0.5f, 0.f });
	V3F rayB = Unproject({ aMousePos.x, aMousePos.y, 1.f });
	//V3F rayB = Unproject({ 0.5f, 0.5f, 1.f });

	SlabRay ray = SlabRay(V4F(rayA, 1.0f), V4F(rayB, 1.0f));

	return ray;

}

void PlayerController::CheckMouseRayVSOctree(SlabRay& r)
{
	if (myOctree != nullptr)
	{
		Entity* pickedEntity = myOctree->RayPickEntity(r);
		if (myCurrentMousedOverEntity && myCurrentMousedOverEntity->GetComponent<Mesh>())
		{
			auto* modl = myCurrentMousedOverEntity->GetComponent<Mesh>()->GetModelInstance();
			if (modl)
			{
				modl->SetIsHighlighted(false);
			}
		}

		if (pickedEntity != nullptr)
		{
			if (pickedEntity->GetComponent<Mesh>())
			{
				auto* modl = pickedEntity->GetComponent<Mesh>()->GetModelInstance();
				if (modl)
				{
					modl->SetIsHighlighted(true);
				}
			}

			myCurrentMousedOverEntity = pickedEntity;
			if (pickedEntity->GetComponent<Life>())
			{
				Message message;
				message.myMessageType = MessageType::EnemyMouseHover;
				message.myFloatValue = pickedEntity->GetComponent<Life>()->GetLifePercentage();
				message.myData = pickedEntity;
				SendMessages(message);
			}
			else
			{
				//IS CONTAINER OR SOMETHING ELSE OTHER THAN ENEMY
			}
			return;
		}
		else
		{
			Message message;
			message.myMessageType = MessageType::EnemyMouseHoverReleased;
			SendMessages(message);
		}
	}

	myCurrentMousedOverEntity = nullptr;
}

V3F PlayerController::GetRayVsMapPoint(SlabRay& aRay)
{
	if (myFollowCameraPtr->GetCamera())
	{
		CommonUtilities::Plane<float> plane(myEntity->GetPosition(), V3F(0, 1, 0));
		float tvoid;
		return myMouseRay->FindIntersection(plane, tvoid);
	}
	return V3F();
}

void PlayerController::CheckForInteractable()
{

	auto StopLookingAt = [this]()
	{
		Message mess;
		mess.myBool = false;
		mess.myMessageType = MessageType::LookingAtInteractable;
		SendMessages(mess);

		Mesh* mesh = myLookAtInteractable->GetComponent <Mesh>();
		if (mesh)
		{
			ModelInstance* inst = mesh->GetModelInstance();
			if (inst)
			{
				inst->SetIsHighlighted(false);
			}
		}

		myLookAtInteractable = nullptr;
	};

	Camera* camera = myFollowCameraPtr->GetCamera();

	BulletHitReport hitReport = myGBPhysX->RayPickActor(myMouseRay->orig, myMouseRay->dir, true);


	if (hitReport.actor)
	{
		Entity* hitEntity = hitReport.actor->GetEntity();
		if (hitEntity && (hitEntity->GetEntityType() == EntityType::EnvironmentInteractable || hitEntity->GetEntityType() == EntityType::Enemy || hitEntity->GetComponent<House>()))
		{
			if (hitEntity->GetEntityType() == EntityType::Enemy)
			{
				AIController* controller = hitEntity->GetComponent<AIController>();
				if (controller && controller->GetCharacterID() == 1)
				{
					return;
				}
			}

			if (myLookAtInteractable && myLookAtInteractable != hitEntity)
			{
				StopLookingAt();
			}

			if (!myLookAtInteractable)
			{
				myLookAtInteractable = hitEntity;


				Mesh* mesh = myLookAtInteractable->GetComponent <Mesh>();
				if (mesh)
				{
					ModelInstance* inst = mesh->GetModelInstance();
					if (inst)
					{
						inst->SetIsHighlighted(true);
					}
				}

				Message mess;
				mess.myBool = true;
				mess.myMessageType = MessageType::LookingAtInteractable;
				SendMessages(mess);
			}
			
			if (myLookAtInteractable)
			{
				if (hitEntity->GetEntityType() == EntityType::Enemy)
				{
					AIController* controller = hitEntity->GetComponent<AIController>();
					if (controller && controller->GetCharacterID() == 2)
					{
						if (myLastHoverMessage != MessageType::MouseHoverSheep)
						{
							myLastHoverMessage = MessageType::MouseHoverSheep;

							Message message;
							message.myMessageType = myLastHoverMessage;
							SendMessages(message);
						}
					}
					else if (myLastHoverMessage != MessageType::MouseHoverNone)
					{
						myLastHoverMessage = MessageType::MouseHoverNone;

						Message message;
						message.myMessageType = myLastHoverMessage;
						SendMessages(message);
					}
				}
				else if (hitEntity->GetEntityType() == EntityType::EnvironmentInteractable)
				{
					GrowthSpot* spot = hitEntity->GetComponent<GrowthSpot>();

					if (spot)
					{
						if (!spot->GetIsHoed())
						{
							if (myLastHoverMessage != MessageType::MouseHoverHoe)
							{
								myLastHoverMessage = MessageType::MouseHoverHoe;

								Message message;
								message.myMessageType = myLastHoverMessage;
								SendMessages(message);
							}
						}
						else if (!spot->myPlant)
						{
							if (myLastHoverMessage != MessageType::MouseHoverCanPlant)
							{
								myLastHoverMessage = MessageType::MouseHoverCanPlant;

								Message message;
								message.myMessageType = myLastHoverMessage;
								SendMessages(message);
							}
						}
						else if (spot->myPlant && spot->myPlant->GetComponent<Growable>()->GetPlantInstance()->GetIsHarvestable() && spot->myPlant->GetComponent<Growable>()->GetPlantInstance()->GetProgress() > 0.9)
						{
							if (myLastHoverMessage != MessageType::MouseHoverCanHarvest)
							{
								myLastHoverMessage = MessageType::MouseHoverCanHarvest;

								Message message;
								message.myMessageType = myLastHoverMessage;
								SendMessages(message);
							}
						}
						else if (!spot->GetIsMOIST())
						{
							if (myLastHoverMessage != MessageType::MouseHoverWaterBucket)
							{
								myLastHoverMessage = MessageType::MouseHoverWaterBucket;

								Message message;
								message.myMessageType = myLastHoverMessage;
								SendMessages(message);
							}
						}
						else if (myLastHoverMessage != MessageType::MouseHoverNone)
						{
							myLastHoverMessage = MessageType::MouseHoverNone;

							Message message;
							message.myMessageType = myLastHoverMessage;
							SendMessages(message);
						}
					}
					else if (hitEntity->GetComponent<WaterSource>())
					{
						if (myLastHoverMessage != MessageType::MouseHoverRiver)
						{
							myLastHoverMessage = MessageType::MouseHoverRiver;

							Message message;
							message.myMessageType = myLastHoverMessage;
							SendMessages(message);
						}
					}
					else if (hitEntity->GetComponent<WildPlant>())
					{
						if (myLastHoverMessage != MessageType::MouseHoverCanHarvest)
						{
							myLastHoverMessage = MessageType::MouseHoverCanHarvest;

							Message message;
							message.myMessageType = myLastHoverMessage;
							SendMessages(message);
						}
					}
					else if (hitEntity->GetComponent<ChickenCoop>())
					{
						if (hitEntity->GetComponent<ChickenCoop>()->HasEggs() && myLastHoverMessage != MessageType::MouseHoverCanHarvest)
						{
							myLastHoverMessage = MessageType::MouseHoverCanHarvest;

							Message message;
							message.myMessageType = myLastHoverMessage;
							SendMessages(message);
						}
					}
				}
				else if (myLastHoverMessage != MessageType::MouseHoverNone)
				{
					myLastHoverMessage = MessageType::MouseHoverNone;

					Message message;
					message.myMessageType = myLastHoverMessage;
					SendMessages(message);
				}
			}

			return;
		}
		else if (myLastHoverMessage != MessageType::MouseHoverNone)
		{
			myLastHoverMessage = MessageType::MouseHoverNone;

			Message message;
			message.myMessageType = myLastHoverMessage;
			SendMessages(message);
		}
	}
	else if (myLastHoverMessage != MessageType::MouseHoverNone)
	{
		myLastHoverMessage = MessageType::MouseHoverNone;

		Message message;
		message.myMessageType = myLastHoverMessage;
		SendMessages(message);
	}

	if (myLookAtInteractable)
	{
		StopLookingAt();
	}
}