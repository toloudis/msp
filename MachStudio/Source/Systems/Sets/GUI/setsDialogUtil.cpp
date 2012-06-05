/*****************************************************************************
**	setsDialogUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/GUI/setsDialogUtil.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/GUI/setsDialogDataUtil.hpp"


//============================================================================
//============================================================================
namespace setsDialogUtil
{
	namespace
	{
		// Callback when any data in setsDataMgr changes
		class MyDataChanged : public setsDataMgr::DataChangedCallback
		{
		public:
			virtual void DataChanged()
			{
				setsDialogDataUtil::UpdateListDialog();
			}
		};

		MyDataChanged l_DataChangedObj;

	}	// end of namespace

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init()
	{
		//setsDataMgr::AddDataChangedCallback(&l_DataChangedObj);

		setsDialogDataUtil::UpdateListDialog();
	}

	//--------------------------------------------------------------------
	// Clean up dialogs
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//setsDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);
	}

	//--------------------------------------------------------------------
	//	Rebuild the list dialog
	//--------------------------------------------------------------------
	void RebuildListDialog()
	{
		setsDialogDataUtil::UpdateListDialog();
	}

}	// end of namespace
