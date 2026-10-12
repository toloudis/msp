/*****************************************************************************
**	rdrLayersDialogUtil.cpp
**
**		see .hpp
**
\****************************************************************************/
#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"

#include "Features/RenderLayers/rdrLayersDataUtil.hpp"
#ifdef MSP_GUI_QT
#include "Features/RenderLayers/qtGUI/rdrLayersDialog.hpp"
#else
#include "Features/RenderLayers/wxGUI/rdrLayersDialog.hpp"
#endif
#include "Support/rprf/rprfPrefsObject.hpp"

#ifdef USE_WXWIDGETS
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#else
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif
#elif defined(USE_QT)
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"
#endif

#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace rdrLayersDialogUtil
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	
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
#elif defined(USE_QT)
		//if (rdrLayersDialog::Instance == NULL)
		//{
		//	rdrLayersDialog::Instance = new rdrLayersDialog( tqtSystem::g_pMainForm );
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
#elif defined(USE_QT)
		//rlyrRenderLayerMgr::Update();
		//rdrLayersDialog::Instance->Update();
		//tqtPaneMgr::Show(rdrLayersDialog::Instance);
		//rdrLayersDialog::Instance->SetFocus();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		
#endif
	}

	//------------------------------------------------------------------------
	// Update - repopulate lists and trees
	//------------------------------------------------------------------------
	void UpdateTabs()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if(rdrLayersDialog::Instance)
			rdrLayersDialog::Instance->ResetSelection();
#endif
	}

	//------------------------------------------------------------------------
	// UpdatePfxShader - repopulate shader ui
	//------------------------------------------------------------------------
	void UpdatePfxShader()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		if(rdrLayersDialog::Instance)
			rdrLayersDialog::Instance->UpdatePfxShader();
#endif
	}

}