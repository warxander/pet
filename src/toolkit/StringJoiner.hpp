#pragma once

#include <toolkit/Macro.hpp>
#include <toolkit/StringBuilder.hpp>

namespace pet
{
	class StringJoiner
	{
	private:
		std::string _separator;

		StringBuilder _sb;

	public:
		explicit StringJoiner(const std::string& separator = ", ") : _separator(separator)
		{
			PET_CHECK(!separator.empty(), ArgumentException("separator"));
		}

		bool IsEmpty()
		{
			return _sb.IsEmpty();
		}

		void Clear()
		{
			_sb.Clear();
		}

		template <typename T>
		StringJoiner& operator%(const T& value)
		{
			if (!_sb.IsEmpty())
				_sb % _separator;

			_sb % value;

			return *this;
		}

		operator std::string() const
		{
			return ToString();
		}

		std::string ToString() const
		{
			return _sb;
		}
	};

	inline std::ostream& operator<<(std::ostream& stream, const StringJoiner& sj)
	{
		stream << sj.ToString();
		return stream;
	}
}
