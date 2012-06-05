/*****************************************************************************
**  mslGraphView.hpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslGraphView.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(mslGraphView, wxPanel)
    EVT_SIZE(mslGraphView::OnSize)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
mslGraphView::mslGraphView(wxWindow* parent)
: wxPanel(parent, wxID_ANY),
  m_graph_view(NULL)
{
	//wxBoxSizer* bSizer1;
	//bSizer1 = new wxBoxSizer( wxVERTICAL );


	//this->SetSizer( bSizer1 );
	this->Layout();

	//twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(L"GraphView").Caption(L"Node Graph").Layer(2).Top().BestSize(400,600));
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(L"GraphView").Caption(L"Node Graph").CenterPane());
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
mslGraphView::~mslGraphView()
{
}

//--------------------------------------------------------------------
// Set the graph view in the window
//--------------------------------------------------------------------
void mslGraphView::SetView(IGraph_view *graph_view)
{
    m_graph_view = graph_view;
}

//--------------------------------------------------------------------
// Get the graph view associated with the window
//--------------------------------------------------------------------
IGraph_view *mslGraphView::GetView()
{
    return m_graph_view;
}

//--------------------------------------------------------------------
// Resize the graph window
//--------------------------------------------------------------------
void mslGraphView::Resize(const wxSize &size)
{
    SetSize(size);
    if(m_graph_view) {
        wxSize size = GetClientSize();
        m_graph_view->set_size(0,0,size.GetWidth(),size.GetHeight());
    }
    Update();
}

// Called on resize
void mslGraphView::OnSize(wxSizeEvent &event)
{
    Resize(event.GetSize());
}

#endif // USE_WXWIDGETS