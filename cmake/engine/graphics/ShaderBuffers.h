#ifndef ENGINE_GRAPHICS_SHADER_BUFFERS_H
#define ENGINE_GRAPHICS_SHADER_BUFFERS_H

#include "tools/Matrix4x4.h"
#include "tools/MathVector.h"

#include "common/Macros.h"

namespace engine::graphics 
{
	struct FrameBufferData
	{
		tools::M44f myWorldToCamera;
		tools::M44f myCameraToProjection;

		tools::V3f myEnvironmentLightDirection;
		float myShadowIntensity;

		tools::V3f myEnvironmentLightColor;
		float myEnviromentLightIntensity; 

		tools::V3f myCameraPosition;
		float myTotalTime;

		tools::M44f myWorldToShadowCamera;
		tools::M44f myCameraToShadowProjection;

		tools::V3f myCameraDirection;

		float padding;
	};

	struct ObjectBufferData
	{
		tools::M44f myModelToWorldSpace;
		tools::V4f myTint;

		unsigned int padding_1;
		unsigned int padding_2;
		float myObjectLifeTime;
		unsigned int myObjectId;
	};

	struct PointLightBuffer
	{
		struct PointLight
		{
			tools::V3f position;
			float intensity;

			tools::V3f color;
			float range;

		} myPointLights[NUMBEROFPOINTLIGHTS];

		unsigned int myNumOfUsedPointLights;
		float padding[3];
	};

	struct AnimationBuffer
	{
		tools::M44f myTransforms[NUMBEROFANIMATIONBONES];
	};
}

#endif // 
