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
	ModelLoader(); 
	~ModelLoader();

	_NODISCARD Asset* LoadModel(const std::string& aFilePath);
	_NODISCARD Asset* LoadSkybox(const std::string& aFilePath);

	bool Init(DirectX11Framework* aFramework);
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myDeviceContext;

	bool CompilePixelShader(std::string aData, ID3D11PixelShader*& aShaderOutput);
	bool CompileVertexShader(std::string aData, ID3D11VertexShader*& aShaderOutput, void* aCompiledOutput);

	void FlushChanges();

	void SetGbPhysX(GBPhysX* aGbPhysX);
private:

	void PrepareModel(Model* aModel, const std::string& aPath);

	void LoadLoop();

	void QueueLoad(Model* aModel, std::string aFilePath);


	bool InternalInit(ID3D11Device* aDevice);

	bool myIsRunning = true;

#if USEFILEWATHCER
	Tools::FileWatcher myWatcher;
	std::vector<Tools::FileWatcher::UniqueID> myfileHandles;
#endif

	std::thread myWorkHorse;
	GBPhysX* myGbPhysX;
};

