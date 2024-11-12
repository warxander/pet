#pragma once

#include <pet/runtime/Object.hpp>

#include <unordered_map>

namespace pet
{
	class Dictionary final : public Object
	{
	private:
		std::unordered_map<std::string, ValuePtr> _properties;

	public:
		void	 Set(const ValuePtr& key, const ValuePtr& value) override;
		ValuePtr Get(const ValuePtr& key) const override;

		std::string ToString() const;
	};
}
