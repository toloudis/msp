/*****************************************************************************
**	cptrRenderOptionsDialog.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderOptionsDialog.hpp"

#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"

#include "ToolUIWx/Pwx/pwxFormControlBuilder.hpp"

#ifdef USE_WXWIDGETS
#define ID_DEFAULT wxID_ANY // Default


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderOptionsDialog::cptrRenderOptionsDialog( wxWindow* parent )
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
	m_notebook_Options->AddPage( pTabPage_MainOptions, wxT("Options"), true );

	//	create the entries for the options tab
	prtyObject* pDO = cptrRenderOutputDataUtil::GetDataObject();
	pDO->SortListByCategory();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	pwxFormControlBuilder::BuildForm(pTabPage_MainOptions, (pDO->GetList()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

	//	Add the Render tab page
	wxScrolledWindow* pTabPage_Render = new wxScrolledWindow( m_notebook_Options, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	pTabPage_Render->SetScrollRate( 0, 5 );
	wxBoxSizer* sizer_Render;
	sizer_Render = new wxBoxSizer( wxVERTICAL );
	pTabPage_Render->SetSizer( sizer_Render );
	pTabPage_Render->Layout();
	sizer_Render->Fit( pTabPage_Render );
	m_notebook_Options->AddPage( pTabPage_Render, wxT("Render"), true );

	//	create the entries for the Render tab
	prtyObject* pRDO = rndrPrefsMgr::GetDataObject(rndrPrefsMgr::e_RenderFullPrefs);
	pwxFormControlBuilder::BuildForm(pTabPage_Render, (pRDO->GetList()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
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
		cptrRenderOptionsDialogUtil::SetExitting(true);
		event.Skip();
	}
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