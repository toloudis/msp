/****************************************************************************\
**  sgpuReportProgress.hpp
**
**      sgpuReportProgress.hpp defines an exception class that is usd by the sdk
**		Expect the routines of the sdk to throw this exception.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_REPORTPROGRESS_HPP
#define SGPU_REPORTPROGRESS_HPP

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


struct SGPUEXPORTLIB_API sgpuReportProgress
{
public:
	sgpuReportProgress(){}
	virtual ~sgpuReportProgress(){}
	virtual void operator()( float i_fProgressSoFar )=0;
};
#endif // #ifndef SGPU_EXCEPTION_HPP
