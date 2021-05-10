#include "pch.h"
#include "Collision.h"
#include "Entity.h"
#include "AABB3D.hpp"
#include "Audio.h"
#include "Mesh.h"
#include "Model.h"
#include "DebugDrawer.h"
#include "Sphere.hpp"
#include "EnemyFactory.h"
#include "Life.h"
#include "AbilityInstance.h"
#include "GBPhysX.h"
#include "Intersection.hpp"
#include "GBPhysXComponent.h"


Collision::Collision() :
	myCollisionRadius(10.0f),
	myIsFriendly(true),
	myIsStatic(false),
	myBoxIsCalculated(false)
{
	myBoxCollider = CU::AABB3D<float>();
	myTriggerID = -1;
	myDamageToOthers = 1;
	myIsColliding = false;
	myWasCollidingWithPlayerLastFrame = false;
	myIsStrikeableTrigger = false;
	myIsActive = false;
}

Collision::~Collision()
{
	WIPE(*this);
}

void Collision::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myDamageToOthers = 1;
	if (myEntity->GetEntityType() == EntityType::EnvironmentStatic)
	{
		myDamageToOthers = 2;
	}

	myIsActive = true;
}

void Collision::Update(const float aDeltaTime)
{
	if (!myIsStatic)
	{
		V3F posPlusHalfHeight = myEntity->GetPosition();
		posPlusHalfHeight.y += myHeightOffset;
		myPos = posPlusHalfHeight;
		myBoxIsCalculated = false;
	}

	for (auto& it : mySetOfCollidedWithLastFrameEntities)
	{
		if (mySetOfCollidingEntities.count(it) == 0)
		{
			Message triggerExitMessage;
			triggerExitMessage.myMessageType = MessageType::TriggerEvent;
			triggerExitMessage.myIntValue = GetTriggerID();
			triggerExitMessage.myIntValue2 = CAST(int, it->GetEntityType());
			triggerExitMessage.myBool = false;
			triggerExitMessage.myData = it;
			SendMessages(triggerExitMessage);
		}
	}
	mySetOfCollidedWithLastFrameEntities = mySetOfCollidingEntities;
	mySetOfCollidingEntities.clear();

#if USEIMGUI
#if EDITFARMBOUNDS	
	if (myIsPlaneVolume)
	{
		Tools::EditPosition("Center", myCenter);
		for (size_t i = 0; i < myPoints.size(); i++)
		{
			DebugDrawer::GetInstance().SetColor((mySides[i]) ? V4F(0,1,0,1) : V4F(1,0,0,1));
			DebugDrawer::GetInstance().DrawArrow(myPoints[i] + V3F(0, 5m, 0), myPoints[(i + 1) % myPoints.size()] + V3F(0, 5m, 0));
			DebugDrawer::GetInstance().DrawArrow(myPoints[i], myPoints[i] + V3F(0, 5m, 0));

			Tools::EditPosition(("Point " + std::to_string(i + 1)).c_str(), myPoints[i]);

			for (size_t y = 0; y < 10; y++)
			{
				DebugDrawer::GetInstance().DrawLine(myPoints[i] + V3F(0, 50cm * y, 0), myPoints[(i + 1) % myPoints.size()] + V3F(0, 50cm * y, 0));
			}
		}
		if (ImGui::Button("Add point"))
		{
			myPoints.push_back(V3F(0, 0, 0));
			mySides.push_back(false);
		}
	}
#endif
#endif // USEIMGUI


}

void Collision::Reset()
{
	myIsFriendly = true;
	myBoxCollider = CU::AABB3D<float>();
	myIsStatic = false;
	myTriggerID = -1;
	myIsColliding = false;
	myWasCollidingWithPlayerLastFrame = false;
	myHeightOffset = 0.0f;
	myIsStrikeableTrigger = false;
}

void Collision::OnCollide(Collision* aCollitionComponent)
{

	if (myEntity->GetEntityType() == EntityType::EnvironmentStatic)
	{
		if (CheckCollisionVsSubMeshes(aCollitionComponent))
		{
			aCollitionComponent->OnCollide(myEntity, myIsFriendly, myDamageToOthers);
		}
	}
	else
	{
		aCollitionComponent->OnCollide(myEntity, myIsFriendly, myDamageToOthers);
	}

}

void Collision::OnCollide(Entity* aCollidingEntity, bool aColliderIsFriendly, int aDamageValue)
{
	EntityType collidingEntityType = aCollidingEntity->GetEntityType();

	if (myEntity->GetEntityType() == EntityType::EnvironmentStatic)
	{
		return;
	}

	if (myEntity->GetEntityType() == EntityType::TriggerBox)
	{
		if (GetTriggerID() == 666 && collidingEntityType == EntityType::Enemy)
		{
			aCollidingEntity->SetIsAlive(false);
			return;
		}


		mySetOfCollidingEntities.emplace(aCollidingEntity);
		if (mySetOfCollidedWithLastFrameEntities.count(aCollidingEntity) == 0)
		{
			mySetOfCollidedWithLastFrameEntities.emplace(aCollidingEntity);
			Message triggerEventMessage;
			triggerEventMessage.myMessageType = MessageType::TriggerEvent;
			triggerEventMessage.myIntValue = GetTriggerID();
			triggerEventMessage.myIntValue2 = CAST(int, collidingEntityType);
			triggerEventMessage.myBool = true;
			triggerEventMessage.myData = aCollidingEntity;
			SendMessages(triggerEventMessage);

			if (triggerEventMessage.myIntValue == 11111)
			{
				myEntity->SetIsAlive(false);
			}
		}

		return;
	}

	if (collidingEntityType == EntityType::TriggerBox)
	{
		return;
	}

	if (collidingEntityType == EntityType::EnvironmentStatic)
	{
		if (myEntity->GetEntityType() == EntityType::Projectile)
		{
			myEntity->SetIsAlive(false);

			return;
		}

		return;
	}
}

void Collision::Collide(CU::Ray<float>& aRay)
{
	V3F point;
	if (CU::IntersectionAABBRay(myBoxCollider, aRay, point))
	{
		myEntity->SendEntityMessage(EntityMessage::WasHit, &point);
	}

}

void Collision::SetCollisionRadius(float aRadius)
{
	myCollisionRadius = aRadius;
}

void Collision::SetStaticBoundingBox(CommonUtilities::Vector3<float> aMin, CommonUtilities::Vector3<float> aMax)
{
	myBoxCollider.Min() = aMin;
	myBoxCollider.Max() = aMax;
	myIsStatic = true;
}

void Collision::SetIsFriendly(bool aIsFriendly)
{
	myIsFriendly = aIsFriendly;
}

bool Collision::GetIsParentAlive()
{
	if (myEntity)
	{
		return myEntity->GetIsAlive();
	}

	return false;
}

bool Collision::IsFriendly() const
{
	return myIsFriendly;
}

bool Collision::IsAABB()
{
	return !myIsPlaneVolume;
}

bool Collision::IsInside(V3F aPoint)
{
	if (myIsPlaneVolume)
	{
		bool result = GetPlaneVolume().Inside(aPoint);
		auto volume = GetPlaneVolume();
		auto planes = volume.Planes();
		for (size_t i = 0; i < planes.size(); i++)
		{
			mySides[i] = planes[i].Inside(aPoint);
		}

		return result;
	}
	return myBoxCollider.IsInside(aPoint);
}

CommonUtilities::AABB3D<float>* Collision::GetBoxCollider()
{
	if (!myIsStatic && !myBoxIsCalculated)
	{
		myBoxCollider.Min() = myPos - CommonUtilities::Vector3<float>(myCollisionRadius, myCollisionRadius, myCollisionRadius);
		myBoxCollider.Max() = myPos + CommonUtilities::Vector3<float>(myCollisionRadius, myCollisionRadius, myCollisionRadius);

		myBoxIsCalculated = true;
	}

	return &myBoxCollider;
}

void Collision::SetPlanevolume(std::vector<V3F> aPoints, V3F aCenter)
{
	myPoints = aPoints;
	myCenter = aCenter;
	mySides.resize(myPoints.size());
	myIsPlaneVolume = true;
}

CommonUtilities::PlaneVolume<float> Collision::GetPlaneVolume()
{
	CommonUtilities::PlaneVolume<float> out;

	for (size_t i = 0; i < myPoints.size(); i++)
	{
		CommonUtilities::Plane<float> p;
		p.InitWith3Points(myPoints[i], myPoints[i] + V3F(0, 1m, 0), myPoints[(i + 1) % myPoints.size()]);
		out.AddPlane(p);
	}

	return out;
}

V3F Collision::GetCenter()
{
	if (myIsPlaneVolume)
	{
		return myCenter;
	}
	return myBoxCollider.Min() + ((myBoxCollider.Max() - myBoxCollider.Min()) / 2.0f);
}

bool Collision::GetIsMoving()
{
	return myEntity->GetIsMoving();
}

void Collision::SetTriggerID(int aID)
{
	myTriggerID = aID;
}

int Collision::GetEntityID()
{
	return myEntity->GetEntityID();
}

int Collision::GetTriggerID()
{
	return myTriggerID;
}

Entity* Collision::GetParentEntityPtr()
{
	return myEntity;
}

void Collision::SetDamageToOthers(int aDamageValue)
{
	myDamageToOthers = aDamageValue;
}

void Collision::SetIsColliding(bool aIsColliding)
{
	myIsColliding = aIsColliding;
}

bool Collision::GetIsColliding()
{
	return myIsColliding;
}

void Collision::SetHeightOffset(float aHeight)
{
	myHeightOffset = aHeight;
}

void Collision::SetIsStrikeableTrigger(bool aIsStrikeable)
{
	myIsStrikeableTrigger = aIsStrikeable;
}

bool Collision::GetIsStrikeableTrigger()
{
	return myIsStrikeableTrigger;
}

bool Collision::IsActive() const
{
	return myIsActive;
}

void Collision::SetIsActive(const bool aFlag)
{
	myIsActive = aFlag;
}

void Collision::OnAttach()
{
}

void Collision::OnDetach()
{
}

void Collision::OnKillMe()
{
}

bool Collision::CheckCollisionVsSubMeshes(Collision* aCollitionComponent)
{
	Mesh* mesh = (Mesh*)myEntity->GetComponent<Mesh>();
	auto model = mesh->GetModelInstance()->GetModelAsset().GetAsModel();

	CommonUtilities::AABB3D<float> oobb;
	CommonUtilities::Vector3<float> oobbPos;

	auto box = aCollitionComponent->GetBoxCollider();
	auto pos = box->Min() + (box->Max() - box->Min()) / 2.0f;

	CommonUtilities::Vector3<float> entityPos = myEntity->GetPosition();
	auto rotMatrix = myEntity->GetRotation();
	CommonUtilities::Vector3<float> scale = myEntity->GetScale();

	auto collisionRad = aCollitionComponent->myCollisionRadius;

	std::vector<V3F> positionsToCheck;
	if (aCollitionComponent->GetParentEntityType() == EntityType::Player)
	{
		collisionRad = 1.0f;
		auto playerEntity = aCollitionComponent->GetParentEntityPtr();
		Mesh* playerMesh = (Mesh*)playerEntity->GetComponent<Mesh>();
		auto playerModelMatrix = playerMesh->GetModelInstance()->GetModelToWorldTransform();
		V4F point1 = V4F(-6.0f, -1.0f, -2.0f, 1.0f) * playerModelMatrix;
		V4F point2 = V4F(-6.0f, -1.0f, 2.0f, 1.0f) * playerModelMatrix;
		V4F point3 = V4F(-6.0f, 1.0f, -2.0f, 1.0f) * playerModelMatrix;
		V4F point4 = V4F(-6.0f, 1.0f, 2.0f, 1.0f) * playerModelMatrix;
		V4F point5 = V4F(6.0f, -1.0f, -2.0f, 1.0f) * playerModelMatrix;
		V4F point6 = V4F(6.0f, -1.0f, 2.0f, 1.0f) * playerModelMatrix;
		V4F point7 = V4F(6.0f, 1.0f, -2.0f, 1.0f) * playerModelMatrix;
		V4F point8 = V4F(6.0f, 1.0f, 2.0f, 1.0f) * playerModelMatrix;
		V4F point9 = V4F(pos.x, pos.y, pos.z, 1.0f);

		positionsToCheck.push_back(point9);
		positionsToCheck.push_back(point1);
		positionsToCheck.push_back(point2);
		positionsToCheck.push_back(point3);
		positionsToCheck.push_back(point4);
		positionsToCheck.push_back(point5);
		positionsToCheck.push_back(point6);
		positionsToCheck.push_back(point7);
		positionsToCheck.push_back(point8);

	}
	else
	{
		positionsToCheck.push_back(pos);
	}


	CommonUtilities::Matrix4x4<float> mat;
	for (int index = 0; index < positionsToCheck.size(); index++)
	{
		pos = positionsToCheck[index];

		CommonUtilities::Vector3<float> PosInObjectSpace = pos - entityPos;
		auto pos4InObjectSpace = CommonUtilities::Vector4<float>(PosInObjectSpace, 0.0f);

		mat = CommonUtilities::Matrix4x4<float>::Identity();
		mat = mat * CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(myEntity->GetSavedRotationValues().x);
		mat = mat * CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(myEntity->GetSavedRotationValues().y);
		mat = mat * CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(myEntity->GetSavedRotationValues().z);

		mat = CommonUtilities::Matrix4x4<float>::GetFastInverse(mat);

		CommonUtilities::Vector3<float> aRotatedPosInObjectSpace = pos4InObjectSpace * CommonUtilities::Matrix4x4<float>::GetFastInverse(myEntity->GetRotation());
		aRotatedPosInObjectSpace = pos4InObjectSpace * mat;
		//CommonUtilities::Vector3<float> aTestRotatedPosInObjectSpace = CommonUtilities::Vector3<float>(aRotatedPosInObjectSpace.y, -aRotatedPosInObjectSpace.z, -aRotatedPosInObjectSpace.x);

		for (auto& it : model->myCollisions)
		{
			oobb = it;
			oobbPos = oobb.Min() + (oobb.Max() - oobb.Min()) / 2.0f;

			oobb.Min().x *= scale.x;
			oobb.Max().x *= scale.x;
			oobb.Min().y *= scale.y;
			oobb.Max().y *= scale.y;
			oobb.Min().z *= scale.z;
			oobb.Max().z *= scale.z;


			//if (CheckCollisionVsOOBB(oobb, aRotatedPosInObjectSpace, collisionRad))
			if (CheckCollisionVsOOBB(oobb, aRotatedPosInObjectSpace, collisionRad))
			{
				SYSINFO("Collided with: " + model->GetFriendlyName());
				return true;
			}
		}
	}
	return false;
}

bool Collision::CheckCollisionVsOOBB(CommonUtilities::AABB3D<float> aAABB, CommonUtilities::Vector3<float> aRotatedPosInObjectSpace, float aCollisionRadius)
{
	CommonUtilities::Vector3<float> closestPos;
	closestPos.x = CLAMP(aAABB.Min().x, aAABB.Max().x, aRotatedPosInObjectSpace.x);
	closestPos.y = CLAMP(aAABB.Min().y, aAABB.Max().y, aRotatedPosInObjectSpace.y);
	closestPos.z = CLAMP(aAABB.Min().z, aAABB.Max().z, aRotatedPosInObjectSpace.z);

	if ((aRotatedPosInObjectSpace - closestPos).LengthSqr() < (aCollisionRadius * aCollisionRadius))
	{
		return true;
	}
	return false;
}
