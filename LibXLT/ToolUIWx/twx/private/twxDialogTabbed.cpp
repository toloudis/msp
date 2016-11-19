/*****************************************************************************
**  twxDialogTabbed.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxDialogTabbed.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxDialogTabbedMgr.hpp"

#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"

#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twxDialogTabbed::twxDialogTabbed( wxWindow* parent, 
								  wxWindowID id, 
								  const wxString& name, 
								  const wxString& title, 
								  const wxPoint& pos, 
								  const wxSize& size, 
								  long style ) 
//: wxDialog( parent, id, title, pos, size, style )
: wxPanel( parent, id, pos, size, style )
{
	this->SetSizeHints( 450, 300 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(450,300)), 0 );
	
	bSizer1->Add( m_Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(name).Caption(title).Hide().Layer(1).Float());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twxDialogTabbed::~twxDialogTabbed()
{
	twxDialogTabbedMgr::NotifyDialogDeleted(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twxDialogTabbed::AddTabPage(const std::string& i_Title)
{	
	wxScrolledWindow *pTabPage = new wxScrolledWindow( m_Notebook, 
				wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	pTabPage->SetScrollRate( 0, 5 );
	pTabPage->SetMinSize(FromDIP(wxSize(100,200)));
	m_Notebook->AddPage( pTabPage, wxString(i_Title.c_str(), wxConvUTF8), false );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twxDialogTabbed::RemoveTabPage(const std::string& i_Title)
{
	const size_t num_pages = m_Notebook->GetPageCount();
	wxString title(i_Title.c_str(),  wxConvUTF8);
	for (int i=0; i<num_pages; ++i)
	{
		if (title == m_Notebook->GetPageText(i))
		{
			//TODO: RemovePage or DeletePage?
			m_Notebook->RemovePage(i);
			break;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twxDialogTabbed::DeleteTabPage(const std::string& i_Title)
{
	const size_t num_pages = m_Notebook->GetPageCount();
	wxString title(i_Title.c_str(),  wxConvUTF8);
	for (int i=0; i<num_pages; ++i)
	{
		if (title == m_Notebook->GetPageText(i))
		{
			//TODO: RemovePage or DeletePage?
			m_Notebook->DeletePage(i);
			break;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twxDialogTabbed::RemoveTabPages()
{
	// Remove all property controls from the tab pages before deleting
	const size_t num_pages = m_Notebook->GetPageCount();
	for (int i=0; i<num_pages; ++i)
	{
		wxPanel *pPanel = dynamic_cast<wxPanel*>(m_Notebook->GetPage(i));
		if (pPanel)
			pwxFormControlBuilder::ClearForm(pPanel);
		m_Notebook->DeletePage(i);
	}

	//TODO: Remove or Delete here?
	//m_Notebook->DeleteAllPages();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxPanel* twxDialogTabbed::GetTabPage(const std::string& i_Title)
{
	const size_t num_pages = m_Notebook->GetPageCount();
	wxString title(i_Title.c_str(),  wxConvUTF8);
	for (int i=0; i<num_pages; ++i)
	{
		if (title == m_Notebook->GetPageText(i))
		{
			return dynamic_cast<wxPanel*>(m_Notebook->GetPage(i));
		}
	}
	return NULL;
}

#endif // USE_WXWIDGETS
