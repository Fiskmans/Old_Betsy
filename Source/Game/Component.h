#pragma once

#include <typeindex>
#include "EntityMessage.h"

struct ImGuiNodePackage;
class Entity;
enum class EntityType;

class ComponentLake;

class Component
{
	friend class Entity;
	friend class ComponentLake;
public:
	Component();
	virtual ~Component();

	virtual void Init(Entity* aEntity) = 0;
	virtual void Update(const float aDeltaTime) = 0;
	virtual void Reset() = 0;

	virtual void InteractWith(Item* aHeldItem);

	void Enable();
	void Disable();

	bool IsEnabled() const;
	void SetParent(Entity* aParentEntity);
#if USEIMGUI
	virtual void ImGuiNode(ImGuiNodePackage& aPackage);
#endif // !_RETAIL

	virtual EntityType GetParentEntityType();

	virtual void Collided(int aDamageValue);
	virtual void SubscribeToMyMessages();
	virtual void UnSubscribeToMyMessages();
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData);


protected:
	virtual void OnAttach();
	virtual void OnDetach();
	virtual void OnKillMe();


	virtual void OnEnable();
	virtual void OnDisable();

	bool myEnabled = false;

	Entity* myEntity;
	std::string myType;
	std::type_index myTypeIndex;
private:

};

