/****************************************************************************\
**	vtxExceptionX.hpp
**
**		vtxExceptionX.hpp defines the exceptions that can be thrown from the
**	vtx package.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/vtxExceptionX.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxCompressErrorX::vtxCompressErrorX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string vtxCompressErrorX::GetErrorMessage() const
{
	return "Error compressing animating: " + GetLocatorString();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
vtxDecompressErrorX::vtxDecompressErrorX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string vtxDecompressErrorX::GetErrorMessage() const
{
	return "Error decompressing animating: " + GetLocatorString();
}

