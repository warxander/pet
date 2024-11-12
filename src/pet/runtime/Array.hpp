#pragma once

#include <pet/runtime/Object.hpp>

namespace pet
{
	class Array final : public Object
	{
	private:
		std::vector<ValuePtr> _values;

	public:
		explicit Array(std::vector<ValuePtr>&& values);

		void	 Set(const ValuePtr& key, const ValuePtr& value) override;
		ValuePtr Get(const ValuePtr& key) const override;

		ValuePtr Get(ValueIntegerType index) const;

		ValueIntegerType GetLength() const;

		std::string ToString() const;
	};
	PET_DECLARE_PTR(Array);
}
