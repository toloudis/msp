/****************************************************************************\
**	chnlPythonNotes.hpp
**
**		Python commands related to channels and driver creation.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_PYTHONNOTES_HPP
#error chnlPythonNotes.hpp multiply included
#endif
#define CHNL_PYTHONNOTES_HPP

#include <string>

//============================================================================
//============================================================================
namespace chnlPythonNotes
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
