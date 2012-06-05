/****************************************************************************\
**	vtxExceptionX.hpp
**
**		vtxExceptionX.hpp defines the exceptions that can be thrown from the
**	vtx package.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_EXCEPTIONX_HPP
#error vtxExceptionX.hpp multiply included
#endif
#define VTX_EXCEPTIONX_HPP

#ifndef FS_FILEX_HPP
#include "Core/Fs/fsFileX.hpp"
#endif 


//============================================================================
//============================================================================
class vtxCompressErrorX : public fsFileExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		vtxCompressErrorX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};


//============================================================================
//============================================================================
class vtxDecompressErrorX : public fsFileExceptionX
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		vtxDecompressErrorX(const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// Returns error message string
		//--------------------------------------------------------------------
		virtual std::string GetErrorMessage() const;
};
