/*****************************************************************************
**	LoadPreferencesDialog.hpp
**
**	Dialog for preferences when loading a file, written in wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_PYTHONDIALOG_HPP
#error LoadPreferencesDialog.hpp multiply included
#endif
#define PYTH_PYTHONDIALOG_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <set>

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Features/LoadPrefs/wxGUI/LoadPreferencesDialogBase.h"


//----------------------------------------------------------------------------
// Class LoadPreferencesDialog
//----------------------------------------------------------------------------
class LoadPreferencesDialog : public LoadPreferencesDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static LoadPreferencesDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		LoadPreferencesDialog( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~LoadPreferencesDialog();

		//--------------------------------------------------------------------
		// Return access to tab page that will contain load prefs properties
		//--------------------------------------------------------------------
		wxPanel* GetTabPageMain();

		//--------------------------------------------------------------------
		// Fill in list box with filenames from the textures 
		// that were skipped
		//--------------------------------------------------------------------
		void SetMissingTextureList(const std::set<fsLocator>& i_Set);

	private:
};

#endif // USE_WXWIDGETS
