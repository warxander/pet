#pragma once

#include <pet/Location.hpp>

namespace pet
{
	enum class TokenKind
	{
		// Single character
		Comma,
		LeftBrace,
		LeftBracket,
		LeftParenthesis,
		RightBrace,
		RightBracket,
		RightParenthesis,
		Semicolon,

		// Operators
		Assign,
		Asterisk,
		Bang,
		Dot,
		Equals,
		GreaterThan,
		GreaterThanOrEquals,
		LessThan,
		LessThanOrEquals,
		Minus,
		NotEquals,
		Percent,
		Plus,
		Slash,
		Power,

		// Literals
		Identifier,
		Integer,
		Number,
		String,

		// Keywords
		And,
		Break,
		Else,
		If,
		False,
		Fun,
		Null,
		Or,
		Return,
		True,
		Var,
		While,

		EndOfStream,
		Invalid
	};

	struct Token
	{
		TokenKind	Kind;
		std::string Value;

	public:
		explicit Token(TokenKind kind = TokenKind::Invalid) : Kind(kind)
		{
		}

		Token(TokenKind kind, std::string&& value) : Kind(kind), Value(std::move(value))
		{
		}

		std::string ToString() const;
	};
}
