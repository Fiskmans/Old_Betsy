#pragma once

struct IDXGISwapChain;
struct IDXGISwapChain4;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;


class DirectX11Framework
{
public:
	const static std::array<float, DXGI_FORMAT_V408 + 1> FormatToSizeLookup;
	DirectX11Framework();
	~DirectX11Framework();

	bool Init();

	void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* aRenterTarget, ID3D11DepthStencilView* aDepthStencil);


	void EndFrame();
	void Resize(void* aHWND);

	ID3D11DeviceContext* GetContext();
	ID3D11Device* GetDevice();
	struct ID3D11Texture2D* GetBackbufferTexture();
	IDXGISwapChain* GetSwapChain();

#if USEIMGUI
	static void Imgui();
#endif // USEIMGUI
	static void AddGraphicsMemoryUsage(size_t aAmount, const std::string& aName, const std::string& aCategory);

private:
	struct memoryUser
	{
		size_t aAmount = 0;
		std::string myName;
	};
	static size_t ourTotalUsage;
	static std::map<std::string, std::pair<size_t, std::vector<memoryUser>>> ourMemoryUsers;


	HRESULT mySwapResult = S_OK;
	ID3D11Device* myDevice = nullptr;
	IDXGISwapChain* mySwapChain = nullptr;
	IDXGISwapChain4* mySwapChain4 = nullptr;
	ID3D11DeviceContext* myDeviceContext = nullptr;
	ID3D11RenderTargetView* myBackBuffer = nullptr;
	ID3D11DepthStencilView* myDepthBuffer = nullptr;
};

