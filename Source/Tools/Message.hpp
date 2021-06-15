#pragma once
#include <string>

enum class MessageType
{
	None,

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

	//STATES
	PushState,
	PopState,

	PlayIntro,

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

	WindowResize,
	ResizeWindow,

	// UI Events
	ShouldUseItem,
	ChangedItem,

	//Audio
	CurrentMasterVolume,
	SetMasterVolume,
	StartInGameAudio,

	//Secrets
	SuperTinyWindow,
	WindowSmallerThanAMouse,
	PleaseResetTheWindowThanks,

	count
};

namespace MessageStructs
{
	struct UpdateTimeData
	{
		float myDayPercent;
		int myHour;
		int myDay;
	};

	struct LevelLoaded
	{
		int myWorldAxisSize;
		int myLevelIndex;
		V3F myPlayerStart;
	};
}

class BaseState;

class Message
{
public:
	Message()
	{
	}

	Message(const MessageType& aMessageType)
	{
		myMessageType = aMessageType;
	}

	~Message() = default;

	MessageType myMessageType = MessageType::None;
	const void* myData = nullptr;
};