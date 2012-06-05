/****************************************************************************\
**  mdlExceptionX.hpp
**
**      mdlExceptionX.hpp defines the exceptions that can be thrown from the
**	mdl package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mdl/mdlExceptionX.hpp"

//#include "Core/env/envPackageErrorIndices.hpp"
//
//#include "Graphics/mdl/mdlErrorCodes.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlInvalidModelFileX::mdlInvalidModelFileX(const fsLocator& i_Locator)
:	m_Locator(i_Locator)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlInvalidModelFileX::~mdlInvalidModelFileX()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const fsLocator& mdlInvalidModelFileX::GetLocator() const
{
	return m_Locator;
}

//--------------------------------------------------------------------
//	Index() returns the error code/string index.
//--------------------------------------------------------------------
//envError::Code mdlInvalidModelFileX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_May, mdlErrorCodes::e_InvalidModelFile);
//}
