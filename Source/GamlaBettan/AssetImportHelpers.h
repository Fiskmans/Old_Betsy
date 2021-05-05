#pragma once
#include <Matrix4x4.hpp>
#include "AssImp/scene.h"

namespace AiHelpers
{
	CommonUtilities::Matrix4x4<float> ConvertToEngineMatrix33(const aiMatrix3x3& AssimpMatrix);

	// constructor from Assimp matrix
	CommonUtilities::Matrix4x4<float> ConvertToEngineMatrix44(const aiMatrix4x4& AssimpMatrix);
}