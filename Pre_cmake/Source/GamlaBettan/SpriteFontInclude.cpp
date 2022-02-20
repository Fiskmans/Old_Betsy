#include "pch.h"
#include "SpriteFontInclude.h"

DirectX::FXMVECTOR ToShitVector(const V2f& aVec)
{
	return DirectX::XMVectorSet(aVec.x, aVec.y, 0.f, 0.f);
}

DirectX::FXMVECTOR ToShitVector(const V4F& aVec)
{
	return DirectX::XMVectorSet(aVec.x, aVec.y, aVec.z, aVec.w);
}

V2f FromShitVector(const DirectX::FXMVECTOR& aVec)
{
	return V2f(DirectX::XMVectorGetX(aVec), DirectX::XMVectorGetY(aVec));
}