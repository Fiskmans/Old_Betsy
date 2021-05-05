#pragma once
#include "Observer.hpp"
#include "Vector2.hpp"
#include "Publisher.hpp"
#include <vector>
#include "ProgressBar.h"
#include "Button.h"
#include "FoodCellar.h"
#include "Item.h"

class SpriteFactory;
class TextFactory;

enum class MouseIcons
{
	Default,
	Hoe,
	Scissor,
	WaterBucket,
	Basket,
	CanPlant,
	CanHarvest,
	CollectWater,

	Size
};

class UIManager : public Observer, public Publisher
{
public:

	UIManager();
	~UIManager();

	void Init(Entity* aPlayer, Scene& aScene, SpriteFactory& aSpriteFactory, TextFactory& aTextFactory, Camera* aCamera);
	void Update(const float aDeltaTime);
	void AddUIToScene(Scene& aScene);
	void FadeIn();
	void FadeOut();
	void RecieveMessage(const Message& aMessage) override;
	void SetEarthCellarEntity(Entity* aEntity);
	void DisplayEndScreen();
	void DisplayTradeScreen();

	Scene* GetScenePtr();
	TextFactory* GetTextFactory();

private:
	SpriteInstance* CreateAndAddSpriteToScene(const std::string& aPath, const V2F& aPosition, const V2F aPivot = V2F(0, 0));
	void RemoveSpriteFromSceneAndDelete(SpriteInstance*& aSprite);

	Camera* myCamera;
	Scene* myScene;
	SpriteFactory* mySpriteFactory;
	TextFactory* myTextFactory;
	Entity* myPlayerPtr;

	SpriteInstance* myFadeSprite;
	SpriteInstance* myVignette;

	SpriteInstance* myInventoryImage;

	SpriteInstance* myInteractMessage;
	SpriteInstance* myInventoryBackground;
	SpriteInstance* myToolBar;

	SpriteInstance* myClockForeground;
	SpriteInstance* myClockWheel;
	TextInstance* myClockText;
	TextInstance* myDayText;

	SpriteInstance* myDaysPassedBarSprite;
	SpriteInstance* myCaloriesCollectedBarSprite;

	SpriteInstance* myMouseCursor;
	std::array<SpriteInstance*, static_cast<int>(MouseIcons::Size)> myCursorIcons; 

	TextInstance* myGameMessage;
	float myGameMessageTimer;

	//END SCREEN ------------------------------------------------------------
	Entity* myEarthCellarEntity = nullptr;
	
	SpriteInstance* myEndScreenBlackBackground;
	SpriteInstance* myEndScreen;
	TextInstance* myEndScreenStoryText;
	TextInstance* myEndScreenStoryText2;
	TextInstance* myEndScreenStoryText3;
	TextInstance* myEndScreenStoryText4;
	TextInstance* myEndScreenStoryText5;
	TextInstance* myEndScreenStoryText6;

	TextInstance* myTotalCaloriesAmountText;
	int myTotalCalories = 0;

	//items
	V2F myItemTextStartXY = V2F(0.36f, 0.55f);
	float myItemTextXOffset = 0.1f;
	float myItemTextYOffset = 0.04f;
	int myItemTextColumnMax = 4;

	std::vector<TextInstance*> myEndScreenTextInstances;

	//Upgrades
	TextInstance* myUpgradesText;
	TextInstance* myUpgradesAmountText;
	int myUpgrades = 0;
	int myMaxUpgrades = 4;
	//Secrets
	TextInstance* mySecretsText;
	TextInstance* mySecretsAmountText;
	int mySecrets = 0;
	int myMaxSecrets = 10;

	Button* myExitGameButton;
	Button* myPlayAgainButton;


	//Fade screen ----------------------------------------------------------
	float myUIFadeTime;
	float myUIFadeTimer;
	float myUIFadeDirection;

	int myCurrentLevel;

	bool myIsFadingOut;
	bool myIsFadingIn;
	bool myShouldKillPlayerAfterFadeOut;
	bool myShouldSwapLevelAfterFade;

	bool myFirstFadeInDone;
	bool myFirstFadeInStart = false;

	void Fade(const float aDeltaTime);

	std::vector<SpriteInstance*> mySprites;
};

