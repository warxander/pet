#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace pet
{
	using StringPoolId = size_t;

	class StringPool
	{
	private:
		std::unordered_map<std::string, StringPoolId> _stringToIds;
		std::vector<std::string>					  _strings;

	public:
		StringPoolId	 Add(std::string&& str);
		std::string_view Get(StringPoolId id) const;
	};
}
