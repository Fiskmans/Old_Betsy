#include "tools/JSON.h"

#include <stack>
#include <sstream>
#include <iostream>
#include <charconv>

namespace tools
{
	namespace json_help
	{
		bool IsWhiteSpace(char aChar)
		{
			return !isprint(aChar) || aChar == ' ';
		}

		const char* FindStart(const char* aBegin, const char* aEnd)
		{
			const char* start = aBegin;
			while (start != aEnd)
			{
				if (!IsWhiteSpace(*start))
					break;

				start++;
			}
			return start;
		}

		const char* FindEnd(const char* aBegin, const char* aEnd)
		{
			const char* end = aEnd;
			while (end != aBegin)
			{
				if(!IsWhiteSpace(*(end - 1)))
					break;

				--end;
			}
			return end;
		}

		const char* FindEndOfString(const char* aBegin, const char* aEnd)
		{
			const char* start = aBegin;
			while ((*start != '"' || (*(start - 1)) == '\\') && ++start < aEnd);
			return start;
		}

		const char* FindFirstWhitespaceInSameScope(const char* aBegin, const char* aEnd)
		{
			const char* begin = aBegin;

			std::stack<char> scopeStack;

			while (begin < aEnd)
			{
				char c = *begin;
				if (scopeStack.empty())
				{
					if(IsWhiteSpace(c) || c == ',')
					{
						break;
					}
				}
				else
				{
					if (c == scopeStack.top())
					{
						scopeStack.pop();
						begin++;
						if (scopeStack.empty())
						{
							break;
						}
						continue;
					}
				}

				switch (c)
				{
				case '"':
				{
					const char* stringEnd = FindEndOfString(begin + 1, aEnd);
					if (stringEnd == aEnd)
					{
						throw Invalid_JSON("Unmatched string");
					}
					begin = stringEnd;
					if (scopeStack.empty())
					{
						return begin + 1;
					}
				}
					break;
				case '[':
					scopeStack.push(']');
					break;
				case '{':
					scopeStack.push('}');
					break;
				}
				begin++;
			}
			return begin;
		}

		bool ContainsChar(const char* aBegin, const char* aEnd, char aValue)
		{
			for (const char* begin = aBegin; begin < aEnd; begin++)
			{
				if (*begin == aValue)
				{
					return true;
				}
			}
			return false;
		}

		const char* EscapeString(const char* aBegin, const char* aEnd)
		{
			const char* read = aBegin;
			char* write = const_cast<char*>(aBegin);
			while (read != aEnd)
			{
				if (*read == '\\')
				{
					read++;
					if (read == aEnd)
					{
						throw Invalid_JSON("Invalid escaped character");
					}
					switch (*read)
					{
					case 'n':
						*write = '\n';
						break;
					case 'r':
						*write = '\r';
						break;
					case 't':
						*write = '\t';
						break;
					case 'b':
						*write = '\b';
						break;
					case 'f':
						*write = '\f';
						break;
					case '\\':
						*write = '\\';
						break;
					case '"':
						*write = '\"';
						break;
					case '\'':
						*write = '\'';
						break;
					default:
						throw Invalid_JSON("Invalid escaped character");
					}
				}
				else
				{
					*write = *read;
				}
				read++;
				write++;
			}
			return write;
		}
	}

	JSONObject& JSONObject::Parse(const std::string& aDocument)
	{
		Parse(aDocument.c_str(), aDocument.c_str() + aDocument.length());
		return *this;
	}

	JSONObject& JSONObject::Parse(const char* aBegin, const char* aEnd)
	{
		const char* begin = json_help::FindStart(aBegin, aEnd);
		const char* end = json_help::FindEnd(aBegin, aEnd);

		if (begin == end)
		{
			throw Invalid_JSON("Json document is empty");
		}


		switch (*begin)
		{
		case '[':
			MakeArray();
			break;
		case '{':
			MakeObject();
			break;
		default:
			MakeValue();
			ParseAsValue(begin, end);
			return *this;
		}

		if (end - begin < 2)
		{
			throw Invalid_JSON("Parsed object is too small");
		}

		begin++; // step into brackets
		end--;

		if ((*end) != ((myType == Type::Array) ? ']' : '}'))
		{
			throw Invalid_JSON("JSONObject end character wrongly matched");
		}

		while (begin < end)
		{
			const char* nextStart = json_help::FindStart(begin, end);
			if (nextStart == end)
			{
				break;
			}

			const char* endOfValue;

			if (myType == Type::JSONObject)
			{
				if ((*nextStart) != '"')
				{
					throw Invalid_JSON("JSONObject contained malformed child");
				}
				const char* nameEnd = json_help::FindEndOfString(nextStart + 1, end);
				if (nameEnd == end)
				{
					throw Invalid_JSON("Childname runs until end of object");
				}
				const char* colon = json_help::FindStart(nameEnd + 1, end);

				if (colon == end)
				{
					throw Invalid_JSON("Child has no value after name");
				}
				if (*colon != ':')
				{
					throw Invalid_JSON("Child is missing :");
				}

				const char* startOfValue = json_help::FindStart(colon + 1, end);
				if (startOfValue == end)
				{
					throw Invalid_JSON("Child does not have a value");
				}
				endOfValue = json_help::FindFirstWhitespaceInSameScope(startOfValue, end);

				JSONObject* obj = new JSONObject();
				obj->Parse(startOfValue, endOfValue);
				AddChild(std::string(nextStart + 1, nameEnd), obj);
			}
			else
			{
				endOfValue = json_help::FindFirstWhitespaceInSameScope(nextStart, end);

				JSONObject* obj = new JSONObject();
				obj->Parse(nextStart, endOfValue);
				PushChild(obj);
			}

			const char* commaOrEnd = json_help::FindStart(endOfValue, end);
			if (commaOrEnd == end)
			{
				break;
			}

			if (*commaOrEnd != ',')
			{
				throw Invalid_JSON("Unexpected character between values");
			}

			begin = commaOrEnd + 1;
		}

		return *this;
	}

	JSONObject::~JSONObject()
	{
		CleanUpChildren();
	}

	JSONObject& JSONObject::operator[](const std::string& aKey)
	{
		if (IsNull() || !Has(aKey))
		{
			return Null();
		}
		auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
		return *children[aKey];
	}

	JSONObject& JSONObject::operator[](const char* aKey)
	{
		return operator[](std::string(aKey));
	}

	JSONObject& JSONObject::operator[](size_t aIndex)
	{
		if (NotNull() && Is<ArrayWrapper>())
		{
			return Get<ArrayWrapper>()[aIndex];
		}
		return Null();
	}

	JSONObject& JSONObject::operator[](int aIndex)
	{
		return operator[](size_t(aIndex));
	}

	JSONObject& JSONObject::operator[](long aIndex)
	{
		return operator[](size_t(aIndex));
	}

	const JSONObject& JSONObject::operator[](const std::string& aKey) const
	{
		if (IsNull() || !Has(aKey))
		{
			return Null();
		}
		const auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
		return *children.at(aKey);
	}

	const JSONObject& JSONObject::operator[](const char* aKey) const
	{
		return operator[](std::string(aKey));
	}

	bool JSONObject::Has(const std::string& aKey) const
	{
		auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
		return children.count(aKey) != 0;
	}

	void JSONObject::AddChild(const std::string& aKey, JSONObject* aChild)
	{

		if (myType == Type::Value || myType == Type::None)
		{
			MakeObject();
		}
		if (myType != Type::JSONObject)
		{
			throw Invalid_JSONObject("Trying to add a child to a object that can't have children");
		}
		auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
		if (children.count(aKey) != 0)
		{
			delete children[aKey];
		}
		children[aKey] = aChild;
	}

	void JSONObject::MakeObject()
	{
		if (myType != Type::JSONObject)
		{
			CleanUpChildren();
			myType = Type::JSONObject;
			myValue = std::unordered_map<std::string, JSONObject*>();
		}
	}

	void JSONObject::PushChild(JSONObject* aChild)
	{
		if (myType != Type::Array)
		{
			throw Invalid_JSONObject("Trying to push a arraychild to a object that is not an array");
		}
		if (!myValue)
		{
			myValue = std::vector<JSONObject*>();
		}
		std::get<std::vector<JSONObject*>>(myValue.value()).push_back(aChild);
	}

	void JSONObject::MakeArray()
	{
		if (myType != Type::Array)
		{
			CleanUpChildren();
			myType = Type::Array;
			myValue = std::vector<JSONObject*>();
		}
	}

	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}

	std::string JSONObject::Serialize(bool aPretty)
	{
		std::stringstream stream;

		const auto Indent = [](const std::string& aInput) -> std::string
		{
			std::string out = aInput;
			replaceAll(out, "\n", "\n\t");
			return out;
		};
		if (myType != Type::JSONObject)
		{
			if (!myValue.has_value())
			{
				throw Invalid_JSONObject("value has no content");
			}
		}

		switch (myType)
		{
		case JSONObject::Type::Array:
			stream << '[';
			if (myValue)
			{
				bool containExpanded = false;
				std::vector<JSONObject*>& content = std::get<std::vector<JSONObject*>>(myValue.value());
				if (aPretty)
				{
					for (auto& i : content)
					{
						if (i->myType != Type::Value)
						{
							containExpanded = true;
						}
					}
				}
				for (size_t index = 0; index < content.size(); index++)
				{
					std::string data = content[index]->Serialize(aPretty);
					if (index != 0)
					{
						stream << ',';
					}
					if (containExpanded)
					{
						stream << '\n';
					}
					if (aPretty)
					{
						if (containExpanded)
						{
							stream << '\t';
						}
						stream << Indent(data);
					}
					else
					{
						stream << data;
					}
				}
				if (containExpanded && aPretty)
				{
					stream << '\n';
				}
			}
			stream << ']';
			break;
		case JSONObject::Type::JSONObject:
			stream << '{';
			{
				bool first = true;
				for (auto& it : *this)
				{
					std::string data = it.second->Serialize(aPretty);
					if (!first)
					{
						stream << ',';
					}
					first = false;
					if (aPretty)
					{
						stream << "\n\t";
					}
					stream << '"' << it.first << "\":";
					if (aPretty)
					{
						stream << ' ';
					}
					stream << Indent(data);
				}
				if (aPretty)
				{
					stream << '\n';
				}
			}
			stream << '}';
			break;
		case JSONObject::Type::Value:
			if (myValue.has_value())
			{

				switch (myValue.value().index())
				{
				case 0: // long
					stream << std::get<long long>(myValue.value());
					break;
				case 1: // double
					stream << std::get<double>(myValue.value());
					break;
				case 2: //	string
					stream << '"' << std::get<std::string>(myValue.value()) << '"';
					break;
				case 3: // bool
					stream << (std::get<bool>(myValue.value()) ? "true" : "false");
					break;

				case 4: // array
				default:
					throw Invalid_JSONObject("Value contains invalid datatype: " + std::to_string(myValue.value().index()));
					break;
				}
			}
			else
			{
				stream << "\"Invalid\"";
			}
			break;
		default:
			throw Invalid_JSONObject("type should never be anything except [Array,JSONObject,Value]");
			break;
		}

		return stream.str();
	}

	void JSONObject::MakeValue()
	{
		CleanUpChildren();
		myType = Type::Value;
	}

	void JSONObject::CleanUpChildren()
	{
		if (myType == Type::Array && myValue.has_value() && myValue.value().index() == 2)
		{
			for (auto& child : ArrayWrapper(this))
			{
				delete& child;
			}
			myValue.reset();
		}
		if (myType == Type::JSONObject)
		{
			auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
			for (auto& child : children)
			{
				delete child.second;
			}
			children.clear();
		}
	}

	void JSONObject::ParseAsValue(const char* aBegin, const char* aEnd)
	{
		if (aEnd - aBegin > 1 && *aBegin == '"' && *(aEnd - 1) == '"')
		{

			myValue = std::string(aBegin + 1, json_help::EscapeString(aBegin + 1, aEnd - 1));
		}
		else if (aEnd - aBegin > 3 && memcmp(aBegin, "true", 4) == 0)
		{
			myValue = true;
		}
		else if (aEnd - aBegin > 4 && memcmp(aBegin, "false", 5) == 0)
		{
			myValue = false;
		}
		else if (aEnd - aBegin > 3 && memcmp(aBegin, "null", 4) == 0)
		{
			myValue.reset();
		}
		else if (json_help::ContainsChar(aBegin, aEnd, '.')) // floating point
		{
			double val;
			auto result = std::from_chars(aBegin, aEnd, val);
			if (result.ptr != aEnd)
			{
				throw Invalid_JSON("Floating point number contained invalid character");
			}
			if (result.ec == std::errc::result_out_of_range)
			{
				throw Invalid_JSON("Floating point number is out of range");
			}
			myValue = val;
		}
		else
		{
			long long val;
			auto result = std::from_chars(aBegin, aEnd, val);
			if (result.ptr != aEnd)
			{
				throw Invalid_JSON("Number contained invalid character");
			}
			if (result.ec == std::errc::result_out_of_range)
			{
				throw Invalid_JSON("Number is out of range");
			}
			myValue = val;
		}
	}

	std::unordered_map<std::string, JSONObject*>::iterator JSONObject::begin()
	{
		if (NotNull() && myType == Type::JSONObject)
		{
			auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
			return children.begin();
		}
		return std::unordered_map<std::string, JSONObject*>::iterator();
	}

	std::unordered_map<std::string, JSONObject*>::iterator JSONObject::end()
	{
		if (NotNull() && myType == Type::JSONObject)
		{
			auto& children = std::get<std::unordered_map<std::string, JSONObject*>>(myValue.value());
			return children.end();
		}
		return std::unordered_map<std::string, JSONObject*>::iterator();
	}


	JSONObject& ArrayWrapper::operator[](size_t aIndex)
	{
		if (!myArrayRef || aIndex >= myArrayRef->size())
		{
			return JSONObject::Null();
		}
		return *(*myArrayRef)[aIndex];
	}
	DereferencingIteratorWrapper<std::vector<JSONObject*>::iterator> ArrayWrapper::begin()
	{
		if (myArrayRef)
		{
			return myArrayRef->begin();
		}
		return {};
	}
	DereferencingIteratorWrapper<std::vector<JSONObject*>::iterator> ArrayWrapper::end()
	{
		if (myArrayRef)
		{
			return myArrayRef->end();
		}
		return {};
	}

	void ArrayWrapper::PushChild(JSONObject* aJSONObject)
	{
		if (!myArrayRef)
		{
			delete aJSONObject;
			return;
		}
		myArrayRef->push_back(aJSONObject);
	}

	ArrayWrapper::ArrayWrapper(JSONObject* aParent)
	{
		if (aParent->NotNull() && aParent->Is<ArrayWrapper>())
		{
			myArrayRef = &std::get<std::vector<JSONObject*>>(aParent->myValue.value());
		}
	}

	const JSONObject& ConstArrayWrapper::operator[](size_t aIndex)
	{
		if (!myArrayRef || aIndex >= myArrayRef->size())
		{
			return JSONObject::Null();
		}
		return *(*myArrayRef)[aIndex];
	}

	DereferencingIteratorWrapper<std::vector<JSONObject*>::const_iterator> ConstArrayWrapper::begin()
	{
		if (myArrayRef)
		{
			return myArrayRef->begin();
		}
		return {};
	}

	DereferencingIteratorWrapper<std::vector<JSONObject*>::const_iterator> ConstArrayWrapper::end()
	{
		if (myArrayRef)
		{
			return myArrayRef->end();
		}
		return {};
	}

	ConstArrayWrapper::ConstArrayWrapper(const JSONObject* aParent)
	{
		if (aParent->NotNull() && aParent->Is<ArrayWrapper>())
		{
			myArrayRef = &std::get<std::vector<JSONObject*>>(aParent->myValue.value());
		}
	}
}
