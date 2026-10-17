/****************************************************************************\
**  appCommandLine.hpp
**
**      appCommandLine splits a raw command line into tokens and matches
**	flags. It knows nothing about any particular application's flags.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef APP_COMMANDLINE_HPP
#error appCommandLine.hpp multiply included
#endif
#define APP_COMMANDLINE_HPP

#include <string>
#include <vector>

//============================================================================
//============================================================================
class appCommandLine
{
	public:
		//--------------------------------------------------------------------
		//	Tokenize - splits on spaces, tabs and newlines. "..." is one
		//	token with the quotes removed (spaces inside are kept; an
		//	unterminated quote runs to the end of the line).
		//--------------------------------------------------------------------
		static std::vector<std::string> Tokenize(const std::string& i_Line);

		//--------------------------------------------------------------------
		//	IsFlag - true if i_Token is "-<i_Name>" or "/<i_Name>"
		//	(exact and case-sensitive).
		//--------------------------------------------------------------------
		static bool IsFlag(const std::string& i_Token, const char* i_Name);
};
