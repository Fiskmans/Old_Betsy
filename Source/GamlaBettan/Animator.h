#pragma once
#include <vector>
#include <Matrix4x4.hpp>
#include "AnimationData.h"

class Animator
{

public:
	void Init(const std::string&, std::vector<BoneInfo>* aBoneinfo, std::vector<std::string>& somePathsToAnimations);
	~Animator();

	// ANIMATION STUFF
	void BoneTransform(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms);
	void BoneTransformWithBlend(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms, float aBlendFactor);
	void Step(float aDelta);
	void SetBlend(float aBlend);
	void SetTime(float aTime);
	float GetTime();
	void SetState(size_t aState, bool aKeepUpdatingOldAnim = true);
	bool DoneBlending();
	size_t GetAnimationCount();
	M44F TransformOfBone(int aBone);
	size_t GetTickCount();
	float GetCurrentProgress();
	std::vector<BoneInfo>* GetBoneInfo();

	bool Looped();
private:
	class AnimationController* myController;
	std::vector<BoneInfo>* myBoneInfo;
};

