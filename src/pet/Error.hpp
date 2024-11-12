#pragma once

#include <pet/Location.hpp>

#include <toolkit/Exception.hpp>
#include <toolkit/StringBuilder.hpp>

namespace pet
{
	struct LanguageError : public Exception
	{
		explicit LanguageError(const std::string& message) : Exception(message)
		{
		}
	};

#define DEFINE_LANG_ERROR(ClassName)                                                                                  \
	struct ClassName : public LanguageError                                                                           \
	{                                                                                                                 \
		explicit ClassName(const std::string& message) : LanguageError(StringBuilder() % #ClassName % ": " % message) \
		{                                                                                                             \
		}                                                                                                             \
		ClassName(const Location& location, const std::string& message)                                               \
			: LanguageError(StringBuilder() % location % ": " % #ClassName % ": " % message)                          \
		{                                                                                                             \
		}                                                                                                             \
	}

	DEFINE_LANG_ERROR(SyntaxError);
	DEFINE_LANG_ERROR(TypeError);
	DEFINE_LANG_ERROR(RuntimeError);

	struct OutOfRangeError : public LanguageError
	{
		template <typename T, typename U>
		OutOfRangeError(T index, U size) : OutOfRangeError(index, 0, size)
		{
		}

		template <typename T, typename U, typename V>
		OutOfRangeError(T index, U begin, V end)
			: LanguageError(StringBuilder() % "Index " % index % " is out of range [ " % begin % ", " % end % " )")
		{
		}
	};

#undef DEFINE_ERROR
}
