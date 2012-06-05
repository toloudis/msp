/*****************************************************************************
**	wxResolvePathDialog.hpp
**
**	Dialog for asking user how to handle a missing file.
**	User can supply a new filename or ask to skip the file 
**	or abort the file load.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef WX_RESOLVEPATHDIALOG_HPP
#error wxResolvePathDialog.hpp multiply included
#endif
#define WX_RESOLVEPATHDIALOG_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "MainApp/wxGUI/wxResolvePathDialogBase.h"


//----------------------------------------------------------------------------
// Class wxResolvePathDialog
//----------------------------------------------------------------------------
class wxResolvePathDialog : public wxResolvePathDialogBase
{
	public:
		enum ResolveChoice
		{
			e_Locate,
			e_Retry,
			e_Skip,
			e_SkipAll,
			e_Abort
		};

		//--------------------------------------------------------------------
		// Pass in name of dialog that cannot be found.
		//--------------------------------------------------------------------
		wxResolvePathDialog( wxWindow* parent, 
							 const fsLocator& i_Filename,
							 const std::string &i_Category );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~wxResolvePathDialog();

		//--------------------------------------------------------------------
		// Get choice user made in dialog.
		//--------------------------------------------------------------------
		ResolveChoice GetResolveChoice() const
		{
			return m_ResolveChoice;
		}

	private:
		//--------------------------------------------------------------------
		// Virtual event handlers
		//--------------------------------------------------------------------
		virtual void buttonLocateClick( wxCommandEvent& i_Event);
		virtual void buttonRetryClick( wxCommandEvent& i_Event);
		virtual void buttonSkipClick( wxCommandEvent& i_Event);
		virtual void buttonSkipAllClick( wxCommandEvent& i_Event);
		virtual void buttonAbortClick( wxCommandEvent& i_Event);

		ResolveChoice m_ResolveChoice;
};

#endif // USE_WXWIDGETS
