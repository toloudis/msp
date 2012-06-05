/****************************************************************************\
**	mdlExceptionX.hpp
**
**		mdlExceptionX.hpp defines the exceptions that can be thrown from the
**	mdl package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlExceptionX.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlInvalidModelFileX::mdlInvalidModelFileX(const fsLocator& i_Locator)
:	fsFileExceptionX(i_Locator)
{
}

//--------------------------------------------------------------------
// Returns error message string
//--------------------------------------------------------------------
std::string mdlInvalidModelFileX::GetErrorMessage() const
{
	return "Invalid or corrupted model file format: " + GetLocatorString();
}
