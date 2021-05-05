#pragma once
#include "Component.h"
#include <Vector3.hpp>
#include <vector>

class ParticleFactory;
class Scene;
class ParticleInstance;

class ParticlesWhileAlive : public Component
{
public:
	void PreInit(Scene* aScene, ParticleFactory* aFactory);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetParticle(std::string aParticle, V3F aBoundsMin = { 0,0,0 }, V3F aBoundsMax = {1,1,1}, V3F aOffset = { 0,0,0 });

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	struct handle
	{
		V3F myBoundsMin;
		V3F myBoundsMax;
		ParticleInstance* myParticle = nullptr;
		V3F myOffset;
	};

	std::vector<handle> myParticles;
	Scene* myScene = nullptr;
	ParticleFactory* myFactory = nullptr;
};