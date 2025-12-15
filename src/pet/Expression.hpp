#pragma once

#include <pet/parser/Token.hpp>
#include <pet/runtime/Value.hpp>

#include <toolkit/Macro.hpp>
#include <toolkit/StringPool.hpp>

namespace pet
{
	struct BinaryExpression;
	struct GroupingExpression;
	struct UnaryExpression;
	struct LiteralExpression;
	struct DictionaryExpression;
	struct ArrayExpression;
	struct MemberExpression;
	struct FunctionExpression;
	struct IdentifierExpression;
	struct AssignmentExpression;
	struct LogicalExpression;
	struct CallExpression;

	enum class ExpressionKind
	{
		Binary,
		Grouping,
		Unary,
		Literal,
		Dictionary,
		Array,
		Member,
		Function,
		Identifier,
		Assignment,
		Logical,
		Call
	};

	struct ExpressionVisitor
	{
		virtual ~ExpressionVisitor() = default;

		virtual void VisitBinary(BinaryExpression& expression) = 0;
		virtual void VisitGrouping(GroupingExpression& expression) = 0;
		virtual void VisitUnary(UnaryExpression& expression) = 0;
		virtual void VisitLiteral(LiteralExpression& expression) = 0;
		virtual void VisitDictionary(DictionaryExpression& expression) = 0;
		virtual void VisitArray(ArrayExpression& expression) = 0;
		virtual void VisitMember(MemberExpression& expression) = 0;
		virtual void VisitFunction(FunctionExpression& expression) = 0;
		virtual void VisitIdentifier(IdentifierExpression& expression) = 0;
		virtual void VisitAssignment(AssignmentExpression& expression) = 0;
		virtual void VisitLogical(LogicalExpression& expression) = 0;
		virtual void VisitCall(CallExpression& expression) = 0;
	};

	struct Statement;
	PET_DECLARE_UNIQ_PTR(Statement);

	struct Expression
	{
		virtual ~Expression() = default;

		virtual ExpressionKind GetKind() const = 0;

		virtual void Visit(ExpressionVisitor& visitor) = 0;

		virtual std::string ToString() const = 0;
	};
	PET_DECLARE_UNIQ_PTR(Expression);

	template <ExpressionKind T>
	struct ExpressionBase : public Expression
	{
		ExpressionKind GetKind() const override
		{
			return T;
		}
	};

	struct BinaryExpression final : public ExpressionBase<ExpressionKind::Binary>
	{
		ExpressionUniqPtr Left;
		TokenKind		  Operator;
		ExpressionUniqPtr Right;

		BinaryExpression(ExpressionUniqPtr&& left, TokenKind operator_, ExpressionUniqPtr&& right)
			: Left(std::move(left)), Operator(operator_), Right(std::move(right))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitBinary(*this);
		}

		std::string ToString() const override;
	};

	struct GroupingExpression final : public ExpressionBase<ExpressionKind::Grouping>
	{
		ExpressionUniqPtr Expression;

		explicit GroupingExpression(ExpressionUniqPtr&& expression) : Expression(std::move(expression))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitGrouping(*this);
		}

		std::string ToString() const override;
	};

	struct UnaryExpression final : public ExpressionBase<ExpressionKind::Unary>
	{
		TokenKind		  Operator;
		ExpressionUniqPtr Right;

		UnaryExpression(TokenKind operator_, ExpressionUniqPtr&& right) : Operator(operator_), Right(std::move(right))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitUnary(*this);
		}

		std::string ToString() const override;
	};

	struct LiteralExpression final : public ExpressionBase<ExpressionKind::Literal>
	{
		pet::Value Value;

		explicit LiteralExpression(pet::Value&& value) : Value(std::move(value))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitLiteral(*this);
		}

		std::string ToString() const override;
	};

	struct DictionaryExpression final : public ExpressionBase<ExpressionKind::Dictionary>
	{
		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitDictionary(*this);
		}

		std::string ToString() const override;
	};

	struct ArrayExpression final : public ExpressionBase<ExpressionKind::Array>
	{
		std::vector<ExpressionUniqPtr> Values;

		explicit ArrayExpression(std::vector<ExpressionUniqPtr>&& values) : Values(std::move(values))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitArray(*this);
		}

		std::string ToString() const override;
	};

	struct MemberExpression final : public ExpressionBase<ExpressionKind::Member>
	{
		ExpressionUniqPtr Target;
		ExpressionUniqPtr Key;

		MemberExpression(ExpressionUniqPtr&& target, ExpressionUniqPtr&& key) : Target(std::move(target)), Key(std::move(key))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitMember(*this);
		}

		std::string ToString() const override;
	};

	struct FunctionExpression final : public ExpressionBase<ExpressionKind::Function>
	{
		std::vector<StringPoolId>	  Parameters;
		std::vector<StatementUniqPtr> Body;

		FunctionExpression(std::vector<StringPoolId>&& parameters, std::vector<StatementUniqPtr>&& body)
			: Parameters(std::move(parameters)), Body(std::move(body))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitFunction(*this);
		}

		std::string ToString() const override;
	};

	struct AssignmentExpression final : public ExpressionBase<ExpressionKind::Assignment>
	{
		ExpressionUniqPtr Target;
		ExpressionUniqPtr Value;

		AssignmentExpression(ExpressionUniqPtr&& target, ExpressionUniqPtr&& value) : Target(std::move(target)), Value(std::move(value))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitAssignment(*this);
		}

		std::string ToString() const override;
	};

	struct IdentifierExpression final : public ExpressionBase<ExpressionKind::Identifier>
	{
		StringPoolId Id;

		explicit IdentifierExpression(StringPoolId id) : Id(id)
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitIdentifier(*this);
		}

		std::string ToString() const override;
	};

	struct LogicalExpression final : public ExpressionBase<ExpressionKind::Logical>
	{
		ExpressionUniqPtr Left;
		TokenKind		  Operator;
		ExpressionUniqPtr Right;

		LogicalExpression(ExpressionUniqPtr&& left, TokenKind operator_, ExpressionUniqPtr&& right)
			: Left(std::move(left)), Operator(operator_), Right(std::move(right))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitLogical(*this);
		}

		std::string ToString() const override;
	};

	struct CallExpression final : public ExpressionBase<ExpressionKind::Call>
	{
		ExpressionUniqPtr			   Callee;
		std::vector<ExpressionUniqPtr> Arguments;

		CallExpression(ExpressionUniqPtr&& callee, std::vector<ExpressionUniqPtr>&& arguments)
			: Callee(std::move(callee)), Arguments(std::move(arguments))
		{
		}

		void Visit(ExpressionVisitor& visitor) override
		{
			visitor.VisitCall(*this);
		}

		std::string ToString() const override;
	};
}
