#pragma once

#include <pet/runtime/Scope.hpp>
#include <pet/Statement.hpp>

#include <toolkit/Macro.hpp>

namespace pet
{
	struct ScriptFunction;

	struct FunctionInvoker
	{
		virtual ~FunctionInvoker() = default;

		virtual ValuePtr InvokeScriptFunction(ScriptFunction& function, const std::vector<ValuePtr>& arguments) = 0;
	};

	struct Function
	{
		virtual ~Function() = default;

		virtual ValuePtr Invoke(FunctionInvoker& invoker, const std::vector<ValuePtr>& arguments) = 0;

		virtual std::string			  GetName() const = 0;
		virtual std::optional<size_t> GetParametersCount() const = 0;
	};
	PET_DECLARE_PTR(Function);

	struct ScriptFunction final : public Function
	{
		ScopePtr					  Closure;
		StringPoolId				  Id;
		std::vector<StringPoolId>	  Parameters;
		std::vector<StatementUniqPtr> Body;

		ScriptFunction(const ScopePtr& closure, StringPoolId id, std::vector<StringPoolId>&& parameters,
					   std::vector<StatementUniqPtr>&& body)
			: Closure(closure), Id(id), Parameters(std::move(parameters)), Body(std::move(body))
		{
		}

		ValuePtr Invoke(FunctionInvoker& invoker, const std::vector<ValuePtr>& arguments) override
		{
			return invoker.InvokeScriptFunction(*this, arguments);
		}

		std::string GetName() const override
		{
			return std::to_string(Id);
		}

		std::optional<size_t> GetParametersCount() const override
		{
			return Parameters.size();
		}
	};
}
