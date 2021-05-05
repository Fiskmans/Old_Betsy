#pragma once

#if __INTELLISENSE__
#include <pch.h>
#endif // __INTELLISENSE__
#include "Decal.h"


class DecalFactory
{
public:
	void Init(ID3D11Device* aDevice);

	Decal* LoadDecal(const std::string& aDecalPath, V3F aRotation, V3F aPosition);
	Decal* LoadDecal(const std::string& aDecalPath, Camera* aCameraToCopyFrom);


private:
	ID3D11Device* myDevice;
};

