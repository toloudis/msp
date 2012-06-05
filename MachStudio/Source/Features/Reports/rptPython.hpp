/****************************************************************************\
**	rptPython.hpp
**
**		Python commands related to report the video statistics
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPT_PYTHON_HPP
#error rptPython.hpp multiply included
#endif
#define RPT_PYTHON_HPP

#include <string>

//============================================================================
//============================================================================
namespace rptPython
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
