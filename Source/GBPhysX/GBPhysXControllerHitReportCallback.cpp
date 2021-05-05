#include "pch.h"
#pragma warning(push, 1)
#pragma warning(disable : 26495)
#include "GBPhysXControllerHitReportCallback.h"
#include "PxRigidActor.h"
#pragma warning(pop)
#include "GBPhysX.h"

void GBPhysXControllerHitReportCallback::onShapeHit(const PxControllerShapeHit& hit)
{
	//TODO Here we ge hits from static objects vs controller

	/*std::cout << hit.dir.x << std::endl;
	std::cout << hit.dir.y << std::endl;
	std::cout << hit.dir.z << std::endl;
*/

	if (hit.dir.y < -0.9f)
	{
		((GBPhysXCharacter*)hit.controller->getUserData())->SetIsGrounded(true);
	}
}

void GBPhysXControllerHitReportCallback::onControllerHit(const PxControllersHit& hit)
{
}

void GBPhysXControllerHitReportCallback::onObstacleHit(const PxControllerObstacleHit& hit)
{
}
