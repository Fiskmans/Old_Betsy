#pragma once
#include <vector>
#include <Matrix4x4.hpp>
#include "AnimationData.h"


struct AnimationTrack
{
	AssetHandle myAnimation;

	float mySpeed = 1.f;
	float myTime = 0.f;
	float myWeight = 1.f;

	bool myLoop = true;
	float myTimeBeforeFalloff;

	bool myFading = false;
	float myStartedFadingAt;
	float myFullyFadedAt;
};

class Animator
{

public:

	class TrackID
	{
	public:
		bool operator==(const TrackID& aOther) { return myId != -1 && aOther.myId == myId; }
		bool operator!=(const TrackID& aOther) { return !(*this == aOther); }

	private:
		friend Animator;
		
		size_t myId = -1;
	};

	void Init(std::vector<BoneInfo>* aBoneinfo);
	~Animator();

	TrackID AddAnimation(const AnimationTrack& aTrack);
	void StopAnimation(const TrackID& aID);
	void FadeAnimation(const TrackID& aID, float aTimeToZero);

	void Update(float aDt);

	void BoneTransform(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms);
	M44f TransormOfBone(size_t aBoneIdex);

private:
	static size_t ourTrackIdCounter;

	struct IDableTrack
	{
		TrackID myID;
		AnimationTrack myTrack;
	};

	std::vector<IDableTrack> myTracks;

	std::vector<BoneInfo>* myBoneInfo;
};

