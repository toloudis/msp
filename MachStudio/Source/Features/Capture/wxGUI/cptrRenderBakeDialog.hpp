/*****************************************************************************
**	cptrRenderBakeDialog.hpp
**
**		implementation of the render baking texture dialog
**
\****************************************************************************/
#ifdef CPTR_RENDERBAKEDIALOG_HPP
#error cptrRenderBakeDialog.hpp multiply included
#endif
#define CPTR_RENDERBAKE_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifndef CPTR_RENDERBAKEDATA_HPP
#include "Features/Capture/cptrRenderBakeData.hpp"
#endif

#include <wx/aui/auibook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <string>


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Forward References
//----------------------------------------------------------------------------
class rprfPrefsObject;

//============================================================================
// Class RdrLayersDialog
//============================================================================
class cptrRenderBakeDialog : public wxDialog 
{
	public:
		static cptrRenderBakeDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderBakeDialog(cptrRenderBakeData& i_Data, wxWindow* parent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cptrRenderBakeDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Update();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateTabbedPage();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateObjectList();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void RegisterRenderPrefs();
	private:
		void button_bake_OnButtonClick( wxCommandEvent& event );
		virtual void OnClose( wxCloseEvent& event );

		wxPanel* m_panel_Dialog;
		wxAuiNotebook* m_layerTabPages;
		//wxTreeCtrl* m_treeCtrl_Objects;
		wxScrolledWindow* m_scrolledWindow_Prefs;

		cptrRenderBakeData m_Data;

		bool m_bakeAborted;
};
#endif
