/********************************************************************************************\
**  mtLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\********************************************************************************************/

#include "mtrErrorHandler.hpp"
#include "envPlatform.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{

mtrErrorHandler* l_ErrorHandler = NULL;

} // end of namespace

//========================================================================
//========================================================================
void
mtrErrorHandler::SetErrorHandler(mtrErrorHandler* i_Handler)
{
	l_ErrorHandler = i_Handler;
}
	
mtrErrorHandler*	mtrErrorHandler::GetErrorHandler()
{
	return l_ErrorHandler;
}
