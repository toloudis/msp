/********************************************************************************************\
**  ReportData.hpp
**
**		Data for the Report.
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef RPT_REPORTDATA_HPP
#error rptReportData.hpp multiply included
#endif
#define RPT_REPORTDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//struct rptReportChunkData
//{
//	std::string m_Desc;
//}

struct rptReportChunkListData
{
	std::string m_Desc;
	std::vector<std::string> m_Items;
	//std::vector<ReportChunkData> m_Items;
};

//
//
struct rptReportData
{
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	std::vector<rptReportChunkListData> m_Chunks;
};

