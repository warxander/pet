#include <pet/runtime/Interpreter.hpp>

#include <pet/Error.hpp>

#include <pet/runtime/Array.hpp>
#include <pet/runtime/Function.hpp>
#include <pet/runtime/Dictionary.hpp>

#include <toolkit/ScopedInvoker.hpp>
#include <toolkit/StringUtils.hpp>

#include <cmath>

namespace pet
{
	void Interpreter::Execute(const StatementUniqPtr& statement)
	{
		statement->Visit(*this);
	}

	void Interpreter::VisitBinary(BinaryExpression& expression)
	{
		const auto left = Evaluate(expression.Left);
		const auto right = Evaluate(expression.Right);
		ValuePtr   result;

		switch (expression.Operator)
		{
		case TokenKind::Minus:
		{
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '-'"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '-'"));
			result = (left->IsInteger() && right->IsInteger()) ? std::make_shared<Value>(left->AsInteger() - right->AsInteger())
															   : std::make_shared<Value>(left->AsFloat() - right->AsFloat());
			break;
		}
		case TokenKind::Asterisk:
		{
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '*'"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '*'"));
			result = (left->IsInteger() && right->IsInteger()) ? std::make_shared<Value>(left->AsInteger() * right->AsInteger())
															   : std::make_shared<Value>(left->AsFloat() * right->AsFloat());
			break;
		}
		case TokenKind::Power:
		{
			PET_CHECK(left->IsNumber(),
					  RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '" % expression.Operator % "'"));
			PET_CHECK(right->IsNumber(),
					  RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '" % expression.Operator % "'"));

			result = (left->IsInteger() && right->IsInteger())
						 ? std::make_shared<Value>(static_cast<ValueIntegerType>(std::pow(left->AsInteger(), right->AsInteger())))
						 : std::make_shared<Value>(std::pow(left->AsFloat(), right->AsFloat()));
			break;
		}
		case TokenKind::Slash:
		{
			PET_CHECK(left->IsNumber(),
					  RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '" % expression.Operator % "'"));
			PET_CHECK(right->IsNumber(),
					  RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '" % expression.Operator % "'"));

			if (left->IsInteger() && right->IsInteger())
			{
				const auto rightValue = right->AsInteger();
				PET_CHECK(rightValue != 0, RuntimeError("Divide by zero exception"));
				result = std::make_shared<Value>(left->AsInteger() / rightValue);
			}
			else
			{
				const auto rightValue = right->AsFloat();
				PET_CHECK(std::fpclassify(rightValue) != FP_ZERO, RuntimeError("Divide by zero exception"));
				result = std::make_shared<Value>(left->AsFloat() / rightValue);
			}

			break;
		}
		case TokenKind::Plus:
		{
			if (left->IsString() && right->IsString())
			{
				result = std::make_shared<Value>(StringUtils::Concat(left->AsString(), right->AsString()));
				break;
			}
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '+'"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '+'"));
			result = (left->IsInteger() && right->IsInteger()) ? std::make_shared<Value>(left->AsInteger() + right->AsInteger())
															   : std::make_shared<Value>(left->AsFloat() + right->AsFloat());
			break;
		}
		case TokenKind::Percent:
		{
			PET_CHECK(left->IsInteger(),
					  RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '" % expression.Operator % "'"));
			PET_CHECK(right->IsInteger(),
					  RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '" % expression.Operator % "'"));

			const auto rightValue = right->AsInteger();
			PET_CHECK(rightValue != 0, RuntimeError("Divide by zero exception"));
			result = std::make_shared<Value>(left->AsInteger() % rightValue);

			break;
		}
		case TokenKind::GreaterThan:
		{
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '>'"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '>'"));
			result = (left->IsInteger() && right->IsInteger()) ? (left->AsInteger() > right->AsInteger() ? TrueValue : FalseValue)
															   : (left->AsFloat() > right->AsFloat() ? TrueValue : FalseValue);
			break;
		}
		case TokenKind::GreaterThanOrEquals:
		{
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '>='"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '>='"));
			result = (left->IsInteger() && right->IsInteger()) ? (left->AsInteger() >= right->AsInteger() ? TrueValue : FalseValue)
															   : (left->AsFloat() >= right->AsFloat() ? TrueValue : FalseValue);
			break;
		}
		case TokenKind::LessThan:
		{
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '<'"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '<'"));
			result = (left->IsInteger() && right->IsInteger()) ? (left->AsInteger() < right->AsInteger() ? TrueValue : FalseValue)
															   : (left->AsFloat() < right->AsFloat() ? TrueValue : FalseValue);
			break;
		}
		case TokenKind::LessThanOrEquals:
		{
			PET_CHECK(left->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number left operand for operator '<='"));
			PET_CHECK(right->IsNumber(), RuntimeError(StringBuilder() % "Invalid non-number right operand for operator '<='"));
			result = (left->IsInteger() && right->IsInteger()) ? (left->AsInteger() <= right->AsInteger() ? TrueValue : FalseValue)
															   : (left->AsFloat() <= right->AsFloat() ? TrueValue : FalseValue);
			break;
		}
		case TokenKind::Equals:
		{
			if (left->IsNull() && right->IsNull())
				result = TrueValue;
			else if (left->IsNull() || right->IsNull())
				result = FalseValue;
			else if (left->IsBoolean() && right->IsBoolean())
				result = left->AsBoolean() == right->AsBoolean() ? TrueValue : FalseValue;
			else if (left->IsInteger() && right->IsInteger())
				result = left->AsInteger() == right->AsInteger() ? TrueValue : FalseValue;
			else if (left->IsFloat() && right->IsFloat())
				result =
					std::abs(right->AsFloat() - left->AsFloat()) <= std::numeric_limits<ValueFloatType>::epsilon() ? TrueValue : FalseValue;
			else if (left->IsString() && right->IsString())
				result = left->AsString() == right->AsString() ? TrueValue : FalseValue;
			else
				PET_THROW(RuntimeError(StringBuilder() % "Invalid operand types for operator '" % expression.Operator % "'"));
			break;
		}
		case TokenKind::NotEquals:
		{
			if (left->IsNull() && right->IsNull())
				result = FalseValue;
			else if (left->IsNull() || right->IsNull())
				result = TrueValue;
			else if (left->IsBoolean() && right->IsBoolean())
				result = left->AsBoolean() == right->AsBoolean() ? FalseValue : TrueValue;
			else if (left->IsInteger() && right->IsInteger())
				result = left->AsInteger() == right->AsInteger() ? FalseValue : TrueValue;
			else if (left->IsFloat() && right->IsFloat())
				PET_THROW(NotImplementedException());
			else if (left->IsString() && right->IsString())
				result = left->AsString() == right->AsString() ? FalseValue : TrueValue;
			else
				PET_THROW(RuntimeError(StringBuilder() % "Invalid operand types for operator '" % expression.Operator % "'"));

			break;
		}
		default:
			PET_THROW(RuntimeError(StringBuilder() % "Invalid binary operator '" % expression.Operator % "'"));
		}
		_evaluationResult = result;
	}

	void Interpreter::VisitGrouping(GroupingExpression& expression)
	{
		_evaluationResult = Evaluate(expression.Expression);
	}

	void Interpreter::VisitUnary(UnaryExpression& expression)
	{
		const auto right = Evaluate(expression.Right);

		switch (expression.Operator)
		{
		case TokenKind::Bang:
		{
			PET_CHECK(right->IsBoolean(),
					  RuntimeError(StringBuilder() % "Invalid non-boolean operand type for operator '" % expression.Operator % "'"));
			_evaluationResult = right->AsBoolean() ? FalseValue : TrueValue;

			break;
		}
		case TokenKind::Minus:
		{
			if (right->IsInteger())
				_evaluationResult = std::make_shared<Value>(-right->AsInteger());
			else if (right->IsFloat())
				_evaluationResult = std::make_shared<Value>(-right->AsFloat());
			else
				PET_THROW(RuntimeError(StringBuilder() % "Invalid operand type for operator '" % expression.Operator % "'"));

			break;
		}
		default:
			PET_THROW(RuntimeError(StringBuilder() % "Invalid unary operator '" % expression.Operator % "'"));
		}
	}

	void Interpreter::VisitLiteral(LiteralExpression& expression)
	{
		_evaluationResult = expression.Value;
	}

	void Interpreter::VisitDictionary(DictionaryExpression&)
	{
		_evaluationResult = std::make_shared<Value>(std::make_shared<Dictionary>());
	}

	void Interpreter::VisitArray(ArrayExpression& expression)
	{
		std::vector<ValuePtr> values;
		values.reserve(expression.Values.size());

		for (const auto& value : expression.Values) values.emplace_back(Evaluate(value));

		_evaluationResult = std::make_shared<Value>(std::make_shared<Array>(std::move(values)));
	}

	void Interpreter::VisitMember(MemberExpression& expression)
	{
		const auto target = Evaluate(expression.Target);
		PET_CHECK(target->IsObject(), RuntimeError(StringBuilder() % "Failed to access member for non-object variable"));

		const auto key = Evaluate(expression.Key);
		_evaluationResult = target->IsDictionary() ? target->AsDictionary()->Get(key) : target->AsArray()->Get(key);  // TODO: Optimize me
	}

	void Interpreter::VisitFunction(FunctionExpression& expression)
	{
		_evaluationResult = std::make_shared<Value>(std::make_shared<ScriptFunction>(
			_scope, _context.GetIdentifierPool().Add(""), std::move(expression.Parameters), std::move(expression.Body)));
	}

	void Interpreter::VisitIdentifier(IdentifierExpression& expression)
	{
		auto scope = _scope;

		while (scope)
		{
			if (const auto value = scope->TryGetValue(expression.Id))
			{
				_evaluationResult = value;
				return;
			}

			scope = scope->GetParent();
		}

		const auto it = _globals.find(expression.Id);
		PET_CHECK(it != _globals.end(), RuntimeError(StringBuilder() % "'" % expression.Id % "' is not defined"));

		_evaluationResult = it->second;
	}

	void Interpreter::VisitAssignment(AssignmentExpression& expression)
	{
		if (expression.Target->GetKind() == ExpressionKind::Member)
		{
			const auto memberExpression = static_cast<MemberExpression*>(expression.Target.get());

			const auto target = Evaluate(memberExpression->Target);
			PET_CHECK(target->IsObject(), RuntimeError(StringBuilder() % "Failed to access member for non-object variable"));

			const auto key = Evaluate(memberExpression->Key);
			const auto value = Evaluate(expression.Value);
			target->IsDictionary() ? target->AsDictionary()->Set(key, value) : target->AsArray()->Set(key, value);	// TODO: Optimize me
		}
		else
		{
			const auto identifierExpression = static_cast<IdentifierExpression*>(expression.Target.get());

			auto scope = _scope;

			while (scope)
			{
				if (scope->HasValue(identifierExpression->Id))
					break;

				scope = scope->GetParent();
			}

			PET_CHECK(scope, RuntimeError(StringBuilder() % "'" % identifierExpression->Id % "' is not defined"));
			scope->SetValue(identifierExpression->Id, Evaluate(expression.Value));
		}
	}

	void Interpreter::VisitLogical(LogicalExpression& expression)
	{
		const auto left = Evaluate(expression.Left);
		PET_CHECK(left->IsBoolean(),
				  RuntimeError(StringBuilder() % "Expect boolean left value for operator '" % expression.Operator % "'"));

		switch (expression.Operator)
		{
		case TokenKind::And:
		{
			if (!left->AsBoolean())
			{
				_evaluationResult = FalseValue;
				break;
			}

			const auto right = Evaluate(expression.Right);
			PET_CHECK(right->IsBoolean(),
					  RuntimeError(StringBuilder() % "Expect boolean right value for operator '" % expression.Operator % "'"));
			_evaluationResult = right->AsBoolean() ? TrueValue : FalseValue;

			break;
		}
		case TokenKind::Or:
		{
			if (left->AsBoolean())
			{
				_evaluationResult = TrueValue;
				break;
			}

			const auto right = Evaluate(expression.Right);
			PET_CHECK(right->IsBoolean(),
					  RuntimeError(StringBuilder() % "Expect boolean right value for operator '" % expression.Operator % "'"));
			_evaluationResult = right->AsBoolean() ? TrueValue : FalseValue;

			break;
		}
		default:
			PET_THROW(RuntimeError(StringBuilder() % "Invalid logical operator '" % expression.Operator % "'"));
		}
	}

	void Interpreter::VisitCall(CallExpression& expression)
	{
		const auto functionResult = Evaluate(expression.Callee);
		PET_CHECK(functionResult->IsFunction(), RuntimeError("Expect function for call expression"));

		const auto function = functionResult->AsFunction();

		const auto functionParametersCount = function->GetParametersCount();
		PET_CHECK(!functionParametersCount || functionParametersCount == expression.Arguments.size(),
				  RuntimeError(StringBuilder() % "Expect " % functionParametersCount % " parameters for '" % function->GetName() %
							   "' function call, " % expression.Arguments.size() % " provided"));

		std::vector<ValuePtr> arguments;
		arguments.reserve(expression.Arguments.size());
		for (const auto& argument : expression.Arguments) arguments.emplace_back(Evaluate(argument));

		_evaluationResult = function->Invoke(*this, arguments);
	}

	void Interpreter::VisitVariableDeclaration(VariableDeclarationStatement& statement)
	{
		PET_CHECK(!_scope->HasValue(statement.Id),
				  RuntimeError(StringBuilder() % "Variable " % statement.Id % " is already declared in this scope"));

		_scope->SetValue(statement.Id, statement.Value ? Evaluate(statement.Value) : NullValue);
		_statementResult = StatementResult::Empty();
	}

	void Interpreter::VisitFunctionDeclaration(FunctionDeclarationStatement& statement)
	{
		PET_CHECK(!_scope->HasValue(statement.Id),
				  RuntimeError(StringBuilder() % "Function '" % statement.Id % "' is already declared in this scope"));

		_scope->SetValue(statement.Id, std::make_shared<Value>(std::make_shared<ScriptFunction>(
										   _scope, statement.Id, std::move(statement.Parameters), std::move(statement.Body))));

		_statementResult = StatementResult::Empty();
	}

	void Interpreter::VisitExpression(ExpressionStatement& statement)
	{
		Evaluate(statement.Expression);
		_statementResult = StatementResult::Empty();
	}

	void Interpreter::VisitBlock(BlockStatement& statement)
	{
		ExecuteBlock(statement.Statements, std::make_shared<Scope>(_scope));
	}

	void Interpreter::VisitIf(IfStatement& statement)
	{
		const auto condition = Evaluate(statement.Condition);
		PET_CHECK(condition->IsBoolean(), RuntimeError("Expect boolean value for 'if' condition"));

		if (condition->AsBoolean())
			Execute(statement.StatementTrue);
		else if (statement.StatementFalse)
			Execute(statement.StatementFalse);
		else
			_statementResult = StatementResult::Empty();
	}

	void Interpreter::VisitWhile(WhileStatement& statement)
	{
		while (true)
		{
			const auto condition = Evaluate(statement.Condition);
			PET_CHECK(condition->IsBoolean(), RuntimeError("Expect boolean value for 'while' condition"));

			if (!condition->AsBoolean())
				break;

			Execute(statement.Body);
			if (_statementResult.Kind == StatementResult::Kind::Break)
				break;
		}

		_statementResult = StatementResult::Empty();
	}

	void Interpreter::VisitBreak(BreakStatement&)
	{
		_statementResult = StatementResult::Break();
	}

	void Interpreter::VisitReturn(ReturnStatement& statement)
	{
		_statementResult = StatementResult::Return(statement.Value ? Evaluate(statement.Value) : NullValue);
	}

	ValuePtr Interpreter::InvokeScriptFunction(ScriptFunction& function, const std::vector<ValuePtr>& arguments)
	{
		const auto scope = std::make_shared<Scope>(function.Closure);

		for (size_t i = 0; i < function.Parameters.size(); ++i) scope->SetValue(function.Parameters[i], arguments[i]);

		ExecuteBlock(function.Body, scope);
		return _statementResult.Kind == StatementResult::Kind::Return ? _statementResult.Value : NullValue;
	}

	ValuePtr Interpreter::Evaluate(const ExpressionUniqPtr& expression)
	{
		expression->Visit(*this);
		return _evaluationResult;
	}

	void Interpreter::ExecuteBlock(const std::vector<StatementUniqPtr>& statements, const ScopePtr& scope)
	{
		const auto previous = _scope;

		const ScopedInvoker si([&]() { _scope = previous; });

		_scope = scope;

		for (const auto& statement : statements)
		{
			Execute(statement);
			if (_statementResult.Kind == StatementResult::Kind::Break || _statementResult.Kind == StatementResult::Kind::Return)
				return;
		}

		_statementResult = StatementResult::Empty();
	}
}
