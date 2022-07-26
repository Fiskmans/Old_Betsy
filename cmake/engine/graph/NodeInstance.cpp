#include "engine/graph/NodeInstance.h"

#include "engine/graph/Graph.h"
#include "engine/graph/Node.h"

namespace engine::graph
{
	std::byte* globalCurrentInstanceContext = nullptr;
	std::stack<std::byte*> NodeInstanceScopedContext::ourContextStack;

	NodeInstance::NodeInstance(BuiltNode& aType, ImVec2 aPosition)
		: DrawablePinBlock(aPosition)
		, myType(aType)
		, myId(GetNextNodeInstanceId())
	{
		myType.Construct(this);
	}

	NodeInstance::~NodeInstance()
	{
		myType.Destruct(this);
	}

	bool NodeInstance::Imgui(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		NodeInstanceScopedContext context(myMemory.data());
		return DrawablePinBlock::Imgui(NodeManager::PrettyfyName(myType.Name()).c_str(), aGraph, aScale, aPosition, myType.InPins(), myType.OutPins());
	}

	void NodeInstance::Activate()
	{
		NodeInstanceScopedContext context(myMemory.data());
		myType.myBaseNode->Activate();
	}

	ImVec2 NodeInstance::CustomImguiSize() 
	{
		NodeInstanceScopedContext context(myMemory.data());
		return myType.ImguiSize(); 
	}

	void NodeInstance::CustomImgui(float aScale, ImVec2 aTopLeft) 
	{
		NodeInstanceScopedContext context(myMemory.data());
		myType.Imgui(aScale, aTopLeft);
	}

	void NodeInstance::RemoveAllRelatedLinks(std::vector<std::unique_ptr<PinLink>>& aLinks)
	{
		for (PinBase* pin : myType.InPins())
		{
			InPinInstanceBase* item = pin->GetInPinInstance();
			std::vector<std::unique_ptr<PinLink>>::iterator it = std::find_if(std::begin(aLinks), std::end(aLinks), [item](const std::unique_ptr<PinLink>& aLink) { return aLink->myTo == item; });

			if (it != std::end(aLinks))
				aLinks.erase(it);
		}

		for (PinBase* pin : myType.OutPins())
		{
			OutPinInstanceBase* item = pin->GetOutPinInstance();
			std::vector<std::unique_ptr<PinLink>>::iterator it = std::find_if(std::begin(aLinks), std::end(aLinks), [item](const std::unique_ptr<PinLink>& aLink) { return aLink->myFrom == item; });

			if (it != std::end(aLinks))
				aLinks.erase(it);
		}
	}

	NodeInstanceScopedContext::NodeInstanceScopedContext(std::byte* aContext)
	{
		ourContextStack.push(globalCurrentInstanceContext);
		globalCurrentInstanceContext = aContext;
	}

	NodeInstanceScopedContext::~NodeInstanceScopedContext()
	{
		globalCurrentInstanceContext = ourContextStack.top();
		ourContextStack.pop();
	}
}
