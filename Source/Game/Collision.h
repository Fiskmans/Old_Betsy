#pragma once
#include "Component.h"
#include "Publisher.hpp"
#include "CommonUtilities\Ray.hpp"

class Entity;
enum class EntityType;

namespace CommonUtilities
{
	template<typename T>
	class AABB3D;
}

class Collision :
	public Component, public Publisher
{
public:
	Collision();
	virtual ~Collision();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void OnCollide(Collision* aCollitionComponent);
	void OnCollide(Entity* aCollidingEntity, bool aColliderIsFriendly, int aDamageValue);
	void Collide(CommonUtilities::Ray<float>& aRay);
	
	void SetIsFriendly(bool aIsFriendly);
	void SetCollisionRadius(float aRadius);
	float myCollisionRadius;
	void SetStaticBoundingBox(CommonUtilities::Vector3<float> aMin, CommonUtilities::Vector3<float> aMax);

	CommonUtilities::Vector3<float> myPos;
	bool GetIsParentAlive();
	bool IsFriendly() const;
	bool IsAABB();
	bool IsInside(V3F aPoint);

	CommonUtilities::AABB3D<float>* GetBoxCollider();


	void SetPlanevolume(std::vector<V3F> aPoints,V3F aCenter);
	CommonUtilities::PlaneVolume<float> GetPlaneVolume();
	V3F GetCenter();

	bool GetIsMoving();

	int GetEntityID();
	void SetTriggerID(int aID);
	int GetTriggerID();
	Entity* GetParentEntityPtr();

	void SetDamageToOthers(int aDamageValue);

	void SetIsColliding(bool aIsColliding);
	bool GetIsColliding();

	void SetHeightOffset(float aHeight);

	void SetIsStrikeableTrigger(bool aIsStrikeable);
	bool GetIsStrikeableTrigger();

	bool IsActive() const;
	void SetIsActive(const bool aFlag);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:

	bool myIsPlaneVolume = false;
	bool myIsColliding;
	bool myWasCollidingWithPlayerLastFrame;
	std::set<Entity*> mySetOfCollidingEntities;
	std::set<Entity*> mySetOfCollidedWithLastFrameEntities;
	std::vector<V3F> myPoints;
	std::vector<bool> mySides;
	V3F myCenter;

	bool CheckCollisionVsSubMeshes(Collision* aCollitionComponent);
	bool CheckCollisionVsOOBB(CommonUtilities::AABB3D<float> aAABB, CommonUtilities::Vector3<float> aRotatedPos, float aCollisionRange);
	CommonUtilities::AABB3D<float> myBoxCollider;
	bool myIsFriendly;
	bool myIsStatic;
	int myTriggerID;
	int myDamageToOthers;
	float myHeightOffset = 0.0f;
	bool myIsStrikeableTrigger;
	bool myBoxIsCalculated;
	bool myIsActive;
};

