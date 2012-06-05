//****************************************************************************
/// \file dbgAssert.hpp 
///
/// A debug-only assert that also writes out strings to a log file.
///
/// dbgAssert contains debug assertion functions and 
///	macros.  Assertions are a powerful debugging tool which cause
///	a window to pop up when a given condition is false.  They can be used
///	to verify assumptions and alert programmers to coding errors.
///
///	Extra Large Technology
///	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef DBG_ASSERT_HPP
#error dbgAssert.hpp multiply included
#endif
#define DBG_ASSERT_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
/// Debug Assertion interface
//============================================================================
namespace dbgAssert
{
	//------------------------------------------------------------------------
	///	This function is used by the debug assert and message macros below.
	///	It always uses single-byte ANSI characters
	///
	/// \param i_File file name the message came FROM
	/// \param i_Line line number
	/// \param i_Format standard sprintf style formatting string
	///
	/// \return void
	//------------------------------------------------------------------------
	void OKMessage(const char* i_File, int i_Line, const char* i_Format, ...);

	//------------------------------------------------------------------------
	///	This function is used by the debug assert and message macros below.
	///	It always uses single-byte ANSI characters
	///
	///	test list
	///		- one
	///		- two
	///		- three
	///		- four
	///
	/// \param i_File file name the message came FROM
	/// \param i_Line line number
	/// \param i_Format standard sprintf style formatting string
	///
	/// \return void
	//------------------------------------------------------------------------
	void AssertMessage(const char* i_File, int i_Line, const char* i_Format, ...);

	//------------------------------------------------------------------------
	///	Don't call Init() and CleanUp() yourself; they are called 
	///	by the package Init and Cleanup.
	///
	/// \return void
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	///	Don't call Init() and CleanUp() yourself; they are called 
	///	by the package Init and Cleanup.
	///
	/// \return void
	//------------------------------------------------------------------------
	void CleanUp() throw();
}

//------------------------------------------------------------------------
//	debug assertions
//	usage example: DBG_ASSERT0(this->CanMove(), "Object couldn't move");
//	usage example: DBG_ASSERT1(pObj->CanMove(), "Object %s couldn't move", pObj->GetName());
//	usage example: DBG_MESSAGE2("File %s reading at line %d", pFile->GetName(), pFile->GetLine());
//------------------------------------------------------------------------

#if ENV_DEBUG

#define	DBG_MESSAGE0( a )		dbgAssert::OKMessage( __FILE__, __LINE__,  a );												
#define	DBG_MESSAGE1( a,b )		dbgAssert::OKMessage( __FILE__, __LINE__,  a, b );											
#define	DBG_MESSAGE2( a,b,c )	dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c );										
#define	DBG_MESSAGE3( a,b,c,d )		dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d );										
#define	DBG_MESSAGE4( a,b,c,d,e )	dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d, e );									
#define	DBG_MESSAGE5( a,b,c,d,e,f )		dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d, e, f );								
#define	DBG_MESSAGE6( a,b,c,d,e,f,g )	dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d, e, f, g );							
#define	DBG_MESSAGE7( a,b,c,d,e,f,g,h )	dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d, e, f, g, h );							
#define	DBG_MESSAGE8( a,b,c,d,e,f,g,h,i )	dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d, e, f, g, h, i );						
#define	DBG_MESSAGE9( a,b,c,d,e,f,g,h,i,j )	dbgAssert::OKMessage( __FILE__, __LINE__,  a, b, c, d, e, f, g, h, i, j );					

#define	DBG_ASSERT0( a,b )		if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b );												
#define	DBG_ASSERT1( a,b,c )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c );											
#define	DBG_ASSERT2( a,b,c,d )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d );										
#define	DBG_ASSERT3( a,b,c,d,e )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e );										
#define	DBG_ASSERT4( a,b,c,d,e,f )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e, f );									
#define	DBG_ASSERT5( a,b,c,d,e,f,g )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e, f, g );								
#define	DBG_ASSERT6( a,b,c,d,e,f,g,h )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e, f, g, h );							
#define	DBG_ASSERT7( a,b,c,d,e,f,g,h,i )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e, f, g, h, i );							
#define	DBG_ASSERT8( a,b,c,d,e,f,g,h,i,j )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e, f, g, h, i, j );						
#define	DBG_ASSERT9( a,b,c,d,e,f,g,h,i,j,k )	if ( (!(a)) )	dbgAssert::AssertMessage( __FILE__, __LINE__,  b, c, d, e, f, g, h, i, j, k );					

#else

#define	DBG_MESSAGE0( a )
#define	DBG_MESSAGE1( a,b )
#define	DBG_MESSAGE2( a,b,c )
#define	DBG_MESSAGE3( a,b,c,d )
#define	DBG_MESSAGE4( a,b,c,d,e )
#define	DBG_MESSAGE5( a,b,c,d,e,f )
#define	DBG_MESSAGE6( a,b,c,d,e,f,g )
#define	DBG_MESSAGE7( a,b,c,d,e,f,g,h )
#define	DBG_MESSAGE8( a,b,c,d,e,f,g,h,i )
#define	DBG_MESSAGE9( a,b,c,d,e,f,g,h,i,j )

#define	DBG_ASSERT0( a,b )																
#define	DBG_ASSERT1( a,b,c )																
#define	DBG_ASSERT2( a,b,c,d )															
#define	DBG_ASSERT3( a,b,c,d,e )															
#define	DBG_ASSERT4( a,b,c,d,e,f )														
#define	DBG_ASSERT5( a,b,c,d,e,f,g )														
#define	DBG_ASSERT6( a,b,c,d,e,f,g,h )													
#define	DBG_ASSERT7( a,b,c,d,e,f,g,h,i )													
#define	DBG_ASSERT8( a,b,c,d,e,f,g,h,i,j )												
#define	DBG_ASSERT9( a,b,c,d,e,f,g,h,i,j,k )												

#endif
