#pragma once

struct ID3D11ShaderResourceView;
struct ID3D11Device;
enum D3D11_RESOURCE_MISC_FLAG;

class Texture
{
	friend void ReloadTexture(const std::string& aFilePath);
	friend void ReleaseTexture(Texture* aTexture);
	friend Texture* LoadTexture(ID3D11Device* aDevice, std::string aFilePath, D3D11_RESOURCE_MISC_FLAG aFlag, D3D11_USAGE aUsage);
public:
	Texture(ID3D11ShaderResourceView* aTexture);
	operator ID3D11ShaderResourceView* ();
	operator ID3D11ShaderResourceView** ();
	void Release();
	static void Flush();
private:
	void AddRef();
	bool Release_internal();
	ID3D11ShaderResourceView* myView;
	size_t myRefCounter;
};

Texture* LoadTexture(ID3D11Device* aDevice, std::string aFilePath,D3D11_RESOURCE_MISC_FLAG aFlag = static_cast<D3D11_RESOURCE_MISC_FLAG>(0),D3D11_USAGE aUsage = D3D11_USAGE_IMMUTABLE);
Texture* GeneratePerlin(ID3D11Device* aDevice, CommonUtilities::Vector2<size_t> aSize, V2F aScale = {1,1}, unsigned int aSeed = 0, D3D11_RESOURCE_MISC_FLAG aFlag = static_cast<D3D11_RESOURCE_MISC_FLAG>(0));
void SetErrorTexture(ID3D11Device* aDevice, std::string aFilePath);
bool IsErrorTexture(Texture* aTexture);
void ReleaseTexture(Texture* aTexture);


inline Texture::Texture(ID3D11ShaderResourceView* aTexture)
{
	myView = aTexture;
	myRefCounter = 0;
}

inline Texture::operator ID3D11ShaderResourceView* ()
{
	if (!this)
	{
		return nullptr;
	}
	return myView;
}

inline Texture::operator ID3D11ShaderResourceView** ()
{
	if (!this)
	{
		return nullptr;
	}
	return &myView;
}

inline void Texture::Release()
{
	ReleaseTexture(this);
}

inline void Texture::AddRef()
{
	++myRefCounter;
}

inline bool Texture::Release_internal()
{
	if (--myRefCounter <= 0)
	{
		myView->Release();
		return true;
	}
	return false;
}
