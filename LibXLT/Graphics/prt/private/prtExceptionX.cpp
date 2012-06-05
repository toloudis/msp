/****************************************************************************\
**	prtExceptionX.hpp
**
**		prtExceptionX.hpp defines the exceptions that can be thrown from the
**	prt package.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtExceptionX.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtInvalidlFileFormatX::prtInvalidlFileFormatX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string prtInvalidlFileFormatX::GetErrorMessage() const
{
	return "Invalid or corrupted particle file format: " + GetLocatorString();
}
