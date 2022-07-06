#ifndef ENGINE_ASSETS_ANIMATION_DATA_H
#define ENGINE_ASSETS_ANIMATION_DATA_H

#include "tools/MathVector.h"
#include "tools/Matrix4x4.h"

#include "common/Macros.h"

#include <string>

namespace engine
{
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
		tools::M44f BoneOffset;
		tools::M44f FinalTransformation;
		tools::V3f BonePosition;
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
		tools::M44f childBoneOffsetInversed;
		BoneInfo parentBoneData;
		tools::M44f parentBoneOffsetInversed;
	};
}

#endif