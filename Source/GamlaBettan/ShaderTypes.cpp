#include "pch.h"
#include "ShaderTypes.h"

#include "ShaderFlags.h"
#include "AssImp/mesh.h"
#include <d3d11.h>


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

	out.uv = at;
	at += sizeof(float) * 2;

	if (aFlags & ShaderFlags::HasUvSets)
	{
		out.uv1 = at;
		at += sizeof(float) * 2;

		out.uv2 = at;
		at += sizeof(float) * 2;
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

size_t ShaderTypes::FlagsFromMesh(aiMesh* aMesh)
{
	size_t result = 0;
	if (aMesh->HasVertexColors(0))
	{
		result |= ShaderFlags::HasVertexColors;
	}
	if (aMesh->HasTextureCoords(1) && aMesh->HasTextureCoords(2))
	{
		result |= ShaderFlags::HasUvSets;
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
		result |= (0xF & numberofBones) << ShaderFlags::NumBonesOffset;
	}

	return result;
}

std::string ShaderTypes::PostfixFromFlags(size_t aFlags)
{
	std::string out;

	if (aFlags & ShaderFlags::HasUvSets)
	{
		out += "_uv3";
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

D3D11_INPUT_ELEMENT_DESC* ShaderTypes::InputLayoutFromFlags(size_t aFlags, size_t& aElements)
{
#pragma warning(push)
#pragma warning(disable : 26812)
	static D3D11_INPUT_ELEMENT_DESC buffer[64];

	WIPE(buffer);
	aElements = 0;

	buffer[aElements++] = { "POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	buffer[aElements++] = { "NORMAL"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	buffer[aElements++] = { "TANGENT"  , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	buffer[aElements++] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (aFlags & ShaderFlags::HasVertexColors)
	{
		buffer[aElements++] = { "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}
	buffer[aElements++] = { "UV"		, 0, DXGI_FORMAT_R32G32_FLOAT		 , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	if (aFlags & ShaderFlags::HasUvSets)
	{
		buffer[aElements++] = { "UV"		,1, DXGI_FORMAT_R32G32_FLOAT		 ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 };
		buffer[aElements++] = { "UV"		,2, DXGI_FORMAT_R32G32_FLOAT		 ,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 };
	}
	if (aFlags & ShaderFlags::HasBones)
	{
		size_t numBones = BonePerVertexCountFromFlags(aFlags);
		for (UINT i = 0; i < numBones; i++)
		{
			buffer[aElements++] = { "BONES"		,i,DXGI_FORMAT_R32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 };
		}
		for (UINT i = 0; i < numBones; i++)
		{
			buffer[aElements++] = { "BONEWEIGHTS",i,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 };
		}
	}
	return buffer;
#pragma warning(pop)
}

size_t ShaderTypes::BonePerVertexCountFromFlags(size_t aFlags)
{
	return (aFlags & ShaderFlags::BoneMask) >> ShaderFlags::NumBonesOffset;
}

const _D3D_SHADER_MACRO* ShaderTypes::DefinesFromFlags(size_t aFlags)
{
	static _D3D_SHADER_MACRO buffer[128];
	WIPE(buffer);
	size_t at = 0;

	buffer[at++] = _D3D_SHADER_MACRO{ STRING(NUMBEROFPOINTLIGHTS), STRINGVALUE(NUMBEROFPOINTLIGHTS) };
	buffer[at++] = _D3D_SHADER_MACRO{ STRING(NUMBEROFANIMATIONBONES), STRINGVALUE(NUMBEROFANIMATIONBONES) };
	buffer[at++] = _D3D_SHADER_MACRO{ STRING(MODELSAMOUNTOFCUSTOMDATA), STRINGVALUE(MODELSAMOUNTOFCUSTOMDATA) };


	if (aFlags & ShaderFlags::HasUvSets)
	{
		buffer[at++] = _D3D_SHADER_MACRO{ "MULTIPLE_UV", "true" };
	}
	if (aFlags & ShaderFlags::HasVertexColors)
	{
		buffer[at++] = _D3D_SHADER_MACRO{ "VERTEXCOLOR", "true" };
	}
	if (aFlags & ShaderFlags::HasBones)
	{
		buffer[at++] = _D3D_SHADER_MACRO{ "HAS_BONES", "true" };
		static const char* fuckIt[] =		// >:'C
		{
			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",
			"10",
			"11",
			"12",
			"13",
			"14",
			"15"
		};
		buffer[at++] = _D3D_SHADER_MACRO{ "BONESPERVERTEX", fuckIt[BonePerVertexCountFromFlags(aFlags)] };
	}
	return buffer;
}