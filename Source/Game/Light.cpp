#include "pch.h"
#include "Light.h"
#include <Scene.h>
#include "Entity.h"

#include <DebugTools.h>
#ifdef _DEBUG
#include <ModelLoader.h>
#include <ModelInstance.h>
#endif // _DEBUG
#include <LightLoader.h>
#include <PointLight.h>
#include "Random.h"

Light::Light()
{
	myOffset = V3F();
}

Light::~Light()
{
#ifdef _DEBUG
	myScene->RemoveModel(myLight->myModel);
	SAFE_DELETE(myLight->myModel);
#endif  
	SAFE_DELETE(myLight);

}

void Light::PreInit(Scene* aScene, LightLoader* aLightLoader)
{
	myScene = aScene;
	myLight = new PointLight();
	myOnPeriod = false;
	myPeriodTime = 0.0f;
	myMinIntensityPercentage = 0.0f;
}

void Light::Init(Entity* aEntity)
{
	myEntity = aEntity;
	myScene->AddPointLight(myLight);

	myLight->color = myColor;
	myLight->intensity = myIntensity;
	myLight->range = myRange;
	myLight->position = myEntity->GetPosition();

}

void Light::Update(const float aDeltaTime)
{
	myLight->position = myEntity->GetPosition() + myOffset;
	if (myOnPeriod)
	{
		if (myPeriodDirection > 0.0f)
		{
			myPeriodTimer += aDeltaTime;
			if (myPeriodTimer > myPeriodTime)
			{
				myPeriodTimer = myPeriodTime;
				myPeriodDirection *= -1;
			}
		}
		else
		{
			myPeriodTimer -= aDeltaTime;
			if (myPeriodTimer < 0.0f)
			{
				myPeriodTimer = 0.0f;
				myPeriodDirection *= -1;
			}
		}

		float currentIntensityPercent = max(myMinIntensityPercentage, myPeriodTimer / myPeriodTime);
		//float currentIntensityPercent = myPeriodTimer;
	/*	float a = sin(currentIntensityPercent);
		float b = cos(currentIntensityPercent);
		float c = a * pow(b, 1.2) + b * 0.5f;*/
		myLight->intensity = mySavedIntensity * currentIntensityPercent;
	}
#ifdef _DEBUG
	if (myLight->myModel)
	{
		myLight->myModel->SetPosition(V4F(myEntity->GetPosition(),1));
	}
#endif // _DEBUG

}

void Light::Reset()
{
	myScene->RemovePointLight(myLight);
#ifdef _DEBUG
	if (myLight->myModel)
	{
		myScene->RemoveModel(myLight->myModel);
	}
#endif // _DEBUG

	myOnPeriod = false;
	myPeriodTime = 0.0f;
	myPeriodTimer = 0.0f;
	myPeriodDirection = 1.0f;
	myMinIntensityPercentage = 0.0f;
}

void Light::SetRange(float aRange)
{
	myLight->range = aRange;
}

void Light::SetIntensity(float aIntensity)
{
	myLight->intensity = aIntensity;
	mySavedIntensity = aIntensity;
}

void Light::SetPeriod(float aPeriod)
{
	if (aPeriod > 0.0f)
	{
		myOnPeriod = true;
		myPeriodTime = aPeriod;
		myPeriodTimer = Tools::RandomNormalized() * aPeriod;
		myPeriodDirection = 1.0f;
	}
}

void Light::SetColor(const CommonUtilities::Vector3<float>& aColor)
{
	myLight->color = aColor;
}

void Light::SetMinIntensity(float aMinIntensityPercent)
{
	myMinIntensityPercentage = aMinIntensityPercent;
}

void Light::SetOffset(V3F aOffset)
{
	myOffset = aOffset;
}

void Light::SetID(int aID)
{
	myID = aID;
}

int Light::GetID()
{
	return myID;
}

void Light::OnAttach()
{
}

void Light::OnDetach()
{
}

void Light::OnKillMe()
{
}
