/****************************************************************************\
**  sgpuException.hpp
**
**      sgpuException.hpp defines an exception class that is usd by the sdk
**		Expect the routines of the sdk to throw this exception.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_EXCEPTION_HPP
#define SGPU_EXCEPTION_HPP

#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"

/**
//	An exception class.
//	The SDK-s functions can trow tis exception.
//	Users should try-catch on this excption, at least at
//	the top level
*/
//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuException
{
public:

	//========================================================================
	//	constructors, destructor, assignment operator
	//========================================================================
	sgpuException( const sgpuString &i_StringVal ):
		m_Description( i_StringVal )
		{}
	sgpuException( const sgpuException &i_Other ):
		m_Description( i_Other.m_Description )
		{}
	~sgpuException()
	{}
	sgpuException& operator=(const sgpuException& i_CopyFrom)
	{
		m_Description = i_CopyFrom.m_Description;
		return *this;
	}
public:
	sgpuString m_Description;
};

#endif // #ifndef SGPU_EXCEPTION_HPP
