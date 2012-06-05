/*****************************************************************************
**  chExceptionX.hpp
**
**      chExceptionX defines the exceptions which can be thrown by the 
**	ch package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CH_EXCEPTIONX_HPP
#error chExceptionX.hpp multiply included
#endif
#define CH_EXCEPTIONX_HPP

#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif

//============================================================================
//============================================================================
class chInvalidChunkX : public envExceptionX
{
	public:

		//====================================================================
		//====================================================================
		chInvalidChunkX();

		//====================================================================
		//====================================================================
		virtual ~chInvalidChunkX();

		//====================================================================
		//	Index() returns the error code/string index.
		//====================================================================
		//virtual envError::Code Index() const;		
};

