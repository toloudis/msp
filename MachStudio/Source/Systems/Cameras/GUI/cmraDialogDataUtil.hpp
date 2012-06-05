/*****************************************************************************
**	cmraDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DIALOGDATAUTIL_HPP
#error cmraDialogDataUtil.hpp multiply included
#endif
#define CMRA_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
//============================================================================
//============================================================================
class cmraData;
class cmraScriptData;
class camCamera;

//============================================================================
//============================================================================
class cmraDialogDataUtil
{
public:
	//--------------------------------------------------------------------
	//  Update dialog of the list
	//--------------------------------------------------------------------
	static void  UpdateListDialog();

	//--------------------------------------------------------------------
	//	Rebuild the list data
	//--------------------------------------------------------------------
	static void RebuildListData(cmmDialogDataList& io_DataList);

	//--------------------------------------------------------------------
	//	Rebuild the list data
	//--------------------------------------------------------------------
	static void RebuildCameraList(std::vector<shared_ptr<camCamera>>& io_CameraList,
								std::vector<std::string>& io_NameList);

	//--------------------------------------------------------------------
	//	Delete
	//--------------------------------------------------------------------
	static void DeleteObject(int i_Index);

};	// end of static class
