#pragma once
#include "Component.h"
#include "Entity.h"
class Camera;

class CameraLookAt : public Component
{
public:
	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	void SetCamera(Camera* aCamera);


protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	Camera* myCamera = nullptr;
};

