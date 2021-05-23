#include "pch.h"
#include "PlantLoader.h"

PlantInstance* PlantLoader::GetPlant(const std::string& aPlantFile)
{
	if (myLoadedPlants.count(aPlantFile) == 0)
	{
		myLoadedPlants[aPlantFile] = LoadPlant(aPlantFile);
	}
	return new PlantInstance(myLoadedPlants[aPlantFile]);
}

Plant* PlantLoader::LoadPlant(const std::string& aPlantFilePath)
{
	SYSINFO("Loading plant: " + aPlantFilePath);
	FiskJSON::Object root;
	try
	{
		root.Parse(Tools::ReadWholeFile(aPlantFilePath));
	}
	catch (const FiskJSON::Invalid_JSON& e)
	{
		LOGERROR("Plant Failed to load [" + std::string(e.what()) + "]", aPlantFilePath);
		return nullptr;
	}
	catch (const FiskJSON::Invalid_Object& e)
	{
		LOGERROR("Plant Failed to load [" + std::string(e.what()) + "]", aPlantFilePath);
		return nullptr;
	}
	catch (const std::exception& e)
	{
		LOGERROR("Plant Failed to load [" + std::string(e.what()) + "]", aPlantFilePath);
		return nullptr;
	}

	Plant* p = new Plant();
	p->myFilePath = aPlantFilePath;


	FiskJSON::Object& stages = root["Stages"];
	if (!stages)
	{
		LOGWARNING("Plant did not have a valid 'Stages' object", aPlantFilePath);
		delete p;
		return nullptr;
	}

	for (auto& rawStage : stages.Get<FiskJSON::Array>())
	{
		PlantStage stage;
		for (auto& rawYield : (*rawStage)["Yield"].Get<FiskJSON::Array>())
		{
			Yield y;
			std::string itemName;
			if ((*rawYield)["Item"].GetIf(itemName) 
				&& (*rawYield)["Amount"].GetIf(y.myAmountAndChance))
			{
				y.myItemType = ItemIdFromString(itemName);
				stage.myYields.push_back(y);
				stage.isHarvestable = true;
			}
		}
		(*rawStage)["Harvestable"].GetIf(stage.isHarvestable);
		if ((*rawStage)["Model"].GetIf(stage.myModelPath))
		{
			p->myStages.push_back(stage);
		}

	}

	if (p->myStages.empty())
	{
		LOGWARNING("Plant did not have any valid stages",aPlantFilePath);
		delete p;
		return nullptr;
	}
	LOGINFO("Loaded [" + aPlantFilePath + "] with [" + std::to_string(p->myStages.size()) + "] stages");

	return p;
}

PlantInstance::PlantInstance(Plant* aPlant)
{
	myBasePlant = aPlant;
}

void PlantInstance::ProgressStage()
{
	if (myBasePlant->myStages.size() - 1 > myStage)
	{
		++myStage;
	}
}

PlantStage& PlantInstance::GetCurrentStage()
{
	return myBasePlant->myStages[myStage]; //TODO Apply stage calc
}

float PlantInstance::GetProgress()
{
	return float(myStage) / float(myBasePlant->myStages.size()-1);
}

bool PlantInstance::GetIsHarvestable()
{
	return myStage > 0;// myBasePlant->myStages.size() - 1;
}

int PlantInstance::GetStage()
{
	return myStage;
}
