#include <pet/parser/Parser.hpp>

#include <pet/parser/ConstantFolder.hpp>

#include <pet/Error.hpp>

namespace pet
{
	Parser::Parser(Context& context, std::istream& stream) : _context(context), _lexer(stream)
	{
	}

	StatementUniqPtr Parser::GetStatement()
	{
		return _lexer.IsEndOfStream() ? nullptr : ParseStatement();
	}

	StatementUniqPtr Parser::ParseStatement()
	{
		if (TryGetToken(TokenKind::Var))
			return ParseVariableDeclarationStatement();
		else if (TryGetToken(TokenKind::Fun))
			return ParseFunctionDeclarationStatement();
		else if (TryGetToken(TokenKind::If))
			return ParseIfStatement();
		else if (TryGetToken(TokenKind::While))
			return ParseWhileStatement();
		else if (TryGetToken(TokenKind::Break))
			return ParseBreakStatement();
		else if (TryGetToken(TokenKind::Return))
			return ParseReturnStatement();
		else
			return ParseExpressionStatement();
	}

	StatementUniqPtr Parser::ParseVariableDeclarationStatement()
	{
		auto nameToken = _lexer.GetToken();
		PET_CHECK(nameToken.Kind == TokenKind::Identifier, SyntaxError(_lexer.GetLocation(), "Expect variable name"));

		ExpressionUniqPtr expression;

		if (TryGetToken(TokenKind::Assign))
			expression = ParseExpression();
		else
			expression = std::make_unique<LiteralExpression>(Value());

		PET_CHECK(TryGetToken(TokenKind::Semicolon), SyntaxError(_lexer.GetLocation(), "Expect ';' after variable declaration"));
		return std::make_unique<VariableDeclarationStatement>(_context.GetIdentifierPool().Add(std::move(nameToken.Value)),
															  std::move(expression));
	}

	StatementUniqPtr Parser::ParseFunctionDeclarationStatement()
	{
		auto nameToken = _lexer.GetToken();
		PET_CHECK(nameToken.Kind == TokenKind::Identifier, SyntaxError(_lexer.GetLocation(), "Expect function name"));

		PET_CHECK(TryGetToken(TokenKind::LeftParenthesis), SyntaxError(_lexer.GetLocation(), "Expect '(' after function name"));

		std::vector<StringPoolId> parameters;
		Token					  paramToken;

		if (_lexer.PeekToken().Kind != TokenKind::RightParenthesis)
			do
			{
				PET_CHECK(TryGetToken(paramToken, TokenKind::Identifier),
						  SyntaxError(_lexer.GetLocation(), "Expect function parameter name or ')'"));
				parameters.emplace_back(_context.GetIdentifierPool().Add(std::move(paramToken.Value)));
			} while (TryGetToken(TokenKind::Comma));

		PET_CHECK(TryGetToken(TokenKind::RightParenthesis), SyntaxError(_lexer.GetLocation(), "Expect ')' after function parameter list"));

		PET_CHECK(TryGetToken(TokenKind::LeftBrace), SyntaxError(_lexer.GetLocation(), "Expect '{' before function body"));
		auto body = ParseBlock();

		return std::make_unique<FunctionDeclarationStatement>(_context.GetIdentifierPool().Add(std::move(nameToken.Value)),
															  std::move(parameters), std::move(body));
	}

	StatementUniqPtr Parser::ParseIfStatement()
	{
		PET_CHECK(TryGetToken(TokenKind::LeftParenthesis), SyntaxError(_lexer.GetLocation(), "Expect '(' after 'if'"));
		auto condition = ParseExpression();
		PET_CHECK(TryGetToken(TokenKind::RightParenthesis), SyntaxError(_lexer.GetLocation(), "Expect ')' after 'if' condition"));

		auto statementTrue = ParseStatement();
		auto statementFalse = TryGetToken(TokenKind::Else) ? ParseStatement() : nullptr;

		return std::make_unique<IfStatement>(std::move(condition), std::move(statementTrue), std::move(statementFalse));
	}

	StatementUniqPtr Parser::ParseWhileStatement()
	{
		PET_CHECK(TryGetToken(TokenKind::LeftParenthesis), SyntaxError(_lexer.GetLocation(), "Expect '(' after 'while'"));
		auto condition = ParseExpression();
		PET_CHECK(TryGetToken(TokenKind::RightParenthesis), SyntaxError(_lexer.GetLocation(), "Expect ')' after 'while' condition"));

		auto body = ParseStatement();

		return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
	}

	StatementUniqPtr Parser::ParseBreakStatement()
	{
		PET_CHECK(TryGetToken(TokenKind::Semicolon), SyntaxError(_lexer.GetLocation(), "Expect ';' after 'break'"));
		return std::make_unique<BreakStatement>();
	}

	StatementUniqPtr Parser::ParseReturnStatement()
	{
		auto result = _lexer.PeekToken().Kind != TokenKind::Semicolon ? ParseExpression() : nullptr;

		PET_CHECK(TryGetToken(TokenKind::Semicolon), SyntaxError(_lexer.GetLocation(), "Expect ';' after 'return'"));

		return std::make_unique<ReturnStatement>(std::move(result));
	}

	StatementUniqPtr Parser::ParseExpressionStatement()
	{
		if (TryGetToken(TokenKind::LeftBrace))
			return std::make_unique<BlockStatement>(ParseBlock());

		auto result = ParseExpression();
		PET_CHECK(TryGetToken(TokenKind::Semicolon), SyntaxError(_lexer.GetLocation(), "Expect ';' after expression"));

		return std::make_unique<ExpressionStatement>(std::move(result));
	}

	std::vector<StatementUniqPtr> Parser::ParseBlock()
	{
		std::vector<StatementUniqPtr> result;

		while (_lexer.PeekToken().Kind != TokenKind::EndOfStream)
		{
			if (TryGetToken(TokenKind::RightBrace))
				return result;

			result.emplace_back(ParseStatement());
		}

		PET_THROW(SyntaxError(_lexer.GetLocation(), "Expect '}' after block"));
	}

	ExpressionUniqPtr Parser::ParseExpression()
	{
		return ParseAssignment();
	}

	ExpressionUniqPtr Parser::ParseAssignment()
	{
		auto result = ParseOr();

		if (TryGetToken(TokenKind::Assign))
			result = std::make_unique<AssignmentExpression>(std::move(result), ParseAssignment());

		return result;
	}

	ExpressionUniqPtr Parser::ParseOr()
	{
		auto result = ParseAnd();

		while (TryGetToken(TokenKind::Or))
		{
			auto right = ParseAnd();

			auto foldedValue = ConstantFolder::FoldLogicalExpression(result, TokenKind::Or, right);
			if (!foldedValue.IsNull())
				result = std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				result = std::make_unique<LogicalExpression>(std::move(result), TokenKind::Or, std::move(right));
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParseAnd()
	{
		auto result = ParseEquality();

		while (TryGetToken(TokenKind::And))
		{
			auto right = ParseEquality();

			auto foldedValue = ConstantFolder::FoldLogicalExpression(result, TokenKind::And, right);
			if (!foldedValue.IsNull())
				result = std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				result = std::make_unique<LogicalExpression>(std::move(result), TokenKind::And, std::move(right));
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParseEquality()
	{
		auto  result = ParseComparison();
		Token token;

		static const std::unordered_set<TokenKind> equalityTokenKinds = {TokenKind::Equals, TokenKind::NotEquals};
		while (TryGetToken(token, equalityTokenKinds))
		{
			auto right = ParseComparison();

			auto foldedValue = ConstantFolder::FoldBinaryExpression(result, token.Kind, right);
			if (!foldedValue.IsNull())
				result = std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				result = std::make_unique<BinaryExpression>(std::move(result), token.Kind, std::move(right));
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParseComparison()
	{
		auto  result = ParseTerm();
		Token token;

		static const std::unordered_set<TokenKind> comparisonTokenKinds = {TokenKind::GreaterThan, TokenKind::GreaterThanOrEquals,
																		   TokenKind::LessThan, TokenKind::LessThanOrEquals};
		while (TryGetToken(token, comparisonTokenKinds))
		{
			auto right = ParseTerm();

			auto foldedValue = ConstantFolder::FoldBinaryExpression(result, token.Kind, right);
			if (!foldedValue.IsNull())
				result = std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				result = std::make_unique<BinaryExpression>(std::move(result), token.Kind, std::move(right));
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParseTerm()
	{
		auto  result = ParseFactor();
		Token token;

		static const std::unordered_set<TokenKind> termTokenKinds = {TokenKind::Minus, TokenKind::Plus};
		while (TryGetToken(token, termTokenKinds))
		{
			auto right = ParseFactor();

			auto foldedValue = ConstantFolder::FoldBinaryExpression(result, token.Kind, right);
			if (!foldedValue.IsNull())
				result = std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				result = std::make_unique<BinaryExpression>(std::move(result), token.Kind, std::move(right));
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParseFactor()
	{
		auto  result = ParseUnary();
		Token token;

		static const std::unordered_set<TokenKind> factorTokenKinds = {TokenKind::Slash, TokenKind::Asterisk, TokenKind::Percent,
																	   TokenKind::Power};
		while (TryGetToken(token, factorTokenKinds))
		{
			auto right = ParseUnary();

			auto foldedValue = ConstantFolder::FoldBinaryExpression(result, token.Kind, right);
			if (!foldedValue.IsNull())
				result = std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				result = std::make_unique<BinaryExpression>(std::move(result), token.Kind, std::move(right));
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParseUnary()
	{
		Token token;

		static const std::unordered_set<TokenKind> unaryTokenKinds = {TokenKind::Bang, TokenKind::Minus};
		if (TryGetToken(token, unaryTokenKinds))
		{
			auto right = ParseUnary();

			auto foldedValue = ConstantFolder::FoldUnaryExpression(right, token.Kind);
			if (!foldedValue.IsNull())
				return std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				return std::make_unique<UnaryExpression>(token.Kind, std::move(right));
		}

		return ParseCall();
	}

	ExpressionUniqPtr Parser::ParseCall()
	{
		auto result = ParsePrimary();

		while (true)
		{
			if (TryGetToken(TokenKind::LeftParenthesis))
			{
				std::vector<ExpressionUniqPtr> arguments;

				if (_lexer.PeekToken().Kind != TokenKind::RightParenthesis)
					do arguments.emplace_back(ParseExpression());
					while (TryGetToken(TokenKind::Comma));

				PET_CHECK(TryGetToken(TokenKind::RightParenthesis), SyntaxError(_lexer.GetLocation(), "Expect ')' after arguments"));

				result = std::make_unique<CallExpression>(std::move(result), std::move(arguments));
			}
			else if (TryGetToken(TokenKind::Dot))
			{
				auto nameToken = _lexer.GetToken();
				PET_CHECK(nameToken.Kind == TokenKind::Identifier, SyntaxError(_lexer.GetLocation(), "Expect property name after '.'"));

				result = std::make_unique<MemberExpression>(std::move(result),
															std::make_unique<LiteralExpression>(Value(std::move(nameToken.Value))));
			}
			else if (TryGetToken(TokenKind::LeftBracket))
			{
				auto index = ParseExpression();

				PET_CHECK(TryGetToken(TokenKind::RightBracket), SyntaxError(_lexer.GetLocation(), "Expect ']' after index"));

				result = std::make_unique<MemberExpression>(std::move(result), std::move(index));
			}
			else
				break;
		}

		return result;
	}

	ExpressionUniqPtr Parser::ParsePrimary()
	{
		Token token;

		if (TryGetToken(token, TokenKind::False))
			return std::make_unique<LiteralExpression>(Value(false));

		if (TryGetToken(token, TokenKind::True))
			return std::make_unique<LiteralExpression>(Value(true));

		if (TryGetToken(token, TokenKind::Null))
			return std::make_unique<LiteralExpression>(Value());

		if (TryGetToken(TokenKind::LeftParenthesis))
		{
			auto expression = ParseExpression();
			token = _lexer.GetToken();
			PET_CHECK(token.Kind == TokenKind::RightParenthesis, SyntaxError(_lexer.GetLocation(), "Expect ')' after expression"));

			auto foldedValue = ConstantFolder::FoldGroupingExpression(expression);
			if (!foldedValue.IsNull())
				return std::make_unique<LiteralExpression>(std::move(foldedValue));
			else
				return std::make_unique<GroupingExpression>(std::move(expression));
		}

		if (TryGetToken(TokenKind::LeftBrace))
		{
			token = _lexer.GetToken();
			PET_CHECK(token.Kind == TokenKind::RightBrace, SyntaxError(_lexer.GetLocation(), "Expect '}' after expression"));
			return std::make_unique<DictionaryExpression>();
		}

		if (TryGetToken(TokenKind::LeftBracket))
		{
			std::vector<ExpressionUniqPtr> values;

			if (_lexer.PeekToken().Kind != TokenKind::RightBracket)
				do values.emplace_back(ParseExpression());
				while (TryGetToken(TokenKind::Comma));

			PET_CHECK(TryGetToken(TokenKind::RightBracket), SyntaxError(_lexer.GetLocation(), "Expect ']' after values"));

			return std::make_unique<ArrayExpression>(std::move(values));
		}

		if (TryGetToken(token, TokenKind::Integer))
			return std::make_unique<LiteralExpression>(Value(std::stoll(token.Value)));

		if (TryGetToken(token, TokenKind::Number))
			return std::make_unique<LiteralExpression>(Value(std::stod(token.Value)));

		if (TryGetToken(token, TokenKind::String))
			return std::make_unique<LiteralExpression>(Value(std::move(token.Value)));

		if (TryGetToken(token, TokenKind::Identifier))
			return std::make_unique<IdentifierExpression>(_context.GetIdentifierPool().Add(std::move(token.Value)));

		if (TryGetToken(TokenKind::Fun))
		{
			PET_CHECK(TryGetToken(TokenKind::LeftParenthesis), SyntaxError(_lexer.GetLocation(), "Expect '(' after function name"));

			std::vector<StringPoolId> parameters;
			Token					  paramToken;

			if (_lexer.PeekToken().Kind != TokenKind::RightParenthesis)
				do
				{
					PET_CHECK(TryGetToken(paramToken, TokenKind::Identifier),
							  SyntaxError(_lexer.GetLocation(), "Expect function parameter name or ')'"));
					parameters.emplace_back(_context.GetIdentifierPool().Add(std::move(paramToken.Value)));
				} while (TryGetToken(TokenKind::Comma));

			PET_CHECK(TryGetToken(TokenKind::RightParenthesis),
					  SyntaxError(_lexer.GetLocation(), "Expect ')' after function parameter list"));

			PET_CHECK(TryGetToken(TokenKind::LeftBrace), SyntaxError(_lexer.GetLocation(), "Expect '{' before function body"));
			auto body = ParseBlock();

			return std::make_unique<FunctionExpression>(std::move(parameters), std::move(body));
		}

		PET_THROW(TypeError(_lexer.GetLocation(), StringBuilder() % "Expect expression, got '" % _lexer.PeekToken().Kind % "'"));
	}

	bool Parser::TryGetToken(TokenKind tokenKind)
	{
		if (_lexer.PeekToken().Kind == tokenKind)
		{
			_lexer.GetToken();
			return true;
		}

		return false;
	}

	bool Parser::TryGetToken(Token& token, TokenKind tokenKind)
	{
		if (_lexer.PeekToken().Kind == tokenKind)
		{
			token = _lexer.GetToken();
			return true;
		}

		return false;
	}

	bool Parser::TryGetToken(Token& token, const std::unordered_set<TokenKind>& tokenKinds)
	{
		if (tokenKinds.count(_lexer.PeekToken().Kind))
		{
			token = _lexer.GetToken();
			return true;
		}

		return false;
	}
}
