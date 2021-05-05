#include "pch.h"
#include "CNodeInstance.h"
#include "GBPhysXStaticComponent.h"
#include "NodeMoveInteractable.h"
#include "Model.h"

NodeMoveInteractable::NodeMoveInteractable()
{
	myPins.clear();

	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Position", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));

	SetPinType<GameObjectId>(2);
	SetPinType<V3F>(3);
	SetPinType<GameObjectId>(4);
}

int NodeMoveInteractable::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	V3F position;

	if (!aTriggeringNodeInstance->ReadData(3, position)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }
	aTriggeringNodeInstance->WriteData(4, id);

	if (id.myInteral)
	{
		Entity* ent = reinterpret_cast<Entity*>(id.myInteral);
		ent->SetPosition(position);

		auto mesh = ent->GetComponent<Mesh>();
		if (mesh)
		{
			mesh->GetModelInstance()->SetPosition(V4F(ent->GetPosition(), 1));

			auto collider = ent->GetComponent<GBPhysXStaticComponent>();
			if (collider)
			{
				collider->Reset();
				collider->Init(ent);
				collider->SetGBPhysXPtr(ourGBPhysXPtr);
				collider->SetFilePath(mesh->GetModelInstance()->GetModel()->GetModelData()->myFilePath);
			}
		}
	}

	return 1;
}
