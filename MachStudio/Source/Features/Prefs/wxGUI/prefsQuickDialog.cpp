///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "prefsQuickDialog.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgMsg.hpp"


#ifdef USE_WXWIDGETS

///////////////////////////////////////////////////////////////////////////
// Virtual event handlers, overide them in your derived class
//virtual 
void prefsQuickDialog::prefsQuickDialog_OnMiddleDown( wxMouseEvent& event )
{
	Show(false);
}

//virtual 
void prefsQuickDialog::anyButton_OnLeftUp( wxMouseEvent& event )
{ 
	this->Show(false);

	//	execute
	std::string tag;
	//WXMOUSE_BTN_NONE
	int btn = event.GetButton();
	if (btn != wxMOUSE_BTN_NONE)
	{
		wxButton* pBtn = dynamic_cast<wxButton*>(event.GetEventObject());
		if (pBtn != NULL)
		{
			std::string tag;
			wxString label = pBtn->GetLabelText();
			tag = label.utf8_str();
			cmaCommandMgr::ExecuteCommand(tag, -1);
		}
	}

	//	close this form
	this->Close();
}

prefsQuickDialog::prefsQuickDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) 
: wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_Sizer = new wxGridSizer( 4, 4, 0, 0 );
	this->SetSizer( m_Sizer );

	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( prefsQuickDialog::prefsQuickDialog_OnMiddleDown ) );
}

prefsQuickDialog::~prefsQuickDialog()
{
	// Disconnect Events
	this->Disconnect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( prefsQuickDialog::prefsQuickDialog_OnMiddleDown ) );

	//	need to disconnect each button?
//	m_button1->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( prefsQuickDialog::anyButton_OnLeftUp ), NULL, this );
}


void prefsQuickDialog::BuildButtons(const prefsQuickData& i_Data, const int i_ScreenX, const int i_ScreenY)
{
	//DBG_LOG("-----Creating Quick Commands-----");
	//DBG_LOG3("user pressed @ (%d,%d) commands=%d", i_ScreenX, i_ScreenY, i_Data.m_Commands.GetNumberOfItems());

	const int lc_BUTTON_WIDTH = 90;
	const int lc_BUTTON_HEIGHT = 38; //23;
	const int lc_BUTTON_WIDTH_GAP = 8;
	const int lc_BUTTON_HEIGHT_GAP = 12;
	const int lc_BUTTON_WIDTH_INC = lc_BUTTON_WIDTH + lc_BUTTON_WIDTH_GAP;
	const int lc_BUTTON_HEIGHT_INC = lc_BUTTON_HEIGHT + lc_BUTTON_HEIGHT_GAP;

	this->Freeze();

	//	build the buttons
	//
	int btncnt = 1;
	for (int j = 0; j < i_Data.m_Commands.GetNumberOfItems(); ++j)
	{
		if (i_Data.m_Commands.GetValueFlag(j) == true)
		{
			wxButton* pButton = new wxButton( this, wxID_ANY, wxT("button"), wxDefaultPosition, FromDIP(wxSize( lc_BUTTON_WIDTH, lc_BUTTON_HEIGHT )), 0 );

			pButton->SetName( wxString(i_Data.m_Commands.GetValueText(j).c_str(), wxConvUTF8) );
			pButton->SetLabel( wxString(i_Data.m_Commands.GetValueText(j).c_str(), wxConvUTF8) );
			pButton->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( prefsQuickDialog::anyButton_OnLeftUp ), NULL, this );

			m_Sizer->Add( pButton, 0, wxALL, 5 );

			++btncnt;
		}
	}

	if (btncnt > 0)
	{
		// big hack formula!
		int formsizex = (((btncnt/4) > 0) ? 4 : btncnt) * lc_BUTTON_WIDTH_INC;
		int formsizey = ((btncnt/5)+1) * lc_BUTTON_HEIGHT_INC;
		//int formlocx = i_ScreenX - (formsizex/2);
		//int formlocy = i_ScreenY - (formsizey/2);
		//DBG_LOG5("client size(%d,%d) loc(%d,%d) buttons=%d", formsizex, formsizey, formlocx, formlocy, btncnt);

		SetPosition( FromDIP(wxPoint(i_ScreenX - (formsizex/2),i_ScreenY - (formsizey/2))) );
		SetSize( FromDIP(wxSize(formsizex, formsizey)) );
	}

	this->Thaw();
}

#endif