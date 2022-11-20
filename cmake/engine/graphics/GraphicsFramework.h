#ifndef ENGINE_GRAPHICS_GRAPHICS_FRAMEWORK_H
#define ENGINE_GRAPHICS_GRAPHICS_FRAMEWORK_H

#include "engine/graphics/Releasable.h"

#include "tools/Singleton.h"

#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <array>

namespace engine
{
	class GraphicsFramework : tools::Singleton<GraphicsFramework>
	{
	public:
		GraphicsFramework();
		~GraphicsFramework();

		bool Init();

		void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* aRenterTarget, ID3D11DepthStencilView* aDepthStencil);

		void EndFrame();

		ID3D11DeviceContext* GetContext();
		ID3D11Device* GetDevice();
		ID3D11Texture2D* GetBackbufferTexture();
		IDXGISwapChain* GetSwapChain();

		void Imgui();

		void AddResource(void* aResource, std::string aName);

		template<concepts::Releaseable T>
		void ReleaseResource(T* aResource)
		{
			aResource->Release();

			int b;
			DoSomething([]() -> int { int a;  return &a; }, b);
		}

		template<class Callable, typename ReturnValue>
			requires std::convertible_to<decltype(std::declval<Callable>()()), ReturnValue>
		void DoSomething(Callable&& aCallable, ReturnValue& aValue)
		{
			//do stuff
			new Callmanager<Callable>(aCallable, value);
		}



	private:
		size_t ourTotalUsage;

		ID3D11Device* myDevice = nullptr;
		IDXGISwapChain* mySwapChain = nullptr;
		ID3D11DeviceContext* myDeviceContext = nullptr;
		ID3D11RenderTargetView* myBackBuffer = nullptr;
	};

}

#endif