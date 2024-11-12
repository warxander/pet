#pragma once

#include <pet/parser/Token.hpp>

#include <toolkit/Macro.hpp>

namespace pet
{
	class Lexer
	{
		PET_NON_COPYABLE(Lexer);

	private:
		std::istream& _stream;
		Location	  _location;
		Token		  _buffer;

	public:
		explicit Lexer(std::istream& stream);

		const Location& GetLocation() const
		{
			return _location;
		}

		bool IsEndOfStream() const
		{
			return _buffer.Kind == TokenKind::EndOfStream;
		}

		const Token& PeekToken() const
		{
			return _buffer;
		}

		Token GetToken();

	private:
		std::string ReadIdentifier();
		Token		ReadNumber();
		std::string ReadString();
		Token		ReadToken();

		void SkipWhitespaces();
		bool TryReadChar(char& ch);
		bool TrySkipChar(char ch);

		void PutCharBack();
	};
}
