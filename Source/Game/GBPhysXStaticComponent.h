#pragma once
#include "Component.h"

class GBPhysX;
class GBPhysXActor;
class Mesh;

class GBPhysXStaticComponent : public Component
{
public:
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetShapeName(const char* aName);
	void SetFilePath(const std::string & aFilePath);
	void AttachGBPhysXActor(GBPhysXActor* aActor);
	GBPhysXActor* GetGBPhysXActor();

	void SetGBPhysXPtr(GBPhysX* aGBPhysX);
protected:

private:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

	GBPhysX* myGBPhysX;
	GBPhysXActor* myGBPhysXActor = nullptr;
	Mesh* myMeshPtr = nullptr;
	bool myHasSpawnedColliderToWorld = false;
	std::string myFilePath = "";
	const char* myPendingShapeNameChange = nullptr;
	bool myFilePathSet = false;
};