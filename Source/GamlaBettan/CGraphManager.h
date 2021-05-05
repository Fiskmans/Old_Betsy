#pragma once
#include <imgui_node_editor.h>
#include <stack>

namespace ed = ax::NodeEditor;
class CGraphManager
{
public:
	~CGraphManager();
	void Load();

	void ReTriggerUpdateringTrees();
	
	void PreFrame(float aTimeDelta);
	void ConstructEditorTreeAndConnectLinks();
	void PostFrame();

	void ReTriggerTree();
	void SaveTreeToFile();
	void LoadTreeFromFile();
	static void ShowFlow(int aLinkID);
private:
	void WillBeCyclic(class CNodeInstance* aFirst, class CNodeInstance* aSecond, bool& aIsCyclic, class CNodeInstance* aBase);
	
	void DoCopy();
	void DoPaste();

	class CNodeInstance* GetNodeFromPinID(unsigned int aID);
	CNodeInstance* GetNodeFromNodeID(unsigned int aID);
	void DrawTypeSpecificPin(struct CPin& aPin, class CNodeInstance* aNodeInstance);
	std::vector<class CNodeInstance*> myNodeInstancesInGraph;

	struct EditorLinkInfo
	{
		ed::LinkId Id;
		ed::PinId  InputId;
		ed::PinId  OutputId;
	};

	ImVector<EditorLinkInfo> myLinks;
	int myNextLinkIdCounter = 100;
	bool myLikeToSave = false;
	bool myLikeToShowFlow = false;
	char* myMenuSeachField = nullptr;
	bool mySetSearchFokus = true;
};
