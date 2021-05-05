#pragma once
#include <unordered_map>

class DirectX11Framework;
class ParticleInstance;
class Particle;
class Scene;

class ParticleFactory
{
public:
	ParticleFactory();
	~ParticleFactory();

	bool Init(DirectX11Framework* aFramework);

	_NODISCARD ParticleInstance* InstantiateParticle(const std::string& aFilePath);

#if USEIMGUI
	void EditParticles(Scene* aScene);
#endif

private:
	_NODISCARD Particle* LoadParticle(const std::string& aFilePath);

	struct ID3D11Device* myDevice;
	struct ID3D11GeometryShader* myGeometryShader;
	class PixelShader* myPixelShader;
	class VertexShader* myVertexShader;
	struct ID3D11InputLayout* myInputLayout;

	std::unordered_map<std::string, Particle*> myParticles;
};

