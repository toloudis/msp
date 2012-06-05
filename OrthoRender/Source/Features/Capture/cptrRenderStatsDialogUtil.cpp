/*****************************************************************************
**	cptrRenderStatsDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"

#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Capture/mGUI/cptrRenderStatsMain.h"
#include "Features/Capture/wxGUI/cptrRenderStatsDialog.hpp"

#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
namespace cptrRenderStatsDialogUtil
{
#ifdef _MANAGED
	public ref class csd
	{
		public: static StudioFramework::cptrRenderStatsMain^ l_pRenderStatsDialog = nullptr;
	};
#endif
#ifdef USE_WXWIDGETS
	static cptrRenderStatsDialog* l_pRenderStatsDialog = NULL;
#endif

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef _MANAGED
		if (csd::l_pRenderStatsDialog == nullptr)
		{
			cptrRenderStatsDataUtil::ReadRenderStats( cptrRenderStatsDataUtil::Data() );
			cptrRenderStatsDataUtil::SetRenderStatsVisible(true);

			csd::l_pRenderStatsDialog = gcnew StudioFramework::cptrRenderStatsMain();
		}

		UpdateStatsDialog();
		csd::l_pRenderStatsDialog->Show();
#endif
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

#ifdef _MANAGED
		csd::l_pRenderStatsDialog->Hide();
		csd::l_pRenderStatsDialog = nullptr;
#endif
#ifdef USE_WXWIDGETS
		l_pRenderStatsDialog->Hide();
		l_pRenderStatsDialog = NULL;
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateStatsDialog()
	{
#ifdef _MANAGED
		std::string outstring;
		cptrRenderStatsDataUtil::BuildOutputString(outstring);
		if (csd::l_pRenderStatsDialog != nullptr)
		{
			csd::l_pRenderStatsDialog->UpdateStatString( gcnew System::String(outstring.c_str()) );
		}
#endif
#ifdef USE_WXWIDGETS
		std::string outstring;
		cptrRenderStatsDataUtil::BuildOutputString(outstring);
		if (l_pRenderStatsDialog != NULL)
		{
			l_pRenderStatsDialog->UpdateStatString( outstring );
		}
#endif
	}
}

