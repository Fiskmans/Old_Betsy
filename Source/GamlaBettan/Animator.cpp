#include "pch.h"
#include "Animator.h"
#include "AnimationController.h"


void Animator::Init(const std::string& aRig, std::vector<BoneInfo>* aBoneinfo, std::vector<std::string>& somePathsToAnimations)
{
	myController = new AnimationController(aRig.c_str());
	bool shouldLoadAnims;
	{
		PERFORMANCETAG("Import")
	myController->Import3DFromFile(aRig, aBoneinfo, shouldLoadAnims);
	}
	SYSVERBOSE("Creating animator for " + aRig + " with " + std::to_string(somePathsToAnimations.size()) + " animation files");
	if (shouldLoadAnims)
	{
		PERFORMANCETAG("Add from file");
		for (std::string& s : somePathsToAnimations)
		{
			SYSVERBOSE("Baking: " + s);
			myController->Add3DAnimFromFile(s);
		}
	}
	myController->SetAnimIndex(1, true, 5.0f);
	myBoneInfo = aBoneinfo;
}

Animator::~Animator()
{
	SAFE_DELETE(myController);
	SYSVERBOSE("Animator Destroyed")
}

void Animator::BoneTransformWithBlend(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms, float aBlendFactor)
{
	myController->SetBlendTime(aBlendFactor);
	myController->BoneTransform(Transforms);
}

void Animator::BoneTransform(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms)
{
	myController->BoneTransform(Transforms);
}

void Animator::Step(float aDelta)
{
	myController->Update(aDelta);
}

void Animator::SetBlend(float aBlend)
{
	myController->SetBlendTime(aBlend);
}
void Animator::SetTime(float aTime)
{
	myController->SetTime(aTime);
}

float Animator::GetTime()
{
	return myController->GetAnimTime();
}
void Animator::SetState(size_t aState, bool aKeepUpdatingOldAnim)
{
	SYSVERBOSE("Animator Set State " + std::to_string(static_cast<int>(aState)));
	myController->SetAnimIndex(CAST(uint, aState), aKeepUpdatingOldAnim, 0.1f);
}

bool Animator::DoneBlending()
{
	return myController->IsDoneBlending();
}

bool Animator::Looped()
{
	return myController->JustLooped();
}

size_t Animator::GetAnimationCount()
{
	return myController->GetMaxIndex();
}

M44F Animator::TransformOfBone(int aBone)
{
	return myController->TransformOfBone(aBone);
}

size_t Animator::GetTickCount()
{
	return myController->GetTickCount();
}

float Animator::GetCurrentProgress()
{
	return myController->GetAnimTime();;
}

std::vector<BoneInfo>* Animator::GetBoneInfo()
{
	return myBoneInfo;
}
