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
public:
	ModelLoader(ID3D11Device* aDevice, const std::string& aDefaultPixelShader);
	~ModelLoader();

	_NODISCARD Asset* LoadModel(const std::string& aFilePath);
	_NODISCARD Asset* LoadSkybox(const std::string& aFilePath);


private:


	ID3D11Device* myDevice;
	bool myWarnAboutTrash = false;

	void PrepareModel(Model* aModel, const std::string& aPath);

	void LoadLoop();

	void LoadModel_Internal(Model* aModel,const std::string& aFilePath);

	void LoadNode(const aiScene* aScene, const aiNode* aNode, aiMatrix4x4 aTransform, Model* aModel, std::unordered_map<std::string, std::string>& aInOutAttributes, const std::string& aFilePath);
	void LoadMesh(const aiScene* aScene, const aiNode* aNode, aiMatrix4x4 aTransform, const aiMesh* aMesh, Model* aModel, std::unordered_map<std::string, std::string>& aInOutAttributes, const std::string& aFilePath);
	void LoadAttributes(const aiNode* aNode, const aiMaterial* aMaterial, std::unordered_map<std::string, std::string>& aInOutAttributes, std::unordered_map<std::string,V3F>& aInOutColors);

	void QueueLoad(Model* aModel, std::string aFilePath);


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


	std::string myDefaultPixelShader;
	bool myIsRunning = true;

	std::thread myWorkHorse;
};

