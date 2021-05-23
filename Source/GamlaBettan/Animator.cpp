#include "pch.h"
#include "Animator.h"
#include "AnimationLoader.h"

size_t Animator::ourTrackIdCounter;

void Animator::Init(std::vector<BoneInfo>* aBoneinfo)
{
	myBoneInfo = aBoneinfo;
}

Animator::~Animator()
{
	SYSVERBOSE("Animator Destroyed")
}

void Animator::BoneTransform(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms)
{
	if (myTracks.empty())
	{
		for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
		{
			Transforms[i] = M44F::Identity();
		}
		return;
	}

	float totalWeight[NUMBEROFANIMATIONBONES] = { 0.f };

	std::vector<TrackID> toRemove;

	for (auto& animationTrack : myTracks)
	{
		Animation* anim = animationTrack.myTrack.myAnimation.GetAsAnimation();
		
		float weight = animationTrack.myTrack.myWeight;

		if (!animationTrack.myTrack.myLoop)
		{
			weight *= CLAMP(0.f, 1.f, INVERSELERP(0.f,animationTrack.myTrack.myTimeBeforeFalloff,anim->GetTimeToLoop(animationTrack.myTrack.myTime)));
		}

		if (animationTrack.myTrack.myFading)
		{
			weight *= CLAMP(0.f, 1.f, 1.f - INVERSELERP(animationTrack.myTrack.myStartedFadingAt, animationTrack.myTrack.myFullyFadedAt, animationTrack.myTrack.myTime));
		}

		if (animationTrack.myTrack.myLoop)
		{
			anim->GetInterpolation(animationTrack.myTrack.myTime, weight, Transforms);
		}
		else
		{
			if (anim->GetInterpolationUnlooped(animationTrack.myTrack.myTime, weight, Transforms))
			{
				toRemove.push_back(animationTrack.myID);
			}
		}

		for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
		{
			totalWeight[i] += anim->myWeights[i];
		}
	}
	for (auto& i : toRemove)
	{
		StopAnimation(i);
	}

	for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
	{
		Transforms[i] /= totalWeight[i];
	}
}

M44F Animator::TransormOfBone(size_t aBoneIdex)
{
	std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES> allBones;
	BoneTransform(allBones);
	return allBones[aBoneIdex];
}

Animator::TrackID Animator::AddAnimation(const AnimationTrack& aTrack)
{
	TrackID id;
	id.myId = ++ourTrackIdCounter;

	myTracks.push_back(
		{
			id,
			aTrack
		});

	return id;
}

void Animator::StopAnimation(const TrackID& aID)
{
	auto it = myTracks.begin();
	while (it != myTracks.end())
	{
		if (it->myID == aID)
		{
			myTracks.erase(it);
			break;
		}
		it++;
	}
}

void Animator::FadeAnimation(const TrackID& aID, float aTimeToZero)
{
	auto it = myTracks.begin();
	while (it != myTracks.end())
	{
		if (it->myID == aID)
		{
			it->myTrack.myFading = true;
			it->myTrack.myStartedFadingAt = it->myTrack.myTime;
			it->myTrack.myFullyFadedAt = it->myTrack.myTime + aTimeToZero;
			break;
		}
		it++;
	}
}

void Animator::Update(float aDt)
{
	for (auto& animationTrack : myTracks)
	{
		animationTrack.myTrack.myTime += animationTrack.myTrack.myAnimation.GetAsAnimation()->myFPS * aDt;
	}
}
