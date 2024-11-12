#pragma once

#include <pet/runtime/Globals.hpp>
#include <pet/runtime/Function.hpp>
#include <pet/runtime/Scope.hpp>

#include <pet/Context.hpp>
#include <pet/Statement.hpp>

namespace pet
{
	class Interpreter final : public ExpressionVisitor, public StatementVisitor, public FunctionInvoker
	{
		struct StatementResult
		{
			enum class Kind
			{
				Empty,
				Break,
				Return
			} Kind;
			ValuePtr Value;

			static StatementResult Empty()
			{
				return {Kind::Empty, NullValue};
			}

			static StatementResult Break()
			{
				return {Kind::Break, NullValue};
			}

			static StatementResult Return(const ValuePtr& value)
			{
				return {Kind::Return, value};
			}
		};

	private:
		Context& _context;

		const Globals _globals;
		ScopePtr	  _scope;

		ValuePtr		_evaluationResult;
		StatementResult _statementResult;

	public:
		Interpreter(Context& context, Globals&& globals)
			: _context(context), _globals(std::move(globals)), _scope(std::make_shared<Scope>())
		{
		}

		void Execute(const StatementUniqPtr& statement);

	private:
		void VisitBinary(BinaryExpression& expression) override;
		void VisitGrouping(GroupingExpression& expression) override;
		void VisitUnary(UnaryExpression& expression) override;
		void VisitLiteral(LiteralExpression& expression) override;
		void VisitDictionary(DictionaryExpression& expression) override;
		void VisitArray(ArrayExpression& expression) override;
		void VisitMember(MemberExpression& expression) override;
		void VisitFunction(FunctionExpression& expression) override;
		void VisitIdentifier(IdentifierExpression& expression) override;
		void VisitAssignment(AssignmentExpression& expression) override;
		void VisitLogical(LogicalExpression& expression) override;
		void VisitCall(CallExpression& expression) override;

		void VisitVariableDeclaration(VariableDeclarationStatement& statement) override;
		void VisitFunctionDeclaration(FunctionDeclarationStatement& statement) override;
		void VisitExpression(ExpressionStatement& statement) override;
		void VisitBlock(BlockStatement& statement) override;
		void VisitIf(IfStatement& statement) override;
		void VisitWhile(WhileStatement& statement) override;
		void VisitBreak(BreakStatement& statement) override;
		void VisitReturn(ReturnStatement& statement) override;

		ValuePtr InvokeScriptFunction(ScriptFunction& function, const std::vector<ValuePtr>& arguments) override;

		ValuePtr Evaluate(const ExpressionUniqPtr& expression);

		void ExecuteBlock(const std::vector<StatementUniqPtr>& statements, const ScopePtr& scope);
	};
}
