#include "pch.h"
#include "DirectX11Framework.h"
#include "WindowHandler.h"
#include <Windows.h>
#include <d3d11_4.h>
#include "PostMaster.hpp"
#include "Sprite.h"

size_t DirectX11Framework::ourTotalUsage = 0;
std::map<std::string, std::pair<size_t, std::vector<DirectX11Framework::memoryUser>>> DirectX11Framework::ourMemoryUsers;
const std::array<float, DXGI_FORMAT_V408 + 1> DirectX11Framework::FormatToSizeLookup =
{
	-1, //DXGI_FORMAT_UNKNOWN = 0,
	16, //DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	16, //DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	16, //DXGI_FORMAT_R32G32B32A32_UINT = 3,
	16, //DXGI_FORMAT_R32G32B32A32_SINT = 4,
	12, //DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	12, //DXGI_FORMAT_R32G32B32_FLOAT = 6,
	12, //DXGI_FORMAT_R32G32B32_UINT = 7,
	12, //DXGI_FORMAT_R32G32B32_SINT = 8,
	8,  //DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	8,  //DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	8,  //DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	8,  //DXGI_FORMAT_R16G16B16A16_UINT = 12,
	8,  //DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	8,  //DXGI_FORMAT_R16G16B16A16_SINT = 14,
	8,  //DXGI_FORMAT_R32G32_TYPELESS = 15,
	8,  //DXGI_FORMAT_R32G32_FLOAT = 16,
	8,  //DXGI_FORMAT_R32G32_UINT = 17,
	8,  //DXGI_FORMAT_R32G32_SINT = 18,
	8,  //DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	8,  //DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	8,  //DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	8,  //DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	4,  //DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	4,  //DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	4,  //DXGI_FORMAT_R10G10B10A2_UINT = 25,
	4,  //DXGI_FORMAT_R11G11B10_FLOAT = 26,
	4,  //DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	4,  //DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	4,  //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	4,  //DXGI_FORMAT_R8G8B8A8_UINT = 30,
	4,  //DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	4,  //DXGI_FORMAT_R8G8B8A8_SINT = 32,
	4,  //DXGI_FORMAT_R16G16_TYPELESS = 33,
	4,  //DXGI_FORMAT_R16G16_FLOAT = 34,
	4,  //DXGI_FORMAT_R16G16_UNORM = 35,
	4,  //DXGI_FORMAT_R16G16_UINT = 36,
	4,  //DXGI_FORMAT_R16G16_SNORM = 37,
	4,  //DXGI_FORMAT_R16G16_SINT = 38,
	4,  //DXGI_FORMAT_R32_TYPELESS = 39,
	4,  //DXGI_FORMAT_D32_FLOAT = 40,
	4,  //DXGI_FORMAT_R32_FLOAT = 41,
	4,  //DXGI_FORMAT_R32_UINT = 42,
	4,  //DXGI_FORMAT_R32_SINT = 43,
	4,  //DXGI_FORMAT_R24G8_TYPELESS = 44,
	4,  //DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	4,  //DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	4,  //DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	2,  //DXGI_FORMAT_R8G8_TYPELESS = 48,
	2,  //DXGI_FORMAT_R8G8_UNORM = 49,
	2,  //DXGI_FORMAT_R8G8_UINT = 50,
	2,  //DXGI_FORMAT_R8G8_SNORM = 51,
	2,  //DXGI_FORMAT_R8G8_SINT = 52,
	2,  //DXGI_FORMAT_R16_TYPELESS = 53,
	2,  //DXGI_FORMAT_R16_FLOAT = 54,
	2,  //DXGI_FORMAT_D16_UNORM = 55,
	2,  //DXGI_FORMAT_R16_UNORM = 56,
	2,  //DXGI_FORMAT_R16_UINT = 57,
	2,  //DXGI_FORMAT_R16_SNORM = 58,
	2,  //DXGI_FORMAT_R16_SINT = 59,
	1,  //DXGI_FORMAT_R8_TYPELESS = 60,
	1,  //DXGI_FORMAT_R8_UNORM = 61,
	1,  //DXGI_FORMAT_R8_UINT = 62,
	1,  //DXGI_FORMAT_R8_SNORM = 63,
	1,  //DXGI_FORMAT_R8_SINT = 64,
	1,  //DXGI_FORMAT_A8_UNORM = 65,
	1,  //DXGI_FORMAT_R1_UNORM = 66,
	4,  //DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	4,  //DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	4,  //DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	0.5,//DXGI_FORMAT_BC1_TYPELESS = 70,
	0.5,//DXGI_FORMAT_BC1_UNORM = 71,
	0.5,//DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	1, //DXGI_FORMAT_BC2_TYPELESS = 73,
	1, //DXGI_FORMAT_BC2_UNORM = 74,
	1, //DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	1, //DXGI_FORMAT_BC3_TYPELESS = 76,
	1, //DXGI_FORMAT_BC3_UNORM = 77,
	1, //DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	0.5, //DXGI_FORMAT_BC4_TYPELESS = 79,
	0.5, //DXGI_FORMAT_BC4_UNORM = 80,
	0.5, //DXGI_FORMAT_BC4_SNORM = 81,
	1, //DXGI_FORMAT_BC5_TYPELESS = 82,
	1, //DXGI_FORMAT_BC5_UNORM = 83,
	1, //DXGI_FORMAT_BC5_SNORM = 84,
	2,  //DXGI_FORMAT_B5G6R5_UNORM = 85,
	2,  //DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	4,  //DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	4,  //DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	4,  //DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	4,  //DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	4,  //DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	4,  //DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	4,  //DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	1, //DXGI_FORMAT_BC6H_TYPELESS = 94,
	1, //DXGI_FORMAT_BC6H_UF16 = 95,
	1, //DXGI_FORMAT_BC6H_SF16 = 96,
	1, //DXGI_FORMAT_BC7_TYPELESS = 97,
	1, //DXGI_FORMAT_BC7_UNORM = 98,
	1, //DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	-1, //DXGI_FORMAT_AYUV = 100,
	-1, //DXGI_FORMAT_Y410 = 101,
	-1, //DXGI_FORMAT_Y416 = 102,
	-1, //DXGI_FORMAT_NV12 = 103,
	-1, //DXGI_FORMAT_P010 = 104,
	-1, //DXGI_FORMAT_P016 = 105,
	-1, //DXGI_FORMAT_420_OPAQUE = 106,
	-1, //DXGI_FORMAT_YUY2 = 107,
	-1, //DXGI_FORMAT_Y210 = 108,
	-1, //DXGI_FORMAT_Y216 = 109,
	-1, //DXGI_FORMAT_NV11 = 110,
	-1, //DXGI_FORMAT_AI44 = 111,
	-1, //DXGI_FORMAT_IA44 = 112,
	1,  //DXGI_FORMAT_P8 = 113,
	2,  //DXGI_FORMAT_A8P8 = 114,
	2	//DXGI_FORMAT_B4G4R4A4_UNORM = 115,
};


DirectX11Framework::DirectX11Framework()
{
}

DirectX11Framework::~DirectX11Framework()
{
	mySwapChain = nullptr;
	myDevice = nullptr;
	myDeviceContext = nullptr;
	myDepthBuffer = nullptr;
}

bool DirectX11Framework::Init()
{
	DXGI_SWAP_CHAIN_DESC desc;
	WIPE(desc);
	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = WindowHandler::GetInstance().GetWindowHandle();
	desc.SampleDesc.Count = 1;
	desc.Windowed = true;

	UINT flags = 0;
#ifdef _DEBUG
	GetAsyncKeyState(VK_LSHIFT);
	if (GetAsyncKeyState(VK_LSHIFT))
	{
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif // _DEBUG


	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		flags, 
		nullptr, 
		0, 
		D3D11_SDK_VERSION,
		&desc, 
		&mySwapChain, 
		&myDevice, 
		nullptr, 
		&myDeviceContext);

	if (FAILED(result))
	{
		SYSCRASH("Could not create swapchain")
		return false;
	}

	V2ui size = WindowHandler::GetInstance().GetSize();
	AddGraphicsMemoryUsage(static_cast<size_t>(DirectX11Framework::FormatToSizeLookup[DXGI_FORMAT_R8G8B8A8_UNORM] * size.x * size.y), "Backbuffer", "Engine");
	return true;
}

void DirectX11Framework::SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* aRenterTarget, ID3D11DepthStencilView* aDepthStencil)
{
	myBackBuffer = aRenterTarget;
	myDepthBuffer = aDepthStencil;
}


void DirectX11Framework::EndFrame()
{
	if (mySwapChain)
	{
		PERFORMANCETAG("Present");
		mySwapChain->Present(0, 0);
	}
}

void DirectX11Framework::Resize(void* aHWND)
{
	if (mySwapChain && myDeviceContext && myDevice)
	{
		RECT rect;
		GetWindowRect(static_cast<HWND>(aHWND), &rect);

		V2ui data{
			static_cast<unsigned int>(rect.right - rect.left),
			static_cast<unsigned int>(rect.bottom - rect.top)
		};
		PostMaster::GetInstance().SendMessages(MessageType::WindowResize, &data);
	}
}

ID3D11DeviceContext* DirectX11Framework::GetContext()
{
	return myDeviceContext;
}

ID3D11Device* DirectX11Framework::GetDevice()
{
	return myDevice;
}

ID3D11Texture2D* DirectX11Framework::GetBackbufferTexture()
{
	HRESULT result;
	if (mySwapChain)
	{
		ID3D11Texture2D* texture;
		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture);
		if (FAILED(result))
		{
			SYSERROR("Could not get backbuffer from swapchain");
			return nullptr;
		}
		return texture;
	}
	
	ID3D11Resource* backbufferTexture = nullptr;
	myBackBuffer->GetResource(&backbufferTexture);
	if (!backbufferTexture)
	{
		SYSERROR("Could not get backbuffer texture");
		return nullptr;
	}
	return static_cast<ID3D11Texture2D*>(backbufferTexture);
}

IDXGISwapChain* DirectX11Framework::GetSwapChain()
{
	return mySwapChain;
}

#if USEIMGUI
namespace nocollide
{
	std::string ReadableByteSize(size_t aSize)
	{
		size_t amount = aSize;
		const char* affixes[] = { "b","kb","mb","gb","tb","pb","eb" };
		const char** affix = affixes;
		while (amount > 8192)
		{
			amount /= 1024;
			affix++;
		}
		return std::to_string(amount) + *affix;
	}
}

void DirectX11Framework::Imgui()
{
	WindowControl::Window("Graphic Memory", []() -> void
		{
			ImGui::Text("Tracked usage: " PFSTRING, nocollide::ReadableByteSize(ourTotalUsage).c_str());
			ImGui::Separator();
			for (auto& list : ourMemoryUsers)
			{
				if (ImGui::TreeNode(("["+ Tools::PadOrTrimTo(list.first,18) + "] " + nocollide::ReadableByteSize(list.second.first)).c_str()))
				{
					for (auto& user : list.second.second)
					{
						ImGui::BulletText("[" PFSTRING"] " PFSTRING,Tools::PadOrTrimTo(user.myName,18).c_str(),nocollide::ReadableByteSize(user.aAmount).c_str());
					}

					ImGui::TreePop();
				}
			}
		});
}
#endif
void DirectX11Framework::AddGraphicsMemoryUsage(size_t aAmount, const std::string& aName, const std::string& aCategory)
{
#if TRACKGFXMEM
	static std::mutex mutex;
	std::lock_guard guard(mutex);

	memoryUser user;
	user.aAmount = aAmount;
	user.myName = aName;

	ourMemoryUsers[aCategory].first += aAmount;
	auto it = ourMemoryUsers[aCategory].second.begin();
	while (it != ourMemoryUsers[aCategory].second.end() && (*it).aAmount > aAmount)
	{
		it++;
	}
	ourMemoryUsers[aCategory].second.insert(it,user);

	ourTotalUsage += aAmount;
#endif
}