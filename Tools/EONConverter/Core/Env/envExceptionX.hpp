/****************************************************************************\
**  envExceptionX.hpp
**
**      envExceptionX.hpp defines envExceptionX, the base class for all 
**	Terawatt exceptions.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ENV_EXCEPTIONX_HPP
#error envExceptionX.hpp multiply included
#endif
#define ENV_EXCEPTIONX_HPP

//#ifndef ENV_ERROR_HPP
//#include "Core/env/envError.hpp"
//#endif

//============================================================================
//============================================================================
class envExceptionX
{
	public:

		//====================================================================
		//====================================================================
		envExceptionX();

		//====================================================================
		//====================================================================
		virtual ~envExceptionX();

		//====================================================================
		//	Index() returns the error code/string index.
		//====================================================================
		//virtual envError::Code Index() const;

		//====================================================================
		//	AdditionalInfo returns a pointer to a single byte string with more
		//	information about the exception.  This could return NULL.
		//====================================================================
		const char* AdditionalInfo() const;
};
