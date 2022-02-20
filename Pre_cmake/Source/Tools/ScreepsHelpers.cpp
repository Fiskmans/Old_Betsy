#include <pch.h>
#include "ScreepsHelpers.h"
#include "CurlHelper.h"
#include "curl/curl.h"
#include "FiskJSON.h"

namespace Tools
{
	std::array<char, 50 * 50> GetRoomTerrain(const std::string& aRoomName)
	{
		static std::unordered_map<std::string, std::array<char, 50 * 50 >> myStashedRooms;
		if (myStashedRooms.count(aRoomName) == 0)
		{
			std::array<char, 50 * 50> terrain;

			size_t divider = aRoomName.find('/');

			std::string roomName = aRoomName.substr(divider + 1);
			std::string shardName = aRoomName.substr(0, divider);
			std::string dummyToken;

			CURL* curl;
			curl = curl_easy_init();
			std::string FethcedRoomterrain = Tools::SimpleAuthOp(curl, "https://screeps.com/api/game/room-terrain?shard=" + shardName + "&room=" + roomName + "&encoded=1", dummyToken);
			curl_easy_cleanup(curl);

			FiskJSON::Object Jsonterrain;
			Jsonterrain.Parse(FethcedRoomterrain);
			std::cout << Jsonterrain.Serialize(true) << std::endl;

			std::string FethcedTerrainString;
			if (Jsonterrain["terrain"][0]["terrain"].GetIf(FethcedTerrainString))
			{
				for (size_t y = 0; y < 50; y++)
				{
					for (size_t x = 0; x < 50; x++)
					{
						char t = FethcedTerrainString[x + y * 50];
						char a;
						switch (t)
						{
						case '0':
							a = 0;
							break;
						case '1':
						case '3':
							a = 2;
							break;
						case '2':
							a = 1;
							break;
						}
						terrain[x + y * 50] = a;
					}
				}
			}

			myStashedRooms[aRoomName] = terrain;
		}
		return myStashedRooms[aRoomName];
	}
}