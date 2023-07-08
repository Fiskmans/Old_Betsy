#include "engine/assets/ShaderTypes.h"
#include "engine/assets/ShaderFlags.h"

#include "engine/graphics/ShaderBuffers.h"
#include "engine/graphics/ShaderMappings.h"

#include "common/Macros.h"

#include <vector>

#include <d3d11.h>
#include <assimp/mesh.h>

namespace engine
{
	ShaderTypes::Offsets ShaderTypes::OffsetsFromFlags(size_t aFlags)
	{
		Offsets out;
		size_t at = 0;

		out.position = at;
		at += sizeof(float) * 4;

		out.normal = at;
		at += sizeof(float) * 4;

		out.tangent = at;
		at += sizeof(float) * 4;

		out.bitanget = at;
		at += sizeof(float) * 4;

		if (aFlags & ShaderFlags::HasVertexColors)
		{
			out.vertexcolor = at;
			at += sizeof(float) * 4;
		}

		at += sizeof(float) * 2;

		if (aFlags & ShaderFlags::HasUvSets)
		{
			out.uv = at;
			at += sizeof(float) * UvSetsCountFromFlags(aFlags) * 2;
		}

		if (aFlags & ShaderFlags::HasBones)
		{
			out.bones = at;
			at += sizeof(UINT) * BonePerVertexCountFromFlags(aFlags);

			out.boneweights = at;
			at += sizeof(float) * BonePerVertexCountFromFlags(aFlags);
		}

		out.size = at;
		return out;
	}

	
	ShaderFlags ShaderTypes::FlagsFromMesh(const aiMesh* aMesh)
	{
		std::underlying_type_t<ShaderFlags> result = 0;

		if (aMesh->HasVertexColors(0))
		{
			result |= ShaderFlags::HasVertexColors;
		}
		if (aMesh->HasTextureCoords(0))
		{
			result |= ShaderFlags::HasUvSets;
			unsigned int count = 0;
			while (aMesh->HasTextureCoords(++count)){ }
			result |= (ShaderFlags::UvMask & ((count-1) << ShaderFlags::NumUvSetsOffset));
		}
		if (aMesh->HasBones())
		{
			result |= ShaderFlags::HasBones;
			size_t numberofBones = 0;
			std::vector<short> data;
			data.resize(aMesh->mNumVertices);
			memset(data.data(), 0U, data.size());
			for (size_t i = 0; i < aMesh->mNumBones; i++)
			{
				for (size_t b = 0; b < aMesh->mBones[i]->mNumWeights; b++)
				{
					data[aMesh->mBones[i]->mWeights[b].mVertexId]++;
				}
			}
			for (auto& i : data)
			{
				numberofBones = MAX(numberofBones, i);
			}
			numberofBones--;
			result |= (ShaderFlags::BoneMask & (numberofBones << ShaderFlags::NumBonesOffset));
		}

		return static_cast<ShaderFlags>(result);
	}
	

	std::string ShaderTypes::PostfixFromFlags(size_t aFlags)
	{
		std::string out;

		if (aFlags & ShaderFlags::HasUvSets)
		{
			out += "_uv" + std::to_string(UvSetsCountFromFlags(aFlags));
		}
		if (aFlags & ShaderFlags::HasVertexColors)
		{
			out += "_vc";
		}
		if (aFlags & ShaderFlags::HasBones)
		{
			out += "_bo" + std::to_string(BonePerVertexCountFromFlags(aFlags));
		}
		return out;
	}

	UINT ShaderTypes::InputLayoutFromFlags(D3D11_INPUT_ELEMENT_DESC* aOutDesc, size_t aFlags)
	{
		UINT count = 0;

		aOutDesc[count++] = { "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		aOutDesc[count++] = { "NORMAL",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		aOutDesc[count++] = { "TANGENT",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		aOutDesc[count++] = { "BITANGENT",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		if (aFlags & ShaderFlags::HasVertexColors)
		{
			aOutDesc[count++] = { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}
		if (aFlags & ShaderFlags::HasUvSets)
		{
			size_t numUvSets = UvSetsCountFromFlags(aFlags);
			for (UINT i = 0; i < numUvSets; i++)
			{
				aOutDesc[count++] = { "UV",			i, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		}
		if (aFlags & ShaderFlags::HasBones)
		{
			size_t numBones = BonePerVertexCountFromFlags(aFlags);
			for (UINT i = 0; i < numBones; i++)
			{
				aOutDesc[count++] = { "BONES",		i,DXGI_FORMAT_R32_UINT,					0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
			for (UINT i = 0; i < numBones; i++)
			{
				aOutDesc[count++] = { "BONEWEIGHTS",	i,DXGI_FORMAT_R32_FLOAT,				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}
		}
		return count;
	}

	size_t ShaderTypes::BonePerVertexCountFromFlags(size_t aFlags)
	{
		return ((aFlags & ShaderFlags::BoneMask) >> ShaderFlags::NumBonesOffset) + 1;
	}

	size_t ShaderTypes::UvSetsCountFromFlags(size_t aFlags)
	{
		return ((aFlags & ShaderFlags::UvMask) >> ShaderFlags::NumUvSetsOffset) + 1;
	}

	ShaderDefines::ShaderDefines(size_t aFlags)
	{
		AddMacro("NUMBEROFPOINTLIGHTS", std::to_string(NUMBEROFPOINTLIGHTS) );
		AddMacro("NUMBEROFANIMATIONBONES", std::to_string(NUMBEROFANIMATIONBONES) );
		AddMacro("NUMBEROFANIMATIONBONES", std::to_string(NUMBEROFANIMATIONBONES) );

		AddMacro("VERTEXCOLOR", ((aFlags & ShaderFlags::HasVertexColors) != 0) ? "true" : "false");

		AddMacro("HAS_UV_SETS", ((aFlags & ShaderFlags::HasUvSets) != 0) ? "true" : "false");
		AddMacro("UV_SETS_COUNT", std::to_string(ShaderTypes::UvSetsCountFromFlags(aFlags)));

		AddMacro("HAS_BONES", ((aFlags & ShaderFlags::HasBones) != 0) ? "true" : "false");
		AddMacro("BONESPERVERTEX", std::to_string(ShaderTypes::BonePerVertexCountFromFlags(aFlags)));

		AddMacro("FRAME_BUFFER", "b" + std::to_string(graphics::shader_mappings::BUFFER_FRAME));
		AddMacro("OBJECT_BUFFER", "b" + std::to_string(graphics::shader_mappings::BUFFER_OBJECT));
		AddMacro("POINT_LIGHT_BUFFER", "b" + std::to_string(graphics::shader_mappings::BUFFER_POINT_LIGHT));
		AddMacro("ANIMATION_BUFFER", "b" + std::to_string(graphics::shader_mappings::BUFFER_ANIMATION));
		AddMacro("DEFERRED_FRAME_BUFFER", "b" + std::to_string(graphics::shader_mappings::BUFFER_DEFFERED_FRAME));
	}

	_D3D_SHADER_MACRO* ShaderDefines::Get()
	{
		myMacros.clear();

		for (size_t i = 0; i + 1 < myDataStorage.size(); i += 2)
		{
			_D3D_SHADER_MACRO macro;
			macro.Name = myDataStorage[i].c_str();
			macro.Definition = myDataStorage[i + 1].c_str();
			myMacros.push_back(macro);
		}

		_D3D_SHADER_MACRO nullTerminand;
		nullTerminand.Name = nullptr;
		nullTerminand.Definition = nullptr;
		myMacros.push_back(nullTerminand);

		return myMacros.data();
	}

	void ShaderDefines::AddMacro(std::string aName, std::string aValue)
	{
		myDataStorage.push_back(aName);
		myDataStorage.push_back(aValue);
	}
}