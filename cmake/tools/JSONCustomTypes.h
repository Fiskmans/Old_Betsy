#ifndef TOOLS_FISK_JSON_CUSTOM_TYPES_H
#define TOOLS_FISK_JSON_CUSTOM_TYPES_H

#include "tools/FiskJSON.h"

namespace tools
{
	template<>
	inline void Object::AddValueChild(const std::string& aKey, tools::V3f aValue)
	{
		Object* child = new Object();
		child->AddValueChild("x", aValue.x);
		child->AddValueChild("y", aValue.y);
		child->AddValueChild("z", aValue.z);
		AddChild(aKey, child);
	}

	template<>
	inline bool Object::GetIf<V3F>(V3F& aValueToPlaceIn) const
	{
		if (Is<FiskJSON::Object>())
		{
			if (!operator[]("x").GetIf(aValueToPlaceIn.x)) { return false; };
			if (!operator[]("y").GetIf(aValueToPlaceIn.x)) { return false; };
			if (!operator[]("z").GetIf(aValueToPlaceIn.x)) { return false; };
			return true;
		}
		return false;
	}
}

#endif