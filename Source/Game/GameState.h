#pragma once
#include "GameWorld.h"
#include <Spline.h>
#include "BaseState.h"
#include "TimerController.h"
#include "SpotLightFactory.h"
#include "DecalFactory.h"

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
	virtual void Update(const float aDeltaTime) override;
	bool Init(InputManager* aInputManager, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer);
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
	void CreateWorld(DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer);
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
	std::unordered_set<std::string> myCurrentLoadedLevels;
	std::vector<std::string> myLevelSequence;
	unsigned int myCurrentLevelindex;

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
};

