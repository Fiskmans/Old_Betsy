#pragma once
#include "Component.h"
#include <MetricValue.h>
#include "Macros.h"
#include <Vector3.hpp>

class PointLight;
class Scene;
class LightLoader;

class Light : public Component
{
public:
	Light();
	~Light();
	// Inherited via Component

	void PreInit(Scene* aScene, LightLoader* aLightLoader);
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;


	void SetRange(float aRange);
	void SetIntensity(float aIntensity);
	void SetPeriod(float aPeriod);
	void SetColor(const CommonUtilities::Vector3<float>& aColor);
	void SetMinIntensity(float aMinIntensityPercent);
	void SetOffset(V3F aOffset);

	void SetID(int aID);
	int GetID();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	
	GAMEMETRIC(float, myRange, SHOTLIGHTRANGE, 40);
	GAMEMETRIC(float, myIntensity, SHOTLIGHTINTENSITY, 10);
	GAMEMETRIC(V3F, myColor, SHOTLIGHTCOLOR, V3F(1,1,0));
	
	float myPeriodTime;
	float myPeriodTimer;
	float myPeriodDirection;

	Scene* myScene;
	PointLight* myLight;

	bool myOnPeriod;
	float mySavedIntensity;
	float myMinIntensityPercentage;

	V3F myOffset;

	int myID;
};

