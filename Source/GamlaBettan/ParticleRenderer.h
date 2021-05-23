#pragma once
#include <vector>
#include "ParticleInstance.h"
#include "Observer.hpp"
class DirectX11Framework;

class ParticleRenderer
{
public:
	ParticleRenderer() = default;
	~ParticleRenderer() = default;

	bool Init(DirectX11Framework* aFramework);
	void Render(class Camera* aCamera,const std::vector<ParticleInstance*>& aParticleList);
private:
	DirectX11Framework* myFrameWork = nullptr;
	struct ID3D11Buffer* myFrameBuffer = nullptr;
	struct ID3D11Buffer* myObjectBuffer = nullptr;

};