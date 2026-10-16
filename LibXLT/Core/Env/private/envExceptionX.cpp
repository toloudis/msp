/****************************************************************************\
**  envExceptionX.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/env/envExceptionX.hpp"

#ifdef ENV_EXCEPTION_STACK_TRACE
#include "Core/env/private/envExceptionXPAC.hpp"
#endif // ENV_EXCEPTION_STACK_TRACE


//------------------------------------------------------------------------
//------------------------------------------------------------------------
envExceptionX::envExceptionX()
{
#ifdef ENV_EXCEPTION_STACK_TRACE
	RecordStackTrace();
#endif
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
envExceptionX::~envExceptionX()
{
}

#ifdef ENV_EXCEPTION_STACK_TRACE
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envExceptionX::RecordStackTrace()
{
	envExceptionXPAC::CaptureStackTrace( nPC, cnMaxStackFrames );
}
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const char* envExceptionX::GetCallStack() const
{
#ifdef ENV_EXCEPTION_STACK_TRACE
	return envExceptionXPAC::FormatStackTrace( nPC, cnMaxStackFrames );
#else // ENV_EXCEPTION_STACK_TRACE
	return "Stack trace unavailable";
#endif // ENV_EXCEPTION_STACK_TRACE
}


