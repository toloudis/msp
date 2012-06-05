/****************************************************************************\
**	chnlPython.hpp
**
**		Python commands related to channels and driver creation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_PYTHON_HPP
#error chnlPython.hpp multiply included
#endif
#define CHNL_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace chnlPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
