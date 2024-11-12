#include <pet/runtime/Array.hpp>

#include <pet/Error.hpp>

#include <toolkit/StringJoiner.hpp>

namespace pet
{
	Array::Array(std::vector<ValuePtr>&& values) : _values(std::move(values))
	{
	}

	void Array::Set(const ValuePtr& key, const ValuePtr& value)
	{
		PET_CHECK(key->IsInteger(), RuntimeError(StringBuilder() % "Invalid array index '" % key % "'"));

		const auto index = key->AsInteger();
		PET_CHECK(index >= 0 && index < static_cast<ValueIntegerType>(_values.size()), OutOfRangeError(index, _values.size()));
		_values[static_cast<size_t>(index)] = value;
	}

	ValuePtr Array::Get(const ValuePtr& key) const
	{
		PET_CHECK(key->IsInteger(), RuntimeError(StringBuilder() % "Invalid array index '" % key % "'"));
		return Get(key->AsInteger());
	}

	ValuePtr Array::Get(ValueIntegerType index) const
	{
		PET_CHECK(index >= 0 && index < static_cast<ValueIntegerType>(_values.size()), OutOfRangeError(index, _values.size()));
		return _values[static_cast<size_t>(index)];
	}

	ValueIntegerType Array::GetLength() const
	{
		return static_cast<ValueIntegerType>(_values.size());
	}

	std::string Array::ToString() const
	{
		StringBuilder sb;

		sb % "[ ";
		StringJoiner sj;

		for (const auto& value : _values) sj % value;

		return sb % sj % (sj.IsEmpty() ? "" : " ") % "]";
	}
}
