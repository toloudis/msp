/*****************************************************************************
**	cptrRenderStatsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"

#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#ifdef MSP_GUI_QT
#include "Features/Capture/qtGUI/cptrRenderStatsDialog.hpp"
#else
#include "Features/Capture/wxGUI/cptrRenderStatsDialog.hpp"
#endif

#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif


//============================================================================
//============================================================================
namespace cptrRenderStatsDialogUtil
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	static cptrRenderStatsDialog* l_pRenderStatsDialog = NULL;
#endif

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{

#ifdef USE_WXWIDGETS
		if (l_pRenderStatsDialog == NULL)
		{
			cptrRenderStatsDataUtil::ReadRenderStats( cptrRenderStatsDataUtil::Data() );
			cptrRenderStatsDataUtil::SetRenderStatsVisible(true);

			l_pRenderStatsDialog = new cptrRenderStatsDialog( twxSystem::g_pMainForm );
		}

		UpdateStatsDialog();
		l_pRenderStatsDialog->Show();
#elif defined(USE_QT)
		if (l_pRenderStatsDialog == NULL)
		{
			cptrRenderStatsDataUtil::ReadRenderStats( cptrRenderStatsDataUtil::Data() );
			cptrRenderStatsDataUtil::SetRenderStatsVisible(true);

			l_pRenderStatsDialog = new cptrRenderStatsDialog( tqtSystem::g_pMainForm );
		}

		UpdateStatsDialog();
		l_pRenderStatsDialog->show();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		cptrRenderStatsDataUtil::SetRenderStatsVisible(false);

#ifdef USE_WXWIDGETS
		l_pRenderStatsDialog->Hide();
		l_pRenderStatsDialog = NULL;
#elif defined(USE_QT)
		l_pRenderStatsDialog->hide();
		l_pRenderStatsDialog = NULL;
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateStatsDialog()
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		std::string outstring;
		cptrRenderStatsDataUtil::BuildOutputString(outstring);
		if (l_pRenderStatsDialog != NULL)
		{
			l_pRenderStatsDialog->UpdateStatString( itString(outstring.c_str()) );
		}
#endif
	}

	//--------------------------------------------------------------------
	// Append message on render stats
	//--------------------------------------------------------------------
	void AddStatsMessage(itString& i_Message)
	{
		if (i_Message.GetLength() > 0)
			DBG_LOG(i_Message);

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (l_pRenderStatsDialog != NULL)
		{
			l_pRenderStatsDialog->AddStatString( i_Message );
		}
#endif
	}

	//--------------------------------------------------------------------
	// Next line
	//--------------------------------------------------------------------
	void Newline()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (l_pRenderStatsDialog != NULL)
		{
			l_pRenderStatsDialog->AddStatString(itString());
		}
#endif
	}

	//------------------------------------------------------------------------
	//  Clear
	//------------------------------------------------------------------------
	void Clear()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if (l_pRenderStatsDialog != NULL)
		{
			l_pRenderStatsDialog->Clear();
		}
#endif
	}
}

