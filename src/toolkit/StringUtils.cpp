#include <toolkit/StringUtils.hpp>

#include <sstream>

namespace pet
{
	std::vector<std::string> StringUtils::Split(std::string_view str, char separator)
	{
		std::vector<std::string> result;

		std::stringstream sstream{std::string(str)};
		std::string		  token;
		while (std::getline(sstream, token, separator)) result.push_back(std::move(token));

		return result;
	}
}
