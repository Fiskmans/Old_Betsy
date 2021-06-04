#pragma once
#include "MetricValue.h"
#include "CommonUtilities\ObjectPool.hpp"


namespace CommonUtilities
{
	class InputHandler;
}

class SpriteFactory;
class ParticleFactory;
class AudioManager;
class GBPhysX;
class GBPhysXColliderFactory;

class GameWorld : public Observer
{
public:
	GameWorld();
	~GameWorld();

	void Init(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework);
	void SystemLoad(SpriteFactory* aSpriteFactory, Scene* aScene, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, LightLoader* aLightLoader);
	
	void Update(CommonUtilities::InputHandler& aInputHandler, float aDeltatime);

#if USEIMGUI
	bool myIsInModelViewerMode = false;
	void ImGuiNode();
#endif // !_RETAIL

	virtual void RecieveMessage(const Message& aMessage) override;

private:

	CommonUtilities::Vector2<float> myWindowSize;
	GAMEMETRIC(float, myFreecamSpeed, FREECAMSPEED, 0.3f);
	GAMEMETRIC(float, myFreecamRotationSpeed, FREECAMROTATIONSPEED, 0.3f);

	Scene* myScene;
	bool myHasDoneInitialSetup = false;

	ParticleFactory* myParticleFactory;
	SpriteFactory* mySpriteFactory;

	GBPhysX* myGBPhysXPtr;
	GBPhysXColliderFactory* myGBPhysXColliderFactory;

	bool myFirstFadeInComplete = false;
};