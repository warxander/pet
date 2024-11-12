#pragma once

#include <toolkit/StringBuilder.hpp>

#include <stdexcept>

namespace pet
{
#define DEFINE_SIMPLE_EXCEPTION(ClassName, Message) \
	struct ClassName : public Exception             \
	{                                               \
		ClassName() : Exception(Message)            \
		{                                           \
		}                                           \
	}

	struct Exception : public std::runtime_error
	{
		explicit Exception(const std::string& message) : std::runtime_error(message)
		{
		}

		std::string ToString() const
		{
			return what();
		}
	};

	struct ArgumentException : public Exception
	{
		explicit ArgumentException(const std::string& argName) : Exception(StringBuilder() % "Invalid argument: " % argName)
		{
		}
	};

	struct IndexOutOfRangeException : public Exception
	{
		template <typename T, typename U>
		IndexOutOfRangeException(T index, U size) : IndexOutOfRangeException(index, 0, size)
		{
		}

		template <typename T, typename U, typename V>
		IndexOutOfRangeException(T index, U begin, V end)
			: Exception(StringBuilder() % "Index " % index % " is out of range [ " % begin % ", " % end % " )")
		{
		}
	};

	DEFINE_SIMPLE_EXCEPTION(NullPointerException, "Accessing null pointer!");
	DEFINE_SIMPLE_EXCEPTION(LogicException, "Something went wrong!");
	DEFINE_SIMPLE_EXCEPTION(NotImplementedException, "This feature is not implemented!");
	DEFINE_SIMPLE_EXCEPTION(NotSupportedException, "This feature is not supported!");
	DEFINE_SIMPLE_EXCEPTION(InputOutputException, "I/O error!");

#undef DEFINE_SIMPLE_EXCEPTION
}
