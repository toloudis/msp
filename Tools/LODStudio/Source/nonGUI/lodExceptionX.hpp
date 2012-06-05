/****************************************************************************\
**  lodExceptionX.hpp
**
**      lodExceptionX.hpp defines the app-specific exception classes
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef LOD_EXCEPTIONX_HPP
#error lodExceptionX.hpp multiply included
#endif
#define LOD_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "envExceptionX.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "fsLocator.hpp"
#endif


//---------------------------------------------------------------------------=
//	The lodModeExitX exception object can be thrown by modes which wish to
//	terminate the mode operation, without going back to any other modes that
//	may be on the stack.  lodModeExitX must be thrown during the mode Think.
//---------------------------------------------------------------------------=
class lodModeExitX : public envExceptionX
{
	public:

		//------------------------------------------------------------------==
		//------------------------------------------------------------------==
		lodModeExitX();

		//------------------------------------------------------------------==
		//------------------------------------------------------------------==
		virtual ~lodModeExitX();

		//------------------------------------------------------------------==
		//	Index() returns the error code/string index.
		//------------------------------------------------------------------==
		virtual envError::Code Index() const;
};
