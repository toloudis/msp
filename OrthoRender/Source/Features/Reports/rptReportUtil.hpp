/*****************************************************************************
**	rptReportUtil.hpp
**
**		API for Report utilities
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef RPT_REPORTUTIL_HPP
#error rpt_ReportUtil.hpp multiply included
#endif
#define RPT_REPORTUTIL_HPP


//============================================================================
//	forward references
//============================================================================
class docDocument;
struct rptReportData;


//============================================================================
//============================================================================
namespace rptReportUtil
{
	//------------------------------------------------------------------------
	//	Build a data list from the passed in doc.
	//------------------------------------------------------------------------
	void BuildInvDataList( docDocument* i_pDoc, rptReportData& o_Data );
}
