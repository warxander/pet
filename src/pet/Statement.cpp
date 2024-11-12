#include <pet/Statement.hpp>

#include <toolkit/StringJoiner.hpp>

namespace pet
{
	std::string VariableDeclarationStatement::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", id: " % Id % ", value: " % Value % " }";
	}

	std::string FunctionDeclarationStatement::ToString() const
	{
		StringBuilder sb;

		sb % "{ kind: " % GetKind() % ", id: " % Id % ", parameters: [";

		{
			StringJoiner sj;
			for (const auto& parameter : Parameters) sj % parameter;
			sb % sj % "], body: [";
		}

		StringJoiner sj(" ");
		for (const auto& statement : Body) sj % statement;

		return sb % sj % " ] }";
	}

	std::string ExpressionStatement::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", expression: " % Expression % " }";
	}

	std::string BlockStatement::ToString() const
	{
		StringBuilder sb;
		sb % "{ kind: " % GetKind() % ", statements: [";

		StringJoiner sj(" ");
		for (const auto& statement : Statements) sj % statement;

		return sb % sj % " ] }";
	}

	std::string IfStatement::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", condition: " % Condition % ", statementTrue: " % StatementTrue %
			   ", statementFalse: " % StatementFalse % " }";
	}

	std::string WhileStatement::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", condition: " % Condition % ", body: " % Body % " }";
	}

	std::string BreakStatement::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % " }";
	}

	std::string ReturnStatement::ToString() const
	{
		return StringBuilder() % "{ kind: " % GetKind() % ", value: " % Value % " }";
	}
}
