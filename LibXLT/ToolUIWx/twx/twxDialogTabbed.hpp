/*****************************************************************************
**  twxDialogTabbed.hpp
**
**      Interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_DIALOGTABBED_HPP
#error twxDialogTabbed.hpp multiply included
#endif
#define TWX_DIALOGTABBED_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

#include <wx/gdicmn.h>
#include <wx/notebook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/aui/auibook.h>


//============================================================================
// Class DialogTabbed
//============================================================================
class twxDialogTabbed : public wxPanel //wxDialog 
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twxDialogTabbed( wxWindow* parent, 
						 wxWindowID id = wxID_ANY, 
						 const wxString& name = wxEmptyString, 
						 const wxString& title = wxEmptyString, 
						 const wxPoint& pos = wxDefaultPosition, 
						 const wxSize& size = wxSize(450, 300), 
						 long style = 0); // = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU  );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~twxDialogTabbed();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddTabPage(const std::string& i_Title);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void RemoveTabPage(const std::string& i_Title);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DeleteTabPage(const std::string& i_Title);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void RemoveTabPages();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		wxPanel* GetTabPage(const std::string& i_Title);

	private:
		wxAuiNotebook* m_Notebook;
	
};

#endif // USE_WXWIDGETS