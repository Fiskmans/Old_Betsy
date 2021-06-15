#pragma once
#include "GamlaBettan\EntityID.h"
#include "GamlaBettan\InputHandler.h"

class Component
{
public:
	struct UseDefaults {};

	struct FrameData
	{
		CommonUtilities::InputHandler& myInputHandler;
		float myDeltaTime;
		float myTotalTime;
	};

	virtual ~Component() = default;

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) = 0;

#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) = 0;
#endif

};

