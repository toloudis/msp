/*****************************************************************************
**	rdrLayerDialog.hpp
**
**		implementation of the render layer dialog
**
\****************************************************************************/
#ifdef RDR_LAYERSDIALOG_HPP
#error rdrLayersDialog.hpp multiply included
#endif
#define RDR_LAYERSDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Features/RenderLayers/wxGUI/rdrLayersDialogBase.h"
#endif

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

#include <string>


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Forward References
//----------------------------------------------------------------------------
class rlyrPassesObject;
class rlyrRenderCam;
class prtyObject;
class captRenderOutputObject;
class rprfPrefsObject;
class pfxPostEffectObject;

//============================================================================
// Class RdrLayersDialog
//============================================================================
class rdrLayersDialog : public rdrLayersDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static rdrLayersDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rdrLayersDialog( wxWindow* parent, 
						  const std::string& i_Title = "Render Layers", 
						  const std::string& i_Caption = "Render Layers" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~rdrLayersDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//virtual void Update(bool i_bShowButtons = false);
		virtual void Update();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Draw();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//virtual void UpdatePfxPage();
				
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void ResetSelection();
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdateTabPages(rprfPrefsObject* i_RenderPrefs = NULL,
			rlyrPassesObject* i_RenderPasses = NULL,
			pfxPostEffectObject* i_RenderPfx = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdatePfxShader();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdateLayerTree();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdateObjectList();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual	void UpdateRenderPrefs(rprfPrefsObject* i_RenderPrefs = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdatePassesList(rlyrPassesObject* i_RenderPasses = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdatePfx(pfxPostEffectObject* i_RenderPfx = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UpdateOutputFormat(captRenderOutputObject* i_CaptureOptions = NULL);
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void buildLayerTree(wxTreeItemId &io_Root);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void buildChildLayers(wxTreeItemId &io_ParentItem, rlyrRenderCam* i_ParentCam);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void deleteSelection();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_OnActivate( wxActivateEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeCharPressed( wxKeyEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeLeftMouseDown( wxMouseEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeBeginLabelEdit( wxTreeEvent& event );
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeDeleteItem( wxTreeEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeEndLabelEdit( wxTreeEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeItemActivated( wxTreeEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeSelChanged( wxTreeEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeSelChanging( wxTreeEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TreeImageClick( wxTreeEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_AddButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_DuplicateButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_DeleteButtonClick( wxCommandEvent& event );

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void rdrLayersDialog_RenameButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TabPageChanged( wxAuiNotebookEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_TabPageChanging( wxAuiNotebookEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void rdrLayersDialog_ObjectToggle( wxCommandEvent& event );

private:
	wxTreeItemId m_curSelection;
	bool m_bPreviewDialog;
};
#endif
