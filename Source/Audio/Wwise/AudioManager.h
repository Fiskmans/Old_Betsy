#pragma once
#include <array>
#include "Observer.hpp"
#include "Publisher.hpp"

typedef unsigned __int64 AudioObjectID;
typedef unsigned long AudioEventID;
typedef unsigned long AudioGameParameterID;

struct LMVector
{
	float x;
	float y;
	float z;
};

struct TimedAudioEvent
{
	float myTimer = 0;
	AudioEventID myEvent;
	AudioObjectID myObjectID;
};

class GameObject;
class WwiseFramework;

class AudioManager : public Observer, public Publisher
{
public:
	AudioManager();
	~AudioManager();

	void Init();
	void ShutDown();

	void Update(float aDeltaTime);

	void RecieveMessage(const Message& aMessage) override;

	//Main Functions
	void SetMasterVolume(float aVolume);

	void PostEvent(const AudioEventID& aEvent, const AudioObjectID& aObjectID);
	void SetGameParameterValue(const AudioGameParameterID& aParameter, const float& aValue, const AudioObjectID& aObjectID, const int& aTimeMS = 0);

	void RegisterGameObject(const int& aObjectID);
	void UnRegisterGameObject(const int& aObjectID);

	void Register2DMasterObject(const AudioObjectID& anID);

	void SetDefaultListener(const int& aObjectID);

	LMVector GetObjectPosition(const AudioObjectID& aObjectID);
	LMVector GetListenerPosition();
	int GetListenerID();

	void UpdateObjectTransform(const LMVector& aPosition, const LMVector& aForward, const LMVector& aTop, const int& aObjectID);

private:

	void SubscribeToMyMessages();
	void UnSubscribeToMyMessages();

	WwiseFramework* myWwiseFramework;

	float myMasterVolume;
	
	AudioObjectID myListenerID;
	AudioObjectID my2DMasterObjectID;

	int myDayTime = 0;
};