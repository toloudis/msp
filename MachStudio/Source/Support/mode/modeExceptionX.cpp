/****************************************************************************\
**  modeExceptionX.cpp
**
**      modeExceptionX.hpp defines the mode-specific exception classes 
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Support/mode/modeExceptionX.hpp"


//============================================================================
//	The modeModeExitX exception object can be thrown by modes which wish to
//	terminate the mode operation, without going back to any other modes that
//	may be on the stack.  modeModeExitX must be thrown during the mode Think.
//============================================================================
modeModeExitX::modeModeExitX()
{
}

//============================================================================
//============================================================================
modeModeExitX::~modeModeExitX()
{
}

//============================================================================
//	Index() returns the error code/string index.
//============================================================================
envError::Code modeModeExitX::Index() const
{
	return envError::GetCode(1000, 0);
}

