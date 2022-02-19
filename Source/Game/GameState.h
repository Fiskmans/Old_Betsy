#pragma once
#include "GameWorld.h"
#include "BaseState.h"
#include "TimerController.h"
#include "SpotLightFactory.h"
#include "DecalFactory.h"
#include "MarchingSquares.h"

class NodePollingStation;
class Scene;
class AudioManager;
class GBPhysX;
class SpriteRenderer;

class GameState : public BaseState, public Observer
{
public:

	GameState(bool aShouldDeleteOnPop = true);
	~GameState();
	virtual void Update() override;
	void ImguiContent(float now);
	bool Init(InputManager* aInputManager, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager);
	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;

	void LoadLevel(const int& aLevel);

	void PreSetup();
private:

	void RunImGui();
	void ImguiContent(bool& drawFrustums);
	void SetDefaultLevel(const std::string& aLevel);
	void LoadDefaultLevel();
	bool LoadLevel(const std::string& aFilePath);
	void MergeNextLevel();
	bool MergeLevel(const std::string& aFilePath);
	bool MergeQueuedLevelPartially(float aTimeBudget);

	void UnloadCurrentLevel();
	void UnloadLevel(std::string aFilepath);
	void CreateWorld(DirectX11Framework* aFramework);

	virtual void Activate() override;
	virtual void Deactivate() override;

	virtual void Unload() override;


	AssetHandle myLoadingLevel;

	bool myHasRenderedAtleastOneFrame;

	InputManager* myInputManager;
	SpriteFactory* mySpriteFactory;
	LightLoader* myLightLoader;
	AudioManager* myAudioManager;
	SpotLightFactory mySpotlightFactory;
	DecalFactory myDecalFactory;

	DirectX11Framework* myFramework;
	ID3D11DeviceContext* myContext;
	ID3D11Device* myDevice;

	ModelInstance* mySkybox;
	GameWorld* myGameWorld;
	NodePollingStation* myNodePollingStation;
	TimerController myTimerController;
	GBPhysX* myGBPhysX;

#if USEFILEWATHCER
	Tools::FileWatcher myWatcher;
	Tools::FileWatcherUniqueID myMetricHandle;
#endif

	std::string myCurrentLevel;
	std::unordered_set<std::string> myCurrentLoadedLevels;
	std::vector<std::string> myLevelSequence;
	unsigned int myCurrentLevelindex;
	float myLastFrame;

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
};

