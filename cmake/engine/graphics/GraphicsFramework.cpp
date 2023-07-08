
#include "engine/graphics/GraphicsFramework.h"
#include "engine/graphics/WindowManager.h"

#include "engine/utilities/Stopwatch.h"

#include "imgui/WindowControl.h"

#include "tools/MathVector.h"
#include "tools/ImGuiHelpers.h"

#include "tools/Logger.h"

#include "common/Macros.h"

#include <mutex>

#define NOMINMAX
#include <Windows.h>

namespace engine
{
	GraphicsFramework::GraphicsFramework()
	{
	}

	GraphicsFramework::~GraphicsFramework()
	{
		mySwapChain = nullptr;
		myDevice = nullptr;
		myDeviceContext = nullptr;
	}

	bool GraphicsFramework::Init()
	{
		LOG_SYS_INFO("Initializing Graphics framework");
		engine::utilities::StopWatch watch;
		{
			DXGI_SWAP_CHAIN_DESC desc;
			WIPE(desc);
			desc.BufferCount = 1;
			desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.OutputWindow = WindowManager::GetInstance().GetWindowHandle();
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
				LOG_SYS_CRASH("failed to create swapchain");
				return false;
			}
		}

		LOG_SYS_INFO("Graphics framework initialized in " + std::to_string(watch.Result().count()) + " seconds");
		return true;
	}

	void GraphicsFramework::SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* aRenterTarget, ID3D11DepthStencilView* aDepthStencil)
	{
		myBackBuffer = aRenterTarget;
	}


	void GraphicsFramework::EndFrame()
	{
		if (mySwapChain)
		{
			mySwapChain->Present(0, 0);
		}
	}

	ID3D11DeviceContext* GraphicsFramework::GetContext()
	{
		return myDeviceContext;
	}

	ID3D11Device* GraphicsFramework::GetDevice()
	{
		return myDevice;
	}

	ID3D11Texture2D* GraphicsFramework::GetBackbufferTexture()
	{
		HRESULT result;
		if (mySwapChain)
		{
			ID3D11Texture2D* texture;
			result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture);
			if (FAILED(result))
			{
				LOG_SYS_ERROR("Could not get backbuffer from swapchain");
				return nullptr;
			}
			return texture;
		}

		ID3D11Resource* backbufferTexture = nullptr;
		myBackBuffer->GetResource(&backbufferTexture);
		if (!backbufferTexture)
		{
			LOG_SYS_ERROR("Could not get backbuffer texture");
			return nullptr;
		}
		return static_cast<ID3D11Texture2D*>(backbufferTexture);
	}

	IDXGISwapChain* GraphicsFramework::GetSwapChain()
	{
		return mySwapChain;
	}

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

	void GraphicsFramework::Imgui()
	{
		old_betsy_imgui::WindowControl::Window("Graphic Memory", []() -> void
		{
		});
	}
}