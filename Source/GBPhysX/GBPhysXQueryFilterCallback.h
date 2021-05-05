#pragma once
#include "PxQueryFiltering.h"
#include "PxShape.h"
#include "PxRigidActor.h"
#include "include/characterkinematic/PxController.h"
using namespace physx;

class GBPhysXQueryFilterCallback : public PxQueryFilterCallback
{
	// Inherited via PxQueryFilterCallback
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;
};

class GBPhysXBulletQueryFilterCallback : public PxQueryFilterCallback 
{
	// Inherited via PxQueryFilterCallback
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;
};

class GBPhysXInteractQueryFilterCallback : public PxQueryFilterCallback
{
	// Inherited via PxQueryFilterCallback
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;
};

class GBPhysXCCTQueryFilterCallback : public PxControllerFilterCallback 
{
	virtual bool filter(const PxController& a, const PxController& b) override;
};