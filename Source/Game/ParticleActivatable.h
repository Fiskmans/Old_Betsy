#pragma once
#include "Component.h"
class Scene;
class ParticleFactory;

class ParticleActivatable : public Component
{
public:
	ParticleActivatable();
	~ParticleActivatable();

	void PreInit(Scene* aScene, ParticleFactory* aFactory);

	// Inherited via Component
	virtual void Init(Entity* aEntity) override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	void SetParticle(const std::string& aParticle, float aSize, const V3F& aPositionOffset);
	void SetOffset(V3F anOffset);

	void ActivateParticle(float aDuration);

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:

	struct SParticle
	{
		std::string myParticleType = " ";
		V3F myPositionOffset = {0,0,0};
		float mySize = 0;
	};

	std::vector<SParticle> myParticles;

	Scene* myScene;
	ParticleFactory* myFactory;
};