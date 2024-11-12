#include <pet/Expression.hpp>

#include <pet/Statement.hpp>

#include <toolkit/StringJoiner.hpp>

namespace pet
{
	namespace
	{
		std::string_view OperatorToString(TokenKind tokenKind)
		{
			switch (tokenKind)
			{
			case TokenKind::Plus:
				return "+";
			case TokenKind::Minus:
				return "-";
			case TokenKind::Asterisk:
				return "*";
			case TokenKind::Slash:
				return "/";
			case TokenKind::Percent:
				return "%";
			case TokenKind::Power:
				return "**";
			case TokenKind::Equals:
				return "==";
			case TokenKind::NotEquals:
				return "!=";
			case TokenKind::GreaterThan:
				return ">";
			case TokenKind::GreaterThanOrEquals:
				return ">=";
			case TokenKind::LessThan:
				return "<";
			default:
				PET_THROW(NotImplementedException());
			}
		}
	}

	std::string BinaryExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", left: " % Left % ", operator: " % OperatorToString(Operator) % ", right: " %
			   Right % " }";
	}

	std::string GroupingExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", expression: " % Expression % " }";
	}

	std::string UnaryExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", operator: " % OperatorToString(Operator) % ", right: " % Right % " }";
	}

	std::string LiteralExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", value: " % Value % " }";
	}

	std::string DictionaryExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % " }";
	}

	std::string ArrayExpression::ToString() const
	{
		StringBuilder sb;
		sb % "{ kind: " % GetKind() % ", values: [ ";

		StringJoiner sj;
		for (const auto& value : Values) sj % value;

		return sb % sj % " ]";
	}

	std::string MemberExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", target: " % Target % ", key: " % Key % " }";
	}

	std::string FunctionExpression::ToString() const
	{
		StringBuilder sb;

		sb % "{ kind: " % GetKind() % ", parameters: [";

		{
			StringJoiner sj;
			for (const auto& parameter : Parameters) sj % parameter;
			sb % sj % "], body: [";
		}

		StringJoiner sj(" ");
		for (const auto& statement : Body) sj % statement;

		return sb % sj % " ] }";
	}

	std::string IdentifierExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", id: " % Id % " }";
	}

	std::string AssignmentExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", target: " % Target % ", value: " % Value % " }";
	}

	std::string LogicalExpression::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", left: " % Left % ", operator: " % OperatorToString(Operator) % ", right: " %
			   Right % " }";
	}

	std::string CallExpression::ToString() const
	{
		StringBuilder sb;

		sb % "{ kind: " % GetKind() % ", callee: " % Callee % ", arguments: [";

		StringJoiner sj;
		for (const auto& argument : Arguments) sj % argument;

		return sb % sj % " ] }";
	}
}
