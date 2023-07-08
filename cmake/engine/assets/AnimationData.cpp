#include "engine/assets/AnimationData.h"

#include "common/Macros.h"

#include "tools/Logger.h"

namespace engine
{
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

		LOG_SYS_CRASH("out of slots for vertex");
	}
}