#ifndef ENGINE_GRAPHICS_POINT_LIGHT_H
#define ENGINE_GRAPHICS_POINT_LIGHT_H

#include "engine/graphics/Camera.h"

#include "tools/MathVector.h"

#include <memory>
#include <d3d11.h>


namespace engine
{
	class PointLight
	{
	public:
		~PointLight()
		{
			SAFE_RELEASE(myDepth);
			SAFE_RELEASE(myDepthsResource);
		}

		PointLight(const PointLight&) = delete;
		void operator=(const PointLight&) = delete;


		tools::V3f position;
		float intensity = 0.f;
		tools::V3f color =  tools::V3f( 1.f, 1.f, 1.f );
		float range = 0.f;

		std::unique_ptr<PerspectiveCamera> myCameras[6];
		ID3D11DepthStencilView* myDepth = nullptr;
		ID3D11ShaderResourceView* myDepthsResource = nullptr;
	};

	/*
	*
		for (size_t i = 0; i < 6; i++)
		{
			myCameras[i] = new PerspectiveCamera( CCameraFactory::CreateCamera(90, false, SHADOWNEARPLANE, SHADOWFARPLANE);
			myCameras[i]->SetResolution(V2ui(SHADOWRESOLUTION, SHADOWRESOLUTION));
		}
		myCameras[0]->SetRotation(V3F(0.f, 0.f, 0.f));
		myCameras[1]->SetRotation(V3F(PI_F, 0.f, 0.f));
		myCameras[2]->SetRotation(V3F(-PI_F / 2.2f, 0.f, 0.f));
		myCameras[3]->SetRotation(V3F(PI_F / 2.2f, 0.f, 0.f));
		myCameras[4]->SetRotation(V3F(0.f, -PI_F / 2.2f, 0.f));
		myCameras[5]->SetRotation(V3F(0.f, PI_F / 2.2f, 0.f));
	*/
}

#endif