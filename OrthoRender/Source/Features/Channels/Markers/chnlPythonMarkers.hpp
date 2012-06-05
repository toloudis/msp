/****************************************************************************\
**	chnlPythonMarkers.hpp
**
**		Python commands related to channels and driver creation.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_PYTHONMARKERS_HPP
#error chnlPythonMarkers.hpp multiply included
#endif
#define CHNL_PYTHONMARKERS_HPP

#include <string>

//============================================================================
//============================================================================
namespace chnlPythonMarkers
{
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName);
};
