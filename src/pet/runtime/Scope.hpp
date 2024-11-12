#pragma once

#include <pet/Error.hpp>
#include <pet/runtime/Value.hpp>

#include <toolkit/Macro.hpp>
#include <toolkit/StringPool.hpp>

namespace pet
{
	class Scope;
	PET_DECLARE_PTR(Scope);

	class Scope
	{
		PET_NON_COPYABLE(Scope);

	private:
		ScopePtr _parent;

		std::unordered_map<StringPoolId, ValuePtr> _values;

	public:
		explicit Scope(const ScopePtr& parent = nullptr) : _parent(parent)
		{
		}

		const ScopePtr& GetParent() const
		{
			return _parent;
		}

		bool HasValue(StringPoolId id) const
		{
			return _values.find(id) != _values.end();
		}

		void SetValue(StringPoolId id, const ValuePtr& value)
		{
			_values.insert_or_assign(id, value);
		}

		ValuePtr TryGetValue(StringPoolId id) const
		{
			const auto it = _values.find(id);
			return it == _values.end() ? nullptr : it->second;
		}
	};
}
