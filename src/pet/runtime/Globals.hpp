#pragma once

#include <pet/Error.hpp>

#include <pet/runtime/Function.hpp>

#include <toolkit/StringPool.hpp>

#include <unordered_map>

namespace pet
{
#define DECLARE_NATIVE_FUNCTION(ClassName, Name, ParametersCount)                                  \
	struct ClassName final : public Function                                                       \
	{                                                                                              \
		ValuePtr Invoke(FunctionInvoker& invoker, const std::vector<ValuePtr>& arguments) override \
		{                                                                                          \
			try                                                                                    \
			{                                                                                      \
				return DoInvoke(invoker, arguments);                                               \
			}                                                                                      \
			catch (const std::exception& ex)                                                       \
			{                                                                                      \
				PET_THROW(RuntimeError(StringBuilder() % GetName() % ": " % ex.what()));           \
			}                                                                                      \
		}                                                                                          \
                                                                                                   \
		std::string GetName() const override                                                       \
		{                                                                                          \
			return Name;                                                                           \
		}                                                                                          \
                                                                                                   \
		std::optional<size_t> GetParametersCount() const override                                  \
		{                                                                                          \
			return ParametersCount;                                                                \
		}                                                                                          \
                                                                                                   \
	private:                                                                                       \
		ValuePtr DoInvoke(FunctionInvoker& invoker, const std::vector<ValuePtr>& arguments);       \
	}

	// Debug
	DECLARE_NATIVE_FUNCTION(AssertFunction, "assert", std::nullopt);

	// Time
	DECLARE_NATIVE_FUNCTION(NowFunction, "now", 0);

	// I/O
	DECLARE_NATIVE_FUNCTION(PrintFunction, "print", std::nullopt);
	DECLARE_NATIVE_FUNCTION(ReadLnFunction, "readln", 0);

	// Types
	DECLARE_NATIVE_FUNCTION(TypeFunction, "type", 1);
	DECLARE_NATIVE_FUNCTION(IntFunction, "int", 1);
	DECLARE_NATIVE_FUNCTION(FloatFunction, "float", 1);
	DECLARE_NATIVE_FUNCTION(StrFunction, "str", 1);

	// General
	DECLARE_NATIVE_FUNCTION(LenFunction, "len", 1);

#undef DECLARE_NATIVE_FUNCTION

	using Globals = std::unordered_map<StringPoolId, ValuePtr>;
}
