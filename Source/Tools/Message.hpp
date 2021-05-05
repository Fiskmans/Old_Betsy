#pragma once
#include <string>

class Entity;

enum class MessageType
{
	None,

	//Input
	InputAccept,
	InputBack,


	InputUpHit,
	InputUpIsDown,
	InputUpReleased,

	InputDownHit,
	InputDownIsDown,
	InputDownReleased,

	InputLeftHit,
	InputLeftIsDown,
	InputLeftReleased,

	InputReload,

	InputRightHit,
	InputRightIsDown,
	InputRightReleased,
	InputEscHit,

	InputPauseHit,
	InputUnPauseHit,

	InputMouseMoved,
	InputMouseStopedMoving,
	InputLeftMouseHit,
	InputLeftMouseDown,
	InputShootButtonDown,
	InputLeftMouseReleased,
	InputRightMouseHit,
	InputRightMouseDown,
	InputRightMouseReleased,
	InputMiddleMouseHit,
	InputMiddleMouseDown,
	InputScrollUp,
	InputScrollDown,

	InputJumpHit,
	InputCrouchHit,
	InputCrouchReleased,
	InputRunHit,
	InputRunReleased,
	InputInteractHit,

	InputToggleInventory,

	InputFPSMode,

	InputEquipmentSlot1Hit,
	InputEquipmentSlot2Hit,
	InputEquipmentSlot3Hit,
	InputEquipmentSlot4Hit,
	InputEquipmentSlot5Hit,
	InputEquipmentSlot6Hit,
	InputEquipmentSlot7Hit,
	InputEquipmentSlot8Hit,
	InputEquipmentSlot9Hit,
	InputEquipmentSlot10Hit,

	//STATES
	PushState,
	PopState,

	PlayIntro,

	NextRenderPass,
	MainMenuStateActivated,

	//LEVEL
	StartLoading,
	UnloadLevel,
	NewLevelLoaded,
	NewOctreeCreated,
	CurrentLevel,
	ChangeLevel,
	NextLevel,
	LoadLevel,
	SerializePhysXObjects,

	//PLAYER
	InventoryToggled,
	PlayerDying,
	PlayerDied,
	SetPlayerMaxHP,
	SetPlayerHP,
	PlayerWalking,
	PlayerStoppedWalking,
	WeaponRecoil,
	LockPlayer,
	UnlockPlayer,

	WateringPlant,
	HoeingGround,
	PlantingSeed,
	HarvestingPlant,
	FillingWaterbucket,
	LeavingHen,

	//Enemies
	EnemyTakeDamage,
	EnemyDied,
	EnemyMouseHover,
	EnemyMouseHoverReleased,

	//Trader
	RespawnTrader,
	DespawnTrader,
	TradeStarted,
	TradeEnded,

	//Spawn
	SpawnEnemy,
	SpawnStaticObject,
	SpawnDynamicObject,
	SpawnDestructibleObject,
	SpawnTriggerBox,
	SpawnPointLight,
	SpawnInteractable,
	CreateSeed,
	SpawnAbility,
	SpawnAbilityAt,
	CreateDamageNumber,
	SpawnGBPhysXBox,
	SpawnBulletHitParticle,
	SpawnMuzzleFlash,
	SpawnItem, // mydata is a pointer to ItemCreationStruct
	RiverCreated,
	SpawnChicken,

	//Camera
	SetCameraOffset,

	//Event
	FadeIn,
	FadeInComplete,
	FadeOut,
	FadeOutComplete,
	FinnishGame,
	StartCameraShake,
	TriggerSplineSpawn,
	TriggerEvent,
	WindowResize,
	ResizeWindow,
	GameActive,
	GameNotActive,
	BulletHit,
	LookingAtInteractable,
	EntityInteractedWith,
	NewDay,
	UpdateTime,
	GoToSleep,
	StartDialogue,
	DialogueStarted,
	DialogueOver,
	EndGame,
	SendUIGameMessage,

	//Mouse Hover objects
	MouseHoverNone,
	MouseHoverSheep,
	MouseHoverHoe,
	MouseHoverWaterBucket,
	MouseHoverCanPlant,
	MouseHoverCanHarvest,
	MouseHoverRiver,

	// UI Events
	ShouldUseItem,
	ChangedItem,

	//Audio
	CurrentMasterVolume,
	SetMasterVolume,
	StartInGameAudio,

	//Secrets
	EnableDiscoMode,
	EnableHastfan,
	SuperTinyWindow,
	WindowSmallerThanAMouse,
	PleaseResetTheWindowThanks,
	GiveMeAdam,
	EnableScreeps,

	//Menu
	MenuButtonActive,
	MenuButtonHit,

	//NetWork
	AnnounceDeathMarker,
	DeathMarkerRecieved,
	ConnectedToServer,

	//Bob
	WinGameAndAlsoLife,
	RequestUISetupPtrs,
	StoredCalories,
	count
};

class BaseState;

class Message
{
public:
	Message()
	{
		Reset();
	}

	Message(const MessageType& aMessageType)
	{
		Reset();
		myMessageType = aMessageType;
	}

	~Message() = default;

	void Reset()
	{
		myMessageType = MessageType::None;
		myText = "";
		myIntValue = 0;
		myIntValue2 = 0;
		myFloatValue = 0.0f;
		myFloatValue2 = 0.0f;
		myBool = false;
		myState = nullptr;
		myData = nullptr;
	}

	MessageType myMessageType;
	std::string_view myText;
	int myIntValue;
	int myIntValue2;
	float myFloatValue;
	float myFloatValue2;
	bool myBool;
	BaseState* myState;
	void* myData;
};