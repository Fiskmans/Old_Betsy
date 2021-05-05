#include "pch.h"
#include "UIManager.h"
#include "Scene.h"
#include "SpriteInstance.h"
#include "SpriteFactory.h"
#include "EmilsEnums.h"
#include "TextFactory.h"
#include "TextInstance.h"
#include "Entity.h"
#include "Random.h"
#include "Camera.h"
#include "FireWeapon.h"
#include "PlayerController.h"
#include "Inventory.h"
#include "Sprite.h"


UIManager::UIManager() :
	myUIFadeTime(3.0f),
	myUIFadeTimer(-1.0f),
	myUIFadeDirection(1.0f),
	myCurrentLevel(0),
	myCamera(nullptr),
	myFadeSprite(nullptr),
	myFirstFadeInDone(false),
	myIsFadingIn(false),
	myIsFadingOut(false),
	myScene(nullptr),
	myShouldKillPlayerAfterFadeOut(false),
	myShouldSwapLevelAfterFade(false),
	mySpriteFactory(nullptr),
	myTextFactory(nullptr),
	myVignette(nullptr),
	myInteractMessage(nullptr),
	myClockForeground(nullptr),
	myClockWheel(nullptr),
	myClockText(nullptr)
{
	SubscribeToMessage(MessageType::FadeIn);
	SubscribeToMessage(MessageType::FadeOut);
	SubscribeToMessage(MessageType::CurrentLevel);
	SubscribeToMessage(MessageType::TriggerEvent);
	SubscribeToMessage(MessageType::LookingAtInteractable);
	SubscribeToMessage(MessageType::InventoryToggled);
	SubscribeToMessage(MessageType::UpdateTime);
	SubscribeToMessage(MessageType::EndGame);
	SubscribeToMessage(MessageType::InputMouseMoved);
	SubscribeToMessage(MessageType::MouseHoverNone);
	SubscribeToMessage(MessageType::MouseHoverSheep);
	SubscribeToMessage(MessageType::MouseHoverWaterBucket);
	SubscribeToMessage(MessageType::MouseHoverHoe);
	SubscribeToMessage(MessageType::MouseHoverCanPlant);
	SubscribeToMessage(MessageType::MouseHoverCanHarvest);
	SubscribeToMessage(MessageType::MouseHoverRiver);
	SubscribeToMessage(MessageType::SendUIGameMessage);
	SubscribeToMessage(MessageType::StoredCalories);
	SubscribeToMessage(MessageType::TradeStarted);
	SubscribeToMessage(MessageType::TradeEnded);
}

UIManager::~UIManager()
{
	UnSubscribeToMessage(MessageType::FadeIn);
	UnSubscribeToMessage(MessageType::FadeOut);
	UnSubscribeToMessage(MessageType::TriggerEvent);
	UnSubscribeToMessage(MessageType::CurrentLevel);
	UnSubscribeToMessage(MessageType::LookingAtInteractable);
	UnSubscribeToMessage(MessageType::InventoryToggled);
	UnSubscribeToMessage(MessageType::UpdateTime);
	UnSubscribeToMessage(MessageType::EndGame);
	UnSubscribeToMessage(MessageType::InputMouseMoved);
	UnSubscribeToMessage(MessageType::MouseHoverNone);
	UnSubscribeToMessage(MessageType::MouseHoverSheep);
	UnSubscribeToMessage(MessageType::MouseHoverWaterBucket);
	UnSubscribeToMessage(MessageType::MouseHoverHoe);
	UnSubscribeToMessage(MessageType::MouseHoverCanPlant);
	UnSubscribeToMessage(MessageType::MouseHoverCanHarvest);
	UnSubscribeToMessage(MessageType::MouseHoverRiver);
	UnSubscribeToMessage(MessageType::SendUIGameMessage);
	UnSubscribeToMessage(MessageType::StoredCalories);
	UnSubscribeToMessage(MessageType::TradeStarted);
	UnSubscribeToMessage(MessageType::TradeEnded);

	for (size_t i = 0; i < mySprites.size(); i++)
	{
		myScene->RemoveSprite(mySprites[i]);
		delete mySprites[i];
	}

	mySprites.clear();

	WIPE(*this);
}

void UIManager::Init(Entity* aPlayer, Scene& aScene, SpriteFactory& aSpriteFactory, TextFactory& aTextFactory, Camera* aCamera)
{
	myCamera = aCamera;
	myScene = &aScene;
	myTextFactory = &aTextFactory;
	mySpriteFactory = &aSpriteFactory;
	myPlayerPtr = aPlayer;

	ProgressBar::SetSpriteFactory(mySpriteFactory);

	myFadeSprite = mySpriteFactory->CreateSprite("Data/UI/fadeScreen.dds");

#if !DEMOSCENE
#if !DIRECTTOGAME
	aScene.AddSprite(myFadeSprite);
#endif
#endif

	myFadeSprite->SetPosition(0.0f, 0.0f);
	myFadeSprite->SetScale(CommonUtilities::Vector2<float>(1000.0f, 1000.0f));
	mySprites.push_back(myFadeSprite);

	myVignette = CreateAndAddSpriteToScene("Data/UI/vignette.dds", V2F(0.0f, 0.0f));
	myVignette->SetUVMinMaxInTexels(V2F(0,0),V2F(1920,1080));
	myVignette->SetSize(V2F(1.f, 1.f));
	myInventoryBackground = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/Inventory.dds");
	myInventoryBackground->SetPosition(V2F(0.5f, 0.5f));
	myInventoryBackground->SetPivot(V2F(0.5f, 0.5f));
	myToolBar = CreateAndAddSpriteToScene("Data/UI/InventoryBar/inventorybar.dds", V2F(0.5f, 1.f), V2F(0.5f, 1.f));
	myToolBar->SetDepth(0.1f);
	myPlayerPtr->GetComponent<Inventory>()->AddToolbarToScene();


	myClockWheel = CreateAndAddSpriteToScene("Data/UI/ClockBackground.dds", V2F(0.9f, 0.125f), V2F(0.5f, 0.5f));
	myClockForeground = CreateAndAddSpriteToScene("Data/UI/ClockForeground.dds", V2F(0.9f, 0.125f), V2F(0.5f, 0.177));
	myClockWheel->SetDepth(0.05f);
	myClockText = myTextFactory->CreateText();
	myClockText->SetText("Hour: 6");
	myClockText->SetPivot(V2F(0.5f, 1.f));
	myClockText->SetPosition(V2F(0.9f - (20.0f / Sprite::ourWindowSize.x), 0.125f + (57.0f / Sprite::ourWindowSize.y)));
	myClockText->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));

	myDayText = myTextFactory->CreateText();
	myDayText->SetText("Day: 1");
	myDayText->SetPivot(V2F(0.5f, 1.f));
	myDayText->SetPosition(V2F(0.9f - (20.0f / Sprite::ourWindowSize.x), 0.125f + (100.0f / Sprite::ourWindowSize.y)));
	myDayText->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	
	myInventoryImage = CreateAndAddSpriteToScene("Data/UI/InventoryBar/Inventory_icon.dds", V2F(1.0f -(128.0f /Sprite::ourWindowSize.x), 1.0f - (128.0f / Sprite::ourWindowSize.y)));

	myDaysPassedBarSprite = CreateAndAddSpriteToScene("Data/UI/InventoryBar/StorageInventoryBar.dds", V2F(0.9f - (60.0f / Sprite::ourWindowSize.x), 0.125f + (121.0f / Sprite::ourWindowSize.y)), V2F(0.0f,0.5f));
	myDaysPassedBarSprite->SetScale(V2F(0.5f, 2.0f));
	myDaysPassedBarSprite->SetColor(V4F(0.35f, 0.4f, 0.48f, 1.0f));

	myCaloriesCollectedBarSprite = CreateAndAddSpriteToScene("Data/UI/InventoryBar/StorageInventoryBar.dds", V2F(0.9f - (60.0f / Sprite::ourWindowSize.x), 0.125f + (154.0f / Sprite::ourWindowSize.y)), V2F(0.0f, 0.5f));
	myCaloriesCollectedBarSprite->SetScale(V2F(0.1f, 2.0f));
	myCaloriesCollectedBarSprite->SetColor(V4F(0.35f, 0.4f, 0.48f, 1.0f));

	myEndScreenBlackBackground = mySpriteFactory->CreateSprite("Data/UI/fadeScreen.dds");
	myEndScreenBlackBackground->SetSize(V2F(1.f, 1.f));
	myEndScreenBlackBackground->SetPosition(V2F(0.0f, 0.0f));

	myEndScreen = mySpriteFactory->CreateSprite("Data/UI/endscreenBackground.dds");
	myEndScreen->SetPivot(V2F(0.1f, 0.5f));
	myEndScreen->SetPosition(V2F(0.5f, 0.5f));

	myCursorIcons[static_cast<int>(MouseIcons::Default)] = mySpriteFactory->CreateSprite("Data/UI/mouse.dds");
	myCursorIcons[static_cast<int>(MouseIcons::WaterBucket)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/WateringCanFilled_icon.dds");
	myCursorIcons[static_cast<int>(MouseIcons::Hoe)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/Hoe_icon.dds");
	myCursorIcons[static_cast<int>(MouseIcons::Basket)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/Basket_icon.dds");
	myCursorIcons[static_cast<int>(MouseIcons::Scissor)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/Scissor_icon.dds");
	myCursorIcons[static_cast<int>(MouseIcons::CanPlant)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/RandomSeed_icon.dds");
	myCursorIcons[static_cast<int>(MouseIcons::CanHarvest)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/Basket_icon.dds");
	myCursorIcons[static_cast<int>(MouseIcons::CollectWater)] = mySpriteFactory->CreateSprite("Data/UI/InventoryBar/WateringCanFilled_icon.dds");

	myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::Default)];
	myMouseCursor->SetPosition(0.5, 0.5);
	mySprites.push_back(myMouseCursor);

	myEndScreenStoryText = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myEndScreenStoryText->SetText("Error ");

	myEndScreenStoryText2 = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myEndScreenStoryText2->SetText("Error ");

	myEndScreenStoryText3 = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myEndScreenStoryText3->SetText("Error ");

	myEndScreenStoryText4 = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myEndScreenStoryText4->SetText("Error ");

	myEndScreenStoryText5 = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myEndScreenStoryText5->SetText("Error ");

	myEndScreenStoryText6 = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myEndScreenStoryText6->SetText("Error ");


	//CALORIES
	myTotalCaloriesAmountText = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	myTotalCaloriesAmountText->SetText("Error ");
	myTotalCaloriesAmountText->SetPivot(V2F(0.5f, 0.5f));
	myTotalCaloriesAmountText->SetPosition(V2F(0.6f, 0.4f));

	//secrets
	mySecretsText = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	mySecretsText->SetText("Secrets: ");
	mySecretsAmountText = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
	mySecretsAmountText->SetText("5 / 10");

	myGameMessage = myTextFactory->CreateText("Data/Fonts/MoonviewDialogue.spritefont");
	myGameMessage->SetScale({1,1});
	myGameMessage->SetPosition({0.5, 0.3});
	myGameMessage->SetPivot({0.5f, 0.5f});
	myGameMessageTimer = 0;
	
	myIsFadingIn = true;
	myFirstFadeInDone = false;
	myCurrentLevel = 0;
	myUIFadeTimer = 5.0f;


	myInteractMessage = CreateAndAddSpriteToScene("Data/UI/Use.dds", V2F(0.5f, 0.1f), V2F(0.5f, 0.5f));
}

void UIManager::Update(const float aDeltaTime)
{
	Fade(aDeltaTime);

	if (myGameMessageTimer > 0)
	{
		myGameMessageTimer -= aDeltaTime;
		if (myGameMessageTimer <= 0)
		{
			myScene->RemoveText(myGameMessage);
		}
	}

	myScene->RemoveSprite(myMouseCursor);
	myScene->AddSprite(myMouseCursor);
}

void UIManager::AddUIToScene(Scene& aScene)
{
	myScene = &aScene;

	for (auto& sprite : mySprites)
	{
		if (!sprite->HasBeenAddedToScene())
		{
			myScene->AddSprite(sprite);
		}
	}
	myScene->RemoveSprite(myInteractMessage);

	myScene->AddText(myClockText);
	myScene->AddText(myDayText);
}

void UIManager::FadeOut()
{
	myFadeSprite->SetScale(CU::Vector2<float>(1000.0f, 1000.0f));
	myFadeSprite->SetPosition(0.0f, 0.0f);
	myUIFadeTimer = 0.01f;
}

void UIManager::FadeIn()
{
	//myUIFadeTimer = myUIFadeTime;
}

void UIManager::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{

	case MessageType::FadeIn:
	{
		FadeIn();
	}
	break;

	case MessageType::FadeOut:
	{
		if (!myIsFadingOut)
		{
			myShouldSwapLevelAfterFade = aMessage.myBool;
			myShouldKillPlayerAfterFadeOut = static_cast<bool>(aMessage.myIntValue);
			FadeOut();
			myIsFadingOut = true;
		}
	}
	break;

	case MessageType::TriggerEvent:
	{
		if (aMessage.myText == "1000")
		{
			if (!myIsFadingOut)
			{
				myShouldSwapLevelAfterFade = true;
				myShouldKillPlayerAfterFadeOut = false;
				FadeOut();
				myIsFadingOut = true;

			}
		}
	}
	break;

	case MessageType::CurrentLevel:
	{
		myCurrentLevel = aMessage.myIntValue;
	}

	case MessageType::LookingAtInteractable:
	{
		if (aMessage.myBool)
		{
			if (!myInteractMessage->HasBeenAddedToScene())
			{
				myScene->AddSprite(myInteractMessage);
			}
		}
		else
		{
			if (myInteractMessage->HasBeenAddedToScene())
			{
				myScene->RemoveSprite(myInteractMessage);
			}
		}
	}
	break;
	case MessageType::InventoryToggled:
	{
		Inventory* inv = myPlayerPtr->GetComponent<Inventory>();

		if (inv->IsToggled())
		{
			myScene->AddSprite(myInventoryBackground);
			myScene->RemoveSprite(myToolBar);
			inv->RemoveToolbarFromScene();
			inv->AddToScene();
		}
		else
		{
			myScene->RemoveSprite(myInventoryBackground);
			myScene->AddSprite(myToolBar);
			inv->RemoveFromScene();
			inv->AddToolbarToScene();
		}
	}
	break;
	case MessageType::UpdateTime:
	{
		if (myClockWheel)
		{
			myClockWheel->SetRotation(aMessage.myFloatValue * 2.0f * PI);
			myClockText->SetText("Hour: " + std::to_string(aMessage.myIntValue));
			myDayText->SetText("Day: " + std::to_string(aMessage.myIntValue2));
			myDaysPassedBarSprite->SetScale(V2F(((aMessage.myIntValue2 - 1) / 15.0f) * 25.0f, 1.0f));
		}
		break;
	}

	case MessageType::StoredCalories:
	{
		float val = aMessage.myFloatValue;
		val = CLAMP(0.0f, 1.0f, val);
		myCaloriesCollectedBarSprite->SetScale(V2F(val * 25.0f, 1.0f));
		break;
	}
	case MessageType::EndGame:
	{
		DisplayEndScreen();
		break;
	}
	case MessageType::InputMouseMoved:
	{
		for (auto icon : myCursorIcons)
		{
			icon->SetPosition(aMessage.myFloatValue, aMessage.myFloatValue2);
		}

		break;
	}
	case MessageType::MouseHoverNone:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::Default)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::MouseHoverSheep:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::Scissor)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::MouseHoverWaterBucket:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::WaterBucket)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::MouseHoverHoe:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::Hoe)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::MouseHoverCanPlant:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::CanPlant)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::MouseHoverCanHarvest:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::CanHarvest)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::MouseHoverRiver:
	{
		myScene->RemoveSprite(myMouseCursor);
		myMouseCursor = myCursorIcons[static_cast<int>(MouseIcons::CollectWater)];
		myScene->AddSprite(myMouseCursor);
		break;
	}
	case MessageType::SendUIGameMessage:
	{
		myScene->RemoveText(myGameMessage);
		myGameMessage->SetText(aMessage.myText.data());
		myScene->AddText(myGameMessage);
		myGameMessageTimer = 2;
		break;
	}


	case MessageType::TradeStarted:
		myScene->RemoveSprite(myToolBar);
		if (myInteractMessage->HasBeenAddedToScene())
		{
			myScene->RemoveSprite(myInteractMessage);
		}
		break;

	case MessageType::TradeEnded:
		myScene->AddSprite(myToolBar);
		break;
	}
}

void UIManager::SetEarthCellarEntity(Entity* aEntity)
{
	myEarthCellarEntity = aEntity;
}

void UIManager::DisplayEndScreen()
{
	PostMaster::GetInstance()->SendMessages(MessageType::LockPlayer);

	myScene->AddSprite(myEndScreenBlackBackground);
	myScene->AddSprite(myEndScreen);
	myEndScreen->SetPivot(V2F(0.5f, 0.5f));
	myEndScreen->SetPosition(V2F(0.5f, 0.5f));
	auto itemsAndAmounts = myEarthCellarEntity->GetComponent<FoodCellar>()->GetItemsAndAmounts();

	myExitGameButton = new Button();
	myExitGameButton->Init("Data/UI/UIButtons/Endscreen_Exit_off.dds", "Data/UI/UIButtons/Endscreen_Exit_hover.dds", "Data/UI/UIButtons/Endscreen_Exit_on.dds", "Data/UI/UIButtons/Data/UI/UIButtons/Endscreen_Exit_off.dds", V2F(0.4f, 0.8f));
	myExitGameButton->SetScenePtr(myScene);
	myExitGameButton->SetOnPressedFunction([this] { exit(0); });
	myScene->AddSprite(myExitGameButton->GetCurrentSprite());
	myPlayAgainButton = new Button();
	myPlayAgainButton->Init("Data/UI/UIButtons/Endscreen_MainMenu_off.dds", "Data/UI/UIButtons/Endscreen_MainMenu_hover.dds", "Data/UI/UIButtons/Endscreen_MainMenu_on.dds", "Data/UI/UIButtons/Endscreen_MainMenu_off.dds", V2F(0.6f, 0.8f));
	myPlayAgainButton->SetScenePtr(myScene);
	myPlayAgainButton->SetOnPressedFunction([this] {
		Message msg;
		msg.myMessageType = MessageType::PopState;
		msg.myBool = true;
		PostMaster::GetInstance()->SendMessages(msg); });
	myScene->AddSprite(myPlayAgainButton->GetCurrentSprite());
	int index = 0;
	for (auto& item : itemsAndAmounts)
	{
		TextInstance* text = myTextFactory->CreateText("Data/Fonts/endScreen.spritefont");
		text->SetText("" + StringFromItemId(ItemFromID(item.first)) + ": x" + std::to_string(item.second));
		text->SetPosition(V2F(myItemTextStartXY.x + (myItemTextXOffset * (index / myItemTextColumnMax)), myItemTextStartXY.y + (myItemTextYOffset * (index % myItemTextColumnMax))));
		text->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
		myScene->AddText(text);
		myEndScreenTextInstances.push_back(text);
		index++;
	}

	myTotalCalories = myEarthCellarEntity->GetComponent<FoodCellar>()->GetStoredTotalCalories();
	myTotalCaloriesAmountText->SetText(std::to_string(myTotalCalories));
	myTotalCaloriesAmountText->SetPosition(V2F(0.5f + (130.0f / Sprite::ourWindowSize.x), 0.5f - (5.0f / Sprite::ourWindowSize.y)));
	myTotalCaloriesAmountText->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myScene->AddText(myTotalCaloriesAmountText);

	if (myTotalCalories < 8000)
	{
		myEndScreenStoryText->SetText("As the snow falls heavy no crops will grow for several months and Ruth hunkers down in her little");    
		myEndScreenStoryText2->SetText("cottage trusting in her stored food to last through the cold months.");
		myEndScreenStoryText3->SetText("The cottage is cold as Ruths rations the few crops she managed to save. An egg can keep you going for");
		myEndScreenStoryText4->SetText("several days after all, especially since she's not moving too much. The winter seems to go on for ever,");
		myEndScreenStoryText5->SetText("and as the food storage grows ever thinner Ruth vows to herself to learn from her mistakes and prepare");
		myEndScreenStoryText6->SetText("better for the next winter. She knows that she can do better than this");
	}
	else if (myTotalCalories < 10000)
	{
		myEndScreenStoryText->SetText("As the snow falls heavy no crops will grow for several months and Ruth hunkers down in her little");
		myEndScreenStoryText2->SetText("cottage, trusting to her stored food to last through the cold months.");
		myEndScreenStoryText3->SetText("Ruth cuts down on her meals to make the stored food last longer. There's no time for feasting as the");
		myEndScreenStoryText4->SetText("world turns white outside, but she's able to keep her stomach from grumbling. As she watches her little");
		myEndScreenStoryText5->SetText("farmland through frost covered windows she plans for the spring, and how to better organize the");
		myEndScreenStoryText6->SetText("coming seasons. Surely she can do better than this, if she just prepares a little bit better.");
	}
	else
	{
		myEndScreenStoryText->SetText("As the snow falls heavy no crops will grow for several months and Ruth hunkers down in her little");
		myEndScreenStoryText2->SetText("cottage, trusting to her stored food to last through the cold months.");
		myEndScreenStoryText3->SetText("The fire crackles comfortably as Ruth enjoys a feast each evening. Food is aplenty and each tasty dinner");
		myEndScreenStoryText4->SetText("fills Ruth's stomach with pride as well as calories. Winter must be the best of seasons. Ruth relaxes and");
		myEndScreenStoryText5->SetText("enjoys the results of previous months while the world sleeps under its white cover. She managed");
		myEndScreenStoryText6->SetText("much better than she had thought. She's never been this happy.");
	}

	myEndScreenStoryText->SetPosition(V2F(0.5f - (406.0f / Sprite::ourWindowSize.x), 0.5f - (371.0f / Sprite::ourWindowSize.y)));
	myEndScreenStoryText->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myEndScreenStoryText->SetScale(V2F(0.7f, 0.7f));
	myScene->AddText(myEndScreenStoryText);

	myEndScreenStoryText2->SetPosition(V2F(0.5f - (406.0f / Sprite::ourWindowSize.x), 0.5f - (341.0f / Sprite::ourWindowSize.y)));
	myEndScreenStoryText2->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myEndScreenStoryText2->SetScale(V2F(0.7f, 0.7f));
	myScene->AddText(myEndScreenStoryText2);

	myEndScreenStoryText3->SetPosition(V2F(0.5f - (406.0f / Sprite::ourWindowSize.x), 0.5f - (311.0f / Sprite::ourWindowSize.y)));
	myEndScreenStoryText3->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myEndScreenStoryText3->SetScale(V2F(0.7f, 0.7f));
	myScene->AddText(myEndScreenStoryText3);

	myEndScreenStoryText4->SetPosition(V2F(0.5f - (406.0f / Sprite::ourWindowSize.x), 0.5f - (281.0f / Sprite::ourWindowSize.y)));
	myEndScreenStoryText4->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myEndScreenStoryText4->SetScale(V2F(0.7f, 0.7f));
	myScene->AddText(myEndScreenStoryText4);

	myEndScreenStoryText5->SetPosition(V2F(0.5f - (406.0f / Sprite::ourWindowSize.x), 0.5f - (251.0f / Sprite::ourWindowSize.y)));
	myEndScreenStoryText5->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myEndScreenStoryText5->SetScale(V2F(0.7f, 0.7f));
	myScene->AddText(myEndScreenStoryText5);

	myEndScreenStoryText6->SetPosition(V2F(0.5f - (406.0f / Sprite::ourWindowSize.x), 0.5f - (221.0f / Sprite::ourWindowSize.y)));
	myEndScreenStoryText6->SetColor(V4F(0.0f, 0.0f, 0.0f, 1.0f));
	myEndScreenStoryText6->SetScale(V2F(0.7f, 0.7f));
	myScene->AddText(myEndScreenStoryText6);

	mySecretsAmountText->SetText(std::to_string(mySecrets));
	myScene->AddText(mySecretsText);
	myScene->AddText(mySecretsAmountText);
}

void UIManager::DisplayTradeScreen()
{

}

Scene* UIManager::GetScenePtr()
{
	return myScene;
}

TextFactory* UIManager::GetTextFactory()
{
	return myTextFactory;
}

void UIManager::Fade(const float aDeltaTime)
{
	if (myIsFadingOut)
	{
		myUIFadeTimer += aDeltaTime;
		myFadeSprite->SetColor(CommonUtilities::Vector4<float>(1.0f, 1.0f, 1.0f, myUIFadeTimer / myUIFadeTime));
		if (myUIFadeTimer > myUIFadeTime)
		{
			myUIFadeTimer = myUIFadeTime;
			myIsFadingOut = false;

			//if (myShouldKillPlayerAfterFadeOut)
			//{
			//	PostMaster::GetInstance()->SendMessages(MessageType::PlayerDied);
			//}
			//else
			//{
			Message fadeOutCompleteMessage;
			fadeOutCompleteMessage.myMessageType = MessageType::FadeOutComplete;
			SendMessages(fadeOutCompleteMessage);
			//}
			myIsFadingIn = true;
		}
	}

	if (myIsFadingIn)
	{
		if (!myFirstFadeInStart)
		{
			myFirstFadeInStart = true;
			Message message;
			message.myMessageType = MessageType::StartInGameAudio;
			SendMessages(message);
		}


		if (myUIFadeTimer > 0.0f)
		{
			myUIFadeTimer -= aDeltaTime;
			myFadeSprite->SetColor(CommonUtilities::Vector4<float>(1.0f, 1.0f, 1.0f, myUIFadeTimer / myUIFadeTime));
		}
		else
		{
			myUIFadeTimer = -1;
			myIsFadingIn = false;

			if (!myFirstFadeInDone)
			{
				myFirstFadeInDone = true;
				Message fadeInCompleteMessage;
				fadeInCompleteMessage.myMessageType = MessageType::FadeInComplete;
				SendMessages(fadeInCompleteMessage);
			}
		}
	}
}

SpriteInstance* UIManager::CreateAndAddSpriteToScene(const std::string& aPath, const V2F& aPosition, const V2F aPivot)
{
	SpriteInstance* sprite = mySpriteFactory->CreateSprite(aPath);

	if (sprite)
	{
		sprite->SetPivot(aPivot);
		sprite->SetPosition(aPosition);
		myScene->AddSprite(sprite);
		mySprites.push_back(sprite);
	}

	return sprite;
}

void UIManager::RemoveSpriteFromSceneAndDelete(SpriteInstance*& aSprite)
{
	if (aSprite)
	{
		for (size_t i = 0; i < mySprites.size(); i++)
		{
			if (mySprites[i] == aSprite)
			{
				mySprites.erase(mySprites.begin() + i);
				break;
			}
		}

		myScene->RemoveSprite(aSprite);

		delete aSprite;
		aSprite = nullptr;
	}
}
