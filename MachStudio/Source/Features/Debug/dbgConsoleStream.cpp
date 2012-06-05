//****************************************************************************
//	\file dbgConsoleStream.cpp
//
//  see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include "Features/Debug/dbgConsoleStream.hpp"

#include "Features/Debug/dbgConsoleDialogUtil.hpp"

//============================================================================
// Debug Console Stream 
//============================================================================

//----------------------------------------------------------------------------
//default constructor 
//----------------------------------------------------------------------------
dbgConsoleBuf::dbgConsoleBuf() 
:	dbgStreamBuf(),
	m_dbgCallbackFunction(NULL)
{
}

//----------------------------------------------------------------------------
//constructor takes in a callback function pointer to be used when something
// is written to the buffer
//----------------------------------------------------------------------------
dbgConsoleBuf::dbgConsoleBuf(void (*i_CallbackFunction)(std::string& i_dbgString ))
:	dbgStreamBuf(),
	m_dbgCallbackFunction(i_CallbackFunction)
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
	//dbgConsoleDialogUtil::UpdateConsoleDialog(consoleString);
	if(m_dbgCallbackFunction)
		m_dbgCallbackFunction(consoleString);
	str(std::basic_string<char>());    // Clear the string buffer
	
    return 0;
}
