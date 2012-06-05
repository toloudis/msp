/****************************************************************************\
**  modeExceptionX.hpp
**
**      modeExceptionX.hpp defines the mode-specific exception classes 
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef MODE_EXCEPTIONX_HPP
#error modeExceptionX.hpp multiply included
#endif
#define MODE_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif

//============================================================================
//	The modeModeExitX exception object can be thrown by modes which wish to
//	terminate the mode operation, without going back to any other modes that
//	may be on the stack.  modeModeExitX must be thrown during the mode Think.
//============================================================================
class modeModeExitX : public envExceptionX
{
	public:

		//====================================================================
		//====================================================================
		modeModeExitX();

		//====================================================================
		//====================================================================
		virtual ~modeModeExitX();

		//====================================================================
		//	Index() returns the error code/string index.
		//====================================================================
		virtual envError::Code Index() const;
};
