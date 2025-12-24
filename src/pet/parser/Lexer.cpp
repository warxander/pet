#include <pet/parser/Lexer.hpp>

#include <pet/Error.hpp>

#include <toolkit/Macro.hpp>

#include <unordered_map>

namespace pet
{
	namespace
	{
		TokenKind GetTokenKindFromKeyword(std::string_view keyword, TokenKind defaultType = TokenKind::Identifier)
		{
			static const std::unordered_map<std::string_view, TokenKind> KeywordsToTokenKinds = {
				{"and", TokenKind::And},	 {"break", TokenKind::Break},	{"continue", TokenKind::Continue}, {"else", TokenKind::Else},
				{"if", TokenKind::If},		 {"false", TokenKind::False},	{"fun", TokenKind::Fun},		   {"null", TokenKind::Null},
				{"or", TokenKind::Or},		 {"return", TokenKind::Return}, {"true", TokenKind::True},		   {"var", TokenKind::Var},
				{"while", TokenKind::While}, {"const", TokenKind::Const}};

			const auto it = KeywordsToTokenKinds.find(keyword);
			return it != KeywordsToTokenKinds.end() ? it->second : defaultType;
		}
	}

	Lexer::Lexer(std::istream& stream) : _stream(stream), _location{1, 0}, _buffer(ReadToken())
	{
	}

	Token Lexer::GetToken()
	{
		auto result = std::move(_buffer);
		_buffer = ReadToken();
		return result;
	}

	std::string Lexer::ReadIdentifier()
	{
		std::string result;
		char		ch;

		while (TryReadChar(ch))
		{
			if (!result.empty() ? std::isalnum(ch) : std::isalpha(ch))
				result += ch;
			else
			{
				PutCharBack();
				break;
			}
		}

		return result;
	}

	Token Lexer::ReadNumber()
	{
		std::string result;
		char		ch;

		bool hasNumbers = false;
		bool hasDot = false;

		while (TryReadChar(ch))
		{
			if (std::isdigit(ch))
			{
				hasNumbers = true;
				result += ch;
			}
			else if (ch == '.')
			{
				PET_CHECK(hasNumbers && !hasDot, SyntaxError(StringBuilder() % _location % ": Unexpected '.' in number"));
				hasDot = true;
				result += ch;
			}
			else
			{
				PutCharBack();
				break;
			}
		}

		PET_CHECK(hasNumbers, SyntaxError(StringBuilder() % _location % ": Invalid number format"));
		return Token(hasDot ? TokenKind::Number : TokenKind::Integer, std::move(result));
	}

	std::string Lexer::ReadString()
	{
		StringBuilder sb;
		char		  ch;

		while (TryReadChar(ch))
		{
			if (ch == '\\')
			{
				PET_CHECK(TryReadChar(ch), SyntaxError(StringBuilder() % _location % ": Unterminated escape character"));

				switch (ch)
				{
				case 'f':
					sb % '\f';
					break;
				case 'n':
					sb % '\n';
					break;
				case 'r':
					sb % '\r';
					break;
				case 't':
					sb % '\t';
					break;
				case 'v':
					sb % '\v';
					break;
				case '\\':
					sb % '\\';
					break;
				case '"':
					sb % '"';
					break;
				default:
					PET_THROW(SyntaxError(StringBuilder() % _location % ": Invalid escape character '\\" % ch % "'"));
				}
			}
			else if (ch != '"')
				sb % ch;
			else
				break;
		}

		PET_CHECK(sb.IsEmpty() || ch == '"', SyntaxError(StringBuilder() % _location % ": Unterminated string"));

		return sb;
	}

	Token Lexer::ReadToken()
	{
		SkipWhitespaces();

		char ch;
		if (!TryReadChar(ch))
			return Token(TokenKind::EndOfStream);

		switch (ch)
		{
		case ',':
			return Token(TokenKind::Comma);
		case '(':
			return Token(TokenKind::LeftParenthesis);
		case ')':
			return Token(TokenKind::RightParenthesis);
		case '[':
			return Token(TokenKind::LeftBracket);
		case ']':
			return Token(TokenKind::RightBracket);
		case '{':
			return Token(TokenKind::LeftBrace);
		case '}':
			return Token(TokenKind::RightBrace);
		case ';':
			return Token(TokenKind::Semicolon);
		case '=':
		{
			if (TrySkipChar('='))
				return Token(TokenKind::Equals);
			else
				return Token(TokenKind::Assign);
		}
		case '*':
		{
			if (TrySkipChar('*'))
				return Token(TokenKind::Power);
			else
				return Token(TokenKind::Asterisk);
		}
		case '!':
		{
			if (TrySkipChar('='))
				return Token(TokenKind::NotEquals);
			else
				return Token(TokenKind::Bang);
		}
		case '>':
		{
			if (TrySkipChar('='))
				return Token(TokenKind::GreaterThanOrEquals);
			else
				return Token(TokenKind::GreaterThan);
		}
		case '<':
		{
			if (TrySkipChar('='))
				return Token(TokenKind::LessThanOrEquals);
			else
				return Token(TokenKind::LessThan);
		}
		case '-':
			return Token(TokenKind::Minus);
		case '+':
			return Token(TokenKind::Plus);
		case '%':
			return Token(TokenKind::Percent);
		case '/':
			return Token(TokenKind::Slash);
		case '.':
			return Token(TokenKind::Dot);
		case '"':
			return Token(TokenKind::String, ReadString());
		default:
		{
			PutCharBack();

			if (std::isalpha(ch))
			{
				auto	   identifier = ReadIdentifier();
				const auto tokenKind = GetTokenKindFromKeyword(identifier);
				return tokenKind == TokenKind::Identifier ? Token(tokenKind, std::move(identifier)) : Token(tokenKind);
			}
			else if (std::isdigit(ch))
				return ReadNumber();
			else
				PET_THROW(SyntaxError(StringBuilder() % _location % ": Invalid token '" % ch % "'"));
		}
		}
	}

	void Lexer::SkipWhitespaces()
	{
		char ch;
		bool isComment = false;

		while (TryReadChar(ch))
		{
			if (ch == '#')
				isComment = true;
			else if (!isComment && !std::isspace(ch))
			{
				PutCharBack();
				break;
			}
			else if (ch == '\n')
			{
				++_location.Line;
				_location.Column = 0;
				isComment = false;
			}
		}
	}

	bool Lexer::TryReadChar(char& ch)
	{
		_stream.get(ch);

		if (!_stream)
			return false;

		++_location.Column;
		return true;
	}

	bool Lexer::TrySkipChar(char ch)
	{
		if (_stream.eof() || _stream.peek() != ch)
			return false;

		PET_CHECK(_stream.seekg(1, std::ios::cur), InputOutputException());	 // TODO: Use internal buffer instead
		++_location.Column;

		return true;
	}

	void Lexer::PutCharBack()
	{
		PET_CHECK(_stream.unget(), InputOutputException());	 // TODO: Use internal buffer instead
		--_location.Column;
	}
}
