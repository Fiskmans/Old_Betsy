#include "pch.h"
#include "NodeGetPlayerPosition.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/Entity.h"
#include "PathFinder.h"
#include "SlabRay.h"

GetPlayerPosition::GetPlayerPosition()
{
	myPins.push_back(CPin("Result", CPin::PinTypeInOut::PinTypeInOut_OUT,  CPin::PinType::Data));
	SetPinType<V3F>(0);
}

int GetPlayerPosition::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	Entity* player = aTriggeringNodeInstance->ourPollingStation->GetPlayer();
	if (!player)
	{
		return -1;
	}

	V3F rayStart = player->GetPosition();
	rayStart.y += 400;

	V3F rayDir = player->GetPosition() - rayStart;

	V3F landPoint;

	int iterations = 0;

	rayDir.Normalize();

	SlabRay ray(rayStart, rayDir);
	landPoint = PathFinder::GetInstance().FindPoint(ray);

	V3F temp;
	if (landPoint.LengthSqr() > 0)
	{
		temp = landPoint;
	}
	else
	{
		temp = player->GetPosition();
	}

	aTriggeringNodeInstance->WriteData(0, temp);

	return -1;
}
