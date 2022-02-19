#pragma once
#include "MetricValue.h"

#include "GamlaBettan\Entity.h"
#include "Terrain.h"

class SpriteFactory;
class ParticleFactory;
class AudioManager;
class GBPhysX;

class GameWorld
{
public:
	GameWorld(SpriteFactory* aSpriteFactory, DirectX11Framework* aFramework, AudioManager* aAudioManager, GBPhysX* aGBPhysX);
	~GameWorld();
	
	void Update();

#if USEIMGUI
	void ImGuiNode();
#endif

private:

	void SetupWorld();

	EntityID myPlayer;

	Terrain myTerrain;

	CommonUtilities::Vector2<float> myWindowSize;
	GAMEMETRIC(float, myFreecamSpeed, FREECAMSPEED, 0.3f);
	GAMEMETRIC(float, myFreecamRotationSpeed, FREECAMROTATIONSPEED, 0.3f);

	bool myHasDoneInitialSetup = false;

	ParticleFactory* myParticleFactory;
	SpriteFactory* mySpriteFactory;

	GBPhysX* myGBPhysXPtr;

	bool myFirstFadeInComplete = false;
};