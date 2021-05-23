#pragma once
#include <vector>
#include <thread>
#include <atomic>
#if USEFILEWATHCER
#include "FileWatcher.h"
#endif
#include <string>
#include <unordered_map>
#include "GBPhysX.h"

#include "Asset.h"

class Model;
class ModelInstance;
struct ID3D11Device;
class DirectX11Framework;
class Scene;
class Skybox;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;

namespace CommonUtilities
{
	template<class T>
	class Vector3;
	template<class T>
	class Vector4;
}

class ModelLoader
{
	static const size_t myHandoverSlots = 40;
	struct LoadPackage
	{
		Model* myModel = nullptr;
		std::string myFilePath = "";
		std::atomic<bool> myEmpty = true;

		LoadPackage& operator=(const LoadPackage& another)
		{
			myModel = another.myModel;
			myFilePath = another.myFilePath;
			myEmpty = another.myEmpty.load();

			return (*this);
		}

	} myHandovers[myHandoverSlots];

public:
	ModelLoader(ID3D11Device* aDevice);
	~ModelLoader();

	_NODISCARD Asset* LoadModel(const std::string& aFilePath);
	_NODISCARD Asset* LoadSkybox(const std::string& aFilePath);


private:
	ID3D11Device* myDevice;

	void PrepareModel(Model* aModel, const std::string& aPath);

	void LoadLoop();

	void QueueLoad(Model* aModel, std::string aFilePath);

	bool myIsRunning = true;

	std::thread myWorkHorse;
};

