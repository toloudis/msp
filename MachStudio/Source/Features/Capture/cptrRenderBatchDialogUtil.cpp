/*****************************************************************************
**	cptrRenderBatchDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderBatchDialogUtil.hpp"

#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#ifdef MSP_GUI_QT
#include "Features/Capture/qtGUI/cptrRenderBatchDialog.hpp"
#else
#include "Features/Capture/wxGUI/cptrRenderBatchDialog.hpp"
#endif

//	Library
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif


//
namespace cptrRenderBatchDialogUtil
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	cptrRenderBatchDialog* l_pRBDialog;
#endif

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

#ifdef USE_WXWIDGETS
		cptrRenderBatchDialog* pDialog = new cptrRenderBatchDialog( l_Data, twxSystem::g_pMainForm );
		pDialog->Show();
		l_pRBDialog = pDialog;
#elif defined(USE_QT)
		cptrRenderBatchDialog* pDialog = new cptrRenderBatchDialog( l_Data, tqtSystem::g_pMainForm );
		pDialog->show();
		l_pRBDialog = pDialog;
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
	
#ifdef USE_WXWIDGETS
		l_pRBDialog->Hide();
#elif defined(USE_QT)
		l_pRBDialog->hide();
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

