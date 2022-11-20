#ifndef ENGINE_GRAPHICS_RELEASABLE_H
#define ENGINE_GRAPHICS_RELEASABLE_H

namespace concepts
{
	template<class T>
	concept Releaseable = requires(T aValue)
	{
		aValue.Release();
	};
}

#endif