/****************************************************************************\
**	tqcDirPicker.hpp
**
**	Text box for a directory path with a button that browses for the
**	directory. Replaces wxDirPickerCtrl.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_DIRPICKER_HPP
#error tqcDirPicker.hpp multiply included
#endif
#define TQC_DIRPICKER_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#ifdef USE_QT

//============================================================================
// Value changed callback (tqcEvent.hpp): called when a directory is chosen,
//	or the path is typed in and ENTER pressed or the control left.
//============================================================================
class tqcDirPicker : public QWidget, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcDirPicker(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Path
		//--------------------------------------------------------------------
		QString GetPath() const;
		void SetPath(const QString& i_Path);

	private:
		void OnBrowseClick();

		QLineEdit* m_pTextBox;
		QPushButton* m_pBrowseButton;
		QString m_LastPath;
};

#endif // USE_QT
