#ifndef ENGINE_GRAPHICS_POINT_LIGHT_H
#define ENGINE_GRAPHICS_POINT_LIGHT_H

#include "tools/MathVector.h"

namespace engine
{
	class PointLight
	{
		friend class LightLoader;
	public:
		tools::V3f position;
		float intensity = 0.f;
		tools::V3f color =  tools::V3f( 1.f, 1.f, 1.f );
		float range = 0.f;
		int id = 0;
	};
}

#endif