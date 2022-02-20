#pragma once

struct EnvironmentLight
{
	AssetHandle myTexture;
	V3F myDirection;
	V3F myColor;
	V3F myShadowCorePosition;
	float myIntensity;
};
