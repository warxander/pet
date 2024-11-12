#pragma once

#include <pet/runtime/Value.hpp>

namespace pet
{
	struct Object
	{
		virtual ~Object() = default;

		virtual void	 Set(const ValuePtr& key, const ValuePtr& value) = 0;
		virtual ValuePtr Get(const ValuePtr& key) const = 0;
	};
	PET_DECLARE_PTR(Object);
}
