#include <pch.h>
#include "FiskJSON.h"

#include <stack>
#include <sstream>
#include <iostream>

#include "StringManipulation.h"



namespace FiskJSON
{
	void Object::Parse(const std::string& aDocument)
	{
		std::string cleaned = aDocument;
		Tools::trim(cleaned);
		CleanUpChildren();
		if (cleaned.empty())
		{
			throw Invalid_JSON("Json document is empty");
		}
		std::stack<char> escapeKeys;
		std::stringstream keyBuffer;
		std::stringstream ValueBuffer;
		if (cleaned[0] == '[')
		{
			myType = Type::Array;
			myValue = std::vector<Object*>();
		}
		else if (cleaned[0] == '{')
		{
			myType = Type::Object;
		}
		else
		{
			myType = Type::Value;
		}
		if (myType == Type::Value)
		{
			ParseAsValue(cleaned);
			return;
		}
		enum class ScopeType
		{
			Key,
			ObjectValue,
			UnkownValue,
			WhiteSpace
		};
		ScopeType currentScope = myType == Type::Array ? ScopeType::UnkownValue : ScopeType::WhiteSpace;


		if (cleaned.length() < 2)
		{
			myValue.reset();
			return;
		}
		std::string data = cleaned.substr(1, cleaned.length() - 2); // trim start and end bracket

		const auto PushChild = [&]()
		{
			Object* obj = new Object();
			obj->Parse(ValueBuffer.str());
			if (myType == Type::Array)
			{
				try
				{
					std::get<std::vector<Object*>>(myValue.value()).push_back(obj);

				}
				catch (const std::bad_variant_access&)
				{
					throw Invalid_JSON("Somehow tried to parse into arrayobject that was not previously an array");
				}
			}
			else
			{
				myChildren[keyBuffer.str()] = obj;
			}
		};

		const auto AppendChar = [&](char c)
		{
			if (currentScope == ScopeType::Key)
			{
				keyBuffer << c;
			}
			else if (currentScope == ScopeType::ObjectValue)
			{
				ValueBuffer << c;
			}
			else if (currentScope == ScopeType::UnkownValue)
			{
				switch (c)
				{
				case ' ':
				case '\n':
				case '\r':
					// Ignore whitespace
					break;
				default:
					ValueBuffer << c;
					break;
				}
			}
		};

		char last = '\0';
		size_t at = 0;
		for (char c : data)
		{
			++at;
			bool poppedStack = false;
			if (!escapeKeys.empty() && c == escapeKeys.top())
			{
				if (!(c == '"' && last == '\\'))
				{
					poppedStack = true;
					escapeKeys.pop();
				}
			}
			last = c;
			if (!escapeKeys.empty())
			{
				if (escapeKeys.top() != '"')
				{
					if (c == '{')
					{
						escapeKeys.push('}');
					}
					else if (c == '[')
					{
						escapeKeys.push(']');
					}
				}
				AppendChar(c);
				continue;
			}
			switch (c)
			{
			case '[':
				if (escapeKeys.empty())
				{
					currentScope = ScopeType::ObjectValue;
				}
				escapeKeys.push(']');
				AppendChar(c);
				break;
			case '{':
				if (escapeKeys.empty())
				{
					currentScope = ScopeType::ObjectValue;
				}
				escapeKeys.push('}');
				AppendChar(c);
				break;
			case '}':
			case ']':
				AppendChar(c);
				if (escapeKeys.empty())
				{
					currentScope = ScopeType::WhiteSpace;
				}
				break;
			case '"':
				if (!poppedStack)
				{
					escapeKeys.push('"');
					if (myType == Type::Array)
					{
						currentScope = ScopeType::ObjectValue;
						AppendChar(c);
					}
					else
					{
						if (keyBuffer.str().empty())
						{
							currentScope = ScopeType::Key;
						}
						else
						{
							if (!ValueBuffer.str().empty())
							{
								ValueBuffer.str("");
								ValueBuffer.clear();
								//throw Invalid_JSON("String values got assigned to an object that already has a value, original value: [" + ValueBuffer.str() + "] Around " + std::to_string(at) + " Are you missing a ','?");
							}
							currentScope = ScopeType::ObjectValue;
							AppendChar(c);
						}
					}
				}
				else
				{
					if (currentScope == ScopeType::Key)
					{
						currentScope = ScopeType::WhiteSpace;
					}
					else
					{
						AppendChar(c);
					}
				}
				break;
			case ',':
				if (escapeKeys.empty())
				{
					PushChild();
				}
				ValueBuffer.str("");
				ValueBuffer.clear();
				keyBuffer.str("");
				keyBuffer.clear();
				break;
			case ':':
				currentScope = ScopeType::UnkownValue;
				break;
			default:
				AppendChar(c);
				break;
			}
		}
		if (escapeKeys.empty())
		{
			PushChild();
		}
		else
		{
			std::ostringstream stack;
			while (escapeKeys.size() > 1)
			{
				stack << escapeKeys.top() << ",";
				escapeKeys.pop();
			}
			stack << escapeKeys.top();

			throw Invalid_JSON("Opening characters don't match up 1:1 with closing characters scopes not yet closed: [" + stack.str() + "]");
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
		return *myChildren[aKey];
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
		return myChildren.count(aKey) != 0;
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
		if (myChildren.count(aKey) != 0)
		{
			delete myChildren[aKey];
		}
		myChildren[aKey] = aChild;
	}
	
	void Object::MakeObject()
	{
		if (myType != Type::Object)
		{
			CleanUpChildren();
			myType = Type::Object;
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
					try
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
					catch (const Invalid_Object& e)
					{
						myExceptions.push_back(new Invalid_Object(e));
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
				for (auto& it : myChildren)
				{
					try
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
					catch (const Invalid_Object& e)
					{
						myExceptions.push_back(new Invalid_Object(e));
					}
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

	std::vector<std::exception*> Object::GetExceptions()
	{
		return myExceptions;
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
			for (auto& child : myChildren)
			{
				delete child.second;
			}
			myChildren.clear();
		}
	}

	void Object::ParseAsValue(const std::string& aValue)
	{
		std::string trimmed = aValue;
		Tools::trim(trimmed);

		if (trimmed.size() > 1 && trimmed[0] == '"' && trimmed[trimmed.size() - 1] == '"')
		{
			myValue = trimmed.substr(1, trimmed.size() - 2); // remove quotationmarks
		}
		else if (!trimmed.empty() && trimmed.find_first_not_of("-.e0123456789") == std::string::npos) // number
		{
			if (trimmed.find_first_of("e.") != std::string::npos) // double
			{
				myValue = std::stod(trimmed);
			}
			else
			{
				myValue = std::stoll(trimmed);
			}
		}
		else if (trimmed == "true" || trimmed == "false") // bool
		{
			myValue = (trimmed == "true");
		}
		else if (trimmed == "null" || trimmed.empty())
		{
			myValue.reset();
		}
		else
		{
			throw Invalid_JSON("Could not parse value: [" + trimmed + "]");
		}
	}

	std::unordered_map<std::string, Object*>::iterator Object::begin()
	{
		if (this && myType == Type::Object)
		{
			return myChildren.begin();
		}
		return std::unordered_map<std::string, Object*>::iterator();
	}
	std::unordered_map<std::string, Object*>::iterator Object::end()
	{
		if (this && myType == Type::Object)
		{
			return myChildren.end();
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
