#include <pet/runtime/Dictionary.hpp>

#include <pet/Error.hpp>

#include <toolkit/Macro.hpp>
#include <toolkit/StringJoiner.hpp>

namespace pet
{
	void Dictionary::Set(const ValuePtr& key, const ValuePtr& value)
	{
		PET_CHECK(key->IsString(), RuntimeError(StringBuilder() % "Invalid dictionary key '" % key % "'"));

		if (value->IsNull())
			_properties.erase(key->AsString());
		else
			_properties.insert_or_assign(key->AsString(), value);
	}

	ValuePtr Dictionary::Get(const ValuePtr& key) const
	{
		PET_CHECK(key->IsString(), RuntimeError(StringBuilder() % "Invalid dictionary key '" % key % "'"));

		const auto it = _properties.find(key->AsString());
		return it != _properties.end() ? it->second : NullValue;
	}

	std::string Dictionary::ToString() const
	{
		StringBuilder sb;

		sb % "{ ";
		StringJoiner sj;

		for (const auto& [key, value] : _properties) sj % (StringBuilder() % key % ": " % value->ToString());

		return sb % sj % (sj.IsEmpty() ? "" : " ") % "}";
	}
}
