#pragma once
#include "Particle.h"

class ParticleInstance
{
public: 

	void Init(Particle* aParticle);
	void SetBounds(V4F aMin, V4F aMax);
	void SetDirection(V4F aDirection);

	void Update(const float aDeltaTime, const V3F& aCameraPosition);

	Particle* GetParticle();
	const std::vector<Particle::Vertex>& GetParticleVertices() const;
	const CU::Matrix4x4<float>& GetTransform() const;

	void RefreshTimeout(float aTimeout);

	bool IsDead();
	bool IsEternal();

	bool SetEternal(bool aEternal);

private:
	bool HasTimedOut();

	bool myIsEternal = false;

	Particle* myParticle = nullptr;
	float myTime = 0.f;
	float myTimeout;
	V4F myBoundsMin;
	V4F myBoundsMax;

	V4F myDirection;

	CU::Matrix4x4<float> myTransform;
	std::vector<Particle::Vertex> myParticleVertices;
};

