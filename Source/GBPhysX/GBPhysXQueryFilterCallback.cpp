#include <pch.h>
#pragma warning(push, 1)
#pragma warning(disable : 26495)
#include "GBPhysXQueryFilterCallback.h"
#pragma warning(pop)
#include "GBPhysX.h"

PxQueryHitType::Enum GBPhysXQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	PX_UNUSED(actor);
	PX_UNUSED(queryFlags);
	int word = shape->getQueryFilterData().word0;
	if ((shape->getQueryFilterData().word0 & filterData.word1) && (filterData.word0 & shape->getQueryFilterData().word1))
	{
		return PxQueryHitType::eBLOCK;
	}
	return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum GBPhysXQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	PxShape* shape = hit.shape;

	if ((filterData.word0 & shape->getQueryFilterData().word1) && (filterData.word1 & shape->getQueryFilterData().word0))
	{
		return PxQueryHitType::eBLOCK;
	}
	return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum GBPhysXBulletQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	PX_UNUSED(actor);
	PX_UNUSED(queryFlags);
	int shapeWord = shape->getQueryFilterData().word0;
	if ((shape->getQueryFilterData().word0 & filterData.word0) == 0)
	{
		return PxQueryHitType::eNONE;
	}
}

PxQueryHitType::Enum GBPhysXBulletQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	PxShape* shape = hit.shape;

	int shapeWord = shape->getQueryFilterData().word0;
	if (filterData.word0 & shape->getQueryFilterData().word0)
	{
		return PxQueryHitType::eBLOCK;
	}
	return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum GBPhysXInteractQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	PX_UNUSED(actor);
	PX_UNUSED(queryFlags);
	int shapeWord = shape->getQueryFilterData().word0;
	if ((shape->getQueryFilterData().word0 & filterData.word0) == 0)
	{
		return PxQueryHitType::eNONE;
	}
}

PxQueryHitType::Enum GBPhysXInteractQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	PxShape* shape = hit.shape;

	//SYSINFO("used bulletqueryfilter");
	int shapeWord = shape->getQueryFilterData().word0;
	if (filterData.word0 & shape->getQueryFilterData().word0)
	{
		return PxQueryHitType::eBLOCK;
	}
	return PxQueryHitType::eNONE;
}

bool GBPhysXCCTQueryFilterCallback::filter(const PxController& a, const PxController& b)
{
	GBPhysXCharacter* char1 = CAST(GBPhysXCharacter*,a.getUserData());
	GBPhysXCharacter* char2 = CAST(GBPhysXCharacter*, b.getUserData());
	if (char1->GetHasBeenReleased() || char2->GetHasBeenReleased()) 
	{
		return false;
	}
	else 
	{
		return true;
	}
}
