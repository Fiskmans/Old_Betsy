#ifndef ENGINE_GRAPHICS_TEXTURE_H
#define ENGINE_GRAPHICS_TEXTURE_H

#include "tools/MathVector.h"

#include <vector>

#include <d3d11.h>

namespace engine
{
	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;
		void Release();

		void ClearTexture(tools::V4f aClearColor = tools::V4f(0.f, 0.f, 0.f, 1.f));
		void ClearDepth(float aClearDepth = 1.0f, unsigned int aClearStencil = 0);
		void SetAsActiveTarget(Texture* aDepth = nullptr);
		void SetAsResourceOnSlot(unsigned int aSlot);

		ID3D11ShaderResourceView* GetResourceView();

		operator bool()
		{
			return !!myContext && !!myTexture && (!!myRenderTarget || !!myDepth) && !!myViewport;
		}

	protected:
		friend class TextureFactory;
		friend class GBuffer;

		struct ID3D11DeviceContext* myContext = nullptr;
		struct ID3D11Texture2D* myTexture = nullptr;
		struct ID3D11RenderTargetView* myRenderTarget = nullptr;
		struct ID3D11DepthStencilView* myDepth = nullptr;
		struct ID3D11ShaderResourceView* myShaderResource = nullptr;
		struct D3D11_VIEWPORT* myViewport = nullptr;
	};


	class UpdatableTexture
		: public Texture
	{
	public:
		UpdatableTexture(tools::V2ui aSize);

		template<class Func>
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

		void Upload();
		float Sample(tools::V2ui aPosition);

	private:

		tools::V2ui mySize;
		std::vector<float> myData;
	};

}

#endif