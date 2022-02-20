#include "pch.h"
#include "Entity.h"
#include "ComponentManager.h"

#if USEIMGUI
void Entity::Serialize(FiskJSON::Object& aObject) const
{
	ComponentManager::GetInstance().Serialize(myID, aObject);
}
#endif