#pragma once
#include "Component.h"
#include "Observer.hpp"
#include <MetricValue.h>

class Camera;

class FPSCamera :
	public Component, public Observer
{

public:
	FPSCamera();
	virtual ~FPSCamera();

	virtual void Init(Entity* aEntity) override;
	
	void SetCamera(Camera* aCamera);

	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void RecieveMessage(const Message& aMessage) override;

	void ZoomIn();
	void ZoomOut();

	void SetTargetFOV(float aSpeed, float aFOV);

	void SetOffset(const V3F& anOffset);

	Camera* GetCamera();

	void RotateCameraFromMouse(const V2F& aMouseMovement);

	GAMEMETRIC(float, myMaxVerticalAngle, PLAYERCAMERAMAXVERTICAL, 0.1f);
	GAMEMETRIC(float, myBaseFOV, FPSBASEFOV, 90.f);
	GAMEMETRIC(float, myZoomInFOV, FPSZOOMINFOV, 40.f);
	GAMEMETRIC(float, myZoomSpeed, FPSZOOMSPEED, 200.f);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	Entity* myEntity;
	Camera* myCamera;

	V3F myOffset;

	float myTargetFOV;
	bool myIsChangingFOV;
};