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
		size_t aAmount;
		std::string myName;
	};
	static size_t ourTotalUsage;
	static std::map<std::string, std::pair<size_t, std::vector<memoryUser>>> ourMemoryUsers;


	HRESULT mySwapResult;
	ID3D11Device* myDevice;
	IDXGISwapChain* mySwapChain;
	IDXGISwapChain4* mySwapChain4;
	ID3D11DeviceContext* myDeviceContext;
	ID3D11RenderTargetView* myBackBuffer;
	ID3D11DepthStencilView* myDepthBuffer;
};

