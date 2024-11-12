#pragma once

#include <toolkit/StringPool.hpp>

namespace pet
{
	class Context
	{
	private:
		StringPool _identifierPool;

	public:
		StringPool& GetIdentifierPool()
		{
			return _identifierPool;
		}
	};
}
