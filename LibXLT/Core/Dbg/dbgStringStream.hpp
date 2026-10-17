//****************************************************************************
//	\file dbgStringStream.hpp
//
// Buffer and stream class that will be used to pipe the output of the
// stream into the debug window in VS.
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef DBG_STRING_STREAM_HPP
#error dbgStringStream.hpp multiply included
#endif
#define DBG_STRING_STREAM_HPP

#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <ostream>
#include <sstream>
#include <string>


//============================================================================
// Derived from std::stringbuf class; this class will take in a stream message and
// output the message to the debug window in VS.
//============================================================================
class dbgStreamBuf: public std::stringbuf
{
public:

	//----------------------------------------------------------------------------
	//default constructor 
	//----------------------------------------------------------------------------
	dbgStreamBuf();
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~dbgStreamBuf() ;
	
	//--------------------------------------------------
	// The sync function takes care of the 
	// streams buffer and places it into the debug window
	//--------------------------------------------------
	virtual int sync();

	//--------------------------------------------------
	// Output sends UTF-8 text to the debug window
	//--------------------------------------------------
	static void Output(const std::string& i_UTF8Text);
};


//============================================================================
// Class derived from ostream that will overload it's <<
// operator and allow writing to the debug window to occur
//============================================================================
class dbgStringStream : public std::ostream 
{
public: 
    //constructor
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	dbgStringStream(std::streambuf* sb) 
	:	std::ostream(sb), 
		std::ios(0) 
	{   }

	//------------------------------------------------------------------------
	// this constructor will give the 
	// ostream the string buffer we created
	// in order to pipe the output into the debug log
	//------------------------------------------------------------------------
	dbgStringStream() 
	:	std::ostream(&myStrBuf), 
		std::ios(0) 
	{	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~dbgStringStream()
	{   } 

	//------------------------------------------------------------------------
	// overload << operator to write the right hand message
	// to the debug window
	//------------------------------------------------------------------------
	template<class T>
	dbgStringStream& operator<<(const T& i_Msg)
	{
		std::stringstream ss;
		std::string msg;
		ss << i_Msg;
		msg = ss.str();
		dbgStreamBuf::Output( msg );
        return *this;
    }

private:
	dbgStreamBuf myStrBuf; 
}; 
