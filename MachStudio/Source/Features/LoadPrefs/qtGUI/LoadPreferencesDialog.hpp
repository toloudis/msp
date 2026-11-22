/*****************************************************************************
**	LoadPreferencesDialog.hpp
**
**	Dialog for preferences when loading a file, written in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_PYTHONDIALOG_HPP
#error LoadPreferencesDialog.hpp multiply included
#endif
#define PYTH_PYTHONDIALOG_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <set>

class QListWidget;

#ifdef USE_QT

//----------------------------------------------------------------------------
// Class LoadPreferencesDialog
//----------------------------------------------------------------------------
class LoadPreferencesDialog : public QDialog
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static LoadPreferencesDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		LoadPreferencesDialog( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~LoadPreferencesDialog();

		//--------------------------------------------------------------------
		// Return access to tab page that will contain load prefs properties
		//--------------------------------------------------------------------
		QWidget* GetTabPageMain();

		//--------------------------------------------------------------------
		// Fill in list box with filenames from the textures
		// that were skipped
		//--------------------------------------------------------------------
		void SetMissingTextureList(const std::set<fsLocator>& i_Set);

	private:
		QWidget* m_panel_LoadPrefs;
		QListWidget* m_listBox1;
};

#endif // USE_QT
