#pragma once
#include "Component.h"
#include "Observer.hpp"

class Camera;

class FollowCamera :
	public Component, public Observer
{

public:
	FollowCamera();
	virtual ~FollowCamera();

	virtual void Init(Entity* aEntity) override;
	void SetTargetEntity(Entity* aTargetEntity);
	void SetTargetCamera(Camera* aCamera);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void RecieveMessage(const Message& aMessage) override;
	void SetCameraTargetFov(int aFov);
	void SetCameraTargetOffset(int aZOffset, int aYOffset);
	Camera* GetCamera();
protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	Entity* myEntity;

	Entity* myTargetEntity;
	Entity* myOffsetEntity;
	GAMEMETRIC(float,myOffsetFromTargetX,CAMERADISTANCEFROMPLAYERX,0.f);
	GAMEMETRIC(float,myOffsetFromTargetY,CAMERADISTANCEFROMPLAYERY,7.f);
	GAMEMETRIC(float,myOffsetFromTargetZ,CAMERADISTANCEFROMPLAYERZ,-5.f);

	GAMEMETRIC(float, myFollowPowerX, CAMERAFOLLOWX, 1.f);
	GAMEMETRIC(float, myFollowPowerY, CAMERAFOLLOWY, 1.f);

	Camera* myCamera;
	float myCameraShakeStartMovementValue = 1.5f;
	float myCameraShakeEndMovementValue = 0.001f;
	float myCameraShakeCurrentMovementValue = 0.001f;

	int myTargetFov = 55;
	bool myIsChangingFov = false;
	V3F myTargetOffset;
	V3F myCurrentOffset;
	V3F myDefaultCameraOffset;
};

