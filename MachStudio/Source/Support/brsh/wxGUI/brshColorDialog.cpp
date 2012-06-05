/*****************************************************************************
**	brshColorDialog.cpp
**
**		see. hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Support/brsh/wxGUI/brshColorDialog.hpp"

#include "Support/brsh/brshColorDialogUtil.hpp"
#include "Support/brsh/brshPaintBrushMgr.hpp"

#include "Core/fs/fsLocator.hpp"

#include "ToolUIWx/Pwx/pwxFormControlBuilder.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif
#ifdef USE_WXWIDGETS


//Event table, callback for window closing
BEGIN_EVENT_TABLE(brshColorDialog, wxPanel)
	EVT_CLOSE(brshColorDialog::brshColorDialog_onClose)
END_EVENT_TABLE()

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
brshColorDialog* brshColorDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
brshColorDialog::brshColorDialog( wxWindow* parent, const wxString& i_Title, 
									const wxString& i_Caption )
:	brshColorDialogBase(parent)
{
	//	Add the Options tab page
	wxScrolledWindow* pTabPage_Brush = new wxScrolledWindow( m_panel_Brush, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	pTabPage_Brush->SetScrollRate( 0, 5 );
	
	wxBoxSizer* sizer_brush;
	sizer_brush = new wxBoxSizer( wxVERTICAL );
	sizer_brush->Add(pTabPage_Brush, 1, wxALL|wxEXPAND, 5 );
	m_panel_Brush->SetSizer(sizer_brush);
	m_panel_Brush->Layout();
	sizer_brush->Fit( m_panel_Brush );

	//	create the entries for the paint brush tab
	prtyObject* pDO = brshPaintBrushMgr::GetBrushObject();
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	pwxFormControlBuilder::BuildForm(pTabPage_Brush, "PaintBrush", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Left());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
brshColorDialog::~brshColorDialog()
{
	if (brshColorDialog::Instance == this)
	{
		twxPaneMgr::Show(brshColorDialog::Instance, false);
		brshColorDialog::Instance = NULL;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void brshColorDialog::brshColorDialog_onClose( wxCloseEvent& event )
{
	brshColorDialogUtil::CleanUp();
	event.Skip();
}
#endif