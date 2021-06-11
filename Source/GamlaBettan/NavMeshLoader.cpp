#include "pch.h"
#include "NavMeshLoader.h"

#pragma warning(push)
#pragma warning(disable: 26812)
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#pragma warning(pop)

namespace NavMeshLoader_Private
{
	void OBJLoader(NavMesh* aNavMesh, const std::string& aFilePath)
	{
		aNavMesh->myNodes.clear();
		aNavMesh->myVertexCollection.clear();

		std::ifstream inFile;
		inFile.open(aFilePath);
		std::string buffer;
		while (std::getline(inFile, buffer))
		{
			std::stringstream ss(buffer);
			std::string type;
			ss >> type;
			if (type == "v")
			{
				float x, y, z;
				if (ss >> x >> y >> z)
				{
					aNavMesh->myVertexCollection.emplace_back(x, y, z);
				}
			}
			else if (type == "f")
			{
				NavMeshNode nextNode;
#if TRACKLOADINGERROS
				for (size_t i = 0; i < 3; i++)
				{
					nextNode.myCorners[i] = 0ULL;
				}
#endif
				for (size_t i = 0; i < 3; i++)
				{
					std::string indexlist;
					if (ss >> indexlist)
					{
						std::stringstream indexStream(indexlist);
						indexStream >> nextNode.myCorners[i];
					}
				}
#if TRACKLOADINGERROS
				for (size_t i = 0; i < 3; i++)
				{
					if (nextNode.myCorners[i] == 0)
					{
						std::cout << "Error reading face" << std::endl;
					}
				}
#endif
				for (size_t i = 0; i < 3; i++)
				{
					nextNode.myCorners[i]--; // 0 indexed
				}
				aNavMesh->myNodes.push_back(nextNode);
			}
		}
	}

	void FBXLoader(NavMesh* aNavMesh, const std::string& aFilePath)
	{
		PERFORMANCETAG("FBX nav loader");
		SYSINFO("Generating nav mesh from: " + aFilePath);
		{
			PERFORMANCETAG("Exist check");
			if (!Tools::FileExists(aFilePath))
			{
				SYSERROR("Navmeshfile could not be found", aFilePath);
			}
		}
		const aiScene* scene;
		{
			PERFORMANCETAG("Importing");
			scene = aiImportFile(aFilePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate);
		}

		if (!scene)
		{
			SYSERROR("Could not load navmesh from", aFilePath);
			SYSERROR(aiGetErrorString(), aFilePath);
			return;
		}
		if (scene->mNumMeshes == 0)
		{
			SYSERROR("Nav mesh contains no meshes", aFilePath);
			aiReleaseImport(scene);
			return;
		}
		if (scene->mNumMeshes > 1)
		{
			SYSERROR("Nav mesh contains more than one mesh", aFilePath);
		}

		std::vector<std::set<size_t>> nodeIndexMapping;
		{
			PERFORMANCETAG("resize and reserve");
			nodeIndexMapping.resize(scene->mMeshes[0]->mNumVertices);
			aNavMesh->myNodes.clear();
			aNavMesh->myNodes.reserve(scene->mMeshes[0]->mNumFaces);
			aNavMesh->myVertexCollection.reserve(scene->mMeshes[0]->mNumVertices);
		}
		std::unordered_map<size_t, size_t> indexMapping;

		const auto ConvertToEngine = [](const aiVector3D& aVec) -> V3F
		{
			return V3F(aVec.x, aVec.y, aVec.z);
		};

		{
			PERFORMANCETAG("duplication checks");
			for (size_t indexIndex = 0; indexIndex < scene->mMeshes[0]->mNumVertices; indexIndex++)
			{
				V3F pos = ConvertToEngine(scene->mMeshes[0]->mVertices[indexIndex]);
				auto it = std::find(aNavMesh->myVertexCollection.begin(), aNavMesh->myVertexCollection.end(), pos);
				indexMapping[indexIndex] = it - aNavMesh->myVertexCollection.begin();
				if (it == aNavMesh->myVertexCollection.end())
				{
					aNavMesh->myVertexCollection.push_back(pos);
				}
			}
		}

		{
			PERFORMANCETAG("Face creation");
			for (size_t faceIndex = 0; faceIndex < scene->mMeshes[0]->mNumFaces; faceIndex++)
			{
				NavMeshNode n;
				for (size_t i = 0; i < 3; i++)
				{
					n.myCorners[i] = indexMapping[scene->mMeshes[0]->mFaces[faceIndex].mIndices[i]];
				}
				aNavMesh->myNodes.push_back(n);
			}
		}


		{
			PERFORMANCETAG("Releasing");
			aiReleaseImport(scene);
		}
	}

	void FindNodeCentersAndPlanes(NavMesh* aNavMesh)
	{
		PERFORMANCETAG("Node centers and planes");
		for (auto& i : aNavMesh->myNodes)
		{
			i.myCenter = V3F(0, 0, 0);
			i.myCenter += aNavMesh->myVertexCollection[i.myCorners[0]];
			i.myCenter += aNavMesh->myVertexCollection[i.myCorners[1]];
			i.myCenter += aNavMesh->myVertexCollection[i.myCorners[2]];
			i.myCenter = i.myCenter / 3.f;
			i.myPlane = CommonUtilities::Plane<float>(aNavMesh->myVertexCollection[i.myCorners[0]], aNavMesh->myVertexCollection[i.myCorners[1]], aNavMesh->myVertexCollection[i.myCorners[2]]);
		}
	}

	void Verify(NavMesh* aNavMesh)
	{
		PERFORMANCETAG("Verify");
		for (auto& node : aNavMesh->myNodes)
		{
			if (abs(node.myPlane.Normal().Length() - 1.0f) > 0.01f)
			{
				SYSWARNING("Found dodgy nav node around " + node.myCenter.ToString(), "");
			}
		}
	}

	void LinkNodes(NavMesh* aNavMesh)
	{
		PERFORMANCETAG("Node linking");
		std::vector<std::set<NavMeshNode*>> nodeMapping;
		std::vector<NavMeshNode*> intersectionVector;
		intersectionVector.resize(2);

		const auto FindIntersection = [&](size_t aFirst, size_t aSecond, NavMeshNode* aToIgnore) -> NavMeshNode*
		{
			auto end = std::set_intersection(nodeMapping[aFirst].begin(), nodeMapping[aFirst].end(), nodeMapping[aSecond].begin(), nodeMapping[aSecond].end(), intersectionVector.begin());
			auto it = intersectionVector.begin();
			while (it != end)
			{
				if (*it != aToIgnore)
				{
					return *it;
				}
				it++;
			}
			return nullptr;
		};


		nodeMapping.resize(aNavMesh->myVertexCollection.size());
		for (auto& i : aNavMesh->myNodes)
		{
			nodeMapping[i.myCorners[0]].emplace(&i);
			nodeMapping[i.myCorners[1]].emplace(&i);
			nodeMapping[i.myCorners[2]].emplace(&i);
		}
		for (auto& node : aNavMesh->myNodes)
		{
			NavMeshNode* neighbors[3];
			neighbors[0] = FindIntersection(node.myCorners[0], node.myCorners[1], &node);
			neighbors[1] = FindIntersection(node.myCorners[1], node.myCorners[2], &node);
			neighbors[2] = FindIntersection(node.myCorners[2], node.myCorners[0], &node);
			for (size_t i = 0; i < 3; i++)
			{
				if (neighbors[i])
				{
					NavMeshLink l;
					l.toNode = neighbors[i] - aNavMesh->myNodes.data();
					l.weight = (node.myCenter - neighbors[i]->myCenter).Length();
					node.myLinks[i] = l;
				}
				else
				{
					NavmeshWall wall;
					wall.start = aNavMesh->myVertexCollection[node.myCorners[(i + 1) % 3]];
					wall.end = aNavMesh->myVertexCollection[node.myCorners[i]];
					wall.top = wall.start + V3F(0, 350, 0);
					wall.plane.InitWith3Points(wall.start, wall.end, wall.top);
					aNavMesh->myWalls.push_back(wall);
				}
			}
		}
	}

    NavMesh* LoadMesh(const std::string& aPath)
    {
		NavMesh* mesh = new NavMesh();

		PERFORMANCETAG("Generating navmesh");
		static std::unordered_map <std::string, std::function<void(NavMesh*, const std::string&)>> fileTypeMapping;
		if (fileTypeMapping.empty())
		{
			fileTypeMapping[".fbx"] = std::bind(&FBXLoader, std::placeholders::_1, std::placeholders::_2);
			fileTypeMapping[".obj"] = std::bind(&OBJLoader, std::placeholders::_1, std::placeholders::_2);
		}
		std::string extension = std::filesystem::path(aPath).extension().string();
		if (fileTypeMapping.count(extension) != 0)
		{
			PERFORMANCETAG("Loading");
			fileTypeMapping[extension](mesh, aPath);
		}
		else
		{
			SYSERROR("Unrecogniesed NavMesh File Format", aPath);
		}
		FindNodeCentersAndPlanes(mesh);
		Verify(mesh);
		LinkNodes(mesh);
		return mesh;
    }

}


Asset* NavMeshLoader::LoadNavMesh(const std::string& aPath)
{
    return new NavMeshAsset(NavMeshLoader_Private::LoadMesh(aPath));
}
