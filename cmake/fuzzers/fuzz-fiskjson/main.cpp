
#include "tools/JSON.h"


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
	std::string data(reinterpret_cast<const char*>(Data), Size);
	tools::JSONObject obj;
	try
	{
		obj.Parse(data);
	}
	catch (tools::Invalid_JSON&) {}
	catch (tools::Invalid_JSONObject&) {}

	return 0;  // Non-zero return values are reserved for future use.
}