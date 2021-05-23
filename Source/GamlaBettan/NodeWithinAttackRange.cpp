#include "pch.h"
#include "NodeWithinAttackRange.h"
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "..//Game/Entity.h"
#include "..//Game/CharacterInstance.h"

NodeWithinAttackRange::NodeWithinAttackRange()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Position", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("True", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("False", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<V3F>(1);
	SetPinType<int>(4);
}

int NodeWithinAttackRange::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	assert(false && L"AI-Node not supported");

	/*GameObjectId id;
	V3F position = {0,0,0};

	if (!aTriggeringNodeInstance->ReadData(4, id)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(1, position)) { return -1; }

	if (id.myInteral)
	{
		Entity* entity = id.myInteral;

		if (entity)
		{
			V3F distance = position - entity->GetPosition();
			distance.y = 0;
			float lengthSqrd = distance.LengthSqr();

			Stats* stats = entity->GetComponent<CharacterInstance>()->GetCharacterStats();


			if (stats->rangedAttack != 0)
			{
				if (!CheckAbilityRangeToFar(entity, stats->rangedAttack, lengthSqrd))
				{
					if (!CheckAbilityRangeToClose(entity, stats->rangedAttack, lengthSqrd, 1.5f) || !entity->GetComponent<CharacterInstance>()->GetAbilityCooldownCounter(stats->rangedAttack))
					{
						return 2;
					}
				}
			}
			else if (stats->basicAttack != 0 && !CheckAbilityRangeToFar(entity, stats->basicAttack, lengthSqrd) && !CheckAbilityRangeToClose(entity, stats->basicAttack, lengthSqrd, 3.f))
			{
				return 2;
			}
		}
	}*/

	return 3;
}

bool NodeWithinAttackRange::CheckAbilityRangeToFar(Entity* aCastingEntity, int aAbilityID, float aRangeSqrd)
{
	/*if (aAbilityID != 0)
	{
		float attackrangeSqrd = aCastingEntity->GetComponent<CharacterInstance>()->GetAbilityRangeSqrd(aAbilityID);
		if (attackrangeSqrd >= aRangeSqrd)
		{
			return false;
		}
	}*/
	return true;
}

bool NodeWithinAttackRange::CheckAbilityRangeToClose(Entity* aCastingEntity, int aAbilityID, float aRangeSqrd, float closenessModifyer)
{
	/*if (aAbilityID != 0)
	{
		float attackrangeSqrd = aCastingEntity->GetComponent<CharacterInstance>()->GetAbilityRangeSqrd(aAbilityID);
		if (aRangeSqrd > attackrangeSqrd / closenessModifyer)
		{
			return false;
		}
	}*/
	return true;
}
