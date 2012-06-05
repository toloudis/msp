/**********************************************************
**  dbgAssertPACWin.cpp
**
**      dbgAssertPACWin is the definition of the windows
**	version of the debug Assert PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include "dbgAssertPACPS2.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace dbgAssertPAC
{

//========================================================================
//	Dumps output to debug terminal.
//========================================================================
void OKMessage(const char* i_Text)
{
	printf(i_Text);
}

//========================================================================
//	How to do this on PS2?  Just dumps the message and aborts for now.
//========================================================================
bool AbortContinueMessage(const char* i_Text)
{
	printf(i_Text);
	asm("break 0");
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
