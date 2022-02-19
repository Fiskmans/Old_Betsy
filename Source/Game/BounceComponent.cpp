#include "pch.h"
#include "BounceComponent.h"
#include "GamlaBettan\EntityManager.h"

BounceComponent::BounceComponent()
{
}

BounceComponent::BounceComponent(const FiskJSON::Object& aObject)
{
	aObject["speed"].GetIf(mySpeed);
}

BounceComponent::BounceComponent(Component::UseDefaults&)
{
}

void BounceComponent::Update(const FrameData& aFrameData, EntityID aEntityID)
{
	float height = CLAMP(0, 1, sin(aFrameData.myTotalTime * mySpeed)) * 200;
	EntityManager::GetInstance().Retrieve(aEntityID)->myTransform.Row(3) = V4F(0, height, 0, 1);

}

#if USEIMGUI
void BounceComponent::ImGui(EntityID /*aEntityID*/)
{
	ImGui::InputFloat("speed", &mySpeed);
}

void BounceComponent::Serialize(FiskJSON::Object& aObject)
{
	aObject.AddValueChild("speed", mySpeed);
}
#endif
