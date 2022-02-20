#include "pch.h"
#include "AnimationData.h"

VertexBoneData::VertexBoneData()
{
	Reset();
};

void VertexBoneData::Reset()
{
	WIPE(IDs);
	WIPE(Weights);
}

void VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (unsigned int i = 0; i < MAX_NUM_BONES_PER_VEREX; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}