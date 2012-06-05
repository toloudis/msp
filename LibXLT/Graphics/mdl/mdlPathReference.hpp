/****************************************************************************\
**	mdlPathReference.hpp
**
**		Contains structures for passing around the path for referencing nodes
**
**	StudioGPU
**	Copyright(C) 2003-9 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_PATHREFERENCE_HPP
#error mdlPathReference.hpp multiply included
#endif
#define MDL_PATHREFERENCE_HPP

#include <deque>
#include <string>


//============================================================================
//	mdlPathReference can be used as a path from the
//  root node of an mdl scene hieararchy
//============================================================================
class  mdlPathReference
{	
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mdlPathReference(){}

	//------------------------------------------------------------------------
	//	requirement: I has to be some iterator of a container 
	//	of string
	//------------------------------------------------------------------------
	template< class I > mdlPathReference( I &b, I & e )
	:	m_Path( b, e ){}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mdlPathReference(){}

	//------------------------------------------------------------------------
	//	output the path as a string, using delimiters to separate the path components
	//	If the path components were 'foo' and 'bar' and delimiter
	//	was '/' this would result in the string'/foo/bar'
	//------------------------------------------------------------------------
	std::string GetPathAsString( const std::string &i_Delim );

public:
	typedef std::deque< std::string > TPath;
	TPath m_Path;
};
