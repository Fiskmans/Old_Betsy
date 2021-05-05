#pragma once

typedef unsigned __int64 AudioObjectID;
typedef unsigned long AudioEventID;
typedef unsigned long AudioGameParameterID;

class AkTransform;

class WwiseFramework 
{
public:
	WwiseFramework();
	~WwiseFramework();

	void Init();
	void Terminate();

	void LoadBank(const char* aBankName);
	void UnlloadBank(const char* aBankName);

	void SetMasterVolume(float aVolume);

	void RegisterObject(const AudioObjectID& aObjectID);
	void UnRegisterObject(const AudioObjectID& aObjectID);
	void SetDefaultListener(const AudioObjectID& aObjectID);

	void ProcessAudio();
	void PostEvent(const AudioEventID& aEvent, const AudioObjectID& aObjectID);

	void SetGameParameterValue(const AudioGameParameterID& aParameter, const float& aValue, const AudioObjectID& aObjectID, const int& aTimeMS);
	void SetObjectTransform(AkTransform aTransform, const AudioObjectID& aObjectID);

	AkTransform GetObjectPosition(const AudioObjectID& aObjectID);

private:

	bool InitWwise();
	void TerminateSoundEngine();
};