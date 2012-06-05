/*****************************************************************************
**  mslParameterView.hpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslParameterView.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(mslParameterView, wxPanel)
    EVT_SIZE(mslParameterView::OnSize)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
mslParameterView::mslParameterView(wxWindow* parent)
: wxPanel(parent, wxID_ANY),
  m_Parameter_view(NULL)
{
	//wxBoxSizer* bSizer1;
	//bSizer1 = new wxBoxSizer( wxVERTICAL );


	//this->SetSizer( bSizer1 );
	this->Layout();

	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(L"ParameterView").Caption(L"Parameters").Layer(3).Right().Position(2).BestSize(200,400));
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
mslParameterView::~mslParameterView()
{
}

//--------------------------------------------------------------------
// Set the Parameter view in the window
//--------------------------------------------------------------------
void mslParameterView::SetView(IParameter_view *Parameter_view)
{
    m_Parameter_view = Parameter_view;
    if(m_Parameter_view) {
        wxSize size = GetClientSize();
		if ((size.GetWidth()>0) && (size.GetHeight()>0))
			m_Parameter_view->set_size(0,0,size.GetWidth(),size.GetHeight());
    }
}

//--------------------------------------------------------------------
// Get the Parameter view associated with the window
//--------------------------------------------------------------------
IParameter_view *mslParameterView::GetView()
{
    return m_Parameter_view;
}

//--------------------------------------------------------------------
// Resize the graph window
//--------------------------------------------------------------------
void mslParameterView::Resize(const wxSize &size)
{
    SetSize(size);
    if(m_Parameter_view) {
        wxSize size = GetClientSize();
        m_Parameter_view->set_size(0,0,size.GetWidth(),size.GetHeight());
    }
    Update();
}

// Called on resize
void mslParameterView::OnSize(wxSizeEvent &event)
{
    Resize(event.GetSize());
}

#endif // USE_WXWIDGETS