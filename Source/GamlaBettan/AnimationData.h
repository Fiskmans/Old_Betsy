#pragma once

struct VertexBoneData
{
	unsigned int IDs[MAX_NUM_BONES_PER_VEREX];
	float Weights[MAX_NUM_BONES_PER_VEREX];

	VertexBoneData();

	void Reset();

	void AddBoneData(unsigned int BoneID, float Weight);
};

enum class HitBoxType
{
	Head,
	Neck,
	Hip,
	LowerTorso,
	LowerMidTorso,
	UpperMidTorso,
	UpperTorso,
	Torso,
	Shoulder,
	Clavicle,
	UpperArm,
	LowerArm,
	Hand,
	Thigh,
	LowerLeg,
	Toe,
	Foot,
	Count,
	None
};

struct BoneInfo
{
	CommonUtilities::Matrix4x4<float> BoneOffset;
	CommonUtilities::Matrix4x4<float> FinalTransformation;
	V4F BonePosition;
	size_t parent = ~0ULL;
	std::string myName;
};

struct HitBox
{
	std::string myName;
	HitBoxType myHitBoxType = HitBoxType::None;
	int myOriginNodeIndex = -1;
	int myTargetNodeIndex = -1;
	BoneInfo childBoneData;
	CommonUtilities::Matrix4x4<float> childBoneOffsetInversed;
	BoneInfo parentBoneData;
	CommonUtilities::Matrix4x4<float> parentBoneOffsetInversed;
};


