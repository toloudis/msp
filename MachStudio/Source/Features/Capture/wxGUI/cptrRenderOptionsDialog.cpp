/*****************************************************************************
**	cptrRenderOptionsDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderOptionsDialog.hpp"

#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"
#include "Features/RenderLayers/wxGUI/rdrLayersDialog.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"

#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "ToolUIWx/Pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#ifdef USE_WXWIDGETS
#define ID_DEFAULT wxID_ANY // Default

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderOptionsDialog::cptrRenderOptionsDialog( wxWindow* parent, const wxString& i_Title, 
												const wxString& i_Caption )
:	cptrRenderOptionsDialogBase(parent)
{
	//	Add the Options tab page
	wxScrolledWindow* pTabPage_MainOptions = new wxScrolledWindow( m_notebook_Options, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	pTabPage_MainOptions->SetScrollRate( 0, 5 );
	wxBoxSizer* sizer_MainOptions;
	sizer_MainOptions = new wxBoxSizer( wxVERTICAL );
	pTabPage_MainOptions->SetSizer( sizer_MainOptions );
	pTabPage_MainOptions->Layout();
	sizer_MainOptions->Fit( pTabPage_MainOptions );
	m_notebook_Options->AddPage( pTabPage_MainOptions, wxT("Output"), true );

	//	create the entries for the options tab
	prtyObject* pDO = captRenderOutputDataUtil::GetDataObject();
	//pDO->SortListByCategory();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	pwxFormControlBuilder::BuildForm(pTabPage_MainOptions, "Render Layers", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

	//	Add the Render Layers tab page
	rdrLayersDialog* pDialog_RenderLayers = new rdrLayersDialog( m_notebook_Options );
	m_notebook_Options->AddPage( pDialog_RenderLayers, wxT("Render Layers"), true );

	pDialog_RenderLayers->Update();
	//pDialog_RenderLayers->Update(true);
	rdrLayersDialog::Instance = pDialog_RenderLayers;
	rprfPrefsUtil::SetUpdateFunction(&rdrLayersDialogUtil::UpdateTabs);
	// Add this panel to the AUI manager
	//twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Left());
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderOptionsDialog::cptrRenderOptionsDialog_OnClose( wxCloseEvent& event )
{
	//	if the dialog is closing and the user didn't hit capture then exit out of capture mode
	//
	if ( !cptrRenderUtil::GetCapture() )
	{
		gpxRenderControl::ConfirmSingleThread();
		cptrRenderOptionsDialogUtil::SetExitting(true);
		event.Skip();
	}

	// Modeless dialog, need to destroy it
	this->Destroy();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderOptionsDialog::button_Render_OnButtonClick( wxCommandEvent& event )
{
	cptrRenderOptionsDialogUtil::Hide();

	this->Close();
	//event.Skip();
}
#endif