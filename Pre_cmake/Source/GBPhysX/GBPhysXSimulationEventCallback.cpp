#include "pch.h"
#pragma warning(push, 1)
#pragma warning(disable : 26495)
#include "GBPhysXSimulationEventCallback.h"
#include <iostream>

void GBPhysXSimulationEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void GBPhysXSimulationEventCallback::onWake(PxActor** actors, PxU32 count)
{

}

void GBPhysXSimulationEventCallback::onSleep(PxActor** actors, PxU32 count)
{

}

void GBPhysXSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{

	//TODO Here we get contact from dynamic vs other

	//for (int i = 0; i < nbPairs; i++)
	//{
	//	auto actor1 = pairHeader.actors[0];
	//	if (actor1)
	//		std::cout << actor1->getName() << std::endl;
	//	else
	//		std::cout << "NULL" << std::endl;
	//	
	//	
	//	auto actor2 = pairHeader.actors[1];
	//	if (actor2)
	//		std::cout << actor2->getName() << std::endl;
	//	else
	//		std::cout << "NULL" << std::endl;
	//}
}

void GBPhysXSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{


}

void GBPhysXSimulationEventCallback::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
#pragma warning(pop)