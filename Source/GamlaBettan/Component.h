#pragma once
#include "GamlaBettan\EntityID.h"

class Component
{
public:
	struct UseDefaults {};

	struct FrameData
	{
		float myDeltaTime;
		float myTotalTime;
	};

	virtual ~Component() = default;

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) = 0;

#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) = 0;
#endif

};

