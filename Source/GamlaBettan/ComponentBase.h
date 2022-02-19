#pragma once
#include "GamlaBettan\EntityID.h"
#include "GamlaBettan\InputHandler.h"
#include "FiskJSON.h"

class ComponentBase
{
public:
	struct UseDefaults {};

	struct FrameData
	{
		CommonUtilities::InputHandler& myInputHandler;
		float myDeltaTime;
		float myTotalTime;
	};

	virtual ~ComponentBase() = default;

	virtual void Update(const FrameData& aFrameData, EntityID aEntityID) = 0;

#if USEIMGUI
	virtual void ImGui(EntityID aEntityID) = 0;
	virtual void Serialize(FiskJSON::Object& aObject) = 0;
#endif
};