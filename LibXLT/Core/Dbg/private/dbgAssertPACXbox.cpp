/**********************************************************
**  dbgAssertPACXbox.cpp
**
**      dbgAssertPACXbox is the definition of the Xbox
**	version of the debug Assert PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include <xtl.h>

#include "dbgAssertPACXbox.hpp"

namespace dbgAssertPAC
{

//========================================================================
//	Output string to debug terminal.
//========================================================================
void OKMessage(const char* i_Text)
{
	::OutputDebugString( i_Text );
}

//========================================================================
//	Just dumps the message and aborts for now.
//========================================================================
bool AbortContinueMessage(const char* i_Text)
{
	::OutputDebugString( i_Text );
	return true;
}

//========================================================================
//	Init is called by the package initializer.  Don't call it yourself.
//========================================================================
void Init()
{
	// no operations needed for now
}

//========================================================================
//	CleanUp is called by the package initializer.  Don't call it yourself.
//========================================================================
void CleanUp() throw()
{
	// no operations needed for now
}

}
