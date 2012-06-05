/*****************************************************************************
**	cptrRenderBatchDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderBatchDialogUtil.hpp"

#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/mGUI/cptrRenderBatchForm.h"


//
namespace cptrRenderBatchDialogUtil
{
	cptrRenderBatchData l_Data;

	namespace
	{
		 bool l_bClosing = false;
		 bool l_bExitting = false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		l_Data = cptrRenderBatchDataUtil::Data();
		//cptrRenderBatchDataUtil::ReadData(l_Data.m_BatchFilename.GetValue(), l_Data);

		l_bClosing = false;
		l_bExitting = false;

#ifdef _MANAGED
		StudioFramework::cptrRenderBatchForm^ pDialog = gcnew StudioFramework::cptrRenderBatchForm( l_Data );
		pDialog->Show();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		cptrRenderBatchDataUtil::UpdateData(l_Data);

		//	if there is a batch filename
		//
		if (l_Data.m_BatchFilename.GetValue().GetNumNames() > 0)
			cptrRenderBatchDataUtil::WriteData(l_Data.m_BatchFilename.GetValue(), l_Data);
	}

	//--------------------------------------------------------------------
	//	IsDialogClosing() - return true if the user has hit the
	//	"capture" button closing the dialog
	//--------------------------------------------------------------------
	bool IsDialogClosing()
	{
		return l_bClosing;
	}

	//--------------------------------------------------------------------
	//	SetDialogClosing() - set to true if the user has hit the
	//	"capture" button closing the dialog
	//--------------------------------------------------------------------
	void SetDialogClosing( bool i_bClosing )
	{
		l_bClosing = i_bClosing;
	}


	//--------------------------------------------------------------------
	//	IsDialogExitting() - return true if the user has hit the
	//	"X" button Exitting the dialog
	//--------------------------------------------------------------------
	bool IsDialogExitting()
	{
		return l_bExitting;
	}

	//--------------------------------------------------------------------
	//	SetDialogExitting() - set to true if the user has hit the
	//	"X" button Exitting the dialog
	//--------------------------------------------------------------------
	void SetDialogExitting( bool i_bExitting )
	{
		l_bExitting = i_bExitting;
	}

}	// end of namespace

