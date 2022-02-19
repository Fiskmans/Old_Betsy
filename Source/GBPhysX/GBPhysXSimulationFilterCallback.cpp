#include "pch.h"
#pragma warning(push, 1)
#pragma warning(disable : 26495)
#include "GBPhysXSimulationFilterCallback.h"
#pragma warning(pop)

PxFilterFlags GBPhysXSimulationFilterCallback::pairFound(PxU32 /*pairID*/, PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/, const PxActor* /*a0*/, const PxShape* /*s0*/, PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/, const PxActor* /*a1*/, const PxShape* /*s1*/, PxPairFlags& /*pairFlags*/)
{
	return PxFilterFlag::eCALLBACK;
}

void GBPhysXSimulationFilterCallback::pairLost(PxU32 /*pairID*/, PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/, PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/, bool /*objectRemoved*/)
{
}

bool GBPhysXSimulationFilterCallback::statusChange(PxU32& /*pairID*/, PxPairFlags& /*pairFlags*/, PxFilterFlags& /*filterFlags*/)
{
	return false;
}
