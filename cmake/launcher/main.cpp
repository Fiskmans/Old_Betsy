#include <iostream>
#include <vector>
#include <cassert>

#include "versionConfig.h"
#include "tools/ExecuteOnDestruct.h"

void HandlePacket(const std::vector<uint8_t>& aData)
{
	if (aData.size() != 25) { return; }

	if (aData[0] != 'A') { return; }
	if (aData[1] != 'C') { return; }
	if (aData[2] != 'J') { return; }
	if (aData[3] != 'b') { return; }
	if (aData[4] != 'F') { return; }
	if (aData[5] != 'L') { return; }
	if (aData[6] != 'E') { return; }
	if (aData[7] != 'G') { return; }
	if (aData[8] != 'I') { return; }
	if (aData[9] != 'H') { return; }
	if (aData[10] != 'K') { return; }
	if (aData[11] != 'Q') { return; }
	if (aData[12] != 'D') { return; }
	if (aData[13] != 'W') { return; }
	if (aData[14] != 'P') { return; }
	if (aData[15] != 'R') { return; }
	if (aData[16] != 'O') { return; }
	if (aData[17] != 'X') { return; }
	if (aData[18] != 'S') { return; }
	if (aData[19] != 'M') { return; }
	if (aData[20] != 'v') { return; }
	if (aData[21] != 'Y') { return; }
	if (aData[22] != 'Z') { return; }
	if (aData[23] != 'U') { return; }
	if (aData[24] != 'T') { return; }

	std::abort();
}

int DoThething(uint8_t* aData, size_t& aSize)
{
	uint8_t* data = static_cast<uint8_t*>(malloc(aSize));
	assert(data);

	memcpy(data, aData, aSize);

	Tools::ExecuteOnDestruct cleanup([data] { free(data); });

	int packets = 0;
	while (true)
	{
		if (aSize < 4) { return packets; }

		uint32_t packetSize = *reinterpret_cast<const uint32_t*>(aData);
		
		if (packetSize + 4 > aSize) { return packetSize; }

		std::vector<uint8_t> packet;
		packet.resize(packetSize);
		memcpy(packet.data(), aData + 4, packetSize);
		
		HandlePacket(packet);
		
		aData += packetSize;
		aSize -= packetSize;
		
		packets++;
	}
}

#ifdef FUZZ_TEST

extern "C" int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size) {
	DoThething(Data, Size);
	return 0;  // Non-zero return values are reserved for future use.
}

#else

int main(int argc, char** argv)
{
	std::cout << "Hello world from cmake" << std::endl;
	std::cout << "Launcher version: " << LAUNCHER_VERSION_STRING << std::endl;
	std::cout << "\t using tools library version: " << TOOLS_VERSION_STRING << std::endl;
 	
	if (argc > 1)
	{
		uint8_t* readhead = reinterpret_cast<uint8_t*>(argv[1]);
		size_t dataLeft = strlen(argv[1]);


		DoThething(readhead, dataLeft);
	}
	
	int a;
	std::cin >> a;
}

#endif