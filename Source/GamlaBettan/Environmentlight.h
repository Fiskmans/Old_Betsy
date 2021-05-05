#pragma once
#include <Vector3.hpp>

struct ID3D11ShaderResourceView;

struct EnvironmentLight
{
	ID3D11ShaderResourceView* myTexture = nullptr;
	V3F myDirection;
	V3F myColor;
	V3F myShadowCorePosition;
	float myIntensity;
};
