#pragma once
#include "DirectXTK\Inc\SpriteFont.h"

DirectX::FXMVECTOR ToShitVector(const V2F& aVec);

DirectX::FXMVECTOR ToShitVector(const V4F& aVec);

V2F FromShitVector(const DirectX::FXMVECTOR& aVec);

