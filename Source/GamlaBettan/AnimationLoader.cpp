#include "pch.h"
#include "AnimationLoader.h"

#include "assimp\cimport.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "MathFunc.h"

void Animation::GetInterpolation(float aTime, float aWeight, AnimationFrame& aOutSnapshot)
{
    size_t firstFrame = size_t(aTime) % myKeyFrames.size();
    size_t secondFrame = (size_t(aTime) + 1) % myKeyFrames.size();

    float fraction = fmodf(aTime, 1.f);
    
    float* raw1 = reinterpret_cast<float*>(myKeyFrames.data() + firstFrame);
    float* raw2 = reinterpret_cast<float*>(myKeyFrames.data() + secondFrame);
    float* rawOut = reinterpret_cast<float*>(&aOutSnapshot);

    for (size_t i = 0; i < sizeof(aOutSnapshot) / sizeof(float); i++)
    {
        rawOut[i] += LERP(raw1[i], raw2[i], fraction) * myWeights[i / (sizeof(M44f)/sizeof(float))] * aWeight;
    }
}

bool Animation::GetInterpolationUnlooped(float aTime, float aWeight, AnimationFrame& aOutSnapshot)
{
    if (aTime + 1 >= myKeyFrames.size())
    {
        for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
        {
            aOutSnapshot[i] += myKeyFrames.back()[i] * aWeight;
        }
        return true;
    }
    GetInterpolation(aTime, aWeight, aOutSnapshot);
    return false;
}

float Animation::GetTimeToLoop(float aTime)
{
    const float totalLength = static_cast<float>(myKeyFrames.size()) / myFPS;
    return totalLength - fmodf(aTime, totalLength);
}

namespace Loader
{
    struct BoneIfo
    {
        size_t myIndex = -1;
        aiMatrix4x4 myOffset;
    };

    std::map<std::string, BoneIfo> MapBones(const aiMesh* pMesh)
    {
        std::map<std::string, BoneIfo> out;
        size_t numBones = 0;

        for (size_t i = 0; i < pMesh->mNumBones; i++) 
        {
            if (out.count(pMesh->mBones[i]->mName.data) != 0)
            {
                SYSWARNING("animation has duplicate bone", pMesh->mBones[i]->mName.data);
            }

            out[pMesh->mBones[i]->mName.data] = 
            {
                i,
                pMesh->mBones[i]->mOffsetMatrix
            };
        }
        return out;
    }

    void ReadNodeHeirarchy(aiMatrix4x4 aRootTransform, const aiAnimation* aAnimation, float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel, std::map<std::string, BoneIfo>& aBoneMapping, AnimationFrame& aOutBuffer)
    {
        float time(AnimationTime);

        std::string NodeName(pNode->mName.data);

        aiMatrix4x4 NodeTransformation(pNode->mTransformation);

        const aiNodeAnim* pNodeAnim = FindNodeAnim(aAnimation, NodeName);

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

        if (aBoneMapping.find(NodeName) != aBoneMapping.end()) {
            uint BoneIndex = aBoneMapping[NodeName].myIndex;
            aOutBuffer[BoneIndex] = AiHelpers::ConvertToEngineMatrix44(aRootTransform * GlobalTransformation *
                aBoneMapping[NodeName].myOffset);
        }

        for (uint i = 0; i < pNode->mNumChildren; i++) {
            ReadNodeHeirarchy(aRootTransform, aAnimation, AnimationTime, pNode->mChildren[i], GlobalTransformation, stopAnimLevel, aBoneMapping, aOutBuffer);
        }
    }

    bool BakeAnimation(const aiScene* aScene, Animation* aAnimation, BakedAnimation& aKeyFrames)
    {
        aAnimation->myFPS = 24;
        for (size_t i = 0; i < sizeof(aAnimation->myWeights) / sizeof(*aAnimation->myWeights); i++)
        {
            aAnimation->myWeights[i] = 1.f;
        }

        if (aScene->mNumAnimations == 0)
        {
            SYSERROR("Can't bake animation", "scene contains no animations");
            return false;
        }

        if (aScene->mNumAnimations > 1)
        {
            SYSWARNING("Scene contains more than one animation only first will be loaded");
        }

        if (aScene->mNumMeshes == 0)
        {
            SYSERROR("Can't bake animation", "scene contains no meshes to extract bone info from");
            return false;
        }

        if (aScene->mNumMeshes > 1)
        {
            SYSWARNING("Scene contains more than one mesh only first will be considered");
        }

        const aiAnimation* anim = aScene->mAnimations[0];
        std::map<std::string, BoneIfo> boneMapping = MapBones(aScene->mMeshes[0]);

        aiMatrix4x4 Identity;
        InitIdentityM4(Identity);

        for (size_t i = 0; i <= ceil(anim->mDuration) + 1; i++)
        {
            AnimationFrame buffer;
            ReadNodeHeirarchy(aScene->mRootNode->mTransformation, anim, static_cast<float>(i), aScene->mRootNode, Identity, 2, boneMapping, buffer);
            aKeyFrames.push_back(buffer);
        }
    }
}



Asset* AnimationLoader::BakeAnimation(const std::string& aPath)
{
    if (!Tools::FileExists(aPath))
    {
        SYSERROR("Animation file missing", aPath);
        return nullptr;
    }
    const aiScene* scene = aiImportFile(aPath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded); 

    if (!scene)
    {
        SYSERROR("Failed to open animationfile", aPath);
        return nullptr;
    }

    Animation* animation = new Animation();
    if (!Loader::BakeAnimation(scene, animation, animation->myKeyFrames))
    {
        SYSERROR("Failed to bake animation", aPath);
        delete animation;
        aiReleaseImport(scene);
        return nullptr;
    }

    aiReleaseImport(scene);
    return new AnimationAsset(animation);
}
