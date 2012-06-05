/********************************************************************************************\
**  mtLevel.cpp
**
**      Keeps track of viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\********************************************************************************************/

#include "nonGUI/chrErrorHandler.hpp"
#include "Core/env/envPlatform.hpp"

//========================================================================
// Local variables and functions
//========================================================================
namespace
{

chrErrorHandler* l_ErrorHandler = NULL;

} // end of namespace

//========================================================================
//========================================================================
void
chrErrorHandler::SetErrorHandler(chrErrorHandler* i_Handler)
{
	l_ErrorHandler = i_Handler;
}
	
chrErrorHandler*	chrErrorHandler::GetErrorHandler()
{
	return l_ErrorHandler;
}
