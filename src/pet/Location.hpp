#pragma once

#include <string>

namespace pet
{
	struct Location
	{
		size_t Line;
		size_t Column;

		std::string ToString() const;
	};
}
