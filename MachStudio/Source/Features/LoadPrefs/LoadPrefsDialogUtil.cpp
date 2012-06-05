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
#include "Features/LoadPrefs/wxGUI/LoadPreferencesDialog.hpp"

#include "Core/prty/prtyObject.hpp"
#include "Graphics/mat/matTextureTracking.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


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
#endif
	}

}	// end of namespace

