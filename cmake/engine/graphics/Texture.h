#ifndef ENGINE_GRAPHICS_TEXTURE_H
#define ENGINE_GRAPHICS_TEXTURE_H

#include "tools/MathVector.h"

#include <vector>

#define NOMINMAX
#include <WinSock2.h>
#include <d3d11.h>

namespace engine::graphics
{
	class TextureFactory;
	class GBuffer;
	class DepthTexture;

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture();
		Texture(const Texture&) = delete;
		void operator=(const Texture&) = delete;
		Texture(Texture&&);
		void operator=(Texture&&);


		bool IsValid() { return !!myTexture && !!myRenderTarget && !!myViewport; }

		void ClearTexture(tools::V4f aClearColor = tools::V4f(0.f, 0.f, 0.f, 1.f));
		void SetAsActiveTarget(const DepthTexture* aDepth = nullptr) const;
		void SetAsResourceOnSlot(unsigned int aSlot);

		ID3D11ShaderResourceView* GetResourceView();

	protected:
		friend TextureFactory;

		ID3D11Texture2D* myTexture = nullptr;
		ID3D11RenderTargetView* myRenderTarget = nullptr;
		ID3D11ShaderResourceView* myShaderResource = nullptr;
		D3D11_VIEWPORT* myViewport = nullptr;
	};

	class DepthTexture
	{
	public:
		DepthTexture() = default;
		~DepthTexture();
		DepthTexture(const DepthTexture&) = delete;
		void operator=(const DepthTexture&) = delete;
		DepthTexture(DepthTexture&&);
		void operator=(DepthTexture&&);

		void ClearDepth(float aClearDepth = 1.0f, unsigned int aClearStencil = 0);
		bool IsValid() const { return !!myDepth; }

		const ID3D11DepthStencilView* GetDepth() const { return myDepth; }
		ID3D11ShaderResourceView*const* GetResourceView() const { return &myShaderResource; }

	private:
		friend TextureFactory;
		friend GBuffer;
		friend Texture;

		ID3D11Texture2D* myTexture = nullptr;
		ID3D11DepthStencilView* myDepth = nullptr;
		ID3D11ShaderResourceView* myShaderResource = nullptr;
	};

	class UpdatableTexture
		: public Texture
	{
	public:
		UpdatableTexture();
		UpdatableTexture(tools::V2ui aSize);

		template<class Func>
		requires requires(Func&& aFunctor)
		{
			{ aFunctor(0, 0) } -> std::convertible_to<float>;
		}
		void GenerateAllPixels(Func&& aFunctor)
		{
			for (unsigned int y = 0; y < mySize[1]; y++)
			{
				for (unsigned int x = 0; x < mySize[0]; x++)
				{
					myData[y * mySize[0] + x] = aFunctor(x, y);
				}
			}
		}

		void DrawDirect(size_t x, size_t y, float* aData, size_t length);

		void Upload();
		float Sample(tools::V2ui aPosition);

	private:

		tools::V2ui mySize;
		std::vector<float> myData;
	};

}

#endif