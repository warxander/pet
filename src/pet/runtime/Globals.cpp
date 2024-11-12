#include <pet/runtime/Globals.hpp>

#include <pet/Error.hpp>
#include <pet/runtime/Array.hpp>
#include <pet/runtime/Dictionary.hpp>

#include <chrono>
#include <iostream>

#include <toolkit/StringJoiner.hpp>

namespace pet
{
	namespace
	{
		template <typename R, typename VisitorType>
		ValuePtr Cast(const ValuePtr& value);

		struct ValueTyper
		{
			template <typename T>
			std::string operator()(const T&) const
			{
				if constexpr (std::is_same_v<ValueNullType, T>)
					return "null";
				else if constexpr (std::is_same_v<ValueBooleanType, T>)
					return "boolean";
				else if constexpr (std::is_same_v<ValueIntegerType, T>)
					return "integer";
				else if constexpr (std::is_same_v<ValueFloatType, T>)
					return "float";
				else if constexpr (std::is_same_v<ValueStringType, T>)
					return "string";
				else if constexpr (std::is_same_v<ValueFunctionType, T>)
					return "function";
				else if constexpr (std::is_same_v<ValueDictionaryType, T>)
					return "dict";
				else if constexpr (std::is_same_v<ValueArrayType, T>)
					return "array";
				else
					PET_THROW(NotImplementedException());
			}
		};

		struct ValueToIntCaster
		{
			template <typename T>
			ValueIntegerType operator()(const T& value) const
			{
				if constexpr (std::is_same_v<ValueIntegerType, T> || std::is_same_v<ValueFloatType, T>)
					return static_cast<ValueIntegerType>(value);
				else if constexpr (std::is_same_v<ValueStringType, T>)
					return std::stoll(value);
				else
					PET_THROW(NotSupportedException());
			}
		};

		struct ValueToFloatCaster
		{
			template <typename T>
			ValueFloatType operator()(const T& value) const
			{
				if constexpr (std::is_same_v<ValueBooleanType, T>)
					return value ? 1.0 : 0.0;
				else if constexpr (std::is_same_v<ValueIntegerType, T> || std::is_same_v<ValueFloatType, T>)
					return static_cast<ValueFloatType>(value);
				else if constexpr (std::is_same_v<ValueStringType, T>)
					return std::stod(value);
				else
					PET_THROW(NotSupportedException());
			}
		};

		struct ValueLenGetter
		{
			template <typename T>
			ValueIntegerType operator()(const T& value) const
			{
				if constexpr (std::is_same_v<ValueStringType, T>)
					return static_cast<ValueIntegerType>(value.size());
				else if constexpr (std::is_same_v<ValueArrayType, T>)
					return value->GetLength();
				else
					PET_THROW(NotSupportedException());
			}
		};

		template <typename R, typename VisitorType>
		ValuePtr Cast(const ValuePtr& value)
		{
			try
			{
				return std::make_shared<Value>(value->Visit<R>(VisitorType()));
			}
			catch (const std::exception&)
			{
				PET_THROW(StringBuilder() % "Invalid argument '" % value % "'");
			}
		}
	}

	ValuePtr AssertFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		const auto& argument = arguments[0];
		PET_CHECK(argument->IsBoolean(), RuntimeError("Expect boolean argument type in 'assert' function call"));

		if (!argument->AsBoolean())
		{
			StringBuilder sb;
			sb % "Assertion failed";

			if (arguments.size() > 1)
			{
				sb % ": ";

				StringJoiner sj(" ");
				for (size_t i = 1; i < arguments.size(); ++i) sj % arguments[i];
				sb % sj;
			}

			PET_THROW(LanguageError(sb));
		}

		return NullValue;
	}

	ValuePtr NowFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>&)
	{
		return std::make_shared<Value>(static_cast<ValueIntegerType>(
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()));
	}

	ValuePtr PrintFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		if (!arguments.empty())
		{
			StringJoiner sj(" ");
			for (const auto& argument : arguments) sj % argument;
			std::cout << sj << std::endl;
		}

		return NullValue;
	}

	ValuePtr ReadLnFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>&)
	{
		std::string result;
		std::getline(std::cin, result);

		return std::make_shared<Value>(result);
	}

	ValuePtr TypeFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		return std::make_shared<Value>(arguments[0]->Visit<std::string>(ValueTyper()));
	}

	ValuePtr IntFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		const auto& argument = arguments[0];
		if (argument->IsBoolean())
			return argument->AsBoolean() ? OneValue : ZeroValue;

		return Cast<ValueIntegerType, ValueToIntCaster>(arguments[0]);
	}

	ValuePtr FloatFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		return Cast<ValueFloatType, ValueToFloatCaster>(arguments[0]);
	}

	ValuePtr StrFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		return std::make_shared<Value>(arguments[0]->ToString());
	}

	ValuePtr LenFunction::DoInvoke(FunctionInvoker&, const std::vector<ValuePtr>& arguments)
	{
		return std::make_shared<Value>(arguments[0]->Visit<ValueIntegerType>(ValueLenGetter()));
	}
}
