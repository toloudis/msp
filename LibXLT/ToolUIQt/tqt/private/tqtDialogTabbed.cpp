/*****************************************************************************
**  tqtDialogTabbed.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtDialogTabbed.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtDialogTabbedMgr.hpp"

#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"


#ifdef USE_QT

#ifdef QT_FINISH_PORT
//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtDialogTabbed::tqtDialogTabbed( QWidget* parent, 
								  QWidgetID id, 
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
	
	m_Notebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxSize(450,300), 0 );
	
	bSizer1->Add( m_Notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	// Add this panel to the AUI manager
	tqtPaneMgr::AddPane(this, wxAuiPaneInfo().Name(name).Caption(title).Hide().Layer(1).Float());
}
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtDialogTabbed::~tqtDialogTabbed()
{
	tqtDialogTabbedMgr::NotifyDialogDeleted(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::AddTabPage(const std::string& i_Title)
{
#ifdef QT_FINISH_PORT
	wxScrolledWindow *pTabPage = new wxScrolledWindow( m_Notebook, 
				wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	pTabPage->SetScrollRate( 0, 5 );
	pTabPage->SetMinSize(wxSize(100,200));
	m_Notebook->AddPage( pTabPage, wxString(i_Title.c_str(), wxConvUTF8), false );
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::RemoveTabPage(const std::string& i_Title)
{
#ifdef QT_FINISH_PORT
	const int num_pages = m_Notebook->GetPageCount();
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
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::DeleteTabPage(const std::string& i_Title)
{
#ifdef QT_FINISH_PORT
	const int num_pages = m_Notebook->GetPageCount();
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
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqtDialogTabbed::RemoveTabPages()
{
#ifdef QT_FINISH_PORT
	// Remove all property controls from the tab pages before deleting
	const int num_pages = m_Notebook->GetPageCount();
	for (int i=0; i<num_pages; ++i)
	{
		wxPanel *pPanel = dynamic_cast<wxPanel*>(m_Notebook->GetPage(i));
		if (pPanel)
			pqtFormControlBuilder::ClearForm(pPanel);
		m_Notebook->DeletePage(i);
	}

	//TODO: Remove or Delete here?
	//m_Notebook->DeleteAllPages();
#endif
}

#ifdef QT_FINISH_PORT
//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxPanel* tqtDialogTabbed::GetTabPage(const std::string& i_Title)
{
	const int num_pages = m_Notebook->GetPageCount();
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
#endif

#endif // USE_QT
