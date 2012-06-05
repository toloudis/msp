/****************************************************************************\
**  mdlPathReference.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlPathReference.hpp"


//----------------------------------------------------------------------------
//	output the path as a string, using delimiters to separate the path components
//	If the path components were 'foo' and 'bar' and delimiter
//	was '/' this would result in the string'/foo/bar'
//----------------------------------------------------------------------------
std::string mdlPathReference::GetPathAsString( const std::string &i_Delim )
{
	TPath::const_iterator pit;
	std::string result;
	for ( pit = m_Path.begin(); pit != m_Path.end(); ++pit )
	{
		result = result + i_Delim + *pit;
	}
	return result;
}
