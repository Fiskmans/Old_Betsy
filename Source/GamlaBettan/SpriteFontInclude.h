#pragma once
#include "../packages/directxtk_desktop_2015.2019.10.17.1/include/SpriteFont.h"
#include "Vector2.hpp"
#include "Vector4.hpp"

DirectX::FXMVECTOR ToShitVector(const V2F& aVec);

DirectX::FXMVECTOR ToShitVector(const V4F& aVec);

V2F FromShitVector(const DirectX::FXMVECTOR& aVec);

