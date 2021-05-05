#include "WwiseFramework.h"

#ifndef _DEBUG
#define AK_OPTIMIZED
#endif // _DEBUG

#include <cstdio>

#include <assert.h>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

#include <AK/SoundEngine/Common/AkQueryParameters.h>

#include <AK/Plugin/AkRoomVerbFXFactory.h>
#include <AK/Plugin/AkSilenceSourceFactory.h>

#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif

#pragma comment (lib, "AkMemoryMgr")
#pragma comment (lib, "AkStreamMgr")

#pragma comment (lib, "AkSoundEngine")

#ifndef Ak_OPTIMIZED
#pragma comment (lib, "CommunicationCentral")
#endif

#pragma comment (lib, "AkMusicEngine")
#pragma comment (lib, "AkSpatialAudio")

#pragma comment (lib, "AkRoomVerbFX")
#pragma comment (lib, "AkSilenceSource")

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

WwiseFramework::WwiseFramework()
{
}

WwiseFramework::~WwiseFramework()
{
}

void WwiseFramework::Init()
{
	if (!InitWwise())
	{
		assert(false);
	}


	g_lowLevelIO.SetBasePath(AKTEXT("Data/Sound/"));
	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));
}

void WwiseFramework::Terminate()
{
	TerminateSoundEngine();
}

void WwiseFramework::LoadBank(const char* aBankName)
{
	AkBankID bankID; // Not used. These banks can be unloaded with their file name.

	AKRESULT eResult = AK::SoundEngine::LoadBank(aBankName, bankID);
	assert(eResult == AK_Success);
}

void WwiseFramework::UnlloadBank(const char* aBankName)
{
	AKRESULT eResult = AK::SoundEngine::UnloadBank(aBankName, NULL);
	assert(eResult == AK_Success);
}


//void WwiseFramework::TestFunction(const AudioObjectID& aObjectID, const AudioObjectID& aListenerID, const AudioObjectID& aAuxID, const float& aValue)
//{
//
//	AK::SoundEngine::Set
//
//	AkAuxSendValue aEnvs[1];
//
//	aEnvs[0].listenerID = AK_INVALID_GAME_OBJECT; // Use the same set of listeners assigned via the SetListeners/SetDefaultListeners API.
//	aEnvs[0].auxBusID = 
//	aEnvs[0].fControlValue = 0.5f;
//
//	AkReal32 volume = 1;
//	AK::SoundEngine::SetGameObjectOutputBusVolume(aObjectID, aListenerID, volume);
//	AK::SoundEngine::SetGameObjectAuxSendValues(aObjectID, aEnvs, 2);
//}

void WwiseFramework::ProcessAudio()
{
	AK::SoundEngine::RenderAudio();
}

void WwiseFramework::RegisterObject(const AudioObjectID& aObjectID)
{
	AK::SoundEngine::RegisterGameObj(aObjectID);
}

void WwiseFramework::UnRegisterObject(const AudioObjectID& aObjectID)
{
	AK::SoundEngine::UnregisterGameObj(aObjectID);
}

void WwiseFramework::SetDefaultListener(const AudioObjectID& aObjectID)
{
	AK::SoundEngine::SetDefaultListeners(&aObjectID, 1);
}

void WwiseFramework::PostEvent(const AudioEventID& aEvent, const AudioObjectID& aObjectID)
{
	AK::SoundEngine::PostEvent(aEvent, aObjectID);
}

void WwiseFramework::SetGameParameterValue(const AudioGameParameterID& aParameter, const float& aValue, const AudioObjectID& aObjectID, const int& aTimeMS)
{
	if (AK::SoundEngine::SetRTPCValue(aParameter, aValue, aObjectID, aTimeMS) != AKRESULT::AK_Success)
	{
		assert(false);
	}
}

void WwiseFramework::SetObjectTransform(AkTransform aTransform, const AudioObjectID& aObjectID)
{
	AK::SoundEngine::SetPosition(aObjectID, aTransform);
}


AkTransform WwiseFramework::GetObjectPosition(const AudioObjectID& aObjectID)
{
	AkTransform pos;
	AK::SoundEngine::Query::GetPosition(aObjectID, pos);
	return pos;
}

bool WwiseFramework::InitWwise()
{
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);
	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		return false;
	}


   // Create and initialize an instance of the default streaming manager. Note
   // that you can override the default streaming manager with your own. 
   //

	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}

	// Create a streaming device with blocking low-level I/O handshaking.
	// Note that you can override the default low-level I/O module with your own. 

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	// Customize the streaming device settings here.
	// CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
	// in the Stream Manager, and registers itself as the File Location Resolver.

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}


	// Create the Sound Engine
	// Using default initialization parameter

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;

	//TODO: ge HWIND till AudioEngine sEman kan stänga av ljud när fönstret inte är aktivt
	//platformInitSettings.hWnd = aHWIND;

	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}


	// Initialize the music engine
    // Using default initialization parameters

	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}


	// Initialize Spatial Audio
	// Using default initialization parameters

	AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 
	
	if (AK::SpatialAudio::Init(settings) != AK_Success)
	{
		assert(!"Could not initialize the Spatial Audio.");
		return false;
	}

#ifndef AK_OPTIMIZED
	// Initialize communications (not in release build!)

	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);

	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}

#endif // AK_OPTIMIZED

	return true;
}

void WwiseFramework::TerminateSoundEngine()
{
	//Kanske ha någon annan stans
	AK::SoundEngine::UnregisterAllGameObj();
	
	AK::SoundEngine::ClearBanks();


#ifndef AK_OPTIMIZED

	AK::Comm::Term();

#endif // AK_OPTIMIZED

	//Finns inte?? Kanske inte behövs??
	//AK::SpatialAudio::Term();

	AK::MusicEngine::Term();

	AK::SoundEngine::Term();

	g_lowLevelIO.Term();

	if (AK::IAkStreamMgr::Get())
	{
		AK::IAkStreamMgr::Get()->Destroy();
	}

	AK::MemoryMgr::Term();
}