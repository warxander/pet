#include <pet/runtime/Scope.hpp>

namespace pet
{
	void Scope::Declare(StringPoolId id, const ValuePtr& value, bool isConst)
	{
		PET_CHECK(_values.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(value, isConst)).second,
				  ArgumentException("id"));
	}

	void Scope::Assign(StringPoolId id, const ValuePtr& value)
	{
		const auto it = _values.find(id);
		PET_CHECK(it != _values.end(), ArgumentException("id"));
		it->second.Value = value;
	}
}
