#pragma once

#include <string>
#include <vector>

namespace pet
{
	struct StringUtils
	{
		static std::vector<std::string> Split(std::string_view str, char separator = ' ');

		template <typename... Args>
		static std::string Concat(Args&&... args)
		{
			const auto size = (std::string_view(args).size() + ...);

			std::string result;
			result.reserve(size);
			(result.append(args), ...);

			return result;
		}
	};

}
