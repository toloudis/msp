/********************************************************************************************\
**  mtLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\********************************************************************************************/

#include "ptclErrorHandler.hpp"
#include "Core/env/envPlatform.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{

ptclErrorHandler* l_ErrorHandler = NULL;

} // end of namespace

//========================================================================
//========================================================================
void
ptclErrorHandler::SetErrorHandler(ptclErrorHandler* i_Handler)
{
	l_ErrorHandler = i_Handler;
}

ptclErrorHandler*	ptclErrorHandler::GetErrorHandler()
{
	return l_ErrorHandler;
}
