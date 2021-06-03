#pragma once
#include "ComponentLake.h"
#include "Entity.h"
#include "MetricValue.h"
#include "Macros.h"
#include "Observer.hpp"
#include "Publisher.hpp"
#include "CommonUtilities\ObjectPool.hpp"
#include "ObjectPoolSizes.h"
#include "AIPollingStation.h"
#include "CinematicEditor.h"

#include "AnimationComponent.h"
#include "TimeHandler.h"


namespace CommonUtilities
{
	class InputHandler;
}

class Octree;
class CCamera;
class ModelLoader;
class SpriteFactory;
class SpriteInstance;
class SpriteRenderer;
class StaticObjectFactory;
class DynamicObjectFactory;
class DestructibleFactory;
class InteractableFactory;
class TriggerBoxFactory;
class PointLightFactory;
class ParticleFactory;
class AbilityFactory;
class UIManager;
class Scene;
class DirectX11Framework;
class Inventory;
class AudioManager;
class CharacterData;
class TextFactory;
class GBPhysX;
class GBPhysXColliderFactory;
class SpotLightFactory;
class DialogInstance;
class DialogFactory;
struct SpotLight;

class GameWorld : public Observer , public Publisher
{
public:
	GameWorld();
	~GameWorld();

	void Init(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework, Camera* aCamera, SpotLightFactory* aSpotlightFactory);
	void SystemLoad(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, SpriteRenderer* aSpriteRenderer, LightLoader* aLightLoader);
	void Update(CommonUtilities::InputHandler& aInputHandler, float aDeltatime);
	void UpdateDirectionalLight(float aDeltatime);
	void SetupPlayerAndCamera(CommonUtilities::Vector3<float> aSpawnPos);

	void ClearWorld(bool isShouldDeletePlayer = false);

	void RecieveMessage(const Message& aMessage) override;

	void RespawnPlayer(V3F& aPosition);
#if USEIMGUI
	bool myIsInModelViewerMode = false;
	void ImGuiNode(ImGuiNodePackage& aPackage);
#endif // !_RETAIL

	ParticleFactory* GetParticleFactory();

	V3F GetPlayerPosition();
	void RespawnTrader();

private:

#if USEIMGUI
	ImGuiNodePackage* myLastPackage;
#endif // USEIMGUI

	CinematicEditor myCinEditor;
	AIPollingStation* myAIPollingStation;
	void SpawnPlayer();
	void TriggerEvent(const Message& anEvent);
	CommonUtilities::Vector2<float> myWindowSize;
	GAMEMETRIC(float, myFreecamSpeed, FREECAMSPEED, 0.3f);
	GAMEMETRIC(float, myFreecamRotationSpeed, FREECAMROTATIONSPEED, 0.3f);
	bool myUsingFreeCamera;
	Camera* myMainCameraPtr;

	Scene* myScenePtr;
	Octree* myObjectTree;

	CommonUtilities::ObjectPool<Entity> myEntityPool;
	std::vector<Entity*> myEntitys;

	unsigned int myEntityID;
	bool myHasDoneInitialSetup = false;

	std::vector<Entity*> myLights;
	std::vector<Entity*> myEnemies;
	std::vector<Entity*> myTriggers;
	std::vector<Entity*> myPickups;
	std::vector<DialogInstance*> myDialogs;
	std::vector<Entity*> myAnimations;


	Entity* myPlayer;
	Entity* myCamera;
	Entity* myTrader;


	void CalculateRiverEmitterPos();
	Entity* myRiverAudioEntity;
	Entity* myRiverMeshEntity = nullptr;

	StaticObjectFactory* myStaticObjectFactory;
	DynamicObjectFactory* myDynamicObjectFactory;
	DestructibleFactory* myDestructibleFactory;
	InteractableFactory* myInteractableFactory;
	TriggerBoxFactory* myTriggerBoxFactory;
	PointLightFactory* myPointLightFactory;
	ParticleFactory* myParticleFactory;
	TextFactory* myTextFactory;
	DialogFactory* myDialogFactory;
	SpotLightFactory* mySpotlightFactory;
	SpriteFactory* mySpriteFactory;

	GBPhysX* myGBPhysXPtr;
	GBPhysXColliderFactory* myGBPhysXColliderFactory;

	UIManager* myUIManager;
	CharacterData* myCharacterData;
	SpotLight* myPlayerSpotlight;
	ModelLoader* myModelLoader;

	std::unordered_map<AnimationComponent::States, std::pair<size_t, size_t>> myAnimMapping;
	bool myHasSetFollowCamera = false;

	bool myFirstFadeInComplete = false;

	bool myShouldUpdateDirectionalLight = true;
	void CreateSeed();
};

inline ParticleFactory* GameWorld::GetParticleFactory()
{
	return myParticleFactory;
}

inline V3F GameWorld::GetPlayerPosition()
{
	return myPlayer->GetPosition();
}
