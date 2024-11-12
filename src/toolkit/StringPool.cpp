#include <toolkit/StringPool.hpp>

#include <toolkit/Macro.hpp>

namespace pet
{
	StringPoolId StringPool::Add(std::string&& str)
	{
		const auto it = _stringToIds.find(str);
		if (it != _stringToIds.end())
			return it->second;

		const auto id = StringPoolId(_strings.size());
		_stringToIds.emplace(str, id);
		_strings.emplace_back(std::move(str));
		return id;
	}

	std::string_view StringPool::Get(StringPoolId id) const
	{
		PET_CHECK(id < _strings.size(), ArgumentException("id"));
		return _strings[id];
	}
}
