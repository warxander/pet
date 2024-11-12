#pragma once

#include <pet/Expression.hpp>

namespace pet
{
	struct VariableDeclarationStatement;
	struct FunctionDeclarationStatement;
	struct ExpressionStatement;
	struct BlockStatement;
	struct IfStatement;
	struct WhileStatement;
	struct BreakStatement;
	struct ReturnStatement;

	enum class StatementKind
	{
		VariableDeclaration,
		FunctionDeclaration,
		Expression,
		Block,
		If,
		While,
		Break,
		Return
	};

	struct StatementVisitor
	{
		virtual ~StatementVisitor() = default;

		virtual void VisitVariableDeclaration(VariableDeclarationStatement& statement) = 0;
		virtual void VisitFunctionDeclaration(FunctionDeclarationStatement& statement) = 0;
		virtual void VisitExpression(ExpressionStatement& statement) = 0;
		virtual void VisitBlock(BlockStatement& statement) = 0;
		virtual void VisitIf(IfStatement& statement) = 0;
		virtual void VisitWhile(WhileStatement& statement) = 0;
		virtual void VisitBreak(BreakStatement& statement) = 0;
		virtual void VisitReturn(ReturnStatement& statement) = 0;
	};

	struct Statement
	{
		virtual ~Statement() = default;

		virtual StatementKind GetKind() const = 0;

		virtual void Visit(StatementVisitor& visitor) = 0;

		virtual std::string ToString() const = 0;
	};
	PET_DECLARE_UNIQ_PTR(Statement);

	template <StatementKind T>
	struct StatementBase : public Statement
	{
		StatementKind GetKind() const
		{
			return T;
		}
	};

	struct VariableDeclarationStatement final : public StatementBase<StatementKind::VariableDeclaration>
	{
		StringPoolId	  Id;
		ExpressionUniqPtr Value;

		VariableDeclarationStatement(StringPoolId id, ExpressionUniqPtr&& expression) : Id(id), Value(std::move(expression))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitVariableDeclaration(*this);
		}

		std::string ToString() const override;
	};

	struct FunctionDeclarationStatement final : public StatementBase<StatementKind::FunctionDeclaration>
	{
		StringPoolId				  Id;
		std::vector<StringPoolId>	  Parameters;
		std::vector<StatementUniqPtr> Body;

		FunctionDeclarationStatement(StringPoolId id, std::vector<StringPoolId>&& parameters, std::vector<StatementUniqPtr>&& body)
			: Id(id), Parameters(std::move(parameters)), Body(std::move(body))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitFunctionDeclaration(*this);
		}

		std::string ToString() const override;
	};

	struct ExpressionStatement final : public StatementBase<StatementKind::Expression>
	{
		ExpressionUniqPtr Expression;

		explicit ExpressionStatement(ExpressionUniqPtr&& expression) : Expression(std::move(expression))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitExpression(*this);
		}

		std::string ToString() const override;
	};

	struct BlockStatement final : public StatementBase<StatementKind::Block>
	{
		std::vector<StatementUniqPtr> Statements;

		explicit BlockStatement(std::vector<StatementUniqPtr>&& statements) : Statements(std::move(statements))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitBlock(*this);
		}

		std::string ToString() const override;
	};

	struct IfStatement final : public StatementBase<StatementKind::If>
	{
		ExpressionUniqPtr Condition;
		StatementUniqPtr  StatementTrue;
		StatementUniqPtr  StatementFalse;

		IfStatement(ExpressionUniqPtr&& condition, StatementUniqPtr&& statementTrue, StatementUniqPtr&& statementFalse)
			: Condition(std::move(condition)), StatementTrue(std::move(statementTrue)), StatementFalse(std::move(statementFalse))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitIf(*this);
		}

		std::string ToString() const override;
	};

	struct WhileStatement final : public StatementBase<StatementKind::While>
	{
		ExpressionUniqPtr Condition;
		StatementUniqPtr  Body;

		WhileStatement(ExpressionUniqPtr&& condition, StatementUniqPtr&& body) : Condition(std::move(condition)), Body(std::move(body))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitWhile(*this);
		}

		std::string ToString() const override;
	};

	struct BreakStatement final : public StatementBase<StatementKind::Break>
	{
		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitBreak(*this);
		}

		std::string ToString() const override;
	};

	struct ReturnStatement final : public StatementBase<StatementKind::Return>
	{
		ExpressionUniqPtr Value;

		explicit ReturnStatement(ExpressionUniqPtr&& value) : Value(std::move(value))
		{
		}

		void Visit(StatementVisitor& visitor) override
		{
			visitor.VisitReturn(*this);
		}

		std::string ToString() const override;
	};
}
