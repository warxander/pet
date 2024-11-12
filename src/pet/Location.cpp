#include <pet/Location.hpp>

#include <toolkit/StringBuilder.hpp>

namespace pet
{
	std::string Location::ToString() const
	{
		return StringBuilder() % Line % ":" % Column;
	}
}
