/*****************************************************************************
**	rptReportInv.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Reports/ReportInv/rptReportInv.hpp"

#include "Features/Reports/rptReportData.hpp"
#include "Features/Reports/rptReportUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
rptReportInv::rptReportInv()
{
}

//--------------------------------------------------------------------
//	Destructor
//--------------------------------------------------------------------
rptReportInv::~rptReportInv()
{
}

//--------------------------------------------------------------------
//	Generate() - generate the report
//--------------------------------------------------------------------
//virtual
void rptReportInv::Generate()
{
	rptReportData data;
	rptReportUtil::BuildInvDataList( docSingleDocumentMgr::GetDocument(), data );

	//
	int cindex;
	int iindex;
	int inum;
	int cnum = data.m_Chunks.size();

	DBG_LOG0(" inventory report ");
	DBG_LOG0(" ---------------- \n");
	DBG_LOG0("This is the inventory report\n");

	for ( cindex = 0 ; cindex < cnum ; cindex++ )
	{
		inum = data.m_Chunks[cindex].m_Items.size();

		DBG_LOG1("\nGroup: %s\n", data.m_Chunks[cindex].m_Desc.c_str() );

		for ( iindex = 0 ; iindex < inum ; iindex++ )
		{
			DBG_LOG1("\t%s", data.m_Chunks[cindex].m_Items[iindex].c_str() );
		}
	}

	DBG_LOG0("\nend of report");
}

