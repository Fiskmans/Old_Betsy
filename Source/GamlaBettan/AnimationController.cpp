#include "pch.h"
#include "AnimationController.h"
#include <future>


std::map<std::string, AnimControllerHelpers::MemoizedData*> AnimationController::ourStashedAnimations;
std::vector<std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>> AnimationController::ourTPose;

AnimationController::~AnimationController()
{
	Release();
}

void AnimationController::Release()
{
	myData->_currentUsers--;
	if (myData->_currentUsers == 0)
	{
		// *** cleanup ***
		for (uint i = 0; i < myData->_importers.size(); ++i)
		{
			delete myData->_importers[i];
		}
		myData->_importers.clear();
		std::string key;
		for (auto& it : ourStashedAnimations)
		{
			if (it.second == myData)
			{
				key = it.first;
			}
		}
		if (!key.empty())
		{
			ourStashedAnimations.erase(key);
		}
		delete myData;
	}
}

AnimationController::AnimationController(const char* modelpath) :
	_curScene(0),
	m_ModelPath(modelpath),
	_prevAnimIndex(0),
	_blendingTime(0.f),
	_blendingTimeMul(1.f),
	_updateBoth(true),
	_temporary(false),
	_playTime(0.f),
	myShouldLoop(false),
	myData(nullptr)
{}


M44F AnimationController::NLerpMatrix(const M44F& aLeft, const M44F& aRight, float aAmount, size_t aBone)
{
	M44F out;
	for (size_t i = 0; i < 3; i++)
	{
		V4F l = aLeft.Column(i);
		V4F r = aRight.Column(i);

		V4F n = LERP(l, r, aAmount);
		out.AssignColumn(i, n.GetNormalized());
	}

	M44F workingBone = M44F::GetRealInverse((*myData->myBoneData)[aBone].BoneOffset);
	V4F start = V4F(0, 0, 0, 1) * M44F::Transpose(aLeft * workingBone);
	V4F end = V4F(0, 0, 0, 1) * M44F::Transpose(aRight * workingBone);
	out.AssignColumn(3, LERP(aLeft.Column(3), aRight.Column(3), aAmount));
	V4F target = LERP(start, end, aAmount);
	V4F actual = V4F(0, 0, 0, 1) * M44F::Transpose(out * workingBone);
	V4F corrected = actual - target;
	out.AssignColumn(3, LERP(aLeft.Column(3), aRight.Column(3), aAmount) - corrected);
	return out;
}

inline std::array<M44F, NUMBEROFANIMATIONBONES> AnimationController::NLerp(const std::array<M44F, NUMBEROFANIMATIONBONES>& aLeft, const std::array<M44F, NUMBEROFANIMATIONBONES>& aRight, float aAmount)
{
	std::array<M44F, NUMBEROFANIMATIONBONES> out;

	for (size_t matIndex = 0; matIndex < MIN(NUMBEROFANIMATIONBONES, (*myData->myBoneData).size()); matIndex++)
	{
		const M44F& left = aLeft[matIndex];
		const M44F& right = aRight[matIndex];

		for (size_t i = 0; i < 3; i++)
		{
			V4F l = left.Column(i);
			V4F r = right.Column(i);

			V4F n = LERP(l, r, aAmount);
			out[matIndex].AssignColumn(i, n.GetNormalized() * LERP(l.Length(), r.Length(), aAmount));
		}

		M44F workingBone = M44F::GetRealInverse((*myData->myBoneData)[matIndex].BoneOffset);
		V4F start = V4F(0, 0, 0, 1) * M44F::Transpose(aLeft[matIndex] * workingBone);
		V4F end = V4F(0, 0, 0, 1) * M44F::Transpose(aRight[matIndex] * workingBone);
		out[matIndex].AssignColumn(3, LERP(left.Column(3), right.Column(3), aAmount));
		V4F target = LERP(start, end, aAmount);
		V4F actual = V4F(0, 0, 0, 1) * M44F::Transpose(out[matIndex] * workingBone);
		V4F corrected = actual - target;
		out[matIndex].AssignColumn(3, LERP(left.Column(3), right.Column(3), aAmount) - corrected);
	}

	return out;
}

void AnimationController::ReadNodeHeirarchy(const aiScene* scene, float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel, std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& aOutBuffer) const
{
	float time(AnimationTime);

	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, time, pNodeAnim);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling, ScalingM);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, time, pNodeAnim);
		aiMatrix4x4 RotationM;
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		{
			float timeStop(stopAnimLevel <= 0 ? AnimationTime : 0.f);
			CalcInterpolatedPosition(Translation, timeStop, pNodeAnim);
		}
		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation, TranslationM);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	stopAnimLevel--;

	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (myData->m_BoneMapping.find(NodeName) != myData->m_BoneMapping.end()) {
		uint BoneIndex = myData->m_BoneMapping[NodeName];
		aOutBuffer[BoneIndex] = AiHelpers::ConvertToEngineMatrix44(myData->m_GlobalInverseTransform * GlobalTransformation *
			myData->m_BoneInfo[BoneIndex].BoneOffset);
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(scene, AnimationTime, pNode->mChildren[i], GlobalTransformation, stopAnimLevel, aOutBuffer);
	}
}

void AnimationController::ReadTransformStateAtTime(size_t aIndex, float aTime, std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& aBuffer)
{
	if (myData->_scenes.size() <= _curScene + 1ULL || !myData->_scenes[_curScene + 1ULL])
	{
		return;
	}
	size_t lookupprev = size_t(floor(aTime)) % ExtractAnimation(aIndex).size();
	size_t lookupnext = size_t(ceil(aTime)) % ExtractAnimation(aIndex).size();

	float blend = fmodf(aTime, 1.f);
	aBuffer = NLerp(ExtractAnimation(aIndex)[lookupprev], ExtractAnimation(aIndex)[lookupnext], blend);
}

M44F AnimationController::ReadTransformStateOfBoneAtTime(size_t aIndex, size_t aBone, float aTime)
{
	if (myData->_scenes.size() <= _curScene + 1ULL || !myData->_scenes[_curScene + 1ULL])
	{
		return M44F();
	}
	int lookupprev = size_t(floor(aTime)) % ExtractAnimation(aIndex).size();
	int lookupnext = size_t(ceil(aTime)) % ExtractAnimation(aIndex).size();

	float blend = fmodf(aTime, 1.f);
	return NLerpMatrix(ExtractAnimation(aIndex)[lookupprev][aBone], ExtractAnimation(aIndex)[lookupnext][aBone], blend, aBone);
}

void AnimationController::BoneTransform(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& Transforms)
{
	if (_blendingTime > 0.f)
	{
		//börja fixa här

		float AnimationTime0 = GetAnimTime();
		float AnimationTime1;
		if (_prevAnimIndex + 1ULL >= myData->_scenes.size())
		{
			_prevAnimIndex = 0;
		}
		if (_curScene + 1ULL >= myData->_scenes.size())
		{
			_curScene = 0;
		}
		if (!myData->_scenes[_prevAnimIndex + 1ULL])
		{
			AnimationTime1 = 0;
		}
		else
		{
			float TicksPerSecond = static_cast<float>(myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mTicksPerSecond) != 0 ?
				static_cast<float>(myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mTicksPerSecond) : 25.0f;
			float TimeInTicks = _animationTime1 * TicksPerSecond;
			AnimationTime1 = TimeInTicks;

			if (!_updateBoth)
			{
				AnimationTime1 = static_cast<float>(myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mDuration);
			}
		}

		std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES> prev;
		std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES> current;
		ReadTransformStateAtTime(_prevAnimIndex, AnimationTime1, prev);
		ReadTransformStateAtTime(_curScene, AnimationTime0, current);

		Transforms = NLerp(current, prev, _blendingTime);
	}
	else
	{
		ReadTransformStateAtTime(_curScene, GetAnimTime(), Transforms);
	}
}

void AnimationController::LoadBones(uint MeshIndex, const aiMesh* pMesh)
{
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (myData->m_BoneMapping.find(BoneName) == myData->m_BoneMapping.end()) {
			BoneIndex = myData->m_NumBones;
			myData->m_NumBones++;
			BoneInfoAnim bi;
			myData->m_BoneInfo.push_back(bi);
		}
		else {
			BoneIndex = myData->m_BoneMapping[BoneName];
		}

		myData->m_BoneMapping[BoneName] = BoneIndex;
		myData->m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = myData->m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			myData->m_Mass[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}

}

bool AnimationController::InitFromScene(const aiScene* pScene)
{
	m_lastTime = -1;
	_animationTime0 = 0.f;

	myData->m_Entries.resize(pScene->mNumMeshes);

	uint NumVertices = 0;
	uint NumIndices = 0;

	// Count the number of vertices and indices
	for (uint i = 0; i < myData->m_Entries.size(); i++) {
		myData->m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		myData->m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		myData->m_Entries[i].BaseVertex = NumVertices;
		myData->m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += myData->m_Entries[i].NumIndices;
	}

	myData->m_Mass.resize(NumVertices);

	for (uint i = 0; i < pScene->mNumMeshes; ++i)
	{
		LoadBones(i, pScene->mMeshes[i]);
	}
	return true;
}

bool AnimationController::Import3DFromFile(const std::string& pFile, std::vector<BoneInfo>* aBoneData, bool& ShouldLoadRest)
{
	if (pFile.length())
		m_ModelPath = pFile;

	if (ourStashedAnimations.count(pFile) > 0)
	{
		myData = ourStashedAnimations[pFile];
		ShouldLoadRest = false;
		myData->_currentUsers++;
		return true;
	}
	else
	{
		myData = new AnimControllerHelpers::MemoizedData;
		ourStashedAnimations[pFile] = myData;
		ShouldLoadRest = true;
		myData->_currentUsers++;

		// Check if file exists
		std::ifstream fin(m_ModelPath.c_str());
		if (!fin.fail())
		{
			fin.close();
		}
		else
		{
			MessageBoxA(NULL, ("Couldn't open file: " + m_ModelPath).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		_curScene = static_cast<int>(myData->_importers.size());
		myData->_importers.push_back(new Assimp::Importer);
		myData->_scenes.push_back(myData->_importers[_curScene]->ReadFile(m_ModelPath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));
		//_curScene = importer.ReadFile( m_ModelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals );

		bool ret = false;
		// If the import failed, report it
		if (myData->_scenes[_curScene]) {
			myData->m_GlobalInverseTransform = myData->_scenes[_curScene]->mRootNode->mTransformation;
			myData->m_GlobalInverseTransform.Inverse();
			ret = InitFromScene(myData->_scenes[_curScene]);
			// Now we can access the file's contents.
			logInfo("Import of _curScene " + m_ModelPath + " succeeded.");
		}
		else {
			logInfo(myData->_importers[_curScene]->GetErrorString());
		}
		myData->myBoneData = aBoneData;
		// We're done. Everything will be cleaned up by the importer destructor
		return ret;
	}
}

void AnimationController::Update(float dt)
{

	if (myData->_scenes.size() <= _curScene + 1ULL || !myData->_scenes[_curScene + 1ULL])
	{
		return;
	}
	if (_curScene + 1ULL > myData->_scenes.size())
	{
		_curScene = 0;
	}
	float TicksPerSecond = static_cast<float>(myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mTicksPerSecond) != 0 ?
		static_cast<float>(myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mTicksPerSecond) : 25.0f;

	float AnimationTimeBefore = fmodf(_animationTime0 * TicksPerSecond, static_cast<float>(myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mDuration));

	_animationTime0 += dt;

	float AnimationTimeAfter = fmodf(_animationTime0 * TicksPerSecond, static_cast<float>(myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mDuration));

	_looped = AnimationTimeBefore > AnimationTimeAfter;

	if (_blendingTime > 0.f)
	{
		_blendingTime -= dt * _blendingTimeMul;
		if (_blendingTime <= 0.f)
		{
			_animationTime0 = _animationTime1;
		}
		if (_updateBoth)
		{
			_animationTime1 += dt;
		}
	}
	else
	{
		_animationTime1 += dt;
	}

	if (_temporary)
	{
		_playTime -= dt;
		if (_playTime <= 0.f)
		{
			_temporary = false;
			SetAnimIndex(_prevAnimIndex);
		}
	}
}

bool AnimationController::Add3DAnimFromFile(const std::string& fileName)
{
	// Check if file exists
	std::ifstream fin(fileName.c_str());
	if (!fin.fail())
	{
		fin.close();
	}
	else
	{
		MessageBoxA(NULL, ("Couldn't open file: " + fileName).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	_curScene = static_cast<int>(myData->_importers.size());

	{
		PERFORMANCETAG("Reading");
		myData->_importers.push_back(new Assimp::Importer);
		const aiScene* scene = myData->_importers[_curScene]->ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded);
		myData->_scenes.push_back(scene);
		// If the import failed, report it
		if (!myData->_scenes[_curScene]) {
			SYSERROR(myData->_importers[_curScene]->GetErrorString(), fileName);
			return false;
		}
	}


	{
		PERFORMANCETAG("Baking");
#if STREAMANIMATIONS
		myData->_bakedAnimations.push_back(std::async(&AnimationController::BakeAnimation, this, _curScene));
#else
		myData->_bakedAnimations.push_back(BakeAnimation(_curScene));
#endif
	}

	return true;
}

BakedAnimation AnimationController::BakeAnimation(int aAnimation) const
{
	std::vector<std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>> aTarget;
	if (!myData->_scenes[aAnimation])
	{
		SYSERROR("Since no animation could be loaded defaulting to T-Pose", "");
		aTarget.resize(24);
	}
	else
	{
		const aiAnimation* anim = myData->_scenes[aAnimation]->mAnimations[0];

		aiMatrix4x4 Identity;
		InitIdentityM4(Identity);

		for (size_t i = 0; i <= ceil(anim->mDuration) + 1; i++)
		{
			std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES> buffer;
			ReadNodeHeirarchy(myData->_scenes[aAnimation], CAST(float, i), myData->_scenes[aAnimation]->mRootNode, Identity, 2, buffer);
			aTarget.push_back(buffer);
		}
	}
	return aTarget;
}

bool AnimationController::SetAnimIndex(uint index, bool updateBoth, float blendDuration, bool temporary, float time)
{
	//_looped = false;

	if (index == static_cast<uint>(_curScene) || index >= static_cast<uint>(myData->_bakedAnimations.size()))
	{
		_blendingTime = 0.f;
		return false;
	}
	_prevAnimIndex = _curScene;
	_curScene = index;
	_blendingTime = 1.f;
	//DANGER TO STOP IDLE 5 second blend
	if (_prevAnimIndex == 0)
	{
		_blendingTimeMul = 1.0f / 0.5f;
	}
	else
	{
		_blendingTimeMul = 1.f / blendDuration;
	}
	_animationTime1 = 0.f;
	_updateBoth = updateBoth;
	_temporary = temporary;
	_playTime = time;
	return true;
}

bool AnimationController::SetBlendTime(float aTime)
{
	_blendingTime = aTime;
	return true;
}

uint AnimationController::GetMaxIndex()
{
	return static_cast<uint>(myData->_bakedAnimations.size());
}

bool AnimationController::IsDoneBlending()
{
	return _blendingTime <= 0.0f;
}

bool AnimationController::JustLooped()
{
	return _looped;
}

float AnimationController::GetAnimTime()
{
	if (myData->_scenes.size() <= _curScene + 1ULL || !myData->_scenes[_curScene + 1ULL])
	{
		return 0.0f;
	}
	float TicksPerSecond = static_cast<float>(myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mTicksPerSecond) != 0 ?
		static_cast<float>(myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mTicksPerSecond) : 25.0f;
	float TimeInTicks = _animationTime0 * TicksPerSecond;
	return TimeInTicks;
}

size_t AnimationController::GetTickCount()
{
	if (myData->_bakedAnimations.size() <= _curScene)
	{
		return 1;
	}
	return ExtractAnimation(_curScene).size();
}

void AnimationController::SetTime(float aTime, AnimationLayer aAnimationLayer)
{
	if (_curScene + 1ULL >= myData->_scenes.size())
	{
		return;
	}
	if (_prevAnimIndex + 1ULL >= myData->_scenes.size())
	{
		return;
	}

	if (aAnimationLayer == AnimationLayer::Both)
	{
		float tps0 = CAST(float, myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mTicksPerSecond);
		_animationTime0 = aTime / tps0;
		float tps1 = CAST(float, myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mTicksPerSecond);
		_animationTime1 = aTime / tps1;
	}
	else if (aAnimationLayer == AnimationLayer::Primary)
	{
		float tps0 = CAST(float, myData->_scenes[_curScene + 1ULL]->mAnimations[0]->mTicksPerSecond);
		_animationTime0 = aTime / tps0;
	}
	else if (aAnimationLayer == AnimationLayer::Secondary)
	{
		float tps1 = CAST(float, myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mTicksPerSecond);
		_animationTime1 = aTime / tps1;
	}
}

M44F AnimationController::TransformOfBone(int aBone)
{

	if (_blendingTime > 0.f)
	{
		//börja fixa här

		float AnimationTime0 = GetAnimTime();
		float AnimationTime1;
		if (_prevAnimIndex + 1ULL >= myData->_scenes.size())
		{
			_prevAnimIndex = 0;
		}
		if (_curScene + 1ULL >= myData->_scenes.size())
		{
			_curScene = 0;
		}
		if (!myData->_scenes[_prevAnimIndex + 1ULL])
		{
			AnimationTime1 = 0;
		}
		else
		{
			float TicksPerSecond = static_cast<float>(myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mTicksPerSecond) != 0 ?
				static_cast<float>(myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mTicksPerSecond) : 25.0f;
			float TimeInTicks = _animationTime1 * TicksPerSecond;
			AnimationTime1 = TimeInTicks;

			if (!_updateBoth)
			{
				AnimationTime1 = static_cast<float>(myData->_scenes[_prevAnimIndex + 1ULL]->mAnimations[0]->mDuration);
			}
		}

		M44F prev = ReadTransformStateOfBoneAtTime(_prevAnimIndex, aBone, AnimationTime1);
		M44F current = ReadTransformStateOfBoneAtTime(_curScene, aBone, AnimationTime0);
		return NLerpMatrix(current, prev, _blendingTime, aBone);
	}
	else
	{
		return ReadTransformStateOfBoneAtTime(_curScene, aBone, GetAnimTime());
	}
}

std::vector<std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>>& AnimationController::ExtractAnimation(int aAnimationIndex)
{
	if (myData->_bakedAnimations[aAnimationIndex].index() == 0)
	{
		return std::get<0>(myData->_bakedAnimations[aAnimationIndex]);
	}
	else
	{
		std::future<BakedAnimation>& promise = std::get<std::future<BakedAnimation>>(myData->_bakedAnimations[aAnimationIndex]);
		if (promise.wait_for(std::chrono::seconds(0)) == std::future_status::ready) // check if ready
		{
			BakedAnimation buffer = promise.get();
			myData->_bakedAnimations[aAnimationIndex] = buffer;
		}
	}
	if (ourTPose.empty())
	{
		ourTPose.resize(1);
	}
	return ourTPose;
}
