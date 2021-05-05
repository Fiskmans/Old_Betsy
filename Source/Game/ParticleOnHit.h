#pragma once
#pragma once
#include "Component.h"
class Scene;
class ParticleFactory;

class ParticlesOnHit : public Component
{
public:
	ParticlesOnHit();
	~ParticlesOnHit();

	void PreInit(Scene* aScene, ParticleFactory* aFactory);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetParticle(const std::string& aParticle, float aSize, const V3F& aPositionOffset);
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	std::string myParticleType;
	V3F myPositionOffset;
	float mySize;


	Scene* myScene;
	ParticleFactory* myFactory;
};

