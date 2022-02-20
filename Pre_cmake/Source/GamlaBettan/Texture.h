#pragma once


class Texture
{
public:
	friend class TextureFactory;
	friend class GBuffer;

public:
	Texture() = default;
	virtual ~Texture() = default;
	void Release();

	void ClearTexture(V4F aClearColor = {});
	void ClearDepth(float aClearDepth = 1.0f, unsigned int aClearStencil = 0);
	void SetAsActiveTarget(Texture* aDepth = nullptr);
	void SetAsResourceOnSlot(unsigned int aSlot);

	ID3D11ShaderResourceView* GetResourceView();

	operator bool()
	{
		return !!myContext && !!myTexture && (!!myRenderTarget || !!myDepth) && !!myViewport;
	}

protected:
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
	UpdatableTexture(CommonUtilities::Vector2<unsigned int> aSize);

	template<class Func>
	void GenerateAllPixels(Func&& aFunctor)
	{
		for (unsigned int y = 0; y < mySize.y; y++)
		{
			for (unsigned int x = 0; x < mySize.x; x++)
			{
				myData[y * mySize.x + x] = aFunctor(x, y);
			}
		}
	}

	void Upload();
	float Sample(unsigned int aX, unsigned int aY);

private:

	CommonUtilities::Vector2<unsigned int> mySize;
	std::vector<float> myData;
};