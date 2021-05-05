#pragma once
#include "Component.h"

class GBPhysXActor;
class Mesh;

class GBPhysXComponent : public Component
{
public:

	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void AttachGBPhysXActor(GBPhysXActor* aActor);
	GBPhysXActor* GetGBPhysXActor();
	void AddForceToActor(V3F aDirection, float aMagnitude);
protected:

private:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

	GBPhysXActor* myGBPhysXActor = nullptr;
	Mesh* myMeshPtr = nullptr;
};

