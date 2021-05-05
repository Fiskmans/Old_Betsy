#pragma once
#include <Singleton.hpp>
#ifdef __INTELLISENSE__
#include <pch.h>
#endif

struct Yield
{
	ItemId myItemType;
	float myAmountAndChance;
};

class PlantStage
{
public:
	std::string myModelPath;
	std::vector<Yield> myYields;
	bool isHarvestable = false;
};

class Plant
{
	friend class PlantLoader;
public:
	std::string myFilePath;
	std::vector<PlantStage> myStages;
private:

};

class PlantInstance
{
public:
	PlantInstance() = default;
	PlantInstance(Plant* aPlant);

	void ProgressStage();


	PlantStage& GetCurrentStage();
	float GetProgress();
	bool GetIsHarvestable();
	int GetStage();
	
	Plant* myBasePlant = nullptr;

private:
	size_t myStage = 0;
};


class PlantLoader :	public CommonUtilities::Singleton<PlantLoader>
{
public:
	PlantInstance* GetPlant(const std::string& aPlantFile);


private:
	Plant* LoadPlant(const std::string& aPlantFilePath);

	std::unordered_map<std::string, Plant*> myLoadedPlants;
};

