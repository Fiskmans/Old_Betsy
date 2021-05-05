#pragma once

//#include "AssImp/cimport.h"
//#include "AssImp/scene.h"
//#include "AssImp/postprocess.h"

namespace physx
{
	class PxCooking;
	class PxCollection;
	class PxFoundation;
	class PxPhysics;
	class PxTriangleMesh;
}


struct aiScene;
struct aiNode;

class StaticMeshCooker : public Observer
{
public:
	StaticMeshCooker();
	~StaticMeshCooker();
	bool Init(physx::PxFoundation* aFoundation, physx::PxPhysics* aPhysics);
	physx::PxTriangleMesh* GetTriangleMeshFromPath(const std::string& aPath);
	bool TriangleMeshExists(const std::string& aPath) const;
	bool LoadCollisionMesh(physx::PxPhysics* aPhysics, std::string& aPath, const aiScene* aAiScene, aiNode* aAiRootNode);
	void CleanUp();

	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
private:
	struct FileData
	{
		std::string path;
		unsigned long long timestamp;
	};
	bool CookTriangleMesh(physx::PxPhysics* aPhysics, float* aVertices, int aNumberOfVertices, unsigned int* aIndices, int aNumberOfFaces, unsigned long long newID, const std::string& aPath);
	void SerializeObjects(physx::PxPhysics* aPhysics, const std::string& aPath);
	bool DeSerializeObjects(physx::PxPhysics* aPhysics, const std::string& aPath);
	
	physx::PxCooking* myCooker;
	physx::PxPhysics* myPhysicsPtr;
	//path, id
	std::unordered_map<std::string, int> myTriangleMeshIndexes;
	physx::PxCollection* myTriangleMeshCollection;
	std::vector<FileData> myFileData;
	int myUniqueMeshID;
};

