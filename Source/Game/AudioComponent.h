#pragma once

#include "Component.h"
#include "Vector.hpp"
	

class AudioComponent : public Component
{
public:
	AudioComponent();
	~AudioComponent();

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;


private:

};