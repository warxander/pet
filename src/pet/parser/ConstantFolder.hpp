#pragma once

#include <pet/Expression.hpp>

namespace pet
{
	struct ConstantFolder
	{
		static Value FoldBinaryExpression(const ExpressionUniqPtr& left, TokenKind operator_, const ExpressionUniqPtr& right);
		static Value FoldUnaryExpression(const ExpressionUniqPtr& expression, TokenKind operator_);
		static Value FoldLogicalExpression(const ExpressionUniqPtr& left, TokenKind operator_, const ExpressionUniqPtr& right);
		static Value FoldGroupingExpression(const ExpressionUniqPtr& expression);

	private:
		static Value Fold(const ExpressionUniqPtr& expression);
	};
}
