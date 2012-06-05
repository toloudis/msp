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
#include "Features/Capture/wxGUI/cptrRenderStatsDialog.hpp"

#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
namespace cptrRenderStatsDialogUtil
{

#ifdef USE_WXWIDGETS
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
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateStatsDialog()
	{

#ifdef USE_WXWIDGETS
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

#ifdef USE_WXWIDGETS
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
#ifdef USE_WXWIDGETS
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
#ifdef USE_WXWIDGETS
		if (l_pRenderStatsDialog != NULL)
		{
			l_pRenderStatsDialog->Clear();
		}
#endif
	}
}

