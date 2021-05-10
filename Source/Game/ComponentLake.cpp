#include "pch.h"
#include "ComponentLake.h"

#include "Component.h"
#include "ParticleFactory.h"
#include "Homing3D.h"

#define SETSIZEOFPOOL(type,count) myComponentPoolSize[typeid(type).name()] = count

#include "Mesh.h"
#include "PlayerController.h"
#include "AbilityInstance.h"
#include "Movement3D.h"
#include "ParticleOnDeath.h"
#include "ParticleOnHit.h"
#include "ParticleOnHit.h"
#include "ParticlesWhileAlive.h"
#include "Audio.h"
#include "Collision.h"
#include "FollowCamera.h"
#include "Life.h"
#include "LifeTime.h"
#include "CharacterInstance.h"
#include "AnimationComponent.h"
#include "GBPhysXComponent.h"
#include "GBPhysXKinematicComponent.h"
#include "GBPhysXStaticComponent.h"
#include "AIController.h"
#include "ParticleActivatable.h"
#include "Light.h"
#include "Growable.h"
#include "GrowthSpot.h"
#include "Inventory.h"
#include "House.h"
#include "InteractImage.h"

ComponentLake::ComponentLake()
{
	SETSIZEOFPOOL(Mesh, 10000);
	SETSIZEOFPOOL(PlayerController, 5);
	SETSIZEOFPOOL(Light, 150);
	SETSIZEOFPOOL(AbilityInstance, 100);
	SETSIZEOFPOOL(Movement3D, 500);
	SETSIZEOFPOOL(Homing3D, 100);
	SETSIZEOFPOOL(ParticlesOnDeath, 500);
	SETSIZEOFPOOL(ParticlesOnHit, 500);
	SETSIZEOFPOOL(ParticlesWhileAlive, 150);
	SETSIZEOFPOOL(Audio, 250);
	SETSIZEOFPOOL(Collision, 500);
	SETSIZEOFPOOL(FollowCamera, 5);
	SETSIZEOFPOOL(Life, 250);
	SETSIZEOFPOOL(LifeTime, 150);
	SETSIZEOFPOOL(CharacterInstance, 150);
	SETSIZEOFPOOL(AnimationComponent, 150);
	SETSIZEOFPOOL(ParticleActivatable, 10);
	SETSIZEOFPOOL(GBPhysXComponent, 150);
	SETSIZEOFPOOL(GBPhysXKinematicComponent, 300);
	SETSIZEOFPOOL(GBPhysXStaticComponent, 5000);
	SETSIZEOFPOOL(AIController, 100);
	SETSIZEOFPOOL(Growable, 500);
	SETSIZEOFPOOL(GrowthSpot, 500);
	SETSIZEOFPOOL(Inventory, 5);
	SETSIZEOFPOOL(House, 3);
	SETSIZEOFPOOL(InteractImage, 150);
}

ComponentLake::~ComponentLake()
{
	for (auto& it : myComponentPools)
	{
		delete(it.second);
	}
}

// THIS IS TO BE DONE AFTER REGISTER OF COMPONENTS HAS HAPPENED
void ComponentLake::PrepareObjectsInPools(
	ModelLoader* aModelLoader,
	Scene* aScene, 
	ParticleFactory* aParticleFactory, 
	AudioManager* aAudioManager, 
	LightLoader* aLightLoader, 
	std::vector<Entity*>* aEntityVector, 
	CommonUtilities::ObjectPool<Entity>* aEntityPool,
	SpriteFactory* aSpriteFactory)
{
	for (auto& it : *((CommonUtilities::ObjectPool<Mesh>*)myComponentPools[typeid(Mesh)]))
	{
		it.PreInit(aScene);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<Light>*)myComponentPools[typeid(Light)]))
	{
		it.PreInit(aScene,aLightLoader);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<ParticlesOnDeath>*)myComponentPools[typeid(ParticlesOnDeath)]))
	{
		it.PreInit(aScene, aParticleFactory);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<ParticlesOnHit>*)myComponentPools[typeid(ParticlesOnHit)]))
	{
		it.PreInit(aScene, aParticleFactory);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<ParticlesWhileAlive>*)myComponentPools[typeid(ParticlesWhileAlive)]))
	{
		it.PreInit(aScene, aParticleFactory);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<ParticleActivatable>*)myComponentPools[typeid(ParticleActivatable)]))
	{
		it.PreInit(aScene, aParticleFactory);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<Audio>*)myComponentPools[typeid(Audio)]))
	{
		it.PreInit(aAudioManager);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<GrowthSpot>*)myComponentPools[typeid(GrowthSpot)]))
	{
		it.PreInit(aEntityVector,aEntityPool);
	}
	for (auto& it : *((CommonUtilities::ObjectPool<InteractImage>*)myComponentPools[typeid(InteractImage)]))
	{
		it.PreInit(aScene,aSpriteFactory);
	}
}

void ComponentLake::RegisterComponents()
{
	RegisterComponent<PlayerController>();
	RegisterComponent<Movement3D>();
	RegisterComponent<Homing3D>();
	RegisterComponent<Mesh>();
	RegisterComponent<Light>();
	RegisterComponent<ParticlesOnDeath>();
	RegisterComponent<ParticlesOnHit>();
	RegisterComponent<ParticlesWhileAlive>();
	RegisterComponent<ParticleActivatable>();
	RegisterComponent<Audio>();
	RegisterComponent<Collision>();
	RegisterComponent<FollowCamera>();
	RegisterComponent<Life>();
	RegisterComponent<LifeTime>();
	RegisterComponent<CharacterInstance>();
	RegisterComponent<AbilityInstance>();
	RegisterComponent<Growable>();
	RegisterComponent<GrowthSpot>();
	RegisterComponent<House>();
	RegisterComponent<InteractImage>();
}

bool ComponentLake::UpdateComponents(float aDeltaTime)
{
	for (int componentType : myUpdateOrder)
	{
		if (componentType < 0 || componentType > myUpdateOrder.size())
		{
			continue;
		}

		for (myActiveComponentIndex[componentType] = CAST(int, myAttachedActiveComponents[componentType].size()) - 1; myActiveComponentIndex[componentType] >= 0; --myActiveComponentIndex[componentType])
		{
			PERFORMANCETAG(myAttachedActiveComponents[componentType][myActiveComponentIndex[componentType]]->myTypeIndex.name());
			myAttachedActiveComponents[componentType][myActiveComponentIndex[componentType]]->Update(aDeltaTime);
		}
	}

	return true;
}

void ComponentLake::ReturnComponent(std::type_index aComponentIndex, Component* aComponent)
{
	int componentIndex = GetComponentTypeIndex(aComponentIndex);
	myComponentPools[aComponentIndex]->Dispose(aComponent);
#if TRACKCOMPONENTUSAGE
	GetComponentsInUse()[aComponentIndex.name()].first--;
#endif // TRACKCOMPONENTUSAGE
	myAttachedActiveComponents[componentIndex].erase(std::remove(myAttachedActiveComponents[componentIndex].begin(), myAttachedActiveComponents[componentIndex].end(), aComponent), myAttachedActiveComponents[componentIndex].end());
}

int ComponentLake::GetComponentTypeIndex(std::type_index aTypeIndex)
{
	return myComponentTypeIndexLookupTable[aTypeIndex];
}

void ComponentLake::DisableActiveComponent(std::type_index aComponentType, Component* aComponent)
{
	int componentIndex = myComponentTypeIndexLookupTable[aComponentType];

	if (myAttachedActiveComponents[componentIndex].size() == 0)
		return void();

	myAttachedActiveComponents[componentIndex].erase(std::find(myAttachedActiveComponents[componentIndex].begin(), myAttachedActiveComponents[componentIndex].end(), aComponent));
	myActiveComponentIndex[componentIndex] = (myActiveComponentIndex[componentIndex] < myAttachedActiveComponents[componentIndex].size()) ? myActiveComponentIndex[componentIndex] : (CAST(int, myAttachedActiveComponents[componentIndex].size()) - 1);
}

void ComponentLake::EnableActiveComponent(std::type_index aComponentType, Component* aComponent)
{
	int componentIndex = myComponentTypeIndexLookupTable[aComponentType];

	myAttachedActiveComponents[componentIndex].push_back(aComponent);
}