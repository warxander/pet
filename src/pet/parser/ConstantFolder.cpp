#include <pet/parser/ConstantFolder.hpp>

#include <pet/Statement.hpp>

#include <toolkit/StringUtils.hpp>

#include <cmath>

namespace pet
{
	Value ConstantFolder::FoldBinaryExpression(const ExpressionUniqPtr& left, TokenKind operator_, const ExpressionUniqPtr& right)
	{
		const auto leftValue = Fold(left);
		if (leftValue.IsNull())
			return Value();

		const auto rightValue = Fold(right);
		if (rightValue.IsNull())
			return Value();

		switch (operator_)
		{
		case TokenKind::Minus:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() - rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() - rightValue.AsFloat());
			}

			break;
		}
		case TokenKind::Asterisk:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() * rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() * rightValue.AsFloat());
			}

			break;
		}
		case TokenKind::Power:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(static_cast<ValueIntegerType>(std::pow(leftValue.AsInteger(), rightValue.AsInteger())));
				else
					return Value(std::pow(leftValue.AsFloat(), rightValue.AsFloat()));
			}

			break;
		}
		case TokenKind::Slash:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
				{
					const auto rightIntValue = rightValue.AsInteger();
					return rightIntValue == 0 ? Value() : Value(leftValue.AsInteger() / rightIntValue);
				}
				else
				{
					const auto rightFloatValue = rightValue.AsFloat();

					if (std::fpclassify(rightFloatValue) == FP_ZERO)
						return Value();
					else
						return Value(leftValue.AsFloat() / rightFloatValue);
				}
			}

			break;
		}
		case TokenKind::Plus:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() + rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() + rightValue.AsFloat());
			}
			else if (leftValue.IsString() && rightValue.IsString())
				return Value(StringUtils::Concat(leftValue.AsString(), rightValue.AsString()));

			break;
		}
		case TokenKind::Percent:
		{
			if (leftValue.IsInteger() && rightValue.IsInteger())
			{
				const auto rightIntValue = rightValue.AsInteger();
				return rightIntValue == 0 ? Value() : Value(leftValue.AsInteger() % rightIntValue);
			}

			break;
		}
		case TokenKind::GreaterThan:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() > rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() > rightValue.AsFloat());
			}

			break;
		}
		case TokenKind::GreaterThanOrEquals:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() >= rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() >= rightValue.AsFloat());
			}

			break;
		}
		case TokenKind::LessThan:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() < rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() < rightValue.AsFloat());
			}

			break;
		}
		case TokenKind::LessThanOrEquals:
		{
			if (leftValue.IsNumber() && rightValue.IsNumber())
			{
				if (leftValue.IsInteger() && rightValue.IsInteger())
					return Value(leftValue.AsInteger() <= rightValue.AsInteger());
				else
					return Value(leftValue.AsFloat() <= rightValue.AsFloat());
			}

			break;
		}
		case TokenKind::Equals:
		{
			if (leftValue.IsNull() && rightValue.IsNull())
				return Value(true);
			else if (leftValue.IsNull() || rightValue.IsNull())
				return Value(false);
			else if (leftValue.IsBoolean() && rightValue.IsBoolean())
				return Value(leftValue.AsBoolean() == rightValue.AsBoolean());
			else if (leftValue.IsInteger() && rightValue.IsInteger())
				return Value(leftValue.AsInteger() == rightValue.AsInteger());
			else if (leftValue.IsFloat() && rightValue.IsFloat())
				return Value(std::abs(rightValue.AsFloat() - leftValue.AsFloat()) <= std::numeric_limits<ValueFloatType>::epsilon());
			else if (leftValue.IsString() && rightValue.IsString())
				return Value(leftValue.AsString() == rightValue.AsString());
			break;
		}
		case TokenKind::NotEquals:
		{
			if (leftValue.IsNull() && rightValue.IsNull())
				return Value(false);
			else if (leftValue.IsNull() || rightValue.IsNull())
				return Value(true);
			else if (leftValue.IsBoolean() && rightValue.IsBoolean())
				return Value(leftValue.AsBoolean() != rightValue.AsBoolean());
			else if (leftValue.IsInteger() && rightValue.IsInteger())
				return Value(leftValue.AsInteger() != rightValue.AsInteger());
			else if (leftValue.IsString() && rightValue.IsString())
				return Value(leftValue.AsString() != rightValue.AsString());
			break;
		}
		default:
			break;
		}

		return Value();
	}

	Value ConstantFolder::FoldUnaryExpression(const ExpressionUniqPtr& right, TokenKind operator_)
	{
		const auto rightValue = Fold(right);
		if (rightValue.IsNull())
			return Value();

		switch (operator_)
		{
		case TokenKind::Minus:
			if (rightValue.IsNumber())
			{
				if (rightValue.IsInteger())
					return Value(-rightValue.AsInteger());
				else
					return Value(-rightValue.AsFloat());
			}

			break;

		case TokenKind::Bang:
			if (rightValue.IsBoolean())
				return Value(!rightValue.AsBoolean());

			break;

		default:
			break;
		}

		return Value();
	}

	Value ConstantFolder::FoldLogicalExpression(const ExpressionUniqPtr& left, TokenKind operator_, const ExpressionUniqPtr& right)
	{
		const auto leftValue = Fold(left);
		if (!leftValue.IsBoolean())
			return Value();

		const auto leftValueBoolean = leftValue.AsBoolean();

		if (operator_ == TokenKind::And && !leftValueBoolean)
			return Value(false);
		else if (operator_ == TokenKind::Or && leftValueBoolean)
			return Value(true);

		const auto rightValue = Fold(right);
		if (!rightValue.IsBoolean())
			return Value();

		switch (operator_)
		{
		case TokenKind::And:
			return Value(leftValueBoolean && rightValue.AsBoolean());
		case TokenKind::Or:
			return Value(leftValueBoolean || rightValue.AsBoolean());
		default:
			return Value();
		}
	}

	Value ConstantFolder::FoldGroupingExpression(const ExpressionUniqPtr& expression)
	{
		return Fold(expression);
	}

	Value ConstantFolder::Fold(const ExpressionUniqPtr& expression)
	{
		switch (expression->GetKind())
		{
		case ExpressionKind::Literal:
			return static_cast<LiteralExpression*>(expression.get())->Value;
		case ExpressionKind::Binary:
		{
			const auto binaryExpression = static_cast<BinaryExpression*>(expression.get());
			return FoldBinaryExpression(binaryExpression->Left, binaryExpression->Operator, binaryExpression->Right);
		}
		case ExpressionKind::Unary:
		{
			const auto unaryExpression = static_cast<UnaryExpression*>(expression.get());
			return FoldUnaryExpression(unaryExpression->Right, unaryExpression->Operator);
		}
		case ExpressionKind::Grouping:
			return Fold(static_cast<GroupingExpression*>(expression.get())->Expression);
		case ExpressionKind::Logical:
		{
			const auto logicalExpression = static_cast<LogicalExpression*>(expression.get());
			return FoldLogicalExpression(logicalExpression->Left, logicalExpression->Operator, logicalExpression->Right);
		}
		default:
			return Value();
		}
	}
}
