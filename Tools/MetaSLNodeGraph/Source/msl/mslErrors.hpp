/*****************************************************************************
**	mslErrors.hpp
**
**	 mslErrors reports MetaSL error messages to debug log.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_ERRORS_HPP
#error mslErrors.hpp multiply included
#endif
#define MSL_ERRORS_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
namespace mslErrors 
{
	//--------------------------------------------------------------------
	// ReportErrors - If the errors object contains error messages,
	//	report them to debug log along with initial context message. 
	//--------------------------------------------------------------------
	void  ReportErrors(const std::string &i_Message, 
					   ICompiler_errors *i_pErrors);

}
