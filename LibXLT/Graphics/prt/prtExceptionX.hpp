/****************************************************************************\
**	prtExceptionX.hpp
**
**		prtExceptionX.hpp defines the exceptions that can be thrown from the
**	prt package.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_EXCEPTIONX_HPP
#error prtExceptionX.hpp multiply included
#endif
#define PRT_EXCEPTIONX_HPP

#ifndef FS_FILEX_HPP
#include "Core/Fs/fsFileX.hpp"
#endif 


//============================================================================
//============================================================================
class prtInvalidlFileFormatX : public fsFileExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtInvalidlFileFormatX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};

