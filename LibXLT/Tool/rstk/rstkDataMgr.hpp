/*****************************************************************************
**	rstkDataMgr.hpp
**
**		Manage the Resource Tracker Data
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RSTK_DATAMGR_HPP
#error rstkDataMgr.hpp multiply included
#endif
#define RSTK_DATAMGR_HPP

#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif


//============================================================================
//============================================================================
namespace rstkDataMgr
{
	//------------------------------------------------------------------------
	//	get the current data
	//------------------------------------------------------------------------
	fsResourceTrackerData& Data();

	//------------------------------------------------------------------------
	//	set the current project data
	//------------------------------------------------------------------------
	void SetData(const fsResourceTrackerData& i_Data);
}
