#include "engine/assets/ShaderTypes.h"
#include "engine/assets/ShaderFlags.h"

#include "common/Macros.h"


//#include "AssImp/mesh.h"

#include <d3d11.h>

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

	/*
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
	*/

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

	void ShaderTypes::DefinesFromFlags(_D3D_SHADER_MACRO* aBuffer, size_t aFlags)
	{
		static const char* numberLookup[] =
		{
			"0", "1",  "2",  "3",  "4",  "5",  "6",  "7",
			"8", "9", "10", "11", "12", "13", "14", "15"
		};

		size_t at = 0;

		aBuffer[at++] = _D3D_SHADER_MACRO{ STRING(NUMBEROFPOINTLIGHTS), STRINGVALUE(NUMBEROFPOINTLIGHTS) };
		aBuffer[at++] = _D3D_SHADER_MACRO{ STRING(NUMBEROFANIMATIONBONES), STRINGVALUE(NUMBEROFANIMATIONBONES) };
		aBuffer[at++] = _D3D_SHADER_MACRO{ STRING(MODELSAMOUNTOFCommonUtilitiesSTOMDATA), STRINGVALUE(MODELSAMOUNTOFCommonUtilitiesSTOMDATA) };

		if (aFlags & ShaderFlags::HasVertexColors)
		{
			aBuffer[at++] = _D3D_SHADER_MACRO{ "VERTEXCOLOR", "true" };
		}

		if (aFlags & ShaderFlags::HasUvSets)
		{
			aBuffer[at++] = _D3D_SHADER_MACRO{ "HAS_UV_SETS", "true" };
			aBuffer[at++] = _D3D_SHADER_MACRO{ "UV_SETS_COUNT", numberLookup[UvSetsCountFromFlags(aFlags)] };
		}
		if (aFlags & ShaderFlags::HasBones)
		{
			aBuffer[at++] = _D3D_SHADER_MACRO{ "HAS_BONES", "true" };
			aBuffer[at++] = _D3D_SHADER_MACRO{ "BONESPERVERTEX", numberLookup[BonePerVertexCountFromFlags(aFlags)] };
		}
		WIPE(aBuffer[at]);
	}
}