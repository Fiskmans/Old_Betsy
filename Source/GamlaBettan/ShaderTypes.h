#pragma once

struct aiMesh;
typedef struct _D3D_SHADER_MACRO  D3D_SHADER_MACRO;
struct D3D11_INPUT_ELEMENT_DESC;


class ShaderTypes
{
public:
	struct Offsets
	{
		size_t position, normal, tangent, bitanget, vertexcolor, uv, uv1, uv2,bones,boneweights;
		size_t size;
	};

	static Offsets OffsetsFromFlags(size_t aFlags);
	static size_t FlagsFromMesh(aiMesh* aMesh);
	static const _D3D_SHADER_MACRO* DefinesFromFlags(size_t aFlags);
	static std::string PostfixFromFlags(size_t aFlags);
	static D3D11_INPUT_ELEMENT_DESC* InputLayoutFromFlags(size_t aFlags, size_t& aElements);
	static size_t BonePerVertexCountFromFlags(size_t aFlags);
};

