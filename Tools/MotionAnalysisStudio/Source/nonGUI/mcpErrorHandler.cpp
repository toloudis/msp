/********************************************************************************************\
**  mtLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\********************************************************************************************/

#include "mcpErrorHandler.hpp"
#include "envPlatform.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{

mcpErrorHandler* l_ErrorHandler = NULL;

} // end of namespace

//========================================================================
//========================================================================
void
mcpErrorHandler::SetErrorHandler(mcpErrorHandler* i_Handler)
{
	l_ErrorHandler = i_Handler;
}
	
mcpErrorHandler*	mcpErrorHandler::GetErrorHandler()
{
	return l_ErrorHandler;
}
