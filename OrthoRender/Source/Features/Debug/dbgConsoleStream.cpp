//****************************************************************************
//	\file dbgConsoleStream.cpp
//
//  see .hpp
//
//	Extra Large Technology
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include "Features/Debug/dbgConsoleStream.hpp"

#include "Features/Debug/dbgConsoleDialogUtil.hpp"
#include <string>

//============================================================================
// Debug Console Stream 
//============================================================================

//----------------------------------------------------------------------------
//default constructor 
//----------------------------------------------------------------------------
dbgConsoleBuf::dbgConsoleBuf() : dbgStreamBuf()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleBuf::~dbgConsoleBuf() 
{
	sync();
}

//----------------------------------------------------------------------------
// The sync function takes care of the 
// streams buffer and places it into the debug window
//----------------------------------------------------------------------------
int dbgConsoleBuf::sync()
{
	// Get the string in the buffer => str().c_str();
	std::string consoleString = std::string(str().c_str());
	dbgConsoleDialogUtil::UpdateConsoleDialog(consoleString);
	str(std::basic_string<char>());    // Clear the string buffer
	
    return 0;
}
