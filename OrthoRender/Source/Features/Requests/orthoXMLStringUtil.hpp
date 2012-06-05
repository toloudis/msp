/*****************************************************************************
**	orthoXMLStringUtil.hpp
**
**		Interface to XML string parsing
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ORTHO_XMLSTRINGUTIL_HPP
#error orthoXMLStringUtil.hpp multiply included
#endif
#define ORTHO_XMLSTRINGUTIL_HPP

#include <string>
#include <vector>

class StringStack
{
public:
	StringStack(){}

	void Push( const std::string& str );
	void Pop();
	const std::string& Top();
	void Clear();
	const std::string& String();
	const std::vector<std::string>& Stack();

private:
	std::vector<std::string>	m_Stack;
	std::string					m_String;
};

//============================================================================
//============================================================================
namespace orthoXMLStringUtil
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum XMLType
	{
		e_XMLNone = 0,	//happens when parsing remaining white space, no error
		e_XMLError,
		e_XMLElement,
		e_XMLValue,
		e_XMLEndElement,
	};

	//----------------------------------------------------------------------------
	//	remove the next token from the string and return it.  This function
	//	changes the XML string as well.
	//----------------------------------------------------------------------------
	XMLType returnTokenXML(std::string& io_XMLString, std::string& o_Token);
}	// end of namespace
