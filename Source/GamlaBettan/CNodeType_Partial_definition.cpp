#include "pch.h"
#include "CNodeType.h"
#include "CNodeInstance.h"


CNodeType* CNodeTypeCollector::myTypes[128];
unsigned short CNodeTypeCollector::myTypeCounter = 0;
unsigned short CNodeTypeCollector::myTypeCount = 0;

std::vector<unsigned int> UID::myAllUIDs;
unsigned int UID::myGlobalUID = 0;




int CNodeType::DoEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	int index = OnEnter(aTriggeringNodeInstance, aEnteredVia);
	aTriggeringNodeInstance->myEnteredTimer = 3.0f;
	return index;
}

std::vector<CPin> CNodeType::GetPins()
{
	return myPins;
}
