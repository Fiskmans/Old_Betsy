#pragma once

#if __INTELLISENSE__
#include <pch.h>
#endif // __INTELLISENSE__


struct SpotLight
{
	Camera* myCamera;
	AssetHandle myTexture;
	float myRange;
	float myIntensity;
};