#include "Component.h"


class ParentComponent : public Component<ParentComponent>
{
	ParentComponent();
	ParentComponent(const FiskJSON::Object& aObject);

	void AddChild(EntityID aEntity) { myChildren.push_back(aEntity); };
	bool RemoveChild(EntityID aEntity);

	virtual void Update(const FrameData&, EntityID) override;

#if USEIMGUI
	virtual void ImGui(EntityID) override;
	virtual void Serialize(FiskJSON::Object& aObject) override;
#endif

private: 
	std::vector<EntityID> myChildren;
};