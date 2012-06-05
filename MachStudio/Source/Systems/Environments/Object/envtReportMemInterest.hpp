/****************************************************************************\
**	envtReportMemInterest.hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef ENVT_REPORTMEMINTEREST_HPP
#error envtReportMemInterest.hpp multiply included
#endif
#define ENVT_REPORTMEMINTEREST_HPP

#ifndef RPT_REPORTMEMINTEREST_HPP
#include "Features/Reports/ReportMem/rptReportMemInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class envtReportMemInterest : public rptReportMemInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Report( gfFileTxt& i_File );
};
