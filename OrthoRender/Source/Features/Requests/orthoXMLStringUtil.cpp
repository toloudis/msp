/*****************************************************************************
**	orthoXMLStringUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Requests/orthoXMLStringUtil.hpp"

//===================================================================
//-------StringStack class-------------------------------------------

void StringStack::Push( const std::string& str )
{
	m_Stack.push_back( str );
	if( !m_String.empty() ) m_String += '-';
	m_String += str;
}

void StringStack::Pop()
{
	if( !m_Stack.empty() )
	{
		m_Stack.pop_back();

		//rebuild nesting string
		m_String.clear();
		std::vector<std::string>::const_iterator itr = m_Stack.begin();
		int size = m_Stack.size();
		for( int count = 1; itr != m_Stack.end(); itr++, count++ )
		{
			m_String += (count < size ? *itr+'-' : *itr );
		}
	}
}

const std::string& StringStack::Top()
{
	static std::string empty;
	if( m_Stack.empty() ) return empty;
	return m_Stack.back();
}

const std::string& StringStack::String()
{
	return m_String;
}

const std::vector<std::string>& StringStack::Stack()
{
	return m_Stack;
}

void StringStack::Clear()
{
	m_Stack.clear();
	m_String.clear();
}

//============================================================================
//============================================================================
namespace orthoXMLStringUtil
{

	XMLType returnTokenXML(std::string& io_XMLString, std::string& o_Token)
	{
		XMLType token_type = e_XMLValue;

		//check for empty
		if( io_XMLString.empty() ) return e_XMLNone;

		//remove any white space
		int loc = io_XMLString.find_first_not_of( " \n\r\t" );
		if( loc >= 0) io_XMLString.erase( 0, loc );
		else io_XMLString.clear();	//no white space found so end of string

		//check for XML start
		if( io_XMLString.empty() ) return e_XMLNone;
		if( io_XMLString[0] == '<' )
		{
			io_XMLString.erase(0,1);
			token_type = e_XMLElement;

			//check for XML End
			if( io_XMLString.empty() ) return e_XMLError;
			if (io_XMLString[0] == '\/')
			{
				io_XMLString.erase(0,1);
				token_type = e_XMLEndElement;
			}
		}

		//handle value (between start and end block)
		o_Token.clear();

		//remove more white space
		if( io_XMLString.empty() ) return e_XMLError;
		loc = io_XMLString.find_first_not_of( " \n\r\t" );
		if( loc > 0 ) io_XMLString.erase( 0, loc );

		if( io_XMLString.empty() ) return e_XMLError;
		loc = io_XMLString.find_first_of( "<> \n\r\t" );
		if( loc > 0) o_Token.append( io_XMLString.c_str(), loc );
		io_XMLString.erase( 0, loc );

		//remove final token
		if( io_XMLString.empty() ) return e_XMLError;
		if (io_XMLString[0] == '>')
		{
			io_XMLString.erase(0,1);
		}

		//convert output to lower case
//		int len = o_Token.size() + 1; //include NULL
//		if( len > 1 ) _strlwr_s( const_cast<char*>(o_Token.c_str()), len );	//modify string in place, overriding const

		return token_type;
	}
}	// end of namespace
