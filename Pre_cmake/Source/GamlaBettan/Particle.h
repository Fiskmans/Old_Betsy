#pragma once


namespace LegacyParticleTypes
{
	#pragma region OldVersions
	struct Customizable_V0
	{
		float mySpawnRate = 0.0f;
		float mySpawnAgnle = 0.0f;
		float myParticleLifetime = 0.0f;
		float myParticleSpeed = 0.0f;

		struct OverTime
		{
			V4F myParticleColor = V4F(0, 0, 0, 0);
			float mySize = 0.0f;
		} myStart, myEnd;

		char myFilePath[96];
	};

	struct Customizable_V1
	{
		float mySpawnRate = 0.0f;
		float mySeperation = 0.0f;
		float myParticleLifetime = 0.0f;
		float myParticleSpeed = 0.0f;
		V3F myGravity = V3F(0.0f, 0.0f, 0.0f);
		float myDrag = 0.f;

		struct OverTime
		{
			V4F myParticleColor = V4F(0, 0, 0, 0);
			float mySize = 0.0f;
		} myStart, myEnd;

		char myFilePath[96] = { 0 };
	};
	#pragma endregion
}


class Particle
{
public:
	struct Vertex
	{
		Vertex(V4F aPosition, V4F aMovement, V4F aColor, V2f aSize)
		{
			myPosition = aPosition;
			myPosition.w = 1;
			myMovement = aMovement;
			myColor = aColor;
			mySize = aSize;
			myDistanceToCamera = 0;
			myLifetime = 0;
		}
		V4F myPosition;
		V4F myMovement;
		V4F myColor;
		V2f mySize;
		float myDistanceToCamera;
		float myLifetime;

		V2f myUVMin = V2f(0.f, 0.f);
		V2f myUVMax = V2f(1.f, 1.f);
		float myFlipBookTimer = 0;
	};
	struct Data
	{
#pragma warning(push)
#pragma warning(disable : 26812)
		unsigned int myNumberOfParticles = 0;
		unsigned int myStride = 0;
		unsigned int myOffset = 0;
		struct ID3D11Buffer* myParticleVertexBuffer = nullptr;
		AssetHandle myVertexShader;
		AssetHandle myGeometryShader;
		AssetHandle myPixelShader;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		struct ID3D11InputLayout* myInputLayout = nullptr;
		AssetHandle myTexture;
#pragma warning(pop)

		struct Customizable
		{
			float mySpawnRate = 0.0f;
			float mySeperation = 0.0f;
			float myParticleLifetime = 0.0f;
			float myParticleSpeed = 0.0f;
			V3F myGravity = V3F(0.0f, 0.0f, 0.0f);
			float myDrag = 0.f;

			struct OverTime
			{
				V4F myParticleColor = V4F(0, 0, 0, 0);
				float mySize = 0.0f;
			} myStart, myEnd;

			char myFilePath[96] = {0};

			struct FlipBook
			{
				float myTime = 0.0f;
				int myPages = 0;
				int mySizeX = 0;
				int mySizeY = 0;
			} myFlipBook;
		} myCustomizable;
	};

public:
	Particle() = default;
	~Particle() = default;

	void Init(const Data& aData);
	
public:
	Data& GetData() { return myData; };

private:
	Data myData;
};

inline void Particle::Init(const Data& aData)
{
	myData = aData;
}
