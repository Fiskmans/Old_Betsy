#include "pch.h"
#include "CNodeInstance.h"
#include "NodeChangeInteractableMesh.h"
#include "NodeDataTypes.h"
#include "GBPhysXStaticComponent.h"

GBPhysX* NodeChangeInteractableMesh::ourGBPhysXPtr = nullptr;

NodeChangeInteractableMesh::NodeChangeInteractableMesh()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));

	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Mesh path", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	myPins.push_back(CPin("Interactable ID", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Data));

	SetPinType<GameObjectId>(2);
	SetPinType<std::string>(3);
	SetPinType<GameObjectId>(4);
}

int NodeChangeInteractableMesh::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	GameObjectId id;
	std::string path;

	if (!aTriggeringNodeInstance->ReadData(3, path)) { return -1; }
	if (!aTriggeringNodeInstance->ReadData(2, id)) { return -1; }
	aTriggeringNodeInstance->WriteData(4, id);

	if (id.myInteral)
	{
		Entity* ent = reinterpret_cast<Entity*>(id.myInteral);
		auto mesh = ent->GetComponent<Mesh>();

		if (mesh)
		{
			mesh->Reset();
			mesh->Init(ent);
			mesh->SetUpModel(path);
			mesh->GetModelInstance()->SetRotation(ent->GetRotation());
			mesh->GetModelInstance()->SetPosition(V4F(ent->GetPosition(), 0));
			mesh->SetScale(ent->GetScale());
			mesh->AddModelToScene();
			//mesh->GetModelInstance()->SetIsHighlighted(true);
			mesh->Disable();
		}

		auto collider = ent->GetComponent<GBPhysXStaticComponent>();
		if (collider)
		{
			collider->Reset();
			collider->Init(ent);
			collider->SetGBPhysXPtr(ourGBPhysXPtr);
			collider->SetFilePath(path);
		}

		if (path == "P_firePlace_02/P_firePlace_02.fbx")
		{
			ent->AddComponent<ParticlesWhileAlive>()->Init(ent);
			ent->GetComponent<ParticlesWhileAlive>()->SetParticle("FireSmoke.part", V3F(0, 0, 0), V3F(1, 1, 1), V3F(25, 75, 10));
			ent->GetComponent<ParticlesWhileAlive>()->SetParticle("Fire.part", V3F(0, 0, 0), V3F(1, 1, 1), V3F(25, 75, 10));
			ent->AddComponent<Audio>()->Init(ent);
			ent->GetComponent<Audio>()->InternalInit(AudioComponentType::FirePlace);
			ent->GetComponent<Audio>()->PostAudioEvent(AudioEvent::LoopPlay);
		}
	}

	return 1;
}

void NodeChangeInteractableMesh::SetGBPhysX(GBPhysX* aGBPhysX)
{
	ourGBPhysXPtr = aGBPhysX;
}
