#pragma once

class Entity;

class Component
{
public:
	struct FrameData
	{
		float myDeltaTime;
		float myTotalTime;
	};

	virtual ~Component() = default;

	virtual void Update(const FrameData& aFrameData, Entity* aEntity) = 0;
};

