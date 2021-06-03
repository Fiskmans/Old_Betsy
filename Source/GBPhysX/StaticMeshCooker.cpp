#include "pch.h"
#pragma warning(push, 1)
#pragma warning(disable : 26495)
#include "StaticMeshCooker.h"
#include "GBPhysX\Include\PxPhysicsAPI.h"
#include "GBPhysXLibraryDependencies.h"
#include "GBPhysX\Include\PxFoundation.h"
#pragma warning(pop)
#include "AssetImportHelpers.h"

#define PHYSICS_MESH_FOLDER "Assets/Physics"
#define FILEDATA_JSON "Assets/Physics/FileData.json"
#define SERIALIZATION_PATH "Assets/Physics/Serialized.fysik"


std::mutex tmemtx;


using namespace physx;

StaticMeshCooker::StaticMeshCooker() :
	myCooker(nullptr),
	myUniqueMeshID(0)
{

}

StaticMeshCooker::~StaticMeshCooker()
{
	UnSubscribeToMessage(MessageType::SerializePhysXObjects);
}

bool StaticMeshCooker::Init(PxFoundation* aFoundation, PxPhysics* aPhysics)
{
	myPhysicsPtr = aPhysics;
	myCooker = PxCreateCooking(PX_PHYSICS_VERSION, *aFoundation, PxCookingParams(aPhysics->getTolerancesScale()));
	if (!myCooker)
	{
		SYSERROR("Create cooker failed in Static Mesh Cooker.", "Create cooker failed.");
		return false;
	}

	myTriangleMeshCollection = PxCreateCollection();

	SubscribeToMessage(MessageType::SerializePhysXObjects);

	std::string path = "Data/Metrics/SerializedObjects.xml";
	DeSerializeObjects(myPhysicsPtr, path);
	return true;
}

physx::PxTriangleMesh* StaticMeshCooker::GetTriangleMeshFromPath(const std::string& aPath)
{
	if (myTriangleMeshCollection)
	{
		if (TriangleMeshExists(aPath))
		{
			std::lock_guard lockGuard(tmemtx);
			int index = myTriangleMeshIndexes[aPath];
			PxBase* object = myTriangleMeshCollection->find(myTriangleMeshIndexes[aPath]);
			
			if (object)
			{
				return object->is<PxTriangleMesh>();
			}
		}
	}
	return nullptr;
}

bool StaticMeshCooker::TriangleMeshExists(const std::string& aPath) const
{
	std::lock_guard lockGuard(tmemtx);
	if (myTriangleMeshIndexes.count(aPath) > 0)
	{
		return true;
	}
	return false;
}

bool StaticMeshCooker::LoadCollisionMesh(physx::PxPhysics* aPhysics, std::string& aPath, const aiScene* aAiScene, aiNode* aAiRootNode)
{
	std::stack<aiNode*> stack;

	unsigned int numOfVertices = 0;
	unsigned int numOfFaces = 0;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	stack.push(aAiRootNode);
	while (!stack.empty())
	{
		aiNode* current;
		current = stack.top();
		stack.pop();

		for (int childIndex = 0; childIndex < current->mNumChildren; childIndex++)
		{
			stack.push(current->mChildren[childIndex]);
		}

		for (int i = 0; i < int(current->mNumMeshes); ++i)
		{
			numOfVertices += aAiScene->mMeshes[current->mMeshes[i]]->mNumVertices;
			numOfFaces += aAiScene->mMeshes[current->mMeshes[i]]->mNumFaces;
		}

		for (int i = 0; i < int(current->mNumMeshes); ++i)
		{
			for (int j = 0; j < int(aAiScene->mMeshes[current->mMeshes[i]]->mNumVertices); ++j)
			{
				vertices.push_back(aAiScene->mMeshes[current->mMeshes[i]]->mVertices[j].x);
				vertices.push_back(aAiScene->mMeshes[current->mMeshes[i]]->mVertices[j].y);
				vertices.push_back(aAiScene->mMeshes[current->mMeshes[i]]->mVertices[j].z);
			}
		}

		for (int i = 0; i < int(current->mNumMeshes); ++i)
		{
			for (int j = 0; j < int(aAiScene->mMeshes[current->mMeshes[i]]->mNumFaces); ++j)
			{
				indices.push_back(aAiScene->mMeshes[current->mMeshes[i]]->mFaces[j].mIndices[0]);
				indices.push_back(aAiScene->mMeshes[current->mMeshes[i]]->mFaces[j].mIndices[1]);
				indices.push_back(aAiScene->mMeshes[current->mMeshes[i]]->mFaces[j].mIndices[2]);
			}
		}

	}

	return CookTriangleMesh(aPhysics, &vertices[0], numOfVertices, &indices[0], numOfFaces, myUniqueMeshID, aPath);
}

void StaticMeshCooker::CleanUp()
{
	myCooker = nullptr;
	myPhysicsPtr = nullptr;
	myTriangleMeshCollection->release();
	std::lock_guard lockGuard(tmemtx);
	myTriangleMeshIndexes.clear();
}

bool StaticMeshCooker::CookTriangleMesh(PxPhysics* aPhysics, float* aVertices, int aNumberOfVertices, unsigned int* aIndices, int aNumberOfFaces, unsigned long long newID, const std::string& aPath)
{

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = aNumberOfVertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = aVertices;

	meshDesc.triangles.count = aNumberOfFaces;
	meshDesc.triangles.stride = 3 * sizeof(UINT32);
	meshDesc.triangles.data = aIndices;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	myCooker->cookTriangleMesh(meshDesc, writeBuffer, &result);

	switch (result)
	{
	case PxTriangleMeshCookingResult::eSUCCESS:
	{
		SYSINFO("Cooking success for: " + aPath, aPath);
	}
	break;
	case PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
	{
		SYSINFO("Triangles to large for cooking for: " + aPath, aPath);
	}
	break;
	case PxTriangleMeshCookingResult::eFAILURE:
	{
		SYSINFO("Cooking failure for: " + aPath, aPath);
	}
	break;

	default:
		break;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = aPhysics->createTriangleMesh(readBuffer);
	
	std::lock_guard lockGuard(tmemtx);
	myTriangleMeshIndexes[aPath] = ++myUniqueMeshID;
	myTriangleMeshCollection->add(*triangleMesh, myUniqueMeshID);
	return true;
}

void StaticMeshCooker::SerializeObjects(physx::PxPhysics* aPhysics, const std::string& aPath)
{
	PxSerializationRegistry* registry = physx::PxSerialization::createSerializationRegistry(*aPhysics);

	if (myTriangleMeshCollection->getNbObjects() > 0)
	{
		PxDefaultFileOutputStream stream(aPath.c_str());
		if (!PxSerialization::serializeCollectionToXml(stream, *myTriangleMeshCollection, *registry))
		{
			SYSERROR("Failed To Serialize collection", "Failed To Serialize collection");
		}

		std::lock_guard lockGuard(tmemtx);
		std::string path = "Data/Metrics/SerializedObjectNames.names";
		std::ofstream bob(path);
		for (auto& val : myTriangleMeshIndexes)
		{
			bob << val.first << '\n';
			bob << val.second << '\n';
		}
		
	}
	registry->release();
}

bool StaticMeshCooker::DeSerializeObjects(physx::PxPhysics* aPhysics, const std::string& aPath)
{
	PxSerializationRegistry* registry = physx::PxSerialization::createSerializationRegistry(*aPhysics);

	std::ifstream file(aPath);
	if (file)
	{
		PxDefaultFileInputData meshData(aPath.c_str());
		PxU32 length = meshData.getLength();

		if (length != 0)
		{
			myTriangleMeshCollection = PxSerialization::createCollectionFromXml(meshData, *myCooker, *registry);
			int numberOfLoadedMeshes = myTriangleMeshCollection->getNbObjects();
			
			std::ifstream instream("Data/Metrics/SerializedObjectNames.names");
			
			std::string path;
			std::string id;

			std::lock_guard lockGuard(tmemtx);
			while (std::getline(instream, path))
			{
				std::getline(instream, id);
				int index = std::stoi(id);
				myTriangleMeshIndexes[path] = index;
			}
			myUniqueMeshID = numberOfLoadedMeshes;
			
			registry->release();
			return true;
		}

	}
	registry->release();
	return false;
}

void StaticMeshCooker::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SerializePhysXObjects)
	{
		std::string path = "Data/Metrics/SerializedObjects.xml";
		SerializeObjects(myPhysicsPtr, path);
	}
}
