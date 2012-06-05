/*****************************************************************************
**	cptrRenderBakeDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderBakeDialogUtil.hpp"

#include "Features/Capture/cptrRenderBakeDataUtil.hpp"
#include "Features/Capture/wxGUI/cptrRenderBakeDialog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"

//	Library
#include "ToolUIWx/twx/twxSystem.hpp"


//
namespace cptrRenderBakeDialogUtil
{
#ifdef USE_WXWIDGETS
	cptrRenderBakeDialog* l_pRBDialog;
#endif

	cptrRenderBakeData l_Data;

	namespace
	{
		 bool l_bClosing = false;
		 bool l_bExitting = false;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		l_Data = cptrRenderBakeDataUtil::Data();

		l_bClosing = false;
		l_bExitting = false;

#ifdef USE_WXWIDGETS
		if (docSingleDocumentMgr::GetFilename().GetNumNames() > 0)
		{
			cptrRenderBakeDialog* pDialog = new cptrRenderBakeDialog(l_Data, twxSystem::g_pMainForm );
			pDialog->Show();
			l_pRBDialog = pDialog;
		}
		else
		{
			guiMessageBox::Show("Please load a scene first.", "Bake Error");

			l_bClosing = true;
			l_bExitting = true;
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		//cptrRenderBatchDataUtil::UpdateData(l_Data);

		//	if there is a batch filename
		//
		/*if (l_Data.m_BatchFilename.GetValue().GetNumNames() > 0)
			cptrRenderBatchDataUtil::WriteData(l_Data.m_BatchFilename.GetValue(), l_Data);*/
	
#ifdef USE_WXWIDGETS
		l_pRBDialog->Hide();
#endif
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

