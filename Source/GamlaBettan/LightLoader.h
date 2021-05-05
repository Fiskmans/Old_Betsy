#pragma once

struct EnvironmentLight;
struct ID3D11Device;
class CCameraFactory;

class LightLoader
{
public:
	bool Init(ID3D11Device* aDevice);
	EnvironmentLight* LoadLight(const std::string& aFilepath); //Uses skybox atm (See ForwardRenderer.cpp)

private:
	ID3D11Device* myDevice;
};

