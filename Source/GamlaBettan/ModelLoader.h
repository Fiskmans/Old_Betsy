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
			myEmpty = (another.myEmpty == true ? true : false); //Mumsmums

			return (*this);
		}

	} myHandovers[myHandoverSlots];

public:
	ModelLoader(); 
	~ModelLoader();

	Skybox* InstanciateSkybox(std::string aFilepath);

	_NODISCARD ModelInstance* InstantiateModel(std::string aFilePath);
	_NODISCARD ModelInstance* InstanciateCube();
	_NODISCARD ModelInstance* InstanciateCube(CommonUtilities::Vector4<float>& aPosition, bool aRandomizeRotation = false);
	void InstanciateCubes(Scene* aScene, std::vector<CommonUtilities::Vector4<float>>& aPositionList, bool aRandomizeRotation = false);
	bool Init(DirectX11Framework* aFramework);
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myDeviceContext;

#if USEIMGUI
	std::unordered_map<std::string, Model*>::iterator begin() { return myLoadedModels.begin(); };
	std::unordered_map<std::string, Model*>::iterator end() { return myLoadedModels.end(); };
#endif // !_RETAIL

	bool CompilePixelShader(std::string aData, ID3D11PixelShader*& aShaderOutput);
	bool CompileVertexShader(std::string aData, ID3D11VertexShader*& aShaderOutput, void* aCompiledOutput);

	void FlushChanges();

	void SetGbPhysX(GBPhysX* aGbPhysX);
private:

	void PrepareModel(Model* aModel, const std::string& aPath);
	_NODISCARD Model* LoadModel(const std::string& aFilePath);
	void ReloadModel(const std::string& aFilePath);

	void LoadLoop();

	void QueueLoad(Model* aModel, std::string aFilePath);


	bool InternalInit(ID3D11Device* aDevice);

	bool myIsRunning = true;
	Model* myCubeModel;
	std::unordered_map<std::string, Model*> myLoadedModels;

#if USEFILEWATHCER
	Tools::FileWatcher myWatcher;
	std::vector<Tools::FileWatcher::UniqueID> myfileHandles;
#endif

	std::thread myWorkHorse;
	GBPhysX* myGbPhysX;
};

