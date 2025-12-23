#pragma once

#include <pet/parser/Lexer.hpp>

#include <pet/Context.hpp>
#include <pet/Expression.hpp>
#include <pet/Statement.hpp>

#include <unordered_set>

namespace pet
{
	class Parser
	{
		PET_NON_COPYABLE(Parser);

	private:
		Context& _context;
		Lexer	 _lexer;

	public:
		Parser(Context& context, std::istream& stream);

		bool IsEndOfStream() const
		{
			return _lexer.IsEndOfStream();
		}

		StatementUniqPtr GetStatement();

	private:
		StatementUniqPtr ParseStatement();
		StatementUniqPtr ParseVariableDeclarationStatement(bool isConst);
		StatementUniqPtr ParseFunctionDeclarationStatement();
		StatementUniqPtr ParseIfStatement();
		StatementUniqPtr ParseWhileStatement();
		StatementUniqPtr ParseBreakStatement();
		StatementUniqPtr ParseReturnStatement();
		StatementUniqPtr ParseExpressionStatement();

		std::vector<StatementUniqPtr> ParseBlock();
		ExpressionUniqPtr			  ParseExpression();
		ExpressionUniqPtr			  ParseAssignment();
		ExpressionUniqPtr			  ParseOr();
		ExpressionUniqPtr			  ParseAnd();
		ExpressionUniqPtr			  ParseEquality();
		ExpressionUniqPtr			  ParseComparison();
		ExpressionUniqPtr			  ParseTerm();
		ExpressionUniqPtr			  ParseFactor();

		ExpressionUniqPtr ParseUnary();
		ExpressionUniqPtr ParseCall();
		ExpressionUniqPtr ParsePrimary();

		bool TryGetToken(TokenKind tokenKind);
		bool TryGetToken(Token& token, TokenKind tokenKind);
		bool TryGetToken(Token& token, const std::unordered_set<TokenKind>& tokenKinds);
	};
}
