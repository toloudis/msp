/*****************************************************************************
**	rdrLayersDialogUtil.cpp
**
**		see .hpp
**
\****************************************************************************/
#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"

#include "Features/RenderLayers/rdrLayersDataUtil.hpp"
#include "Features/RenderLayers/wxGUI/rdrLayersDialog.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace rdrLayersDialogUtil
{

#ifdef USE_WXWIDGETS
	
#endif
	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void  Init()
	{

#ifdef USE_WXWIDGETS
		//if (rdrLayersDialog::Instance == NULL)
		//{
		//	rdrLayersDialog::Instance = new rdrLayersDialog( twxSystem::g_pMainForm );
		//}
		//rdrLayersDialog::Instance->Update();
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef USE_WXWIDGETS
		//rlyrRenderLayerMgr::Update();
		//rdrLayersDialog::Instance->Update();
		//twxPaneMgr::Show(rdrLayersDialog::Instance);
		//rdrLayersDialog::Instance->SetFocus();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef USE_WXWIDGETS
		
#endif
	}

	//------------------------------------------------------------------------
	// Update - repopulate lists and trees
	//------------------------------------------------------------------------
	void UpdateTabs()
	{
#ifdef USE_WXWIDGETS
		if(rdrLayersDialog::Instance)
			rdrLayersDialog::Instance->ResetSelection();
#endif
	}

	//------------------------------------------------------------------------
	// UpdatePfxShader - repopulate shader ui
	//------------------------------------------------------------------------
	void UpdatePfxShader()
	{
#ifdef USE_WXWIDGETS
		if(rdrLayersDialog::Instance)
			rdrLayersDialog::Instance->UpdatePfxShader();
#endif
	}

}