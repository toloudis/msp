/********************************************************************************************\
**  mexpExportData.hpp
**
**		Data for the export to Maya ascii.
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef MEXP_EXPORTDATA_HPP
#error mexpExportData.hpp multiply included
#endif
#define MEXP_EXPORTDATA_HPP

#include <string>
#include <vector>


//============================================================================
//============================================================================
struct mexpExportChunkListData
{
	std::string m_Desc;
	bool m_bRemovableChunk;
	std::vector<std::string> m_Items;
};

//============================================================================
//============================================================================
struct mexpExportData
{
	mexpExportData();

	//fsLocator m_LastFile;

	bool m_bExportAnimation;
	float m_SimulationFrameRate;
	bool m_bExportJoints;

	std::vector<mexpExportChunkListData> m_Chunks;
};

