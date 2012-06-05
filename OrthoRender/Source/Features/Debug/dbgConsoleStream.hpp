//****************************************************************************
//	\file dbgStringStream.hpp
//
// Buffer and stream class that will be used to pipe the output of the
// stream into the debug window in VS.
//
//	Extra Large Technology
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef DBG_CONSOLE_STREAM_HPP
#error dbgConsoleStream.hpp multiply included
#endif
#define DBG_CONSOLE_STREAM_HPP

#include "Core/Dbg/dbgStringStream.hpp"




//--------------------------------------------------------------------------------
// Derived from std::stringbuf class; this class will take in a stream message and
// output the message to the debug console.
//---------------------------------------------------------------------------------
class dbgConsoleBuf: public dbgStreamBuf
{

public:

	//----------------------------------------------------------------------------
	//default constructor 
	//----------------------------------------------------------------------------
	dbgConsoleBuf();
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~dbgConsoleBuf() ;
	
	//--------------------------------------------------
	// The sync function takes care of the 
	// streams buffer and places it into the debug console
	//--------------------------------------------------
	int sync();
};