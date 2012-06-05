/****************************************************************************\
**  envExceptionX.cpp
**
**      envExceptionX.cpp defines envExceptionX, the base class for all 
**	Terawatt exceptions.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/env/envExceptionX.hpp"
#include "Core/env/envPlatform.hpp"

//#include "Core/env/envErrorCodes.hpp"
//#include "Core/env/envPackageErrorIndices.hpp"

//========================================================================
//========================================================================
envExceptionX::envExceptionX()
{
}

//========================================================================
//========================================================================
envExceptionX::~envExceptionX()
{
}

//========================================================================
//	Index() returns the error code/string index.
//========================================================================
//envError::Code envExceptionX::Index() const
//{
//	return envError::GetCode(envPackageErrorIndices::e_Env, envErrorCodes::e_Exception);
//}

//========================================================================
//	AdditionalInfo returns a pointer to a single byte string with more
//	information about the exception.  This could return NULL.
//========================================================================
const char* envExceptionX::AdditionalInfo() const
{
	return NULL;
}

