#pragma once
#include "characterkinematic/PxController.h"

using namespace physx;
class GBPhysXControllerHitReportCallback : public PxUserControllerHitReport
{
	// Inherited via PxUserControllerHitReport
	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const PxControllersHit& hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;
};


