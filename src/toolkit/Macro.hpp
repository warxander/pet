#pragma once

#include <toolkit/Exception.hpp>
#include <toolkit/StringBuilder.hpp>

namespace pet
{
	namespace details
	{
		template <typename ExceptionType>
		std::enable_if_t<std::is_base_of_v<std::exception, ExceptionType>, Exception> MakeException(const ExceptionType& ex,
																									const char* fileName, unsigned line)
		{
			StringBuilder sb;
			sb % ex.what();

			(void)fileName;
			(void)line;

#ifndef NDEBUG
			sb % " (" % fileName % ":" % line % ")";
#endif
			return Exception(sb);
		}

		inline Exception MakeException(const std::string& message, const char* fileName, unsigned line)
		{
			return MakeException(Exception(message), fileName, line);
		}

		template <typename T>
		T&& RequireNotNull(T&& pointer, const char* expression, const char* fileName, unsigned line)
		{
			if (pointer)
				return std::forward<T>(pointer);

			throw details::MakeException(StringBuilder() % expression % " is not initialized", fileName, line);
		}
	}

#define PET_THROW(Exception) throw details::MakeException(Exception, __FILE__, __LINE__)

#define PET_CHECK(Expression, Exception) \
	do                                   \
	{                                    \
		if ((Expression))                \
			break;                       \
		else                             \
			PET_THROW(Exception);        \
	} while (false)

#define PET_REQUIRE_NOT_NULL(Expression) details::RequireNotNull(Expression, #Expression, __FILE__, __LINE__)

#define PET_TRY(Message, Expression)                                                \
	do                                                                              \
	{                                                                               \
		try                                                                         \
		{                                                                           \
			Expression;                                                             \
		}                                                                           \
		catch (const std::exception& ex)                                            \
		{                                                                           \
			std::cout << StringBuilder() % Message % ": " % ex.what() << std::endl; \
		}                                                                           \
	} while (false)

#define PET_DECLARE_PTR(ClassName)                     \
	using ClassName##Ptr = std::shared_ptr<ClassName>; \
	using ClassName##ConstPtr = std::shared_ptr<const ClassName>

#define PET_DECLARE_UNIQ_PTR(ClassName)                    \
	using ClassName##UniqPtr = std::unique_ptr<ClassName>; \
	using ClassName##ConstUniqPtr = std::unique_ptr<const ClassName>

#define PET_NON_COPYABLE(ClassName)       \
	ClassName(const ClassName&) = delete; \
	ClassName& operator=(const ClassName&) = delete
}
