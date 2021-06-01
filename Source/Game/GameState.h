#pragma once
#include "GameWorld.h"
#include <string>
#include <map>
#include <Spline.h>
#include "Publisher.hpp"
#include "BaseState.h"
#include <MetricValue.h>
#include <Macros.h>
#include <array>
#include "TimerController.h"
#include "SpotLightFactory.h"
#include "DecalFactory.h"
#include "DataStructs.h"
#include <unordered_set>
#include "../TClient/TClient.h"

#if USEFILEWATHCER
#include <FileWatcher.h>
#endif // !_RETAIL

class NodePollingStation;
class Scene;
class AudioManager;
class GBPhysX;
class SpriteRenderer;

class GameState : public BaseState,public Publisher,public Observer
{
public:

	GameState(bool aShouldDeleteOnPop = true);
	~GameState();
	virtual void Update(const float aDeltaTime) override;
	bool Init(WindowHandler* aWindowHandler, InputManager* aInputManager, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer);
	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;

	void LoadLevel(const int& aLevel);

	void PreSetup(const float aDeltaTime);
private:

	void RunImGui(float aDeltatime);
	void SetDefaultLevel(const std::string& aLevel);
	void LoadDefaultLevel();
	bool LoadLevel(const std::string& aFilePath);
	void MergeNextLevel();
	bool MergeLevel(const std::string& aFilePath);
	bool MergeQueuedLevelPartially(float aTimeBudget);
	AssetHandle myLoadingLevel;

	void UnloadCurrentLevel();
	void UnloadLevel(std::string aFilepath);
	void CreateWorld(WindowHandler* aWindowHandler, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer);
	void NextLevel();
	void WinGame();

	virtual void Activate()override;
	virtual void Deactivate()override;

	virtual void Unload()override;

	bool myIsGameWon;
	bool myHasRenderedAtleastOneFrame;
	bool myFinnishGameAfterFadeOut;

	InputManager* myInputManager;
	SpriteFactory* mySpriteFactory;
	LightLoader* myLightLoader;
	AudioManager* myAudioManager;
	WindowHandler* myWindowHandler;
	SpotLightFactory mySpotlightFactory;
	DecalFactory myDecalFactory;

	DirectX11Framework* myFramework;
	ID3D11DeviceContext* myContext;
	ID3D11Device* myDevice;

	Scene* myScene;
	ModelInstance* mySkybox;
	GameWorld* myGameWorld;
	NodePollingStation* myNodePollingStation;
	TimerController myTimerController;
	GBPhysX* myGBPhysX;

#if DONETWORK
	NetworkClient myClient;
#endif

	V3F myLatestCheckpointPos;

	GAMEMETRIC(float, myLockedCameraSpeed, LOCKEDCAMERASPEED, 0.3f);

#if USEIMGUI
	void SearchForFiles();
	std::map<std::string, std::vector<std::string>> myFoundFiles;
#endif // !_RETAIL
#if USEFILEWATHCER
	Tools::FileWatcher myWatcher;
	Tools::FileWatcher::UniqueID myMetricHandle;
#endif

	std::string myCurrentLevel;

#if DEMOSCENE
	std::vector<PointLight*> myLights;
#endif // DEMOSCENE
	std::unordered_set<std::string> myCurrentLoadedLevels;
	std::vector<std::string> myLevelSequence;
	unsigned int myCurrentLevelindex;

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
};

