#pragma once

class Camera;
struct ID3D11ShaderResourceView;

class PointLight
{
	friend class LightLoader;
public:
	V3F position = { 0.f, 0.f, 0.f };
	float intensity = 0.f;
	V3F color = { 1.f, 1.f, 1.f };
	float range = 0.f;
	int id = 0;
#ifndef _RETAIL
	class ModelInstance* myModel = nullptr;
#endif // _RETAIL
};