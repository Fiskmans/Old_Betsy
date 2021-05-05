#pragma once
#include <vector>
#include "ParticleInstance.h"
#include "Observer.hpp"
class DirectX11Framework;

class ParticleRenderer : public Observer
{
public:
	ParticleRenderer() = default;
	~ParticleRenderer() = default;

	bool Init(DirectX11Framework* aFramework);
	void Render(class Camera* aCamera,const std::vector<ParticleInstance*>& aParticleList);

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;

	void SubscribeToMessages();
	void UnsubscribeToMessages();
private:
	DirectX11Framework* myFrameWork = nullptr;
	struct ID3D11Buffer* myFrameBuffer = nullptr;
	struct ID3D11Buffer* myObjectBuffer = nullptr;

	bool myIsInHastfanMode = false;
	Texture* myHastFan = nullptr;
};