#pragma once

#include <deque>
#include <string>
#include <unordered_map>

namespace pet
{
	using StringPoolId = size_t;

	class StringPool
	{
	private:
		std::unordered_map<std::string, StringPoolId> _stringToIds;
		std::deque<std::string>						  _strings;

	public:
		StringPoolId	 Add(std::string&& str);
		std::string_view Get(StringPoolId id) const;
	};
}
