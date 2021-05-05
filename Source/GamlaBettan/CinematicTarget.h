#pragma once
class ModelInstance;
class CinematicTarget
{
public:
	ModelInstance* Get();
	V4F GetPosition();
	void SetScene(Scene* aScene);
	void Set(ModelInstance* aTarget);
	void Load(FiskJSON::Object& aObject, Scene* aScene);
	void AppendTo(FiskJSON::Object& aObject);
	void Edit();

private:
	ModelInstance* myModel = nullptr;
	V4F myOriginalPosition;
	Scene* myScene;
};

