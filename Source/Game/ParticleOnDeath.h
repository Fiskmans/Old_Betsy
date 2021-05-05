#pragma once
#include "Component.h"
class Scene;
class ParticleFactory;
class ParticlesOnDeath :
	public Component
{
public:
	ParticlesOnDeath();

	void PreInit(Scene* aScene, ParticleFactory* aFactory);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetParticle(std::string aParticle, float aSize, V3F aOffset = V3F(0.f, 0.f, 0.f));
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	std::string myParticleType;
	float mySize;
	V3F myOffset;


	Scene* myScene;
	ParticleFactory* myFactory;
};

