/*****************************************************************************
**  fsysDirListUtil.hpp
**
**      fsysDirListUtil holds a list of child directories
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef FSYS_DIRLISTUTIL_HPP
#error fsysDirListUtil.hpp multiply included
#endif
#define FSYS_DIRLISTUTIL_HPP


#include <vector>

//============================================================================
//============================================================================
class fsLocator;
class itString;


//============================================================================
//============================================================================
namespace fsysDirListUtil
{
	//------------------------------------------------------------------------
	//	Build the directory list from the full directory passed in,
	//	returning vector of itStrings.
	//------------------------------------------------------------------------
	std::vector<itString> BuildDirectoryList(const fsLocator &i_FullDir);

	//------------------------------------------------------------------------
	//	Builds a short directory list using just the given object directory
	//	name and "General".
	//------------------------------------------------------------------------
	std::vector<itString> BuildDirectoryList(const fsLocator &i_FullDir,
											 const itString &i_ObjDir);
}
