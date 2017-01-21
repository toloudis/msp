/****************************************************************************\
**	cptrDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrDialogUtil.hpp"

#include "Support/capt/captRenderOutputTagsUtil.hpp"

#include "Core/it/itStringUtil.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#ifdef USE_WXWIDGETS
#include <wx/richtext/richtextctrl.h>
#endif
#include <list>
#include <sstream>
#include <iomanip>


//--------------------------------------------------------------------
// Show dialog with the valid render directory/file tags
//--------------------------------------------------------------------
void cptrDialogUtil::ShowTagsInfo()
{
	// Set the default text of the control.
	std::ostringstream taginfo;
	taginfo <<  "Render File and Directory Tags" << std::endl << std::endl;

	//	loop through the tags
	for (int i = 0; i < captRenderOutputTagsUtil::GetNumTags(); ++i)
	{
		itString tag = captRenderOutputTagsUtil::GetTag(i);
		taginfo << "<" << itStringUtil::GetStdString(tag).c_str() << ">" << std::endl;
	}

	taginfo << std::endl;
	taginfo << "Note: For counter tags (0 and #): the number of those characters within the tag indicates number of places.  <0000> - means 4 digits long, empty space filled with zeros.";
	taginfo << std::endl;

#ifdef USE_WXWIDGETS
	wxDialog renderTagInfoDialog(twxSystem::g_pMainForm, wxID_ANY, L"Render Tag Info", 
		wxDefaultPosition, twxSystem::g_pMainForm->FromDIP(wxSize(300,400)));

	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxString help_str(taginfo.str().c_str(), wxConvUTF8);
	wxRichTextCtrl *richText1 = new wxRichTextCtrl( &renderTagInfoDialog, wxID_ANY, 
		help_str, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	richText1->SetEditable(false);
	//richText1->SetFont(*wxSWISS_FONT);
	bSizer1->Add( richText1, 1, wxEXPAND | wxALL, 5 );
	
	renderTagInfoDialog.SetSizer( bSizer1 );
	renderTagInfoDialog.Layout();

	renderTagInfoDialog.ShowModal();
#endif
}
