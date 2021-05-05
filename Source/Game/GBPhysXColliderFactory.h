#pragma once
#include "Observer.hpp"
#include "Vector3.hpp"
#include "GBPhysX.h"

class GBPhysXColliderFactory : public Observer
{
public:

	GBPhysXColliderFactory();
	~GBPhysXColliderFactory();

	void Init(GBPhysX* aGBPhysXPtr);
	void CreateGBPhysXCollider(CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale);

private:
	virtual void RecieveMessage(const Message& aMessage) override;
	GBPhysX* myGBPhysX;
};

