/*****************************************************************************
**	mbrwMaterialSwatchDialog.hpp
**
**	Material library browser pane, written in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MBRW_MATERIALSWATCHDIALOG_HPP
#error mbrwMaterialSwatchDialog.hpp multiply included
#endif
#define MBRW_MATERIALSWATCHDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <deque>

#ifdef USE_QT

class QComboBox;
class QListWidget;
class QListWidgetItem;
class QToolButton;

//----------------------------------------------------------------------------
// Class mbrwMaterialSwatchDialog
//----------------------------------------------------------------------------
class mbrwMaterialSwatchDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static mbrwMaterialSwatchDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mbrwMaterialSwatchDialog( QWidget* parent,
						  const char* i_Title = "Material Browser" );

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
		void buttonHome_Click();
		void buttonRefresh_Click();
		void buttonBack_Click();
		void buttonForward_Click();
		void buttonUp_Click();
		void buttonPaint_Click();
		void buttonNewFolder_Click();
		void buttonBrowse_Click();
		void listCtrl_beginDrag();
		void listCtrl_doubleClick( QListWidgetItem* i_pItem );
		void OnChoice();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void browse_directory(const fsLocator &i_Directory,
							  bool i_bAddToHistory = true);
		void enable_buttons();
		void regenerate_icons();

		QToolButton* m_button_Back;
		QToolButton* m_button_Forward;
		QToolButton* m_button_Paint;
		QComboBox* m_choice_view;
		QListWidget* m_listCtrl_Icons;

		fsLocator m_CurrentMaterialDir;
		std::deque<fsLocator> m_PreviousDirectories;
		std::deque<fsLocator> m_NextDirectories;
};

#endif // USE_QT
