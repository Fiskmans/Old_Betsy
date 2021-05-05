#include <string>
#include <map>
#include <vector>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/LogStream.hpp"
#include <fstream>

#include "MathFunc.h"

#include "AssetImportHelpers.h"
#include "AnimationData.h"

struct VertexBoneDataAnim
{        
    uint IDs[MAX_NUM_BONES_PER_VEREX];
    float Weights[MAX_NUM_BONES_PER_VEREX];

	VertexBoneDataAnim()
    {
        Reset();
    };
        
    void Reset()
    {
		memset(IDs, 0, sizeof(IDs));
		memset(Weights, 0, sizeof(Weights));
    }
        
    
	void AddBoneData(uint BoneID, float Weight)
	{
		for (uint i = 0 ; i < MAX_NUM_BONES_PER_VEREX; i++) {
			if (Weights[i] == 0.0) {
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			} 
		}

		// should never get here - more bones than we have space for
		assert(false);
	} 
}; 

struct BoneInfoAnim
{
	aiMatrix4x4 BoneOffset;
	aiMatrix4x4 FinalTransformation;
};

#define INVALID_MATERIAL 0xFFFFFFFF;

struct MeshEntry {
	MeshEntry()
	{
		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		MaterialIndex = INVALID_MATERIAL;
	}

	unsigned int NumIndices;
	unsigned int BaseVertex;
	unsigned int BaseIndex;
	unsigned int MaterialIndex;
};

typedef std::vector<std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>> BakedAnimation;

namespace AnimControllerHelpers
{
	struct MemoizedData
	{
		std::vector<Assimp::Importer*> _importers;
		std::vector<const aiScene*> _scenes;
		
		std::vector<std::variant<
			BakedAnimation,
			std::future<BakedAnimation>
		>> _bakedAnimations;
		
		aiMatrix4x4 m_GlobalInverseTransform;
		std::map<std::string, uint> m_BoneMapping;
		std::vector<MeshEntry> m_Entries;
		std::vector<BoneInfoAnim> m_BoneInfo;
		uint m_NumBones = 0;
		std::vector<VertexBoneDataAnim> m_Mass;
		uint _currentUsers = 0;
		std::vector<BoneInfo>* myBoneData = nullptr;
	};
}

class AnimationController
{
private:

	std::string m_ModelPath;

	aiVector3D _rotation;
	long long m_lastTime = 0;
	float _animationTime0 = 0;
	float _animationTime1 = 0;
	float _blendingTime = 0;
	float _blendingTimeMul = 0;
	int _prevAnimIndex = 0;
	bool _updateBoth = false;
	bool _temporary = false;
	float _playTime = 0;
	bool _looped = false;

	AnimControllerHelpers::MemoizedData* myData;
	static std::map<std::string, AnimControllerHelpers::MemoizedData*> ourStashedAnimations;
	int _curScene;
	
public:
	AnimationController(const char* modelpath);
	~AnimationController();


	enum class AnimationLayer
	{
		Primary,
		Secondary,
		Both
	};

	void Release();

	void ReadNodeHeirarchy(const aiScene* scene, float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel, std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& aOutBuffer) const;

	void ReadTransformStateAtTime(size_t aIndex, float aTime, std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& aBuffer);

	M44F ReadTransformStateOfBoneAtTime(size_t aIndex, size_t aBone, float aTime);

	void BoneTransform(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms);

	void LoadBones(uint MeshIndex, const aiMesh* pMesh);

	bool InitFromScene(const aiScene* pScene);

	bool Import3DFromFile(const std::string& pFile, std::vector<BoneInfo>* aBoneData, bool& ShouldLoadRest);

	void Update(float dt);

	bool Add3DAnimFromFile(const std::string& fileName);

	BakedAnimation BakeAnimation(int aAnimation) const;

	bool SetAnimIndex(uint index, bool updateBoth = true, float blendDuration = 0.3f, bool temporary = false, float time = 0.f);

	bool SetBlendTime(float aTime);

	uint GetMaxIndex();

	bool IsDoneBlending();

	bool JustLooped();

	float GetAnimTime();

	size_t GetTickCount();

	void SetTime(float aTime, AnimationLayer aAnimationLayer = AnimationLayer::Both);

	M44F TransformOfBone(int aBone);
private:
	bool myShouldLoop;

	M44F NLerpMatrix(const M44F& aLeft, const M44F& aRight, float aAmount,size_t aBone);

	std::array<M44F, NUMBEROFANIMATIONBONES> NLerp(const std::array<M44F, NUMBEROFANIMATIONBONES>& aLeft, const std::array<M44F, NUMBEROFANIMATIONBONES>& aRight, float aAmount);
	std::vector<std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>>& ExtractAnimation(int aAnimationIndex);
	static BakedAnimation ourTPose;
};
