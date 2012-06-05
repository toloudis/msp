/*****************************************************************************
**	mbrwMaterialSwatchDialog.hpp
**
**	Dialog for displaying material thumbnails, drag and drop onto
**	surfaces to apply materials. Written in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MBRW_MATERIALSWATCHDIALOG_HPP
#error mbrwMaterialSwatchDialog.hpp multiply included
#endif
#define MBRW_MATERIALSWATCHDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

#include <deque>

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Features/MaterialBrowse/wxGUI/mbrwMaterialSwatchDialogBase.h"


//----------------------------------------------------------------------------
// Class mbrwMaterialSwatchDialog
//----------------------------------------------------------------------------
class mbrwMaterialSwatchDialog : public mbrwMaterialSwatchDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static mbrwMaterialSwatchDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mbrwMaterialSwatchDialog( wxWindow* parent, 
						  const wxString& i_Title = L"Material Browser" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~mbrwMaterialSwatchDialog();

		//--------------------------------------------------------------------
		// Set home directory for material browsing
		//--------------------------------------------------------------------
		void SetInitialDirectory(const fsLocator& i_InitDir);

		//--------------------------------------------------------------------
		// Refresh Icons from the current directory, called when
		// new materials are exported.
		//--------------------------------------------------------------------
		void RefreshIcons();

	private:
		//--------------------------------------------------------------------
		// event handlers
		//--------------------------------------------------------------------
		virtual void buttonHome_Click( wxCommandEvent& i_Event );
		virtual void buttonRefresh_Click( wxCommandEvent& i_Event );
		virtual void buttonBack_Click( wxCommandEvent& i_Event );
		virtual void buttonForward_Click( wxCommandEvent& i_Event );
		virtual void buttonUp_Click( wxCommandEvent& i_Event );
		virtual void buttonPaint_Click( wxCommandEvent& i_Event );
		virtual void buttonNewFolder_Click( wxCommandEvent& i_Event );
		virtual void buttonBrowse_Click( wxCommandEvent& i_Event );
		//virtual void choiceSubDirs_Change( wxCommandEvent& i_Event );
		virtual void listCtrl_beginDrag( wxListEvent& i_Event );
		virtual void listCtrl_doubleClick( wxListEvent& i_Event );
		virtual void listCtrl_selectionChange( wxListEvent& i_Event );
		virtual void OnChoice( wxCommandEvent& i_Event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void browse_directory(const fsLocator &i_Directory,
							  bool i_bAddToHistory = true);
		void enable_buttons();
		void regenerate_icons();

		fsLocator m_CurrentMaterialDir;
		std::deque<fsLocator> m_PreviousDirectories;
		std::deque<fsLocator> m_NextDirectories;

};

#endif // USE_WXWIDGETS
