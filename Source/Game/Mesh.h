#pragma once
#include "Graphics.h"
#include "Observer.hpp"

class ModelLoader;
class Scene;

class Mesh : public Graphics, public Observer
{

public:
	Mesh();
	virtual ~Mesh();
	void PreInit(Scene* aScene);
	virtual void Init(Entity* aEntity) override;
	void SetUpModel(const std::string& aPath = "Cube.fbx") override;
	void AddModelToScene(int aModelIndex = 0);
	void RemoveCurrentModelFromScene();
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;
	void SetTint(V4F aTint);
	void SetPostRotation(V3F aPostRotation); 
	void SetScale(V3F aScale, int aModelIndex = 0);
	void SetOffsetPosition(V3F aOffset);
	void SetSecondOffsetPosition(V3F aOffset);
	void SetOffsetRotation(V3F aOffset);
	void SetShouldBeDrawnThroughWalls(const bool aFlag);
	void SetUsePlayerThroughWallShader(const bool aFlag);

	void SetModelLimit(int aLimit);

	void AttachToBone(ModelInstance* aParentEntity, size_t aBone, int aModelIndex = 0);
	void SetCastsShadows(bool aValue);

	void SetFading(bool aIsFading);

	ModelInstance* GetModelInstance();
	int GetNumberOfMeshes();

	virtual void SubscribeToMyMessages() override;
	virtual void UnSubscribeToMyMessages() override;
	virtual void RecieveEntityMessage(EntityMessage aMessage, void* someData) override;

	void UpdateInstancePosition();

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;

private:
	V3F myPostRotation;
	V3F myScale;
	V3F myOffsetPosition;
	V3F mySecondOffsetPosition;
	V3F myOffsetRotation;
	V4F myTint;
	float myFalloff = 2.8f;
	bool myIsFadingOut;
	Scene* myScene;

	ModelInstance* myModelInstance;


	// Inherited via Observer
	virtual void RecieveMessage(const Message& aMessage) override;
	//RENDERCOMMAND myRenderCommand;
	bool myUpdateOverridden;
};

