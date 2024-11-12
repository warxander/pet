#include <pet/runtime/Value.hpp>

#include <pet/runtime/Array.hpp>
#include <pet/runtime/Function.hpp>
#include <pet/runtime/Dictionary.hpp>

#include <toolkit/StringJoiner.hpp>

namespace pet
{
	namespace
	{
		struct ValueToStringCaster
		{
			template <typename T>
			std::string operator()(const T& value) const
			{
				if constexpr (std::is_same_v<ValueNullType, T>)
					return "null";
				else if constexpr (std::is_same_v<ValueBooleanType, T>)
					return value ? "true" : "false";
				else if constexpr (std::is_same_v<ValueIntegerType, T> || std::is_same_v<ValueFloatType, T>)
					return std::to_string(value);
				else if constexpr (std::is_same_v<ValueStringType, T>)
					return value;
				else if constexpr (std::is_same_v<ValueFunctionType, T>)
					return StringBuilder() % "<fun " % value->GetName() % ">";
				else if constexpr (std::is_same_v<ValueDictionaryType, T> || std::is_same_v<ValueArrayType, T>)
					return value->ToString();
				else
					PET_THROW(NotSupportedException());
			}
		};
	}

	ValueFunctionType Value::AsFunction() const
	{
		return std::get<ValueFunctionType>(*this);
	}

	ValueDictionaryType Value::AsDictionary() const
	{
		return std::get<ValueDictionaryType>(*this);
	}

	ValueArrayType Value::AsArray() const
	{
		return std::get<ValueArrayType>(*this);
	}

	std::string Value::ToString() const
	{
		return Visit<std::string>(ValueToStringCaster());
	}
}
