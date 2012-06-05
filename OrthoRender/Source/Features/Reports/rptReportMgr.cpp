/*****************************************************************************
**	rptReportMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Reports/rptReportMgr.hpp"

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace rptReportMgr
{
	namespace
	{
		std::vector<rptReport*> l_Reports;
		int l_SelIndex = -1;

		//--------------------------------------------------------------------
		void clear_reports()
		{
			envSTLHelpers::DeleteContainer(l_Reports);
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
	}

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		clear_reports();
		l_SelIndex = -1;
	}

	//--------------------------------------------------------------------
	//  Clear
	//--------------------------------------------------------------------
	//void  Clear()
	//{
	//	clear_reports();
	//	l_SelIndex = -1;
	//}

	//--------------------------------------------------------------------
	//  Get number of reports
	//--------------------------------------------------------------------
	int GetNumReports()
	{
		return l_Reports.size();
	}

	//--------------------------------------------------------------------
	//  Add new report
	//--------------------------------------------------------------------
	int  AddReport( rptReport* i_pReport )
	{
		int index = l_Reports.size();
		l_Reports.resize( index + 1 );

		l_Reports[index] = i_pReport;

		return index;
	}

	//--------------------------------------------------------------------
	//  Select report with given index
	//--------------------------------------------------------------------
	//void  SelectReport(int i_Index)
	//{
	//	l_SelIndex = i_Index;
	//}


	//--------------------------------------------------------------------
	//  Get the report
	//--------------------------------------------------------------------
	rptReport* GetReport(int i_Index)
	{
		DBG_ASSERT0( (i_Index >= 0 && i_Index < l_Reports.size()), "index out of range" );

		return l_Reports[i_Index];
	}

}
