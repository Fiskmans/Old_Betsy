#ifndef ENGINE_GRAPHICS_GRAPHICS_FRAMEWORK_H
#define ENGINE_GRAPHICS_GRAPHICS_FRAMEWORK_H

#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <array>

namespace engine
{
	class GraphicsFramework
	{
	public:
		const static std::array<float, DXGI_FORMAT_V408 + 1> FormatToSizeLookup;
		GraphicsFramework();
		~GraphicsFramework();

		bool Init();

		void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* aRenterTarget, ID3D11DepthStencilView* aDepthStencil);


		void EndFrame();

		ID3D11DeviceContext* GetContext();
		ID3D11Device* GetDevice();
		struct ID3D11Texture2D* GetBackbufferTexture();
		IDXGISwapChain* GetSwapChain();

		static void Imgui();
		static void AddGraphicsMemoryUsage(size_t aAmount, const std::string& aName, const std::string& aCategory);

	private:
		struct memoryUser
		{
			size_t aAmount = 0;
			std::string myName;
		};
		static size_t ourTotalUsage;
		static std::unordered_map<std::string, std::pair<size_t, std::vector<memoryUser>>> ourMemoryUsers;

		ID3D11Device* myDevice = nullptr;
		IDXGISwapChain* mySwapChain = nullptr;
		ID3D11DeviceContext* myDeviceContext = nullptr;
		ID3D11RenderTargetView* myBackBuffer = nullptr;
		ID3D11DepthStencilView* myDepthBuffer = nullptr;
	};

}

#endif