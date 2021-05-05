#pragma once
#include "Vector.hpp"
#include "../Game/Enums.h"
#include "Logger.h"

class Entity;
class GameObjectId
{
public:
	GameObjectId() : myInteral(nullptr) {};
	GameObjectId(Entity* aEntity) :myInteral(aEntity) {};
	Entity* myInteral;
};

typedef size_t TimerId;
