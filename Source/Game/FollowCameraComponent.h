#pragma once
#include "Component.h"
#include "Vector.hpp"

class FollowCamera :
	public Component
{

public:
	FollowCamera();
	virtual ~FollowCamera();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

private:
	CommonUtilities::Vector3<float> myOffsetFromTarget;
	
};

