/****************************************************************************\
**	mdlExceptionX.hpp
**
**		mdlExceptionX.hpp defines the exceptions that can be thrown from the
**	mdl package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_EXCEPTIONX_HPP
#error mdlExceptionX.hpp multiply included
#endif
#define MDL_EXCEPTIONX_HPP

#ifndef FS_FILEX_HPP
#include "Core/Fs/fsFileX.hpp"
#endif 


//============================================================================
//============================================================================
class mdlInvalidModelFileX : public fsFileExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mdlInvalidModelFileX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};
