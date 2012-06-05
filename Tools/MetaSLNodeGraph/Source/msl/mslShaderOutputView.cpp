/*****************************************************************************
**  mslShaderOutputView.hpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslShaderOutputView.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(mslShaderOutputView, wxPanel)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
mslShaderOutputView::mslShaderOutputView(wxWindow* parent)
: wxPanel(parent, wxID_ANY)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_richText_Console = new wxRichTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	bSizer1->Add( m_richText_Console, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(L"ShaderOutput").Caption(L"Shader Output").Layer(1).Bottom().BestSize(600,400).Hide());
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
mslShaderOutputView::~mslShaderOutputView()
{
}

//--------------------------------------------------------------------
// Clear the text box 
//--------------------------------------------------------------------
void mslShaderOutputView::ClearShaderString()
{
	m_richText_Console->Clear();
}

//--------------------------------------------------------------------
// Set the text box to contain the given shader string
//--------------------------------------------------------------------
void mslShaderOutputView::UpdateShaderString( const std::string& i_String )
{
	m_richText_Console->SetValue( wxString(i_String.c_str(), wxConvUTF8)  );
}

//--------------------------------------------------------------------
// Move cursor to the given line number
//--------------------------------------------------------------------
void mslShaderOutputView::MoveToLine(int i_LineNum)
{
	m_richText_Console->MoveHome();
	m_richText_Console->MoveDown(i_LineNum);
	m_richText_Console->ShowPosition( m_richText_Console->GetCaretPosition() );
}

#endif // USE_WXWIDGETS
