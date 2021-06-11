#pragma once
#include "MetricValue.h"

#include "GamlaBettan\Entity.h"


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

	virtual void RecieveMessage(const Message& aMessage) override;

	bool Init(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX, LightLoader* aLightLoader);
	
	void Update(CommonUtilities::InputHandler& aInputHandler, float aDeltaTime);

#if USEIMGUI
	void ImGuiNode();
#endif

private:

	void SetupWorld();

	void FreecamMovement(CommonUtilities::InputHandler& aInputHandler, float aDeltaTime);

	EntityID myPlayer;

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