#pragma once
#include "Component.h"
#include "ModelInstance.h"

class ModelLoader;

class Graphics : public Component
{
public:
	Graphics();
	~Graphics();

	virtual void Init(Entity* aEntity) override;
	virtual void SetUpModel(const std::string& aPath);
	virtual void Update(const float aDeltaTime) override;
	virtual void Reset() override;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnKillMe() override;
private:
};

