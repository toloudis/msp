/****************************************************************************\
**	prjltReportMemInterest.hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef PRJLT_REPORTMEMINTEREST_HPP
#error prjltReportMemInterest.hpp multiply included
#endif
#define PRJLT_REPORTMEMINTEREST_HPP

#ifndef RPT_REPORTMEMINTEREST_HPP
#include "Features/Reports/ReportMem/rptReportMemInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class prjltReportMemInterest : public rptReportMemInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Report( gfFileTxt& i_File );
};
