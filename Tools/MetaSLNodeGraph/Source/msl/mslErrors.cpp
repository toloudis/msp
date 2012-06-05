/*****************************************************************************
**	mslErrors.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslErrors.hpp"

#include "Core/dbg/dbgMsg.hpp"

namespace
{
}

//--------------------------------------------------------------------
// ReportErrors - If the errors object contains error messages,
//	report them to debug log along with initial context message. 
//--------------------------------------------------------------------
void  mslErrors::ReportErrors(const std::string &i_Message, 
							  ICompiler_errors *i_pErrors)
{        
	std::string message = i_Message;
	if (i_pErrors->count()) 
	{
        for(int i = 0; i < i_pErrors->count(); i++) {
            int length = i_pErrors->get_error_string_length(i);
            char *error = new char[length+1];
            i_pErrors->get_error_string(i,error,length);
            message += "\n";
            message += error;
            delete[] error;
        }
    }
	DBG_ERROR(message);
}


