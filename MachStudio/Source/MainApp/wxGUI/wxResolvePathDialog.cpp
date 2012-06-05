/*****************************************************************************
**	wxResolvePathDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/wxGUI/wxResolvePathDialog.hpp"

#include "Core/Fs/fsFileUtil.hpp"


#ifdef USE_WXWIDGETS

namespace
{
}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxResolvePathDialog::wxResolvePathDialog( wxWindow* parent, 
										  const fsLocator& i_Filename,
										  const std::string &i_Category )
: wxResolvePathDialogBase( parent ),
  m_ResolveChoice(e_Skip) // this sets default value for when user closes dialog with "X" in corner
{
	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_Filename,filename);
	m_staticText_Filename->SetLabel(filename.GetString());

	std::string skipAllString("Skip All");
	if (!i_Category.empty())
	{
		skipAllString += " ";
		skipAllString += i_Category;
	}
	m_button_SkipAll->SetLabel(wxString(skipAllString.c_str(), wxConvUTF8));

	this->Layout();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxResolvePathDialog::~wxResolvePathDialog()
{
}

//--------------------------------------------------------------------
// Event handlers
//--------------------------------------------------------------------
void wxResolvePathDialog::buttonLocateClick( wxCommandEvent& i_Event)
{
	m_ResolveChoice = e_Locate;
	this->EndModal(wxID_OK);
}

void wxResolvePathDialog::buttonRetryClick( wxCommandEvent& i_Event)
{
	m_ResolveChoice = e_Retry;
	this->EndModal(wxID_OK);
}

void wxResolvePathDialog::buttonSkipClick( wxCommandEvent& i_Event)
{
	m_ResolveChoice = e_Skip;
	this->EndModal(wxID_OK);
}

void wxResolvePathDialog::buttonSkipAllClick( wxCommandEvent& i_Event)
{
	m_ResolveChoice = e_SkipAll;
	this->EndModal(wxID_OK);
}

void wxResolvePathDialog::buttonAbortClick( wxCommandEvent& i_Event)
{
	m_ResolveChoice = e_Abort;
	this->EndModal(wxID_OK);
}


#endif // USE_WXWIDGETS
