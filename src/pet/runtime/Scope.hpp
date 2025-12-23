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

		struct ValueEntry
		{
			ValuePtr Value;
			bool	 IsConst;

			ValueEntry(const ValuePtr& value, bool isConst) : Value(value), IsConst(isConst)
			{
			}
		};

	private:
		ScopePtr _parent;

		std::unordered_map<StringPoolId, ValueEntry> _values;

	public:
		explicit Scope(const ScopePtr& parent = nullptr) : _parent(parent)
		{
		}

		const ScopePtr& GetParent() const
		{
			return _parent;
		}

		bool Has(StringPoolId id) const
		{
			return _values.find(id) != _values.end();
		}

		bool IsConst(StringPoolId id) const
		{
			const auto it = _values.find(id);
			return it != _values.end() && it->second.IsConst;
		}

		void Declare(StringPoolId id, const ValuePtr& value, bool isConst);
		void Assign(StringPoolId id, const ValuePtr& value);

		ValuePtr TryGet(StringPoolId id) const
		{
			const auto it = _values.find(id);
			return it == _values.end() ? nullptr : it->second.Value;
		}
	};
}
