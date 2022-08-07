#ifndef ENGINE_ASSETS_SHADER_TYPES_H
#define ENGINE_ASSETS_SHADER_TYPES_H

#include "engine/assets/ShaderFlags.h"

#include <string>
#include <vector>

#define NOMINMAX
#include <windows.h>
#include <d3d11.h>

struct aiMesh;

namespace engine
{
	class ShaderTypes
	{
	public:
		static const size_t MaxInputElementSize = 4 + 1 + 4 + 8 * 2 + 1;
		static const size_t MaxDefineSize = 3 + 1 + 2 + 2 + 1;

		struct Offsets
		{
			size_t position, normal, tangent, bitanget, vertexcolor, uv, bones, boneweights;
			size_t size;
		};
		static ShaderFlags FlagsFromMesh(const aiMesh* aMesh);

		static std::string PostfixFromFlags(size_t aFlags);
		static Offsets OffsetsFromFlags(size_t aFlags);

		static UINT InputLayoutFromFlags(D3D11_INPUT_ELEMENT_DESC* aOutDesc, size_t aFlags);

		static size_t BonePerVertexCountFromFlags(size_t aFlags);
		static size_t UvSetsCountFromFlags(size_t aFlags);
	};

	class ShaderDefines
	{
	public:
		ShaderDefines(size_t aFlags);

		_D3D_SHADER_MACRO* Get();

	private:

		void AddMacro(std::string aName, std::string aValue);
		std::vector<_D3D_SHADER_MACRO> myMacros;
		std::vector<std::string> myDataStorage;
	};
}


#endif // 