#ifndef ENGINE_GRAPHICS_BONE_BUFFER_H
#define ENGINE_GRAPHICS_BONE_BUFFER_H

#include "common/Macros.h"

#include "tools/Matrix4x4.h"

#include <array>

namespace engine
{
	using BoneTextureCPUBuffer = std::array<std::array<tools::M44f, NUMBEROFANIMATIONBONES>, MAXNUMBEROFANIMATIONSONSCREEN>;
}

#endif