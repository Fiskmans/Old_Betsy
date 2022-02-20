#include "pch.h"
#include "AssetImportHelpers.h"

CommonUtilities::Matrix4x4<float> AiHelpers::ConvertToEngineMatrix33(const aiMatrix3x3& AssimpMatrix)
{
	CommonUtilities::Matrix4x4<float> mat;
	mat(1, 1) = AssimpMatrix.a1;	mat(1, 2) = AssimpMatrix.a2;	mat(1, 3) = AssimpMatrix.a3;	mat(1, 4) = 0.0f;
	mat(2, 1) = AssimpMatrix.b1;	mat(2, 2) = AssimpMatrix.b2;	mat(2, 3) = AssimpMatrix.b3;	mat(2, 4) = 0.0f;
	mat(3, 1) = AssimpMatrix.c1;	mat(3, 2) = AssimpMatrix.c2;	mat(3, 3) = AssimpMatrix.c3;	mat(3, 4) = 0.0f;
	mat(4, 1) = 0.0f;				mat(4, 2) = 0.0f;				mat(4, 3) = 0.0f;				mat(4, 4) = 1.0f;
	return mat;
}

CommonUtilities::Matrix4x4<float> AiHelpers::ConvertToEngineMatrix44(const aiMatrix4x4& AssimpMatrix)
{
	CommonUtilities::Matrix4x4<float> mat;
	mat(1, 1) = AssimpMatrix.a1; mat(1, 2) = AssimpMatrix.a2; mat(1, 3) = AssimpMatrix.a3; mat(1, 4) = AssimpMatrix.a4;
	mat(2, 1) = AssimpMatrix.b1; mat(2, 2) = AssimpMatrix.b2; mat(2, 3) = AssimpMatrix.b3; mat(2, 4) = AssimpMatrix.b4;
	mat(3, 1) = AssimpMatrix.c1; mat(3, 2) = AssimpMatrix.c2; mat(3, 3) = AssimpMatrix.c3; mat(3, 4) = AssimpMatrix.c4;
	mat(4, 1) = AssimpMatrix.d1; mat(4, 2) = AssimpMatrix.d2; mat(4, 3) = AssimpMatrix.d3; mat(4, 4) = AssimpMatrix.d4;
	return mat;
}
