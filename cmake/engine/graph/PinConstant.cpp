#include "engine/graph/PinConstant.h"

namespace engine::graph
{
	namespace pin_constant_helpers
	{
		size_t ChannelsInFormat(DXGI_FORMAT aFormat)
		{
			switch (aFormat)
			{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				break;
			case DXGI_FORMAT_R32G32B32_FLOAT:
				break;
			case DXGI_FORMAT_R32G32_FLOAT:
				break;
			case DXGI_FORMAT_R32G32_UINT:
				break;
			case DXGI_FORMAT_R32G32_SINT:
				break;
			case DXGI_FORMAT_R32G8X24_TYPELESS:
				break;
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
				break;
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
				break;
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
				break;
			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
				break;
			case DXGI_FORMAT_R10G10B10A2_UNORM:
				break;
			case DXGI_FORMAT_R10G10B10A2_UINT:
				break;
			case DXGI_FORMAT_R11G11B10_FLOAT:
				break;
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
				break;
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				break;
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				break;
			case DXGI_FORMAT_R8G8B8A8_UINT:
				break;
			case DXGI_FORMAT_R8G8B8A8_SNORM:
				break;
			case DXGI_FORMAT_R8G8B8A8_SINT:
				break;
			case DXGI_FORMAT_R16G16_TYPELESS:
				break;
			case DXGI_FORMAT_R16G16_FLOAT:
				break;
			case DXGI_FORMAT_R16G16_UNORM:
				break;
			case DXGI_FORMAT_R16G16_UINT:
				break;
			case DXGI_FORMAT_R16G16_SNORM:
				break;
			case DXGI_FORMAT_R16G16_SINT:
				break;
			case DXGI_FORMAT_R32_TYPELESS:
				break;
			case DXGI_FORMAT_D32_FLOAT:
				break;
			case DXGI_FORMAT_R32_FLOAT:
				break;
			case DXGI_FORMAT_R32_UINT:
				break;
			case DXGI_FORMAT_R32_SINT:
				break;
			case DXGI_FORMAT_R24G8_TYPELESS:
				break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				break;
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
				break;
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
				break;
			case DXGI_FORMAT_R8G8_TYPELESS:
				break;
			case DXGI_FORMAT_R8G8_UNORM:
				break;
			case DXGI_FORMAT_R8G8_UINT:
				break;
			case DXGI_FORMAT_R8G8_SNORM:
				break;
			case DXGI_FORMAT_R8G8_SINT:
				break;
			case DXGI_FORMAT_R16_TYPELESS:
				break;
			case DXGI_FORMAT_R16_FLOAT:
				break;
			case DXGI_FORMAT_D16_UNORM:
				break;
			case DXGI_FORMAT_R16_UNORM:
				break;
			case DXGI_FORMAT_R16_UINT:
				break;
			case DXGI_FORMAT_R16_SNORM:
				break;
			case DXGI_FORMAT_R16_SINT:
				break;
			case DXGI_FORMAT_R8_TYPELESS:
				break;
			case DXGI_FORMAT_R8_UNORM:
				break;
			case DXGI_FORMAT_R8_UINT:
				break;
			case DXGI_FORMAT_R8_SNORM:
				break;
			case DXGI_FORMAT_R8_SINT:
				break;
			case DXGI_FORMAT_A8_UNORM:
				break;
			case DXGI_FORMAT_R1_UNORM:
				break;
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
				break;
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
				break;
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
				break;
			case DXGI_FORMAT_BC1_TYPELESS:
				break;
			case DXGI_FORMAT_BC1_UNORM:
				break;
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC2_TYPELESS:
				break;
			case DXGI_FORMAT_BC2_UNORM:
				break;
			case DXGI_FORMAT_BC2_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC3_TYPELESS:
				break;
			case DXGI_FORMAT_BC3_UNORM:
				break;
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC4_TYPELESS:
				break;
			case DXGI_FORMAT_BC4_UNORM:
				break;
			case DXGI_FORMAT_BC4_SNORM:
				break;
			case DXGI_FORMAT_BC5_TYPELESS:
				break;
			case DXGI_FORMAT_BC5_UNORM:
				break;
			case DXGI_FORMAT_BC5_SNORM:
				break;
			case DXGI_FORMAT_B5G6R5_UNORM:
				break;
			case DXGI_FORMAT_B5G5R5A1_UNORM:
				break;
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				break;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				break;
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
				break;
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:
				break;
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
				break;
			case DXGI_FORMAT_B8G8R8X8_TYPELESS:
				break;
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC6H_TYPELESS:
				break;
			case DXGI_FORMAT_BC6H_UF16:
				break;
			case DXGI_FORMAT_BC6H_SF16:
				break;
			case DXGI_FORMAT_BC7_TYPELESS:
				break;
			case DXGI_FORMAT_BC7_UNORM:
				break;
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				break;
			case DXGI_FORMAT_AYUV:
				break;
			case DXGI_FORMAT_Y410:
				break;
			case DXGI_FORMAT_Y416:
				break;
			case DXGI_FORMAT_NV12:
				break;
			case DXGI_FORMAT_P010:
				break;
			case DXGI_FORMAT_P016:
				break;
			case DXGI_FORMAT_420_OPAQUE:
				break;
			case DXGI_FORMAT_YUY2:
				break;
			case DXGI_FORMAT_Y210:
				break;
			case DXGI_FORMAT_Y216:
				break;
			case DXGI_FORMAT_NV11:
				break;
			case DXGI_FORMAT_AI44:
				break;
			case DXGI_FORMAT_IA44:
				break;
			case DXGI_FORMAT_P8:
				break;
			case DXGI_FORMAT_A8P8:
				break;
			case DXGI_FORMAT_B4G4R4A4_UNORM:
				break;
			case DXGI_FORMAT_P208:
				break;
			case DXGI_FORMAT_V208:
				break;
			case DXGI_FORMAT_V408:
				break;
			case DXGI_FORMAT_FORCE_UINT:
				break;
			default:
				break;
			}

			return 0;
		}
	}

}
