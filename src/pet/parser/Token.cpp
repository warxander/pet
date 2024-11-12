#include <pet/parser/Token.hpp>

#include <toolkit/StringBuilder.hpp>

namespace pet
{
	std::string Token::ToString() const
	{
		return StringBuilder() % "{ kind: " % Kind % ", value: " % Value % " }";
	}
}
