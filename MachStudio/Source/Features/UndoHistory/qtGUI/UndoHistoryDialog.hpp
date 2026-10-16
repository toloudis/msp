/*****************************************************************************
**	UndoHistoryDialog.hpp
**
**		implementation of the UndoHistory dialog
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_UNDOHISTORYDIALOG_HPP
#error UndoHistoryDialog.hpp multiply included
#endif
#define CPTR_UNDOHISTORYDIALOG_HPP


//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <string>

class QListWidget;
class QPushButton;

#ifdef USE_QT

//============================================================================
// Class UndoHistoryDialog
//============================================================================
class UndoHistoryDialog : public QDialog
{
	public:
		static UndoHistoryDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		UndoHistoryDialog( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~UndoHistoryDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateList();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool event( QEvent* i_pEvent ) override;

	private:
		QListWidget* m_listBox_UndoList;
		QPushButton* m_button_Undo;
		QPushButton* m_button_Redo;
};

#endif
