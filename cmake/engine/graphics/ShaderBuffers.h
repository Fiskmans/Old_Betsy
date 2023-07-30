#ifndef ENGINE_GRAPHICS_SHADER_BUFFERS_H
#define ENGINE_GRAPHICS_SHADER_BUFFERS_H

#include "tools/Matrix4x4.h"
#include "tools/MathVector.h"

#include "common/Macros.h"

namespace engine::graphics 
{
	struct FrameBuffer
	{
		tools::M44f myWorldToCamera;
		tools::M44f myCameraToProjection;

		float myTotalTime = 1.f;
		float myPadding[3] = { -1.f, -1.f, -1.f };
	};

	static_assert(offsetof(FrameBuffer, myWorldToCamera) == 0);
	static_assert(offsetof(FrameBuffer, myCameraToProjection) == 64);
	static_assert(offsetof(FrameBuffer, myTotalTime) == 128);

	struct ObjectBuffer
	{
		tools::M44f myModelToWorldSpace;
		tools::V4f myDiffuseColor;

		float myObjectLifeTime = -1.f;
		unsigned int myObjectId = std::numeric_limits<unsigned int>::max();

		float myPadding[2] = { -1.f, -1.f};
	};

	static_assert(offsetof(ObjectBuffer, myModelToWorldSpace) == 0);
	static_assert(offsetof(ObjectBuffer, myDiffuseColor) == 64);
	static_assert(offsetof(ObjectBuffer, myObjectLifeTime) == 80);
	static_assert(offsetof(ObjectBuffer, myObjectId) == 84);

	struct PointLight
	{
		tools::V3f myPosition;
		float myIntensity = -1.f;

		tools::V3f mycolor;
		float myRange = -1.f;
	};

	struct PointLightBuffer
	{
		PointLight myPointLights[NUMBEROFPOINTLIGHTS];

		unsigned int myNumOfUsedPointLights = std::numeric_limits<unsigned int>::max();
		float myPadding[3] = { -1.f, -1.f, -1.f };
	};

	struct AnimationBuffer
	{
		tools::M44f myTransforms[NUMBEROFANIMATIONBONES];
	};

	struct DeferredPixelEnvLightBuffer
	{
		tools::V3f myCameraPosition;
		float myLightIntensity;

		tools::V3f myLightColor;
		float time;

		tools::V3f myLightDirection;
		float trash;

		tools::M44f myToCamera;
		tools::M44f myToProjection;

		tools::V3f myCameraDirection;
		float padding;
	};
}

#endif // 
