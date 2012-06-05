/*****************************************************************************
**  envDisplayErrorForwarder.cpp
**
**		This componnt is for the sole use of components who have no knowledge
**		of the gfPackage, and thus cannot make direct use of the gfErrorHandler
**		This simply forwards the parameters given to the gfErrorHandler via a
**		simple callback that the gfErrorHandler supplies when it is initialized
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/env/envErrorDisplayForwarder.hpp"

//============================================================================
//============================================================================
namespace envErrorDisplayForwarder
{

namespace
{
envErrorDisplayForwarderFunction l_pErrorDisplayForwarderFunction = NULL;
}

//--------------------------------------------------------------------
//	DisplayError forward i_nPackageID and i_nErrorCode to the gfErrorHandler
//	via the envErrorDisplayForwarderFunction if it has been passed down already
//--------------------------------------------------------------------
ErrorDisplayReturnCodes DisplayError(envErrorDisplayForwarder::DisplayType i_DisplayType,
									 int i_nPackageID, int i_nErrorCode, const char *i_pAppendingString )
{
	if (l_pErrorDisplayForwarderFunction)
	{
		return (*l_pErrorDisplayForwarderFunction)(i_DisplayType, i_nPackageID, i_nErrorCode, i_pAppendingString);
	}

	return e_Abort;
}

//--------------------------------------------------------------------
//	SetErrorDisplayForwarderFunction will set the callback function.
//--------------------------------------------------------------------
void SetErrorDisplayForwarderFunction( envErrorDisplayForwarderFunction i_pFunction )
{
	l_pErrorDisplayForwarderFunction = i_pFunction;
}

}