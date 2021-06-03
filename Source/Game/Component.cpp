#include "pch.h"
#include "Component.h"
#include "Entity.h"
#include "ComponentLake.h"

#if USEIMGUI
#include "ImGuiPackage.h"
#endif // !_RETAIL

Component::Component():
	myTypeIndex(std::type_index(typeid(Component))),
	myEntity(nullptr)
{
}

Component::~Component()
{
}

#if USEIMGUI
void Component::ImGuiNode(ImGuiNodePackage& aPackage)
{
	ImGui::Text("Unknown component");
	/*NOOP*/
}
#endif // !_RETAIL

void Component::InteractWith(Item* aHeldItem)
{
	//NoOp
}

void Component::Enable()
{
	if (myEntity == nullptr || myEnabled == true)
	{
		return;
	}

	myEnabled = true;
	ComponentLake::GetInstance().EnableActiveComponent(myTypeIndex, this);

	OnEnable();
}

void Component::Disable()
{
	if (myEntity == nullptr || myEnabled == false)
	{
		return;
	}

	myEnabled = false;
	ComponentLake::GetInstance().DisableActiveComponent(myTypeIndex, this);

	OnDisable();
}

void Component::SetParent(Entity* aParentEntity)
{
	myEntity = aParentEntity;
}

bool Component::IsEnabled() const
{
	return myEnabled;
}

void Component::OnEnable()
{
}

void Component::OnDisable()
{
}

EntityType Component::GetParentEntityType()
{
	return myEntity->GetEntityType();
}

void Component::Collided(int aDamageValue)
{

}

void Component::SubscribeToMyMessages()
{
}

void Component::UnSubscribeToMyMessages()
{
}

void Component::RecieveEntityMessage(EntityMessage aMessage, void* someData)
{
	//NO-OP
}

void Component::OnAttach()
{
}

void Component::OnDetach()
{
}

void Component::OnKillMe()
{
}
