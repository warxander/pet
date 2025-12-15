#pragma once

#include <toolkit/Macro.hpp>

#include <variant>

namespace pet
{
	struct Function;
	PET_DECLARE_PTR(Function);

	class Dictionary;
	PET_DECLARE_PTR(Dictionary);

	class Array;
	PET_DECLARE_PTR(Array);

	using ValueNullType = std::monostate;
	using ValueBooleanType = bool;
	using ValueFloatType = double;
	using ValueIntegerType = long long;
	using ValueStringType = std::string;
	using ValueFunctionType = FunctionPtr;
	using ValueDictionaryType = DictionaryPtr;
	using ValueArrayType = ArrayPtr;

	class Value : public std::variant<ValueNullType, ValueBooleanType, ValueFloatType, ValueIntegerType, ValueStringType, ValueFunctionType,
									  ValueDictionaryType, ValueArrayType>
	{
		using variant::variant;

	public:
		bool IsNull() const
		{
			return std::holds_alternative<ValueNullType>(*this);
		}

		bool IsBoolean() const
		{
			return std::holds_alternative<ValueBooleanType>(*this);
		}

		bool IsFloat() const
		{
			return std::holds_alternative<ValueFloatType>(*this);
		}

		bool IsInteger() const
		{
			return std::holds_alternative<ValueIntegerType>(*this);
		}

		bool IsNumber() const
		{
			return IsFloat() || IsInteger();
		}

		bool IsString() const
		{
			return std::holds_alternative<ValueStringType>(*this);
		}

		bool IsFunction() const
		{
			return std::holds_alternative<ValueFunctionType>(*this);
		}

		bool IsObject() const
		{
			return IsDictionary() || IsArray();
		}

		bool IsDictionary() const
		{
			return std::holds_alternative<ValueDictionaryType>(*this);
		}

		bool IsArray() const
		{
			return std::holds_alternative<ValueArrayType>(*this);
		}

		ValueBooleanType AsBoolean() const
		{
			return std::get<ValueBooleanType>(*this);
		}

		ValueFloatType AsFloat() const
		{
			return IsInteger() ? static_cast<ValueFloatType>(AsInteger()) : std::get<ValueFloatType>(*this);
		}

		ValueIntegerType AsInteger() const
		{
			return std::get<ValueIntegerType>(*this);
		}

		const std::string& AsString() const
		{
			return std::get<ValueStringType>(*this);
		}

		ValueFunctionType	AsFunction() const;
		ValueDictionaryType AsDictionary() const;
		ValueArrayType		AsArray() const;

		template <typename R, typename VisitorType>
		R Visit(VisitorType&& visitor) const
		{
			return std::visit(std::forward<VisitorType>(visitor), *this);
		}

		std::string ToString() const;
	};
	PET_DECLARE_PTR(Value);

	inline const auto NullValue = std::make_shared<Value>();
	inline const auto TrueValue = std::make_shared<Value>(true);
	inline const auto FalseValue = std::make_shared<Value>(false);
	inline const auto ZeroValue = std::make_shared<Value>(0);
	inline const auto OneValue = std::make_shared<Value>(1);
}
