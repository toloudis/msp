/****************************************************************************\
**	setsReportMemInterest.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef SETS_REPORTMEMINTEREST_HPP
#error setsReportMemInterest.hpp multiply included
#endif
#define SETS_REPORTMEMINTEREST_HPP

#ifndef RPT_REPORTMEMINTEREST_HPP
#include "Features/Reports/ReportMem/rptReportMemInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class setsReportMemInterest : public rptReportMemInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Report( gfFileTxt& i_File );
};
