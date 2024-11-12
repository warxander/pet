#pragma once

#include <toolkit/ToString.hpp>

#include <string>
#include <string_view>
#include <type_traits>

namespace pet
{
	class StringBuilder
	{
		static const size_t InitialCapacity = 256;

	private:
		std::string _buffer;

	public:
		StringBuilder()
		{
			_buffer.reserve(InitialCapacity);
		}

		template <typename T>
		StringBuilder& operator%(const T& value)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				if (value.empty())
					_buffer += "\"\"";
				else
					_buffer += value;
			}
			else if constexpr (std::is_same_v<T, std::string_view>)
			{
				if (value.empty())
					_buffer += "\"\"";
				else
					_buffer += value;
			}
			else if constexpr (std::is_same_v<T, char>)
				_buffer.push_back(value);
			else
				_buffer += pet::ToString(value);

			return *this;
		}

		bool IsEmpty() const
		{
			return _buffer.empty();
		}

		void Clear()
		{
			_buffer.clear();
		}

		operator std::string() const
		{
			return ToString();
		}

		std::string ToString() const
		{
			return _buffer;
		}
	};

	inline std::ostream& operator<<(std::ostream& stream, const StringBuilder& sb)
	{
		stream << sb.ToString();
		return stream;
	}
}
