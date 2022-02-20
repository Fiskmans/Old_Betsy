
#include "tools/FiskJSON.h"


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
	std::string data(reinterpret_cast<const char*>(Data), Size);
	Tools::FiskJSON::Object obj;
	try
	{
		obj.Parse(data);
	}
	catch (Tools::FiskJSON::Invalid_JSON&) {}
	catch (Tools::FiskJSON::Invalid_Object&) {}

	return 0;  // Non-zero return values are reserved for future use.
}