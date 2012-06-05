/*****************************************************************************
**  mslToolView.hpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslToolView.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(mslToolView, wxPanel)
    EVT_SIZE(mslToolView::OnSize)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
mslToolView::mslToolView(wxWindow* parent)
: wxPanel(parent, wxID_ANY),
  m_tool_view(NULL)
{
	//wxBoxSizer* bSizer1;
	//bSizer1 = new wxBoxSizer( wxVERTICAL );


	//this->SetSizer( bSizer1 );
	this->Layout();

	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(L"ToolView").Caption(L"Toolbox").Layer(3).Left().BestSize(200,400));
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
mslToolView::~mslToolView()
{
}

//--------------------------------------------------------------------
// Set the tool view in the window
//--------------------------------------------------------------------
void mslToolView::SetView(ITool_view *tool_view)
{
    m_tool_view = tool_view;
}

//--------------------------------------------------------------------
// Get the tool view associated with the window
//--------------------------------------------------------------------
ITool_view *mslToolView::GetView()
{
    return m_tool_view;
}

//--------------------------------------------------------------------
// Resize the graph window
//--------------------------------------------------------------------
void mslToolView::Resize(const wxSize &size)
{
    SetSize(size);
    if(m_tool_view) {
        wxSize size = GetClientSize();
        m_tool_view->set_size(0,0,size.GetWidth(),size.GetHeight());
    }
    Update();
}

// Called on resize
void mslToolView::OnSize(wxSizeEvent &event)
{
    Resize(event.GetSize());
}

#endif // USE_WXWIDGETS