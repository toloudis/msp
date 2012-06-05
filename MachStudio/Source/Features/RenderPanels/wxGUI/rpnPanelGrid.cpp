/*****************************************************************************
**  rpnPanelGrid.hpp
**
**     Window using wxWidgets for doing MachStudio rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"

#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"


#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(rpnPanelGrid, wxPanel)
//	EVT_PAINT(rpnRenderPanel::OnPaint)
END_EVENT_TABLE()

//--------------------------------------------------------------------
//	Static pointer to the instance of the control, will be cleared
//	when the panel grid is deleted.
//--------------------------------------------------------------------
rpnPanelGrid* rpnPanelGrid::Instance = NULL;

//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
rpnPanelGrid::rpnPanelGrid(wxWindow* parent)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0), 
	m_Layout(e_FourPanels),
	m_MaxWidth(-1), 
	m_MaxHeight(-1)
{
	rpnPanelGrid::Instance = this;

	// We want the whole grid of panes to be shaped so that they stay packed together
	// but that shape changes when the pane layout is dual pane. So, we need an
	// extra panel and sizer to control this shaping
	m_pLayoutPane = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	//m_pLayoutPane->SetBackgroundColour( *wxBLACK );
	m_pLayoutSizer = new wxBoxSizer(wxHORIZONTAL);

	//m_pGridSizer = new wxFlexGridSizer(2, 2, 0, 0);
	//m_pGridSizer = new wxGridSizer(2, 0, 0);
	m_pGridSizer = new wxBoxSizer(wxVERTICAL);

	m_pTopSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGridSizer->Add( m_pTopSizer, 1, wxEXPAND|wxALL, 0 );
	m_pBottomSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGridSizer->Add( m_pBottomSizer, 1, wxEXPAND|wxALL, 0 );

	for (int i=0; i<4; ++i)
	{
		//m_RenderPanes[i] = new rpnRenderPanel(this);
		m_RenderPanes[i] = new rpnRenderPanel(m_pLayoutPane);

		// this setting fills the window with the render area
		//const int c_RenderAreaSizerFlags = wxEXPAND|wxALL;
		// this setting fixes the aspect ratio and centers the window in that space
		//const int c_RenderAreaSizerFlags = wxALIGN_CENTER|wxSHAPED;
		// this setting fixes the aspect ratio and keeps the window in the upper left
		const int c_RenderAreaSizerFlags = wxSHAPED;
		if (i % 2 == 0)
			m_pTopSizer->Add( m_RenderPanes[i], 1, c_RenderAreaSizerFlags, 1 );
		else
			m_pBottomSizer->Add( m_RenderPanes[i], 1, c_RenderAreaSizerFlags, 1 );
	}
	
	// allow render pass context menu in main panel
	m_RenderPanes[0]->SetCanChangeRenderPass(true); 

	m_pLayoutPane->SetSizer( m_pGridSizer );
	m_pLayoutSizer->Add( m_pLayoutPane, 1, wxSHAPED, 0 ); 
	this->SetSizer(m_pLayoutSizer);
	//this->SetSizer(m_pGridSizer);
	this->Layout();

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rpnPanelGrid::~rpnPanelGrid()
{
	if (rpnPanelGrid::Instance == this)
		rpnPanelGrid::Instance = NULL;

}

//----------------------------------------------------------------------------
// Access to the render panels
//----------------------------------------------------------------------------
rpnRenderPanel* rpnPanelGrid::GetRenderPanel(int i_Panel)
{
	return m_RenderPanes[i_Panel];
}

//----------------------------------------------------------------------------
// Set camera and label for the render pane with the given index.
//----------------------------------------------------------------------------
void rpnPanelGrid::SetCamera(int i_Panel, gpxCamera *i_pCamera, const std::string& i_Label)
{
	m_RenderPanes[i_Panel]->SetCamera(i_pCamera, i_Label);
}

//----------------------------------------------------------------------------
// Set the organization of panes in the grid
//----------------------------------------------------------------------------
void rpnPanelGrid::SetLayoutStyle(LayoutStyle i_Layout)
{
	switch (i_Layout)
	{
	default:
	case e_SinglePane:
		m_pBottomSizer->Hide((size_t)0);
		m_RenderPanes[1]->EnableViewer(false);
		m_pTopSizer->Hide((size_t)1);
		m_RenderPanes[2]->EnableViewer(false);
		m_pBottomSizer->Hide((size_t)1);
		m_RenderPanes[3]->EnableViewer(false);
		break;
	case e_TwoStacked:
		m_pBottomSizer->Show((size_t)0);
		m_RenderPanes[1]->EnableViewer(true);
		m_pTopSizer->Hide((size_t)1);
		m_RenderPanes[2]->EnableViewer(false);
		m_pBottomSizer->Hide((size_t)1);
		m_RenderPanes[3]->EnableViewer(false);
		break;
	case e_TwoSideBySide:
		m_pBottomSizer->Hide((size_t)0);
		m_RenderPanes[1]->EnableViewer(false);
		m_pTopSizer->Show((size_t)1);
		m_RenderPanes[2]->EnableViewer(true);
		m_pBottomSizer->Hide((size_t)1);
		m_RenderPanes[3]->EnableViewer(false);
		break;
	case e_FourPanels:
		m_pBottomSizer->Show((size_t)0);
		m_RenderPanes[1]->EnableViewer(true);
		m_pTopSizer->Show((size_t)1);
		m_RenderPanes[2]->EnableViewer(true);
		m_pBottomSizer->Show((size_t)1);
		m_RenderPanes[3]->EnableViewer(true);
		break;
	}
	m_Layout = i_Layout;
	//m_pGridSizer->Layout();
	adjust_layout_sizer();
}
rpnPanelGrid::LayoutStyle rpnPanelGrid::GetLayoutStyle() const
{
	return m_Layout;
}

//----------------------------------------------------------------------------
// SetMaxRenderSize - set maximum size allowed for the render area.
//	Use -1 -1 in order to remove the constraints and allow any resizing.
//----------------------------------------------------------------------------
void rpnPanelGrid::SetMaxRenderSize(int i_Width, int i_Height)
{
	m_MaxWidth = i_Width;
	m_MaxHeight = i_Height;

	// Set aspect ratio for all render areas to keep them the
	// same proportion as the main render window no matter which layout.
	//
	for (int i=0; i<4; i++)
	{
		bool bRecursive = true;
		wxSizerItem *pItem = m_pGridSizer->GetItem(m_RenderPanes[i], bRecursive);
		pItem->SetRatio(i_Width, i_Height);
		m_RenderPanes[i]->SetMaxRenderSize(i_Width, i_Height);
	}
	//m_pGridSizer->Layout();
	adjust_layout_sizer();
}
void rpnPanelGrid::GetMaxRenderSize(int &o_Width, int &o_Height)
{
	o_Width = m_MaxWidth;
	o_Height = m_MaxHeight;
}

//------------------------------------------------------------------------
// Override the wxWidgets SetSize function in order to enforce a
//	maximum size for the render area.
//------------------------------------------------------------------------
//virtual 
void rpnPanelGrid::DoSetSize(int x, int y,
                       int width, int height,
                       int sizeFlags)
{
	//if (m_MaxWidth > 0 && width > m_MaxWidth) 
	//	width = m_MaxWidth;
	//if (m_MaxHeight > 0 && height > m_MaxHeight) 
	//	height = m_MaxHeight;

	//wxPanel::DoSetSize(x, y, width, height, sizeFlags);

	// limits are now on render panels, not on the grid
	wxPanel::DoSetSize(x, y, width, height, sizeFlags);
}


//--------------------------------------------------------------------
// paint event handler
//--------------------------------------------------------------------
//void rpnPanelGrid::OnPaint(wxPaintEvent& i_Event)
//{
//	// Draw border around render pane with the focus
//	if (m_Layout != e_SinglePane)
//	{
//		wxPaintDC pdc(this);
//
//		PrepareDC(pdc);
//
//		pdc.SetPen(*wxBLUE);
//		pdc.SetBrush(*wxTRANSPARENT_BRUSH);
//		wxRect rect = m_RenderPanes[0]->GetRect();
//		pdc.DrawRectangle(rect);
//	}
//	i_Event.Skip();
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rpnPanelGrid::adjust_layout_sizer()
{
	// If we are given a resolution to match, then set our aspect ratio
	// based on the layout style
	if (m_MaxWidth > 0 && m_MaxHeight > 0)
	{
		int grid_width = m_MaxWidth;
		int grid_height = m_MaxHeight;
		if (m_Layout == e_TwoStacked)
			grid_height *= 2;
		else if (m_Layout == e_TwoSideBySide)
			grid_width *= 2;
		m_pLayoutSizer->GetItem((size_t)0)->SetRatio(grid_width, grid_height);
	}
	m_pLayoutSizer->Layout();
	m_pGridSizer->Layout();
}

#endif // USE_WXWIDGETS
