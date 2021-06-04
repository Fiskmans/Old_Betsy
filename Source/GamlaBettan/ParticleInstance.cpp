#include "pch.h"
#include "ParticleInstance.h"
#include <Random.h>
#include "WindSystem.h"

void ParticleInstance::Init(Particle* aParticle)
{
	myTimeout = 2;
	myTime = aParticle->GetData().myCustomizable.mySpawnRate * Tools::RandomNormalized();
	myParticle = aParticle;
}

void ParticleInstance::SetBounds(V4F aMin, V4F aMax)
{
	myBoundsMin = aMin;
	myBoundsMax = aMax;
}

void ParticleInstance::SetDirection(V4F aDirection)
{
	myDirection = aDirection;
}

void ParticleInstance::Update(const float aDeltaTime, const V3F& aCameraPosition)
{
	myTimeout -= aDeltaTime;
	Particle::Data& particleData = myParticle->GetData();

	if (!HasTimedOut())
	{
		myTime += aDeltaTime;
		float particleInterval = 1.f / particleData.myCustomizable.mySpawnRate;
		while (myTime > particleInterval)
		{
			V4F vel = myDirection.GetNormalized() + V4F(Tools::RandomDirection(),0) * particleData.myCustomizable.mySeperation;
			vel.Normalize();
			vel *= Tools::RandomNormalized() * particleData.myCustomizable.myParticleSpeed;

			myParticleVertices.push_back(Particle::Vertex(
				Tools::RandomRange(myBoundsMin, myBoundsMax),
					vel,		
					particleData.myCustomizable.myStart.myParticleColor,
					V2f(particleData.myCustomizable.myStart.mySize, particleData.myCustomizable.myStart.mySize)
				));
			myTime -= particleInterval;

		}
	}

	V4F gravity = V4F(particleData.myCustomizable.myGravity * aDeltaTime, 0);

	WindSystem& wind = WindSystem::GetInstance();
	for (int index = static_cast<int>(myParticleVertices.size())-1; index >= 0; --index)
	{
		myParticleVertices[index].myLifetime += aDeltaTime;
		float part = myParticleVertices[index].myLifetime / particleData.myCustomizable.myParticleLifetime;
		part = CLAMP(0.0f, 1.0f, part);

		myParticleVertices[index].myMovement += gravity;
		V3F referenceFrame = wind.GetWindAmount(myParticleVertices[index].myPosition);
		V4F referenceSpeed = myParticleVertices[index].myMovement - V4F(referenceFrame,0);
		referenceSpeed *= (1.f - particleData.myCustomizable.myDrag * aDeltaTime);
		myParticleVertices[index].myMovement = referenceSpeed + V4F(referenceFrame,0);
	
		myParticleVertices[index].myPosition += myParticleVertices[index].myMovement * aDeltaTime;


		float size = LERP(particleData.myCustomizable.myStart.mySize, particleData.myCustomizable.myEnd.mySize, part);

		myParticleVertices[index].myColor = LERP(particleData.myCustomizable.myStart.myParticleColor, particleData.myCustomizable.myEnd.myParticleColor, part);
		myParticleVertices[index].mySize = V2f(size, size);
		myParticleVertices[index].myDistanceToCamera = V3F(myParticleVertices[index].myPosition).DistanceSqr(aCameraPosition);

		if (particleData.myCustomizable.myFlipBook.myPages > 0)
		{
			float timePerFrame = particleData.myCustomizable.myFlipBook.myTime / particleData.myCustomizable.myFlipBook.myPages;

			V2f frameSize = { 1.f / particleData.myCustomizable.myFlipBook.mySizeX, 1.f / particleData.myCustomizable.myFlipBook.mySizeY };

			for (int i = 0; i <= particleData.myCustomizable.myFlipBook.myPages; ++i)
			{
				if (myParticleVertices[index].myFlipBookTimer < timePerFrame + (i * timePerFrame))
				{
					float x = CAST(float, i % (particleData.myCustomizable.myFlipBook.mySizeX / 1));
					float y = CAST(float, i / (particleData.myCustomizable.myFlipBook.mySizeY / 1));

					float minX = x * frameSize.x;
					float minY = y * frameSize.y;

					float maxX = minX + frameSize.x;
					float maxY = minY + frameSize.y;

					myParticleVertices[index].myUVMin = { minX, minY };
					myParticleVertices[index].myUVMax = { maxX, maxY };
					break;
				}
			}

			myParticleVertices[index].myFlipBookTimer += aDeltaTime;
			if (myParticleVertices[index].myFlipBookTimer >= particleData.myCustomizable.myFlipBook.myTime)
			{
				myParticleVertices[index].myFlipBookTimer = 0;
			}
		}

		if (myParticleVertices[index].myLifetime > particleData.myCustomizable.myParticleLifetime)
		{
			myParticleVertices.erase(myParticleVertices.begin() + index);
		}
	}

	std::sort(myParticleVertices.begin(), myParticleVertices.end(), [](const Particle::Vertex& aFirst, const Particle::Vertex& aSecond) -> bool
		{
			return aFirst.myDistanceToCamera > aSecond.myDistanceToCamera;
		});
}

Particle* ParticleInstance::GetParticle()
{
	return myParticle;
}

const CU::Matrix4x4<float>& ParticleInstance::GetTransform() const
{
	return myTransform;
}

void ParticleInstance::RefreshTimeout(float aTimeout)
{
	myTimeout = aTimeout;
}

bool ParticleInstance::IsDead()
{
	return HasTimedOut() && myParticleVertices.size() == 0;
}

bool ParticleInstance::IsEternal()
{
	return myIsEternal;
}

bool ParticleInstance::SetEternal(bool aEternal)
{
	return myIsEternal = aEternal;
}

bool ParticleInstance::HasTimedOut()
{
	return myTimeout < 0;
}

const std::vector<Particle::Vertex>& ParticleInstance::GetParticleVertices() const
{
	return myParticleVertices;
}
