/********************************************************************************************\
**  mtLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\********************************************************************************************/

#include "lodErrorHandler.hpp"
#include "envPlatform.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{

lodErrorHandler* l_ErrorHandler = NULL;

} // end of namespace

//========================================================================
//========================================================================
void
lodErrorHandler::SetErrorHandler(lodErrorHandler* i_Handler)
{
	l_ErrorHandler = i_Handler;
}

lodErrorHandler*	lodErrorHandler::GetErrorHandler()
{
	return l_ErrorHandler;
}
