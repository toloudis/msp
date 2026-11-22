/*****************************************************************************
**	qtResolvePathDialog.hpp
**
**	Dialog for asking user how to handle a missing file.
**	User can supply a new filename or ask to skip the file 
**	or abort the file load.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef QT_RESOLVEPATHDIALOG_HPP
#error qtResolvePathDialog.hpp multiply included
#endif
#define QT_RESOLVEPATHDIALOG_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT
//----------------------------------------------------------------------------
// Base class with the dialog layout
//----------------------------------------------------------------------------
#include "MainApp/qtGUI/qtResolvePathDialogBase.h"


//----------------------------------------------------------------------------
// Class qtResolvePathDialog
//----------------------------------------------------------------------------
class qtResolvePathDialog : public qtResolvePathDialogBase
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
		qtResolvePathDialog( QWidget* parent, 
							 const fsLocator& i_Filename,
							 const std::string &i_Category );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~qtResolvePathDialog();

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
		virtual void buttonLocateClick();
		virtual void buttonRetryClick();
		virtual void buttonSkipClick();
		virtual void buttonSkipAllClick();
		virtual void buttonAbortClick();

		ResolveChoice m_ResolveChoice;
};

#endif // USE_QT
