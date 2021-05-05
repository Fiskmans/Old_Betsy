#include <pch.h>

#if !BAKEVALUES


#include "MetricValue.h"
#include <fstream>
#include <imgui.h>
#include "../GamlaBettan/WindowControl.h"


std::vector<std::pair<std::string, SerializableMetricValue*>> MetricHandler::myListeners;
std::unordered_map<std::string, std::string> MetricHandler::myLoadedMetrics;


SerializableMetricValue::SerializableMetricValue()
{
}

void SerializableMetricValue::Setup(const std::string& aLookup)
{
	MetricHandler::StartListening(aLookup, this);
}

SerializableMetricValue::SerializableMetricValue(const SerializableMetricValue& aOther)
{
	MetricHandler::CopySubscription(&aOther, this);
}

SerializableMetricValue::~SerializableMetricValue()
{
	MetricHandler::StopListening(this);
}

void MetricHandler::Load(std::string aFilePath)
{
	std::ifstream stream;
	stream.open(aFilePath);
	std::string define;
	std::string lookup;
	std::string value;

	if (stream)
	{
		while (stream >> define >> lookup >> value)
		{
			myLoadedMetrics[lookup] = value;
			for (auto& i : myListeners)
			{
				if (i.first == lookup)
				{
					i.second->DeSerialize(value);
				}
			}
		}
	}
}

void MetricHandler::Save(std::string aFilePath)
{
	std::ofstream stream;
	stream.open(aFilePath);
	std::string define = "#define ";

	if (stream)
	{
		for (auto& i : myLoadedMetrics)
		{
			stream << define << i.first << " " << i.second << std::endl;
		}
	}
}

void MetricHandler::Apply(SerializableMetricValue* aValue)
{
	std::string name;
	for (auto& i : myListeners)
	{
		if (i.second == aValue)
		{
			myLoadedMetrics[i.first] = i.second->Serialize();
			name = i.first;
			break;
		}
	}
	for (auto& i : myListeners)
	{
		if (i.first == name)
		{
			i.second->DeSerialize(myLoadedMetrics[i.first]);
		}
	}
}

void MetricHandler::Unload()
{
	myListeners.clear();
	myLoadedMetrics.clear();
}

void MetricHandler::StartListening(const std::string& aLookup, SerializableMetricValue* aTarget)
{
	myListeners.push_back(std::make_pair(aLookup, aTarget));
	if (myLoadedMetrics.count(aLookup) != 0)
	{
		aTarget->DeSerialize(myLoadedMetrics[aLookup]);
	}
}

void MetricHandler::CopySubscription(const SerializableMetricValue* aFrom, SerializableMetricValue* aTo)
{
	for (auto& i : myListeners)
	{
		if (i.second == aFrom)
		{
			StartListening(i.first, aTo);
			return;
		}
	}
}

void MetricHandler::StopListening(SerializableMetricValue* aTarget)
{
	for (size_t i = 0; i < myListeners.size(); i++)
	{
		if (myListeners[i].second == aTarget)
		{
			myListeners.erase(myListeners.begin() + i);
			return;
		}
	}
}
void MetricHandler::ImGuiValueEditor()
{
	static char filter[64];
	for (size_t i = 0; i < 64; i++)
	{
		filter[i] &= ~(1 << 5);
	}


	WindowControl::Window("Metrics", [&]()
		{
			if (ImGui::Button("Save"))
			{
				for (auto& i : myListeners)
				{
					myLoadedMetrics[i.first] = i.second->Serialize();
				}
				Save(METRICFILE);
			}
			ImGui::SameLine();
			if (ImGui::Button("Revert"))
			{
				Load(METRICFILE);
			}
			ImGui::SameLine();
			static std::unordered_map<std::string, SerializableMetricValue*> myFiltered;
			static std::string last = "Start";
			if (ImGui::InputText("Filter", filter, 64))
			{
				for (size_t i = 0; i < 64; i++)
				{
					if (filter[i] > 'Z')
					{
						filter[i] -= 'a' - 'A';
					}
				}
			}
			if (last != filter)
			{
				last = filter;
				myFiltered.clear();
				for (auto& i : myListeners)
				{
					if (i.first.find(std::string(filter)) != std::string::npos)
					{
						myFiltered[i.first] = i.second;
					}
				}
			}
			ImGui::Separator();


			for (auto& i : myFiltered)
			{
				ImGui::PushID(i.first.c_str());
				ImGui::Text(i.first.c_str());
				if (i.second->EditImGui())
				{
					Apply(i.second);
				}
				ImGui::PopID();
			}
		});
}
#endif // !_RETAIL