#include <pch.h>
#include "FiskJSON.h"

#include <stack>
#include <sstream>
#include <iostream>
#include <charconv>



namespace FiskJSON
{
	namespace FiskJson_Help
	{
		bool IsWhiteSpace(char aChar)
		{
			return !isprint(aChar) || aChar == ' ';
		}

		const char* FindStart(const char* aBegin, const char* aEnd)
		{
			const char* start = aBegin;
			while (IsWhiteSpace(*start) && ++start < aEnd);
			return start;
		}

		const char* FindEnd(const char* aBegin, const char* aEnd)
		{
			const char* end = aEnd;
			while (IsWhiteSpace(*(end - 1)) && --end > aBegin);
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

	void Object::Parse(const std::string& aDocument)
	{
		Parse(aDocument.c_str(), aDocument.c_str() + aDocument.length());
	}

	void Object::Parse(const char* aBegin, const char* aEnd)
	{
		const char* begin = FiskJson_Help::FindStart(aBegin, aEnd);
		const char* end = FiskJson_Help::FindEnd(aBegin, aEnd);

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
			return;
		}

		if (end - begin < 2)
		{
			throw Invalid_JSON("Parsed object is too small");
		}

		begin++; // step into brackets
		end--;

		if ((*end) != ((myType == Type::Array) ? ']' : '}'))
		{
			throw Invalid_JSON("Object end character wrongly matched");
		}

		while (begin < end)
		{
			const char* nextStart = FiskJson_Help::FindStart(begin, end);
			if (nextStart == end)
			{
				break;
			}

			const char* endOfValue;

			if (myType == Type::Object)
			{
				if ((*nextStart) != '"')
				{
					throw Invalid_JSON("Object contained malformed child");
				}
				const char* nameEnd = FiskJson_Help::FindEndOfString(nextStart + 1,end);
				if(nameEnd == end)
				{
					throw Invalid_JSON("Childname runs until end of object");
				}
				const char* colon = FiskJson_Help::FindStart(nameEnd + 1, end);

				if (colon == end)
				{
					throw Invalid_JSON("Child has no value after name");
				}
				if(*colon != ':')
				{
					throw Invalid_JSON("Child is missing :");
				}

				const char* startOfValue = FiskJson_Help::FindStart(colon + 1, end);
				if (startOfValue == end)
				{
					throw Invalid_JSON("Child does not have a value");
				}
				endOfValue = FiskJson_Help::FindFirstWhitespaceInSameScope(startOfValue, end);

				if (endOfValue == end)
				{
					throw Invalid_JSON("Childvalue ends unexpectedly");
				}

				Object* obj = new Object();
				obj->Parse(startOfValue,endOfValue);
				AddChild(std::string(nextStart + 1, nameEnd), obj);
			}
			else
			{
				endOfValue = FiskJson_Help::FindFirstWhitespaceInSameScope(nextStart, end);

				if (endOfValue == end)
				{
					throw Invalid_JSON("Childvalue ends unexpectedly");
				}

				Object* obj = new Object();
				obj->Parse(nextStart, endOfValue);
				PushChild(obj);
			}

			const char* commaOrEnd = FiskJson_Help::FindStart(endOfValue, end);
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
	}

	Object::~Object()
	{
		CleanUpChildren();
	}

	Object& FiskJSON::Object::operator[](const std::string& aKey)
	{
		if (!this || !Has(aKey))
		{
			return *static_cast<Object*>(nullptr);
		}
		auto& children = std::get<std::unordered_map<std::string, Object*>>(myValue.value());
		return *children[aKey];
	}

	Object& Object::operator[](const char* aKey)
	{
		return operator[](std::string(aKey));
	}

	Object& Object::operator[](size_t aIndex)
	{
		if (this && Is<Array>())
		{
			return Get<Array>()[aIndex];
		}
		return *static_cast<Object*>(nullptr);
	}

	Object& Object::operator[](int aIndex)
	{
		return operator[](size_t(aIndex));
	}

	Object& Object::operator[](long aIndex)
	{
		return operator[](size_t(aIndex));
	}

	bool Object::Has(const std::string& aKey)
	{
		auto& children = std::get<std::unordered_map<std::string, Object*>>(myValue.value());
		return children.count(aKey) != 0;
	}

	void Object::AddChild(const std::string& aKey, Object* aChild)
	{

		if (myType == Type::Value || myType == Type::None)
		{
			MakeObject();
		}
		if (myType != Type::Object)
		{
			throw Invalid_Object("Trying to add a child to a object that can't have children");
		}
		auto& children = std::get<std::unordered_map<std::string, Object*>>(myValue.value());
		if (children.count(aKey) != 0)
		{
			delete children[aKey];
		}
		children[aKey] = aChild;
	}

	void Object::MakeObject()
	{
		if (myType != Type::Object)
		{
			CleanUpChildren();
			myType = Type::Object;
			myValue = std::unordered_map<std::string, Object*>();
		}
	}

	void Object::PushChild(Object* aChild)
	{
		if (myType != Type::Array)
		{
			throw Invalid_Object("Trying to push a arraychild to a object that is not an array");
		}
		if (!myValue)
		{
			myValue = std::vector<Object*>();
		}
		std::get<std::vector<Object*>>(myValue.value()).push_back(aChild);
	}

	void Object::MakeArray()
	{
		if (myType != Type::Array)
		{
			CleanUpChildren();
			myType = Type::Array;
			myValue = std::vector<Object*>();
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

	std::string Object::Serialize(bool aPretty)
	{
		std::stringstream stream;

		const auto Indent = [](const std::string& aInput) -> std::string
		{
			std::string out = aInput;
			replaceAll(out, "\n", "\n\t");
			return out;
		};
		if (myType != Type::Object)
		{
			if (!myValue.has_value())
			{
				throw Invalid_Object("value has no content");
			}
		}

		switch (myType)
		{
		case FiskJSON::Object::Type::Array:
			stream << '[';
			if (myValue)
			{
				bool containExpanded = false;
				std::vector<Object*>& content = std::get<std::vector<Object*>>(myValue.value());
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
		case FiskJSON::Object::Type::Object:
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
		case FiskJSON::Object::Type::Value:
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
					throw Invalid_Object("Value contains invalid datatype: " + std::to_string(myValue.value().index()));
					break;
				}
			}
			else
			{
				stream << "\"Invalid\"";
			}
			break;
		default:
			throw Invalid_Object("type should never be anything except [Array,Object,Value]");
			break;
		}

		return stream.str();
	}

	void Object::MakeValue()
	{
		CleanUpChildren();
		myType = Type::Value;
	}

	void Object::CleanUpChildren()
	{
		if (myType == Type::Array && myValue.has_value() && myValue.value().index() == 2)
		{
			for (auto& child : Array(this))
			{
				delete child;
			}
			myValue.reset();
		}
		if (myType == Type::Object)
		{
			auto& children = std::get<std::unordered_map<std::string, Object*>>(myValue.value());
			for (auto& child : children)
			{
				delete child.second;
			}
			children.clear();
		}
	}

	void Object::ParseAsValue(const char* aBegin, const char* aEnd)
	{
		if (aEnd - aBegin > 1 && *aBegin == '"' && *(aEnd - 1) == '"')
		{

			myValue = std::string(aBegin + 1, FiskJson_Help::EscapeString(aBegin + 1, aEnd - 1));
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
		else if (FiskJson_Help::ContainsChar(aBegin,aEnd,'.')) // floating point
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

	std::unordered_map<std::string, Object*>::iterator Object::begin()
	{
		if (this && myType == Type::Object)
		{
			auto& children = std::get<std::unordered_map<std::string, Object*>>(myValue.value());
			return children.begin();
		}
		return std::unordered_map<std::string, Object*>::iterator();
	}

	std::unordered_map<std::string, Object*>::iterator Object::end()
	{
		if (this && myType == Type::Object)
		{
			auto& children = std::get<std::unordered_map<std::string, Object*>>(myValue.value());
			return children.end();
		}
		return std::unordered_map<std::string, Object*>::iterator();
	}


	Object& Array::operator[](size_t aIndex)
	{
		if (!myArrayRef || aIndex >= myArrayRef->size())
		{
			return *static_cast<Object*>(nullptr);
		}
		return *(*myArrayRef)[aIndex];
	}
	std::vector<Object*>::iterator Array::begin()
	{
		if (myArrayRef)
		{
			return myArrayRef->begin();
		}
		return std::vector<Object*>::iterator();
	}
	std::vector<Object*>::iterator Array::end()
	{
		if (myArrayRef)
		{
			return myArrayRef->end();
		}
		return std::vector<Object*>::iterator();
	}
	Array::Array(Object* aParent)
	{
		if (aParent && aParent->Is<Array>())
		{
			myArrayRef = &std::get<std::vector<Object*>>(aParent->myValue.value());
		}
	}
}
