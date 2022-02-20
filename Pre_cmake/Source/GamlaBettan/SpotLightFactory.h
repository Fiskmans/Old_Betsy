#pragma once

#if __INTELLISENSE__
#include <pch.h>
#endif // __INTELLISENSE__
#include "SpotLight.h"


class SpotLightFactory
{
public:
	void Init(ID3D11Device* aDevice);

	SpotLight* LoadSpotlight(const std::string& aTexturePath, float aFOV, float aRange, float aIntensity,V3F aRotation,V3F aPosition);
	SpotLight* LoadSpotlight(const std::string& aTexturePath, float aFOV, float aRange, float aIntensity,Camera* aCameraToCopyFrom);

private:
	ID3D11Device* myDevice;
};

