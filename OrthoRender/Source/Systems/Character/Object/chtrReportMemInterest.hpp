/****************************************************************************\
**	chtrReportMemInterest.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef CHTR_REPORTMEMINTEREST_HPP
#error chtrReportMemInterest.hpp multiply included
#endif
#define CHTR_REPORTMEMINTEREST_HPP

#ifndef RPT_REPORTMEMINTEREST_HPP
#include "Features/Reports/ReportMem/rptReportMemInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class chtrReportMemInterest : public rptReportMemInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Report( gfFileTxt& i_File );
};
