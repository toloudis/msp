/*****************************************************************************
**  fsysDirListUtil.hpp
**
**      fsysDirListUtil holds a list of child directories
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef FSYS_DIRLISTUTIL_HPP
#error fsysDirListUtil.hpp multiply included
#endif
#define FSYS_DIRLISTUTIL_HPP


#include <vector>
#include <string>

//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace fsysDirListUtil
{
	//------------------------------------------------------------------------
	//	Build the directory list from the full directory passed in,
	//	returning vector of itStrings.
	//------------------------------------------------------------------------
	std::vector<std::string> BuildDirectoryList(const fsLocator &i_FullDir);

	//------------------------------------------------------------------------
	//	Builds a short directory list using just the given object directory
	//	name and "General".
	//------------------------------------------------------------------------
	std::vector<std::string> BuildDirectoryList(const fsLocator &i_FullDir,
											 const std::string &i_ObjDir);
}
