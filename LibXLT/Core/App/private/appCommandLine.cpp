/****************************************************************************\
**  appCommandLine.cpp
**
**      See appCommandLine.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef APP_COMMANDLINE_HPP
#include "Core/App/appCommandLine.hpp"
#endif

namespace
{
	bool is_space(char i_Char)
	{
		return i_Char == ' ' || i_Char == '\t' || i_Char == '\n' || i_Char == '\r';
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector<std::string> appCommandLine::Tokenize(const std::string& i_Line)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	const size_t length = i_Line.length();
	while (i < length)
	{
		if (is_space(i_Line[i]))
		{
			++i;
			continue;
		}

		std::string token;
		if (i_Line[i] == '"')
		{
			size_t end = i_Line.find('"', i + 1);
			if (end == std::string::npos)
			{
				end = length;
			}
			token = i_Line.substr(i + 1, end - i - 1);
			i = end + 1;
		}
		else
		{
			size_t end = i;
			while (end < length && !is_space(i_Line[end]))
			{
				++end;
			}
			token = i_Line.substr(i, end - i);
			i = end;
		}
		tokens.push_back(token);
	}
	return tokens;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool appCommandLine::IsFlag(const std::string& i_Token, const char* i_Name)
{
	if (i_Token.length() < 2 || (i_Token[0] != '-' && i_Token[0] != '/'))
	{
		return false;
	}
	return i_Token.compare(1, std::string::npos, i_Name) == 0;
}
