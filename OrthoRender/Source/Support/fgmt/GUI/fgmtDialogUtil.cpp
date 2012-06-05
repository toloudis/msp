/*****************************************************************************
**	fgmtDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"

#include "Support/fgmt/GUI/fgmtFragmentsForm.h"
#include "Support/fgmt/wxGUI/fgmtUVEditor.hpp"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
#endif // _MANAGED


//============================================================================
//============================================================================
namespace fgmtDialogUtil
{
	namespace 
	{
		matMaterial* l_HighlightMaterial = NULL;
		g2dSystem* l_pSystem = NULL;
#ifdef USE_WXWIDGETS
		static fgmtUVEditor* l_pUVEditor = NULL;
#endif
		std::string l_UVEditorTitle = "UV Editor : ";
	} // end namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init(g2dSystem* i_pSystem)
	{
#ifdef _MANAGED
		// Create tab page dialog
		if (!fgmtFragmentsForm::FormInstance)
		{
			fgmtFragmentsForm::FormInstance = gcnew fgmtFragmentsForm();
		}
#endif // _MANAGED


		l_pSystem = i_pSystem;
#ifdef USE_WXWIDGETS
		if (l_pUVEditor == NULL)
		{
			// parented to main form so it will be auto deleted.
			l_pUVEditor = new fgmtUVEditor( twxSystem::g_pMainForm, l_UVEditorTitle + "none selected" );
		}
#endif


		l_HighlightMaterial = new matMaterial(maFloatRGBA(0,0,0,1),
			maFloatRGBA(0,0,0,0),
			maFloatRGBA(1,0.8f,1,1));

		an2StateAnimation<maFloatRGBA>* color_anim = 
				new an2StateAnimation<maFloatRGBA>(	
						maFloatRGBA(0.2f, 0.2f, 0.7f, 1.0f),
						maFloatRGBA(0.7f, 0.7f, 0.7f, 1.0f),
						1.0f);
		color_anim->SetLooping(true);
		color_anim->SetReversing(true);

		matMatAnim* sel_anim = new matMatAnim(color_anim, matMatParamIndex::e_Emissive, 0.0f);
		sel_anim->SetUseRealTime(true);
		l_HighlightMaterial->AddMatAnim(sel_anim);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		l_pSystem = NULL;

		RemoveDataPage();

#ifdef _MANAGED
		fgmtFragmentsForm::FormInstance = nullptr;
#endif // _MANAGED
#ifdef USE_WXWIDGETS
		// auto-deleted by parent window.
		l_pUVEditor = NULL;
		delete l_pUVEditor;
#endif

		delete l_HighlightMaterial;
		l_HighlightMaterial = NULL;
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(fgmtScriptObject *i_pObject)
	{
#ifdef _MANAGED
		if (fgmtFragmentsForm::FormInstance )
		{
			fgmtFragmentsForm::FormInstance->Update(i_pObject);
		}
#endif // _MANAGED

		// On any change of the selected object, the material list is de-selected, so
		// close the material dialog
//		ShowFragmentDialog(false);
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{
#ifdef _MANAGED
		if (!cmmObjectDialogUtil::HasTabPage(fgmtFragmentsForm::FormInstance->GetTabPage(0)))
			cmmObjectDialogUtil::AddTabPage( fgmtFragmentsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
	}
	void  RemoveDataPage()
	{
#ifdef _MANAGED
		if (cmmObjectDialogUtil::HasTabPage(fgmtFragmentsForm::FormInstance->GetTabPage(0)))
			cmmObjectDialogUtil::RemoveTabPage( fgmtFragmentsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
		//ShowFragmentDialog(false);
	}

	matMaterial* GetHighlightMaterial()
	{
		return l_HighlightMaterial;
	}

	//--------------------------------------------------------------------
	// Update checked state of highlight button
	//--------------------------------------------------------------------
	void UpdateHighlightToggle()
	{
#ifdef _MANAGED
		if (fgmtFragmentsForm::FormInstance)
			fgmtFragmentsForm::FormInstance->UpdateHighlightToggle();
#endif // _MANAGED
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
#endif
	}
	void SetUVEditorFragment(g3dFragment* i_pFrag, const std::string& i_Name)
	{
		if (l_pUVEditor)
		{
			l_pUVEditor->SetFragment(i_pFrag);
			twxPaneMgr::SetCaption(l_pUVEditor, l_UVEditorTitle + i_Name);
		}
	}

	void NotifyUVEditorDestroyed()
	{
		l_pUVEditor = NULL;
	}

}	// end of namespace
