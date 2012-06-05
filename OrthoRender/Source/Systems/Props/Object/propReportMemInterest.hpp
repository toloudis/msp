/****************************************************************************\
**	propReportMemInterest.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef PROP_REPORTMEMINTEREST_HPP
#error propReportMemInterest.hpp multiply included
#endif
#define PROP_REPORTMEMINTEREST_HPP

#ifndef RPT_REPORTMEMINTEREST_HPP
#include "Features/Reports/ReportMem/rptReportMemInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class propReportMemInterest : public rptReportMemInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Report( gfFileTxt& i_File );
};
