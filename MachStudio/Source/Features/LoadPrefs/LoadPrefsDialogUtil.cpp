/*****************************************************************************
**	LoadPrefsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/LoadPrefs/LoadPrefsDialogUtil.hpp"

#include "Features/LoadPrefs/LoadPrefsMgr.hpp"
#ifdef MSP_GUI_QT
#include "Features/LoadPrefs/qtGUI/LoadPreferencesDialog.hpp"
#else
#include "Features/LoadPrefs/wxGUI/LoadPreferencesDialog.hpp"
#endif

#include "Core/prty/prtyObject.hpp"
#include "Graphics/mat/matTextureTracking.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"
#else
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif


//============================================================================
//============================================================================
namespace LoadPrefsDialogUtil
{
	//--------------------------------------------------------------------
	//  Show load preferences dialog modally
	//--------------------------------------------------------------------
	void  Show()
	{
		// Have to sync up the max subdiv level setting before showing,
		// because this flag can be altered by other parts of the code.
		int max_subdiv = smdlSubdivCharacter::GetMaxSubdivLevel();
		LoadPrefsMgr::Data().m_MaxSubdivLevel = max_subdiv;

		//
		prtyObject* pDO = LoadPrefsMgr::GetDataObject();
		pDO->SortListByCategory();
		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;


#ifdef USE_WXWIDGETS
		DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
		LoadPreferencesDialog dialog(twxSystem::g_pMainForm);
		
		pwxFormControlBuilder::BuildForm(dialog.GetTabPageMain(), "LoadPrefs",
			(pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

		// Load up the list of missing textures
		dialog.SetMissingTextureList( matTextureTracking::GetMissingTextureSet() );

		dialog.ShowModal();
#elif defined(USE_QT)
		DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
		LoadPreferencesDialog dialog(tqtSystem::g_pMainForm);
		
		pqtFormControlBuilder::BuildForm(dialog.GetTabPageMain(), "LoadPrefs",
			(pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

		// Load up the list of missing textures
		dialog.SetMissingTextureList( matTextureTracking::GetMissingTextureSet() );

		dialog.exec();
#endif
	}

}	// end of namespace

