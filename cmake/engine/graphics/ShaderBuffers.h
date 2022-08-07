#ifndef ENGINE_GRAPHICS_SHADER_BUFFERS_H
#define ENGINE_GRAPHICS_SHADER_BUFFERS_H

#include "tools/Matrix4x4.h"
#include "tools/MathVector.h"

#include "common/Macros.h"

namespace engine::graphics 
{
	constexpr size_t FRAME_BUFFER_INDEX = 0;
	constexpr size_t OBJECT_BUFFER_INDEX = 1;
	constexpr size_t POINT_LIGHT_BUFFER_INDEX = 2;
	constexpr size_t ANIMATION_BUFFER_INDEX = 3;

	struct FrameBuffer
	{
		tools::M44f myWorldToCamera;
		tools::M44f myCameraToProjection;

		float myTotalTime;
		float myPadding[3];
	};

	struct ObjectBuffer
	{
		tools::M44f myModelToWorldSpace;
		tools::V4f myDiffuseColor;

		float myObjectLifeTime;
		unsigned int myObjectId;

		float myPadding[2];
	};
	
	struct PointLight
	{
		tools::V3f myPosition;
		float myIntensity;

		tools::V3f mycolor;
		float myRange;
	};

	struct PointLightBuffer
	{
		PointLight myPointLights[NUMBEROFPOINTLIGHTS];

		unsigned int myNumOfUsedPointLights;
		float myPadding[3];
	};

	struct AnimationBuffer
	{
		tools::M44f myTransforms[NUMBEROFANIMATIONBONES];
	};
}

#endif // 
