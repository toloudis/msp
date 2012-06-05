/*****************************************************************************
**  tqtDialogTabbed.hpp
**
**      Interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_DIALOGTABBED_HPP
#error tqtDialogTabbed.hpp multiply included
#endif
#define TQT_DIALOGTABBED_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

#include <QtGui/QWidget>

#ifdef QT_FINISH_PORT
#include <wx/gdicmn.h>
#include <wx/notebook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/aui/auibook.h>
#endif


//============================================================================
// Class DialogTabbed
//============================================================================
class tqtDialogTabbed  : public QWidget
{
	public:
#ifdef QT_FINISH_PORT
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqtDialogTabbed( QWidget* parent, 
						 QWidgetID id = wxID_ANY, 
						 const wxString& name = wxEmptyString, 
						 const wxString& title = wxEmptyString, 
						 const wxPoint& pos = wxDefaultPosition, 
						 const wxSize& size = wxSize(450, 300), 
						 long style = 0); // = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU  );
#endif

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tqtDialogTabbed();

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

#ifdef QT_FINISH_PORT
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		wxPanel* GetTabPage(const std::string& i_Title);

	private:
		wxAuiNotebook* m_Notebook;
#endif
};

#endif // USE_QT