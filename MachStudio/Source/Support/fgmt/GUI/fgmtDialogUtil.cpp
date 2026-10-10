/*****************************************************************************
**	fgmtDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"

#ifdef MSP_GUI_QT
#include "Support/fgmt/qtGUI/fgmtUVEditor.hpp"
#else
#include "Support/fgmt/wxGUI/fgmtUVEditor.hpp"
#endif

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
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

//============================================================================
//============================================================================
namespace fgmtDialogUtil
{
	namespace 
	{
		g2dSystem* l_pSystem = NULL;
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		static fgmtUVEditor* l_pUVEditor = NULL;
#endif
		std::string l_UVEditorTitle = "UV Viewer : ";
	} // end namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init(g2dSystem* i_pSystem)
	{

		l_pSystem = i_pSystem;
#ifdef USE_WXWIDGETS
		if (l_pUVEditor == NULL)
		{
			// parented to main form so it will be auto deleted.
			l_pUVEditor = new fgmtUVEditor( twxSystem::g_pMainForm, l_UVEditorTitle + "none selected" );
		}
#elif defined(USE_QT)
		if (l_pUVEditor == NULL)
		{
			// parented to main form so it will be auto deleted.
			l_pUVEditor = new fgmtUVEditor( tqtSystem::g_pMainForm, l_UVEditorTitle + "none selected" );
		}
#endif


	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		l_pSystem = NULL;

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		// auto-deleted by parent window.
		if (l_pUVEditor)
		{
#ifdef USE_WXWIDGETS
			l_pUVEditor->Close(true);
#endif
			delete l_pUVEditor;
		}
		l_pUVEditor = NULL;
#endif
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(fgmtScriptObject *i_pObject)
	{

		// On any change of the selected object, the material list is de-selected, so
		// close the material dialog
//		ShowFragmentDialog(false);
	}

	//--------------------------------------------------------------------
	// Get the graphics windowing system
	//--------------------------------------------------------------------
	g2dSystem* GetSystem()
	{
		return l_pSystem;
	}

	void ShowUVEditor()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show(l_pUVEditor);
#elif defined(USE_QT)
		tqtPaneMgr::Show(l_pUVEditor);
#endif
	}
	void SetUVEditorFragment(g3dSceneNode* i_pFrag, const std::string& i_Name)
	{
#ifdef USE_WXWIDGETS
		if (l_pUVEditor)
		{
			l_pUVEditor->SetNode(i_pFrag);
			twxPaneMgr::SetCaption(l_pUVEditor, l_UVEditorTitle + i_Name);
		}
#elif defined(USE_QT)
		if (l_pUVEditor)
		{
			l_pUVEditor->SetNode(i_pFrag);
			tqtPaneMgr::SetCaption(l_pUVEditor, l_UVEditorTitle + i_Name);
		}
#endif
	}

	void NotifyUVEditorDestroyed()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		l_pUVEditor = NULL;
#endif
	}

}	// end of namespace
