#pragma once
#include "MetricValue.h"


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

	void Init(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework);
	void SystemLoad(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, LightLoader* aLightLoader);
	
	void Update(CommonUtilities::InputHandler& aInputHandler, float aDeltatime);

#if USEIMGUI
	void ImGuiNode();
#endif // !_RETAIL

	virtual void RecieveMessage(const Message& aMessage) override;

private:

	CommonUtilities::Vector2<float> myWindowSize;
	GAMEMETRIC(float, myFreecamSpeed, FREECAMSPEED, 0.3f);
	GAMEMETRIC(float, myFreecamRotationSpeed, FREECAMROTATIONSPEED, 0.3f);

	bool myHasDoneInitialSetup = false;

	ParticleFactory* myParticleFactory;
	SpriteFactory* mySpriteFactory;

	GBPhysX* myGBPhysXPtr;
	GBPhysXColliderFactory* myGBPhysXColliderFactory;

	bool myFirstFadeInComplete = false;
};