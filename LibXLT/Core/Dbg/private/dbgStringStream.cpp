//****************************************************************************
//	\file dbgStringStream.cpp
//
//  see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include "Core/dbg/dbgStringStream.hpp"
#include "Core/dbg/private/dbgLogPAC.hpp"


//============================================================================
// Debug String Stream 
//============================================================================

//----------------------------------------------------------------------------
//default constructor 
//----------------------------------------------------------------------------
dbgStreamBuf::dbgStreamBuf() 
:	std::stringbuf()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgStreamBuf::~dbgStreamBuf() 
{
	sync();
}

//----------------------------------------------------------------------------
// The sync function takes care of the 
// streams buffer and places it into the debug window
//----------------------------------------------------------------------------
int dbgStreamBuf::sync()
{
	Output( str() );

	str(std::basic_string<char>());    // Clear the string buffer
	
    return 0;
}

//----------------------------------------------------------------------------
// Output sends UTF-8 text to the debug window
//----------------------------------------------------------------------------
void dbgStreamBuf::Output(const std::string& i_UTF8Text)
{
	dbgLogPAC::WriteDebugOutput( i_UTF8Text );
}
