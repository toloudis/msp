/********************************************************************************************\
**  mtLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\********************************************************************************************/

#include "tasErrorHandler.hpp"
#include "envPlatform.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{

tasErrorHandler* l_ErrorHandler = NULL;

} // end of namespace

//========================================================================
//========================================================================
void
tasErrorHandler::SetErrorHandler(tasErrorHandler* i_Handler)
{
	l_ErrorHandler = i_Handler;
}

tasErrorHandler*	tasErrorHandler::GetErrorHandler()
{
	return l_ErrorHandler;
}
