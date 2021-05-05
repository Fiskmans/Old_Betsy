#pragma once
#include "GraphicsComponent.h"

class CModelLoader;
class CScene;

class Mesh : public GraphicsComponent
{

public:
	Mesh();
	virtual ~Mesh();
	void PreInit(CModelLoader* aModelLoader, CScene* aScene);
	virtual void Init(Entity* aEntity) override;
	void SetUpModel(std::string aPath = "Cube.fbx") override;
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	CModelInstance* GetModelInstance();

private:

	CModelLoader* myModelLoader;
	CScene* myScene;
	CModelInstance* myModelInstance;
	//RENDERCOMMAND myRenderCommand;
};

