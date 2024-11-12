#pragma once

#include <toolkit/TypeTraits.hpp>

#include <magic_enum/magic_enum.hpp>

#include <array>
#include <optional>
#include <sstream>
#include <utility>

namespace pet
{
	template <typename T>
	std::string ToString(const T& value);

	namespace details
	{
		template <typename T>
		std::string ToStringImpl(const std::optional<T>& value)
		{
			return value.has_value() ? ToString(*value) : "<null-opt>";
		}

		template <typename T, size_t N>
		std::string ToStringImpl(const std::array<T, N>& value)
		{
			std::stringstream stream;

			stream << "[ ";

			for (size_t i = 0; i < N; ++i)
			{
				stream << ToString(value[i]);

				if (i != N - 1)
					stream << ", ";
			}

			stream << " ]";

			return stream.str();
		}

		template <typename K, class V>
		std::string ToStringImpl(const std::pair<K, V>& value)
		{
			std::stringstream stream;

			stream << "{ ";
			stream << ToString(value.first) << ", " << ToString(value.second);
			stream << " }";

			return stream.str();
		}

		template <typename T>
		std::enable_if_t<IsIterable<T>, std::string> ToStringImpl(const T& value)
		{
			std::stringstream stream;

			stream << "{ ";

			size_t i = 0;
			for (const auto& v : value)
			{
				stream << ToString(v);

				if (i++ != value.size() - 1)
					stream << ", ";
			}

			stream << " }";

			return stream.str();
		}
	}

	template <typename T>
	std::string ToString(const T& value)
	{
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
			return !value.empty() ? static_cast<std::string>(value) : "\"\"";
		else if constexpr (std::is_same_v<T, char>)
			return std::string(1, value);
		else if constexpr (IsStringLike<T>)
			return value;
		else if constexpr (std::is_pointer_v<T> || IsSmartPointerValue<T>)
			return value ? ToString(*value) : "null";
		else if constexpr (HasToStringMethod<T>)
			return value.ToString();
		else if constexpr (std::is_arithmetic_v<T>)
			return std::to_string(value);
		else if constexpr (std::is_enum_v<T>)
			return static_cast<std::string>(magic_enum::enum_name<T>(value));
		else
			return details::ToStringImpl(value);
	}
}
