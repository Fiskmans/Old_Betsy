#include "pch.h"
#include "Scene.h"
#include "Camera.h"
#include "ModelInstance.h"
#include "PointLight.h"
#include "ParticleInstance.h"
#include "CommonUtilities\Ray.hpp"
#include "CommonUtilities\Intersection.hpp"
#include "WindSystem.h"
#include "CommonUtilities\Sphere.hpp"
#include "SpriteInstance.h"
#include "TextInstance.h"

Scene::~Scene()
{
	RemoveAll();
}

void Scene::Update(float aDeltaTime)
{
	{
		PERFORMANCETAG("Particles");
		for (auto& i : myParticles)
		{
			i->Update(aDeltaTime, myMainCamera->GetPosition());
		}
	}
	for (int i = CAST(int, myParticles.size()) - 1; i >= 0; i--)
	{
		if (myParticles[i]->IsDead() && !myParticles[i]->IsEternal())
		{
			RemoveFromScene(myParticles[i]);
			delete myParticles[i];
		}
	}
	{
		PERFORMANCETAG("Wind System");
		WindSystem::GetInstance().Update(aDeltaTime);
	}
}

void Scene::RefreshAll(float aAmount)
{
	for (auto& i : myParticles)
	{
		i->RefreshTimeout(aAmount);
	}
}

void Scene::AddToScene(ModelInstance* aModel)
{
	PERFORMANCETAG("Added model to scene");
	myPreCull.push_back(aModel);
	myModels.push_back(aModel);
}

void Scene::AddToScene(SpotLight* aSpotLight)
{
	mySpotlights.push_back(aSpotLight);
}

void Scene::AddToScene(Decal* aDecal)
{
	myDecals.push_back(aDecal);
}

void Scene::AddToScene(SpriteInstance* aSprite)
{
	if (aSprite)
	{
		if (!aSprite->HasBeenAddedToScene())
		{
			mySprites.push_back(aSprite);
			aSprite->AddToScene();
		}
		else
		{
			SYSWARNING("Sprite instance added to scene more than once!");
		}
	}
}

void Scene::AddToScene(TextInstance* aText)
{
	if (!aText->HadBeenAddedToScene())
	{
		myTexts.push_back(aText);
		aText->SetAddedToSceneStatus(true);
	}
}

void Scene::RemoveFromScene(ModelInstance* aModel)
{
	auto at = std::find(myPreCull.begin(), myPreCull.end(), aModel);
	if (at != myPreCull.end())
	{
		myPreCull.erase(at);
	}
	at = std::find(myModels.begin(), myModels.end(), aModel);
	if (at != myModels.end())
	{
		myModels.erase(at);
	}
	else
	{
		//SYSWARNING("Trying to remove model that is not currently in the scene");
	}

}

void Scene::SetMainCamera(Camera* aCamera)
{
	myMainCamera = aCamera;
}

void Scene::SetEnvironmentLight(EnvironmentLight* aLight)
{
	myEnvironmentLight = aLight;
}

void Scene::AddToScene(PointLight* aLight)
{
	myPointLights.push_back(aLight);
}

void Scene::RemoveFromScene(PointLight* aLight)
{
	auto it = std::find(myPointLights.begin(), myPointLights.end(), aLight);

	if (it != myPointLights.end())
	{
		myPointLights.erase(it);
		return;
	}

	SYSWARNING("Tried to remove invalid point light :c");
}

void Scene::RemoveFromScene(ParticleInstance* aParticle)
{
	for (size_t i = 0; i < myParticles.size(); i++)
	{
		if (myParticles[i] == aParticle)
		{
			myParticles.erase(myParticles.begin() + i);
			break;
		}
	}
}

void Scene::RemoveFromScene(Decal* aDecal)
{
	for (size_t i = 0; i < myDecals.size(); i++)
	{
		if (myDecals[i] == aDecal)
		{
			myDecals.erase(myDecals.begin() + i);
			break;
		}
	}
}

void Scene::RemoveFromScene(SpotLight* aSpotLight)
{
	for (size_t i = 0; i < mySpotlights.size(); i++)
	{
		if (mySpotlights[i] == aSpotLight)
		{
			mySpotlights.erase(mySpotlights.begin() + i);
			break;
		}
	}
}

void Scene::RemoveFromScene(SpriteInstance* aSprite)
{
	for (size_t i = 0; i < mySprites.size(); i++)
	{
		if (mySprites[i] == aSprite)
		{
			aSprite->RemoveFromScene();
			mySprites.erase(mySprites.begin() + i);
			break;
		}
	}

}

void Scene::RemoveFromScene(TextInstance* aText)
{
	for (size_t i = 0; i < myTexts.size(); i++)
	{
		if (myTexts[i] == aText)
		{
			myTexts[i]->SetAddedToSceneStatus(false);
			myTexts.erase(myTexts.begin() + i);
			return;
		}
	}

	SYSWARNING("Tried to remove invalid text :c");
}

void Scene::RemoveAll()
{
	myModels.clear();
	for (auto& i : myParticles)
	{
		SAFE_DELETE(i);
	}
	myParticles.clear();

	mySprites.clear();

	myTexts.clear();
}

void Scene::SetSkybox(ModelInstance* aSkybox)
{
	if (aSkybox->GetModelAsset().GetType() != ModelAsset::AssetType::SkyBox)
	{
		SYSERROR("SetSkybox with non skybox asset");
		return;
	}
	mySkybox = aSkybox;
}

std::vector<ParticleInstance*> Scene::GetParticles()
{
	return myParticles;
}

ModelInstance* Scene::GetSkybox()
{
	return mySkybox;
}

std::vector<ModelInstance*> Scene::Cull(const CommonUtilities::Sphere<float>& aBoundingSphere)
{
	std::vector<ModelInstance*> filtered;
	std::copy_if(myModels.begin(), myModels.end(), std::back_inserter(filtered), [&aBoundingSphere](ModelInstance* inst) {return CommonUtilities::IntersectionSphereSphere(inst->GetGraphicBoundingSphere(), aBoundingSphere); });
	return filtered;
}

std::vector<ModelInstance*> Scene::Cull(Camera* aCamera, bool aShouldSort)
{
	return Cull(aCamera, myModels, aShouldSort, 2.f);
}

std::vector<ModelInstance*> Scene::Cull(Camera* aCamera, std::vector<ModelInstance*>& aSelection, bool aShouldSort, float aRangeModifier)
{
	return Cull(aCamera->GenerateFrustum(), aSelection, aCamera->GetPosition(), aShouldSort, aRangeModifier);
}

std::vector<ModelInstance*> Scene::Cull(const CommonUtilities::PlaneVolume<float>& aPlaneVolume, std::vector<ModelInstance*>& aSelection, const V3F& aCameraPos, bool aShouldSort, float aRangeModifier)
{
	static std::vector<ModelInstance*> culledModels;
	culledModels.clear();
	culledModels.reserve(aSelection.size());

	for (size_t i = 0; i < aSelection.size(); ++i)
	{
		if (aSelection[i]->ShouldRender() && CommonUtilities::IntersectionSphereFrustum(aSelection[i]->GetGraphicBoundingSphere(aRangeModifier), aPlaneVolume))
		{
			culledModels.push_back(aSelection[i]);
		}
	}

	if (aShouldSort)
	{
		PERFORMANCETAG("Sorting");
		struct
		{
			bool operator()(const ModelInstance* lhs, const ModelInstance* rhs)
			{
				return (lhs->GetPosition().DistanceSqr(cameraPos) < rhs->GetPosition().DistanceSqr(cameraPos));
			}

			V3F cameraPos;
		} sort;

		sort.cameraPos = aCameraPos;

		std::sort(culledModels.begin(), culledModels.end(), sort);
	}

	return culledModels;
}

std::vector<PointLight*>& Scene::GetPointLights()
{
	return myPointLights;
}

std::vector<SpotLight*>& Scene::GetSpotLights()
{
	return mySpotlights;
}

std::vector<Decal*>& Scene::GetDecals()
{
	return myDecals;
}

std::array<PointLight*, NUMBEROFPOINTLIGHTS> Scene::CullPointLights(ModelInstance* aModel)
{
	std::array<PointLight*, NUMBEROFPOINTLIGHTS> returnArr = { nullptr };

	int counter = 0;
	for (auto& light : myPointLights)
	{
		if (light->position.DistanceSqr(aModel->GetPosition()) < light->range * light->range)
		{
			returnArr[counter++] = light;

			if (counter == NUMBEROFPOINTLIGHTS)
			{
				break;
			}
		}
	}

	return returnArr;
}

const std::vector<SpriteInstance*>& Scene::GetSprites()
{
	return mySprites;
}

const std::vector<TextInstance*>& Scene::GetText()
{
	return myTexts;
}

Camera* Scene::GetMainCamera()
{
	return myMainCamera;
}

EnvironmentLight* Scene::GetEnvironmentLight()
{
	return myEnvironmentLight;
}

std::vector<ModelInstance*>::iterator Scene::begin()
{
	return myModels.begin();
}

std::vector<ModelInstance*>::iterator Scene::end()
{
	return myModels.end();
}

void Scene::AddToScene(ParticleInstance* aParticle)
{
	myParticles.push_back(aParticle);
}