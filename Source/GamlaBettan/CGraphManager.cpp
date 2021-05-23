#include "pch.h"
#include "CGraphManager.h"

#pragma warning(push, 1)
#pragma warning(disable : 26451 26812 26495)
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#pragma warning(pop)

#include <iostream>
#include <fstream>
#include <sstream>
#include "CNodeInstance.h"
#include "CNodeType.h"
#include <imgui_node_editor.h>
#include "BlueprintUtilities/Include/ax/Drawing.h"
#include "BlueprintUtilities/Include/ax/Widgets.h"
#include "BlueprintUtilities/Include/Interop.h"
#include "NodeTypes.h"
#include <imgui_impl_dx11.h>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include "NodeDataTypes.h"
#include "DebugDrawer.h"
#include <FiskJSON.h>
#include <ClipBoardHelper.h>
#include <unordered_set>
#include "TriggerBoxFactory.h"
#include "NodeOnButtonHit.h"

using namespace rapidjson;
namespace ed = ax::NodeEditor;

using namespace ax::Drawing;
static ed::EditorContext* g_Context = nullptr;
CGraphManager::~CGraphManager()
{
	ed::DestroyEditor(g_Context);
}
void CGraphManager::Load()
{
	ed::Config config;
	config.SettingsFile = "Simple.json";
	g_Context = ed::CreateEditor(&config);
	CNodeTypeCollector::PopulateTypes();
	myMenuSeachField = new char[127];
	memset(&myMenuSeachField[0], 0, sizeof(myMenuSeachField));

}

void CGraphManager::ReTriggerUpdateringTrees()
{
	//Locate start nodes, we support N start nodes, we might want to remove this, as we dont "support" different trees with different starrtnodes to be connected. It might work, might not
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		if (nodeInstance->myNodeType->IsStartNode() && nodeInstance->myShouldTriggerAgain)
		{
			nodeInstance->Enter(-1);
		}
	}
}

void CGraphManager::ReTriggerTree()
{
	//Locate start nodes, we support N start nodes, we might want to remove this, as we dont "support" different trees with different starrtnodes to be connected. It might work, might not
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		if (nodeInstance->myNodeType->IsStartNode() && !nodeInstance->myShouldTriggerAgain)
		{
			nodeInstance->Enter(-1);
		}
	}
}

void CGraphManager::SaveTreeToFile()
{
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

		writer1.StartObject();
		writer1.Key("UID_MAX");

		writer1.StartObject();
		writer1.Key("Num");
		writer1.Int(UID::myGlobalUID);
		writer1.EndObject();

		writer1.Key("NodeInstances");
		writer1.StartArray();
		for (auto& nodeInstance : myNodeInstancesInGraph)
		{
			nodeInstance->Serialize(writer1);
		}
		writer1.EndArray();
		writer1.EndObject();



		std::ofstream of("nodeinstances.json");
		of << s.GetString();
	}
	//Links
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

		writer1.StartObject();
		writer1.Key("Links");
		writer1.StartArray();
		for (auto& link : myLinks)
		{
			writer1.StartObject();
			writer1.Key("ID");
			writer1.Int(CAST(int, link.Id.Get()));
			writer1.Key("Input");
			writer1.Int(CAST(int, link.InputId.Get()));
			writer1.Key("Output");
			writer1.Int(CAST(int, link.OutputId.Get()));
			writer1.EndObject();

		}
		writer1.EndArray();
		writer1.EndObject();


		std::ofstream of("links.json");
		of << s.GetString();
	}

}


template<class T>
void LoadPinTemplate(std::any& aData, rapidjson::Value& someData)
{
	if (someData.Is<T>())
	{
		aData = someData.Get<T>();
	}
}
template<>
void LoadPinTemplate<float>(std::any& aData, rapidjson::Value& someData)
{
	if (someData.IsDouble())
	{
		aData = float(someData.GetDouble());
	}
}
template<>
void LoadPinTemplate<std::string>(std::any& aData, rapidjson::Value& someData)
{
	if (someData.IsString())
	{
		aData = std::string(someData.GetString());
	}
}
template<>
void LoadPinTemplate<EntityType>(std::any& aData, rapidjson::Value& someData)
{
	if (someData.IsNumber())
	{
		aData = static_cast<EntityType>(someData.GetInt());
	}
}

template<>
void LoadPinTemplate<V3F>(std::any& aData, rapidjson::Value& someData)
{
	if (someData.IsArray())
	{
		float buf[3] = { 0.f, 0.f, 0.f };
		const auto& arr = someData.GetArray();
		if (arr.Size() == 3)
		{
			for (size_t i = 0; i < 3; i++)
			{
				const auto& item = arr[i];
				if (item.IsDouble())
				{
					buf[i] = item.GetDouble();
				}
				else if (item.IsNumber())
				{
					buf[i] = float(item.GetInt64());
				}
			}
		}
		aData = V3F(buf[0], buf[1], buf[2]);
	}
}

template<>
void LoadPinTemplate<GameObjectId>(std::any& aData, rapidjson::Value& someData)
{
	aData = GameObjectId();
}
template<>
void LoadPinTemplate<void>(std::any& aData, rapidjson::Value& someData)
{
}



CPin::PinType LoadPinData(std::any& aDataToSet, rapidjson::Value& someData, const std::string& aDataType)
{
	static std::unordered_map < std::string, std::function<void(std::any & aData, rapidjson::Value & someData)>> myConstructorMap;
	if (myConstructorMap.empty())
	{
		myConstructorMap["BOOL"] = LoadPinTemplate<bool>;
		myConstructorMap["FLOAT"] = LoadPinTemplate<float>;
		myConstructorMap["INT"] = LoadPinTemplate<int>;
		myConstructorMap["STRING"] = LoadPinTemplate<std::string>;
		myConstructorMap["ENTITYTYPE"] = LoadPinTemplate<EntityType>;
		myConstructorMap["ENTITYID"] = LoadPinTemplate<GameObjectId>;
		myConstructorMap["VECTOR3"] = LoadPinTemplate<V3F>;
		myConstructorMap["NOTHING"] = LoadPinTemplate<void>;
		myConstructorMap["UNKOWN"] = LoadPinTemplate<void>;
	}
	if (myConstructorMap.count(aDataType) != 0)
	{
		myConstructorMap[aDataType](aDataToSet, someData);
		return CPin::PinType::Data;
	}
	else
	{
		SYSERROR("Could not load pin of type", aDataType);
	}

	return CPin::PinType::Unknown;
}

void CGraphManager::LoadTreeFromFile()
{
#if 0
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		delete nodeInstance;
	}

	myLinks.clear();
	myNodeInstancesInGraph.clear();
	UID::myAllUIDs.clear();
	UID::myGlobalUID = 0;

	{
		std::ifstream inputFile("nodeinstances.json");
		std::stringstream jsonDocumentBuffer;
		std::string inputLine;

		while (std::getline(inputFile, inputLine))
		{
			jsonDocumentBuffer << inputLine << "\n";
		}
		rapidjson::Document document;
		document.Parse(jsonDocumentBuffer.str().c_str());

		rapidjson::Value& uidmax = document["UID_MAX"];
		int test = uidmax["Num"].GetInt();
		UID::myGlobalUID = test;

		rapidjson::Value& results = document["NodeInstances"];

		for (rapidjson::SizeType i = 0; i < results.Size(); i++)
		{
			rapidjson::Value& nodeInstance = results[i];
			CNodeInstance* object = new CNodeInstance(false);
			int nodeType = nodeInstance["NodeType"].GetInt();
			int UID = nodeInstance["UID"].GetInt();
			object->myUID = UID;
			object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeType);

			object->myEditorPos[0] = CAST(float, nodeInstance["Position"]["X"].GetInt());
			object->myEditorPos[1] = CAST(float, nodeInstance["Position"]["Y"].GetInt());

			object->ConstructUniquePins();

			bool skipObject = false;
			for (int i = 0; i < CAST(int, nodeInstance["Pins"].Size()); i++)
			{
				int index = nodeInstance["Pins"][i]["Index"].GetInt();
				object->myPins[index].myUID.SetUID(nodeInstance["Pins"][i]["UID"].GetInt());
				CPin::PinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][i]["DATA"], nodeInstance["Pins"][i]["DATA_TYPE"].GetString());
				if (object->myPins[index].myVariableType == CPin::PinType::Unknown)
				{
					object->ChangePinTypes(newType);
				}
			}

			if (!skipObject)
			{
				myNodeInstancesInGraph.push_back(object);
			}
		}
	}
	{
		std::ifstream inputFile("links.json");
		std::stringstream jsonDocumentBuffer;
		std::string inputLine;

		while (std::getline(inputFile, inputLine))
		{
			jsonDocumentBuffer << inputLine << "\n";
		}
		rapidjson::Document document;
		document.Parse(jsonDocumentBuffer.str().c_str());

		rapidjson::Value& results = document["Links"];

		myNextLinkIdCounter = 0;
		for (rapidjson::SizeType i = 0; i < document["Links"].Size(); i++)
		{
			int id = document["Links"][i]["ID"].GetInt();
			int inputID = document["Links"][i]["Input"].GetInt();
			int Output = document["Links"][i]["Output"].GetInt();

			CNodeInstance* firstNode = GetNodeFromPinID(inputID);
			CNodeInstance* secondNode = GetNodeFromPinID(Output);

			if (firstNode && secondNode)
			{
				firstNode->AddLinkToVia(secondNode, inputID, Output, id);
				secondNode->AddLinkToVia(firstNode, Output, inputID, id);
			}
			else
			{
				SYSERROR("could not load node tree", "");
				return;
			}

			myLinks.push_back({ ed::LinkId(id), ed::PinId(inputID), ed::PinId(Output) });
			if (myNextLinkIdCounter < id + 1)
			{
				myNextLinkIdCounter = id + 1;
			}

		}
	}
#endif
}

std::vector<int> myFlowsToBeShown;
void CGraphManager::ShowFlow(int aLinkID)
{
	myFlowsToBeShown.push_back(aLinkID);
}


ImColor GetIconColor(CPin::PinType type, const std::any& aData)
{
	static std::unordered_map<std::type_index, ImColor> typeColors;
	if (typeColors.empty())
	{
		typeColors[typeid(bool)] = ImColor(220, 48, 48);
		typeColors[typeid(int)] = ImColor(68, 201, 156);
		typeColors[typeid(float)] = ImColor(147, 226, 74);
		typeColors[typeid(std::string)] = ImColor(124, 21, 153);
		typeColors[typeid(GameObjectId)] = ImColor(24, 200, 42);
		typeColors[typeid(V3F)] = ImColor(212, 212, 21);
		typeColors[typeid(Logger::Type)] = ImColor(31, 21, 203);
	}

	if (type == CPin::PinType::Flow)
	{
		return ImColor(255, 255, 255);
	}

	return typeColors[aData.type()];
};

void DrawPinIcon(const CPin& pin, bool connected, int alpha)
{
	IconType iconType;
	ImColor  color = GetIconColor(pin.myVariableType, pin.myData);
	color.Value.w = alpha / 255.0f;
	switch (pin.myVariableType)
	{
	case CPin::PinType::Flow:     iconType = IconType::Flow;   break;
	case  CPin::PinType::Data:    iconType = IconType::Circle; break;
	case CPin::PinType::Unknown:  iconType = IconType::Circle; break;
	default:
		return;
	}
	const int s_PinIconSize = 24;
	ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};

CNodeInstance* CGraphManager::GetNodeFromNodeID(unsigned int aID)
{
	auto it = myNodeInstancesInGraph.begin();
	while (it != myNodeInstancesInGraph.end())
	{
		if ((*it)->myUID.AsInt() == aID)
		{
			return *it;
		}
		else
		{
			++it;
		}
	}

	return nullptr;

}


CNodeInstance* CGraphManager::GetNodeFromPinID(unsigned int aID)
{
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		std::vector<CPin>& pins = nodeInstance->GetPins();

		for (auto& pin : pins)
		{
			if (pin.myUID.AsInt() == aID)
			{
				return nodeInstance;
			}
		}

	}
	return nullptr;
}

void DrawString(CPin& aPin, CNodeInstance* aNodeInstance)
{
	std::string data = std::any_cast<std::string>(aPin.myData);
	char* string = new char[data.size()+32];
	strcpy_s(string, data.size() + 32, data.c_str());

	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		if (ImGui::InputText("##edit", string, data.size()+30))
		{
			aPin.myData = std::string(string);
		}
	}
	ImGui::PopItemWidth();

	ImGui::PopID();

	delete string;
}

void DrawInt(CPin& aPin, CNodeInstance* aNodeInstance)
{
	int data = std::any_cast<int>(aPin.myData);

	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		if (ImGui::InputInt("##edit", &data))
		{
			aPin.myData = data;
		}
	}
	ImGui::PopItemWidth();

	ImGui::PopID();
}

void DrawBool(CPin& aPin, CNodeInstance* aNodeInstance)
{
	bool data = std::any_cast<bool>(aPin.myData);
	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		if (ImGui::Checkbox("##edit", &data))
		{
			aPin.myData = data;
		}
	}
	ImGui::PopItemWidth();

	ImGui::PopID();
}

void DrawFloat(CPin& aPin, CNodeInstance* aNodeInstance)
{
	float data = std::any_cast<float>(aPin.myData);
	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		if (ImGui::InputFloat("##edit", &data))
		{
			aPin.myData = data;
		}
	}
	ImGui::PopItemWidth();

	ImGui::PopID();
}
void DrawVector3(CPin& aPin, CNodeInstance* aNodeInstance)
{
	V3F data = std::any_cast<V3F>(aPin.myData);

	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
		if (ImGui::IsItemHovered())
		{
			ImVec4 col = GetIconColor(aPin.myVariableType, aPin.myData);
			DebugDrawer::GetInstance().SetColor(V4F(col.x, col.y, col.z, col.w));
			DebugDrawer::GetInstance().DrawCross(data, 100);
		}
	}
	else
	{
		if (ImGui::InputFloat3("##edit", &data.x))
		{
			aPin.myData = data;
		}
		if (ImGui::IsItemHovered())
		{
			ImVec4 col = GetIconColor(aPin.myVariableType, aPin.myData);
			DebugDrawer::GetInstance().SetColor(V4F(col.x, col.y, col.z, col.w));
			DebugDrawer::GetInstance().DrawCross(data, 100);
		}
	}
	ImGui::PopItemWidth();

	ImGui::PopID();
}

void DrawUneditable(CPin& aPin, CNodeInstance* aNodeInstance)
{
	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	DrawPinIcon(aPin, aNodeInstance->IsPinConnected(aPin), 255);
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void DrawEntityType(CPin& aPin, CNodeInstance* aNodeInstance)
{
	EntityType data = std::any_cast<EntityType>(aPin.myData);
	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		static const char* names[] =
		{
			"Player"				,
			"Enemy"					,
			"Projectile"			,
			"EnvironmentStatic"		,
			"EnvironmentDynamic"	,
			"EnvironmentDestructible",
			"TriggerBox"			,
			"Pickups"				,
			"BreakableContainer"	,
			"UnbreakableContainer"	,
			"LevelGate"				,
			"Ability",
			"AudioOneShot"			,
			"Camera"
		};
		static_assert(static_cast<int>(EntityType::Count) == 14, "Add/Remove name and update counter");

		if (ImGui::Button("<"))
		{
			if (static_cast<int>(data) != 0)
			{
				aPin.myData = static_cast<EntityType>(static_cast<int>(data) - 1);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(">"))
		{
			if (static_cast<int>(data) + 1 != static_cast<int>(EntityType::Count))
			{
				aPin.myData = static_cast<EntityType>(static_cast<int>(data) + 1);
			}
		}
		ImGui::SameLine();
		ImGui::Text(data != EntityType::None ? names[static_cast<int>(data)] : "None");
	}
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void DrawLoggerType(CPin& aPin, CNodeInstance* aNodeInstance)
{
	Logger::Type data = std::any_cast<Logger::Type>(aPin.myData);
	if (data == Logger::None)
	{
		data = Logger::Info;
		aPin.myData = data;
	}
	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		static std::unordered_map<LOGGERTYPE, const char*> names
		{
			{Logger::Info,"Info"},
			{Logger::Warning,"Warning"},
			{Logger::Error,"Error"}
		};

		if (ImGui::Button("<"))
		{
			switch (data)
			{
			case Logger::Warning:
				aPin.myData = Logger::Info;
				break;
			case Logger::Error:
				aPin.myData = Logger::Warning;
				break;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(">"))
		{
			switch (data)
			{
			case Logger::Info:
				aPin.myData = Logger::Warning;
				break;
			case Logger::Warning:
				aPin.myData = Logger::Error;
				break;
			}
		}
		ImGui::SameLine();
		if (names[static_cast<LOGGERTYPE>(data)])
		{
			ImGui::Text(names[static_cast<LOGGERTYPE>(data)]);
		}
		else
		{
			ImGui::Text("LoggerType is not supported: %llu", static_cast<unsigned long long>(data));
		}
	}
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void DrawButtonInput(CPin& aPin, CNodeInstance* aNodeInstance)
{
	static short data = 0;
	ImGui::PushID(aPin.myUID.AsInt());
	ImGui::PushItemWidth(100.0f);
	if (aNodeInstance->IsPinConnected(aPin))
	{
		DrawPinIcon(aPin, true, 255);
	}
	else
	{
		static std::pair<std::string, ButtonInput> buttons[] =
		{
			{ "None", ButtonInput::None },
			{ "Reload", ButtonInput::Reload },
			{ "Jump", ButtonInput::Jump },
			{ "Pause", ButtonInput::Pause },
			{ "Unpause", ButtonInput::Unpause },
			{ "Crouch", ButtonInput::Crouch },
			{ "Run", ButtonInput::Run },
			{ "LeftMouseBtn", ButtonInput::LeftMouse },
			{ "RightMouseBtn", ButtonInput::RightMouse },
			{ "Interact", ButtonInput::Interact }
		};

		const short btnSize = 10;

		
		static_assert(static_cast<int>(ButtonInput::None) == 38, "Add/Remove name and update counter");

		if (ImGui::Button("<"))
		{
			if (data != 0)
			{
				aPin.myData = buttons[--data].second;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(">"))
		{
			if (data + 1 != btnSize)
			{
				aPin.myData = buttons[++data].second;
			}
		}
		ImGui::SameLine();
		ImGui::Text(buttons[data].first.c_str());
	}
	ImGui::PopItemWidth();
	ImGui::PopID();
}

void CGraphManager::DrawTypeSpecificPin(CPin& aPin, CNodeInstance* aNodeInstance)
{
	static std::unordered_map < std::type_index, std::function<void(CPin&, CNodeInstance*)>> drawMapping;
	if (drawMapping.empty())
	{
		drawMapping[std::any().type()] = [](CPin& aPin, CNodeInstance* aNodeInstance) {/* NOOP */};
		drawMapping[typeid(std::string)] = DrawString;
		drawMapping[typeid(int)] = DrawInt;
		drawMapping[typeid(bool)] = DrawBool;
		drawMapping[typeid(float)] = DrawFloat;
		drawMapping[typeid(V3F)] = DrawVector3;
		drawMapping[typeid(GameObjectId)] = DrawUneditable;
		drawMapping[typeid(EntityType)] = DrawEntityType;
		drawMapping[typeid(Logger::Type)] = DrawLoggerType;
		drawMapping[typeid(TimerId)] = DrawUneditable;
		drawMapping[typeid(ButtonInput)] = DrawButtonInput;
	}

	if (drawMapping.count(aPin.myData.type()) != 0)
	{
		drawMapping[aPin.myData.type()](aPin, aNodeInstance);
	}
	else
	{
		SYSERROR("Does not support type", std::string(aPin.myData.type().name()));
	}
}

void CGraphManager::WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* aSecond, bool& aIsCyclic, CNodeInstance* aBase)
{
	if (aIsCyclic)
	{
		return;
	}

	std::vector<CPin>& pins = aFirst->GetPins();
	for (auto& pin : pins)
	{
		if (pin.myPinType == CPin::PinTypeInOut::PinTypeInOut_OUT)
		{
			std::vector<SNodeInstanceLink*> links = aFirst->GetLinksFromPin(pin.myUID.AsInt());
			for (auto& link : links)
			{
				CNodeInstance* connectedNodeToOutPut = GetNodeFromPinID(link->myToPinID);

				if (connectedNodeToOutPut == aBase)
				{
					aIsCyclic |= true;
					return;
				}
				WillBeCyclic(connectedNodeToOutPut, nullptr, aIsCyclic, aBase);
			}
		}
	}
}

void CGraphManager::PreFrame(float aTimeDelta)
{
	static float timer = 0;
	timer += aTimeDelta;
	auto& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize({ io.DisplaySize.x / 2,  io.DisplaySize.y });
	ImGui::Begin("Content", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus);

	if (ImGui::Button("Retrigger"))
	{
		ReTriggerTree();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		myLikeToSave = true;

	}
	static bool showFlow = false;
	if (ImGui::Checkbox("Show Flow", &showFlow))
	{
		myLikeToShowFlow = showFlow;

	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		LoadTreeFromFile();
		ReTriggerTree();
	}
	static bool drawBoxes = false;
	ImGui::Checkbox("Draw Triggerboxes", &drawBoxes);
	if (drawBoxes)
	{
		TriggerBoxFactory::DrawTriggerBoxes();
	}


	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		nodeInstance->DebugUpdate();
		nodeInstance->VisualUpdate(aTimeDelta);
	}

	static int outRate = 30;
	ImGui::SliderInt("Max framerate", &outRate, 0, 100);
	if (timer > (1.0f / (float)outRate))
	{
		ReTriggerUpdateringTrees();
		timer = 0;
	}


	ed::SetCurrentEditor(g_Context);
	ed::Begin("My Editor", ImVec2(0.0, 0.0f));
}

bool ArePinTypesCompatible(CPin& aFirst, CPin& aSecond)
{
	if (aSecond.myVariableType != aFirst.myVariableType)
	{
		return false;
	}
	return true;
}


struct SDistBestResult
{
	size_t myScore;
	CNodeType* myInstance;
};

struct less_than_key
{
	inline bool operator() (const SDistBestResult& struct1, const SDistBestResult& struct2)
	{
		return (struct1.myScore < struct2.myScore);
	}
};

template<typename T>
size_t uiLevenshteinDistance(const T& source, const T& target)
{
	if (source.size() > target.size()) {
		return uiLevenshteinDistance(target, source);
	}

	using TSizeType = typename T::size_type;
	const TSizeType min_size = source.size(), max_size = target.size();
	std::vector<TSizeType> lev_dist(min_size + 1);

	for (TSizeType i = 0; i <= min_size; ++i) {
		lev_dist[i] = i;
	}

	for (TSizeType j = 1; j <= max_size; ++j) {
		TSizeType previous_diagonal = lev_dist[0], previous_diagonal_save;
		++lev_dist[0];

		for (TSizeType i = 1; i <= min_size; ++i) {
			previous_diagonal_save = lev_dist[i];
			if (source[i - 1] == target[j - 1]) {
				lev_dist[i] = previous_diagonal;
			}
			else {
				lev_dist[i] = MIN(MIN(lev_dist[i - 1], lev_dist[i]), previous_diagonal) + 1;
			}
			previous_diagonal = previous_diagonal_save;
		}
	}

	return lev_dist[min_size];
}

void CGraphManager::ConstructEditorTreeAndConnectLinks()
{
	for (auto& nodeInstance : myNodeInstancesInGraph)
	{
		if (!nodeInstance->myHasSetEditorPos)
		{
			ed::SetNodePosition(nodeInstance->myUID.AsInt(), ImVec2(nodeInstance->myEditorPos[0], nodeInstance->myEditorPos[1]));
			nodeInstance->myHasSetEditorPos = true;
		}

		// Start drawing nodes.
		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
		ed::BeginNode(nodeInstance->myUID.AsInt());
		ImGui::PushID(nodeInstance->myUID.AsInt());
		ImGui::BeginVertical("node");

		ImGui::BeginHorizontal("header");
		ImGui::Spring(0);
		ImGui::TextUnformatted(nodeInstance->GetNodeName().c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		ImGui::Spring(0);

		ImGui::EndHorizontal();
		ax::rect HeaderRect = ImGui_GetItemRect();
		ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);


		bool previusWasOut = false;
		bool isFirstInput = true;
		bool isFirstIteration = true;
		for (auto& pin : nodeInstance->GetPins())
		{
			if (isFirstIteration)
			{
				if (pin.myPinType == CPin::PinTypeInOut::PinTypeInOut_OUT)
				{
					isFirstInput = false;
				}
				isFirstIteration = false;
			}
			if (pin.myPinType == CPin::PinTypeInOut::PinTypeInOut_IN)
			{
				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Input);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);
				if (pin.myVariableType == CPin::PinType::Flow)
				{
					DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				}
				else
				{
					DrawTypeSpecificPin(pin, nodeInstance);
				}


				ed::EndPin();
				previusWasOut = false;

			}
			else
			{
				if (isFirstInput)
				{
					ImGui::SameLine(100, 0);
				}

				ImGui::Indent(150.0f);


				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Output);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);

				DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				ed::EndPin();
				previusWasOut = true;
				ImGui::Unindent(150.0f);
				isFirstInput = false;
			}
		}

		ImGui::EndVertical();
		auto ContentRect = ImGui_GetItemRect();
		ed::EndNode();

		if (ImGui::IsItemVisible())
		{
			auto drawList = ed::GetNodeBackgroundDrawList(nodeInstance->myUID.AsInt());

			const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;
			auto headerColor = nodeInstance->GetColor();
			static ImTextureID HeaderTextureId = ImGui_LoadTexture("NodeImages/BlueprintBackground.png");
			const auto uv = ImVec2(
				HeaderRect.w / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureId)),
				HeaderRect.h / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureId)));

			drawList->AddImageRounded(HeaderTextureId,
				to_imvec(HeaderRect.top_left()) - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
				to_imvec(HeaderRect.bottom_right()) + ImVec2(8 - halfBorderWidth, 0),
				ImVec2(0.0f, 0.0f), uv,
				headerColor, ed::GetStyle().NodeRounding, 1 | 2);


			auto headerSeparatorRect = ax::rect(HeaderRect.bottom_left(), ContentRect.top_right());
			drawList->AddLine(
				to_imvec(headerSeparatorRect.top_left()) + ImVec2(-(8 - halfBorderWidth), -0.5f),
				to_imvec(headerSeparatorRect.top_right()) + ImVec2((8 - halfBorderWidth), -0.5f),
				ImColor(255, 255, 255, 255), 1.0f);
		}
		ImGui::PopID();
		ed::PopStyleVar();
	}


	for (auto& linkInfo : myLinks)
		ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

	// Handle creation action, returns true if editor want to create new object (node or link)
	if (ed::BeginCreate())
	{
		ed::PinId inputPinId, outputPinId;
		if (ed::QueryNewLink(&inputPinId, &outputPinId))
		{
			if (inputPinId && outputPinId)
			{
				if (ed::AcceptNewItem())
				{
					CNodeInstance* firstNode = GetNodeFromPinID(CAST(unsigned int, inputPinId.Get()));
					CNodeInstance* secondNode = GetNodeFromPinID(CAST(unsigned int, outputPinId.Get()));
					assert(firstNode);
					assert(secondNode);

					if (firstNode == secondNode)
					{
						// User trying connect input and output on the same node :/, who does this!?!
						// SetBlueScreenOnUserComputer(true)
					}
					else
					{
						{
							CPin* firstPin = firstNode->GetPinFromID(CAST(unsigned int, inputPinId.Get()));
							CPin* secondPin = secondNode->GetPinFromID(CAST(unsigned int, outputPinId.Get()));

							bool canAddlink = true;
							if (firstPin && secondPin)
							{
								if (firstPin->myPinType == CPin::PinTypeInOut::PinTypeInOut_IN && secondPin->myPinType == CPin::PinTypeInOut::PinTypeInOut_IN)
								{
									canAddlink = false;
								}
							}

							if (!ArePinTypesCompatible(*firstPin, *secondPin))
							{
								canAddlink = false;
							}

							if (!firstNode->CanAddLink(CAST(unsigned int, inputPinId.Get())))
							{
								canAddlink = false;
							}
							if (!secondNode->CanAddLink(CAST(unsigned int, outputPinId.Get())))
							{
								canAddlink = false;
							}
							if (canAddlink)
							{
								if (secondPin->myVariableType == CPin::PinType::Unknown)
								{
									secondNode->ChangePinTypes(firstPin->myVariableType);
								}
								int linkId = ++myNextLinkIdCounter;
								firstNode->AddLinkToVia(secondNode, CAST(unsigned int, inputPinId.Get()), CAST(unsigned int, outputPinId.Get()), linkId);
								secondNode->AddLinkToVia(firstNode, CAST(unsigned int, outputPinId.Get()), CAST(unsigned int, inputPinId.Get()), linkId);

								bool aIsCyclic = false;
								WillBeCyclic(firstNode, secondNode, aIsCyclic, firstNode);
								if (aIsCyclic || !canAddlink)
								{
									firstNode->RemoveLinkToVia(secondNode, CAST(unsigned int, inputPinId.Get()));
									secondNode->RemoveLinkToVia(firstNode, CAST(unsigned int, outputPinId.Get()));
								}
								else
								{
									myLinks.push_back({ ed::LinkId(linkId), inputPinId, outputPinId });
								}
							}
						}
					}
				}
			}
		}
	}
	ed::EndCreate();

	// Handle deletion action
	if (ed::BeginDelete())
	{
		// There may be many links marked for deletion, let's loop over them.
		ed::LinkId deletedLinkId;
		while (ed::QueryDeletedLink(&deletedLinkId))
		{
			// If you agree that link can be deleted, accept deletion.
			if (ed::AcceptDeletedItem())
			{
				// Then remove link from your data.
				for (auto& link : myLinks)
				{
					if (link.Id == deletedLinkId)
					{
						CNodeInstance* firstNode = GetNodeFromPinID(CAST(unsigned int, link.InputId.Get()));
						CNodeInstance* secondNode = GetNodeFromPinID(CAST(unsigned int, link.OutputId.Get()));
						assert(firstNode);
						assert(secondNode);

						firstNode->RemoveLinkToVia(secondNode, CAST(unsigned int, link.InputId.Get()));
						secondNode->RemoveLinkToVia(firstNode, CAST(unsigned int, link.OutputId.Get()));

						myLinks.erase(&link);

						break;
					}
				}
			}
		}
		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{

				auto it = myNodeInstancesInGraph.begin();
				while (it != myNodeInstancesInGraph.end())
				{
					if ((*it)->myUID.AsInt() == nodeId.Get())
					{
						it = myNodeInstancesInGraph.erase(it);
					}
					else
					{
						++it;
					}
				}

			}
		}
	}
	ed::EndDelete();

	auto openPopupPosition = ImGui::GetMousePos();
	static ImVec2 mousePosAtPopupStart;
	ed::Suspend();

	if (ed::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		mousePosAtPopupStart = openPopupPosition;
	}
	ed::Resume();

	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if (ImGui::BeginPopup("Create New Node"))
	{

		auto newNodePostion = openPopupPosition;
		CNodeType** types = CNodeTypeCollector::GetAllNodeTypes();
		unsigned short noOfTypes = CNodeTypeCollector::GetNodeTypeCount();

		std::map< std::string, std::vector<CNodeType*>> cats;

		for (int i = 0; i < noOfTypes; i++)
		{
			cats[types[i]->GetNodeTypeCategory()].push_back(types[i]);
		}



		ImGui::PushItemWidth(100.0f);
		ImGui::InputText("##edit", (char*)myMenuSeachField, 127);
		if (mySetSearchFokus)
		{
			ImGui::SetKeyboardFocusHere(0);
		}
		mySetSearchFokus = false;
		ImGui::PopItemWidth();

		CNodeType* typeOfNewNode = nullptr;

		if (myMenuSeachField[0] != '\0')
		{

			std::vector<SDistBestResult> distanceResults;
			for (int i = 0; i < noOfTypes; i++)
			{
				distanceResults.push_back(SDistBestResult());
				SDistBestResult& dist = distanceResults.back();
				dist.myInstance = types[i];
				dist.myScore = uiLevenshteinDistance<std::string>(types[i]->GetNodeName(), myMenuSeachField);
			}

			std::sort(distanceResults.begin(), distanceResults.end(), less_than_key());

			int firstCost = CAST(int, distanceResults[0].myScore);
			for (int i = 0; i < distanceResults.size(); i++)
			{
				if (ImGui::MenuItem(distanceResults[i].myInstance->GetNodeName().c_str()))
				{
					typeOfNewNode = distanceResults[i].myInstance;
				}
				int distance = CAST(int, distanceResults[i].myScore) - firstCost;
				if (distance > 3)
				{
					break;
				}
			}
		}
		else
		{
			for (auto& category : cats)
			{
				std::string theCatName = category.first;
				if (theCatName.empty())
				{
					theCatName = "General";
				}

				if (ImGui::BeginMenu(theCatName.c_str()))
				{
					for (int i = 0; i < category.second.size(); i++)
					{
						CNodeType* type = category.second[i];
						if (ImGui::MenuItem(type->GetNodeName().c_str()))
						{
							typeOfNewNode = type;
						}
					}
					ImGui::EndMenu();
				}
			}
		}
		if (typeOfNewNode)
		{
			CNodeInstance* node = new CNodeInstance();

			node->myNodeType = typeOfNewNode;
			node->ConstructUniquePins();
			ed::SetNodePosition(node->myUID.AsInt(), mousePosAtPopupStart);
			node->myHasSetEditorPos = true;
			LOGINFO("Created Node of type [" + node->myNodeType->mySemanticName + "]");

			myNodeInstancesInGraph.push_back(node);
		}
		ImGui::EndPopup();
	}
	else
	{
		mySetSearchFokus = true;
		memset(&myMenuSeachField[0], 0, sizeof(myMenuSeachField));
	}


	ImGui::PopStyleVar();
	ed::Resume();
	DoCopy();
	DoPaste();


}

void CGraphManager::PostFrame()
{
	if (myLikeToSave)
	{
		myLikeToSave = false;
		SaveTreeToFile();
	}
	if (myLikeToShowFlow)
	{
		for (int i = 0; i < myLinks.size(); i++)
		{
			ed::Flow(myLinks[i].Id);
		}
	}
	for (auto i : myFlowsToBeShown)
	{
		ed::Flow(i);
	}
	myFlowsToBeShown.clear();
	ed::End();
	ed::SetCurrentEditor(nullptr);
	ImGui::End();
}

void CGraphManager::DoCopy()
{
	static bool HasCopied = false;
	bool ctrlDown = GetAsyncKeyState(VK_CONTROL);
	if (GetAsyncKeyState('C') && ctrlDown)
	{
		if (!HasCopied)
		{
			HasCopied = true;
			const int bufferSize = 256;
			ed::NodeId buffer[bufferSize];
			int count = ed::GetSelectedNodes(buffer, bufferSize);

			if (count == 0)
			{
				return;
			}

			std::unordered_map<int, size_t> selectedNodes;

			//buffer[0].AsPointer()

			FiskJSON::Object root;
			FiskJSON::Object* nodes = new FiskJSON::Object();
			nodes->MakeArray();
			for (size_t i = 0; i < count; i++)
			{
				FiskJSON::Object* jsonNode = new FiskJSON::Object();
				CNodeInstance* inst = GetNodeFromNodeID(buffer[i].Get());
				selectedNodes.emplace(inst->myUID.AsInt(), i);

				int id = inst->myUID.AsInt();

				FiskJSON::Object* JsonId = new FiskJSON::Object();
				*JsonId = id;
				jsonNode->AddChild("id", JsonId);
				nodes->PushChild(jsonNode);

				FiskJSON::Object* JsonPos = new FiskJSON::Object();
				{
					FiskJSON::Object* JsonX = new FiskJSON::Object();
					*JsonX = inst->myEditorPos[0];
					JsonPos->AddChild("x", JsonX);

					FiskJSON::Object* JsonY = new FiskJSON::Object();
					*JsonY = inst->myEditorPos[1];
					JsonPos->AddChild("y", JsonY);
				}
				jsonNode->AddChild("Pos", JsonPos);

				FiskJSON::Object* JsonType = new FiskJSON::Object();
				*JsonType = inst->myNodeType->mySemanticName;
				jsonNode->AddChild("type", JsonType);

				FiskJSON::Object* JsonPinData = new FiskJSON::Object();
				JsonPinData->MakeArray();

				auto pins = inst->GetPins();
				for (size_t j = 0; j < pins.size(); j++)
				{
					FiskJSON::Object* JsonPinContent = new FiskJSON::Object();
					{
						FiskJSON::Object* JsonPinType = new FiskJSON::Object();
						*JsonPinType = inst->WriteVariableType(pins[j]);
						JsonPinContent->AddChild("DATA_TYPE", JsonPinType);

						rapidjson::StringBuffer s;
						rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);
						inst->WritePinValue(writer1, pins[j]);
						FiskJSON::Object* JsonPinData = new FiskJSON::Object();
						JsonPinData->Parse(s.GetString());
						JsonPinContent->AddChild("DATA", JsonPinData);
						JsonPinContent->AddValueChild("Index", int(j));
					}
					JsonPinData->PushChild(JsonPinContent);
				}
				jsonNode->AddChild("PinData", JsonPinData);
			}
			root.AddChild("Nodes", nodes);
			FiskJSON::Object* links = new FiskJSON::Object();
			links->MakeArray();
			for (size_t j = 0; j < count; j++)
			{
				CNodeInstance* inst = GetNodeFromNodeID(buffer[j].Get());
				for (auto& li : inst->GetLinks())
				{
					CNodeInstance* first = GetNodeFromPinID(li.myFromPinID);
					CNodeInstance* second = GetNodeFromPinID(li.myToPinID);
					auto firstFind = selectedNodes.find(first->myUID.AsInt());
					auto secondFind = selectedNodes.find(second->myUID.AsInt());
					if (firstFind != selectedNodes.end() && secondFind != selectedNodes.end())
					{
						int firstIndex = first->GetPinIndexFromPinUID(li.myFromPinID);
						int secondIndex = second->GetPinIndexFromPinUID(li.myToPinID);
						FiskJSON::Object* jsonLink = new FiskJSON::Object();
						jsonLink->AddValueChild("startPinIndex", firstIndex);
						jsonLink->AddValueChild("endPinIndex", secondIndex);

						jsonLink->AddValueChild("startNodeIndex", firstFind->second);
						jsonLink->AddValueChild("endNodeIndex", secondFind->second);

						links->PushChild(jsonLink);
					}
				}
			}

			root.AddChild("Links", links);
			Tools::WriteToClipboard(root.Serialize(false));
		}
	}
	else
	{
		HasCopied = false;
	}
}
void CGraphManager::DoPaste()
{
	static bool HasPasted = false;
	bool ctrlDown = GetAsyncKeyState(VK_CONTROL);
	if (GetAsyncKeyState('V') && ctrlDown)
	{
		if (!HasPasted)
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			ed::ClearSelection();
			HasPasted = true;
			std::string data = Tools::ReadFromClipboard();
			FiskJSON::Object root;
			try
			{
				std::vector<CNodeInstance*> nodes;
				root.Parse(data);
				ImVec2 orig;
				orig.x = _HUGE_ENUF;
				orig.y = _HUGE_ENUF;
				for (auto& i : root["Nodes"].Get<FiskJSON::Array>())
				{
					float x, y;
					if ((*i)["Pos"]["x"].GetIf(x) && (*i)["Pos"]["y"].GetIf(y))
					{
						orig.x = MIN(orig.x, x);
						orig.y = MIN(orig.y, y);
					}
				}
				for (auto& i : root["Nodes"].Get<FiskJSON::Array>())
				{
					CNodeType* typeOfNewNode = nullptr;
					std::string typeClassName;
					if ((*i)["type"].GetIf(typeClassName))
					{
						CNodeType** types = CNodeTypeCollector::GetAllNodeTypes();
						unsigned short noOfTypes = CNodeTypeCollector::GetNodeTypeCount();

						for (int j = 0; j < noOfTypes; j++)
						{
							if (types[j]->mySemanticName == typeClassName)
							{
								typeOfNewNode = types[j];
							}
						}
					}


					if (typeOfNewNode)
					{

						ImVec2 pos;
						if ((*i)["Pos"]["x"].GetIf(pos.x) && (*i)["Pos"]["y"].GetIf(pos.y))
						{
							CNodeInstance* node = new CNodeInstance();

							node->myNodeType = typeOfNewNode;
							node->ConstructUniquePins();


							rapidjson::Document doc;
							doc.Parse(i->Serialize(false).c_str());

							for (int j = 0; j < CAST(int, doc["PinData"].Size()); j++)
							{
								int index = doc["PinData"][j]["Index"].GetInt();
								CPin::PinType newType = LoadPinData(node->myPins[index].myData, doc["PinData"][j]["DATA"], doc["PinData"][j]["DATA_TYPE"].GetString());
							}


							ImVec2 edpos = pos - orig + mousePos;
							ed::SetNodePosition(node->myUID.AsInt(), edpos);
							node->myEditorPos[0] = edpos.x;
							node->myEditorPos[1] = edpos.y;
							node->myHasSetEditorPos = true;
							LOGINFO("Created Node of type [" + node->myNodeType->mySemanticName + "]");

							myNodeInstancesInGraph.push_back(node);
							nodes.push_back(node);
							ed::SelectNode(node->myUID.AsInt(), true);
						}
					}
				}

				for (auto& i : root["Links"].Get<FiskJSON::Array>())
				{
					int startNodeIndex, endNodeIndex, startPinIndex, endPinIndex;
					if ((*i)["startPinIndex"].GetIf(startPinIndex) &&
						(*i)["endPinIndex"].GetIf(endPinIndex) &&
						(*i)["startNodeIndex"].GetIf(startNodeIndex) &&
						(*i)["endNodeIndex"].GetIf(endNodeIndex))
					{
						int id = ++myNextLinkIdCounter;
						nodes[startNodeIndex]->AddLinkToVia(nodes[endNodeIndex], nodes[startNodeIndex]->myPins[startPinIndex].myUID.AsInt(), nodes[endNodeIndex]->myPins[endPinIndex].myUID.AsInt(), id);
						nodes[endNodeIndex]->AddLinkToVia(nodes[startNodeIndex], nodes[endNodeIndex]->myPins[endPinIndex].myUID.AsInt(), nodes[startNodeIndex]->myPins[startPinIndex].myUID.AsInt(), id);

						myLinks.push_back({ ed::LinkId(id), nodes[startNodeIndex]->myPins[startPinIndex].myUID.AsInt(), nodes[endNodeIndex]->myPins[endPinIndex].myUID.AsInt() });
					}
				}
			}
			catch (const FiskJSON::Invalid_JSON & e)
			{
				LOGERROR("Paste action invalid, reason: [" + std::string(e.what()) + "]");
			}
		}
	}
	else
	{
		HasPasted = false;
	}
}