/****************************************************************************\
**	tqcTextBox.hpp
**
**	Text box that calls the value changed callback after ENTER is pressed
**	(single line) or the control is left with different text.
**	Multiline boxes use a plain text editor inside the same wrapper.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_TEXTBOX_HPP
#error tqcTextBox.hpp multiply included
#endif
#define TQC_TEXTBOX_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcTextBox : public QWidget, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcTextBox(QWidget* i_pParent,
				   const QString& i_Value = QString(),
				   bool i_bMultiline = false,
				   bool i_bMultilineFixed = false);

		//--------------------------------------------------------------------
		// Text of the control. SetValue also remembers it as the current
		//	value, so the callback only comes for changes by the user.
		//--------------------------------------------------------------------
		QString GetValue() const;
		virtual void SetValue(const QString& value);
		void ChangeValue(const QString& value);
		void Clear();
		void AppendText(const QString& i_Text);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetEditable(bool i_bEditable);
		bool IsMultiline() const { return m_bMultiline; }

		//--------------------------------------------------------------------
		// The value before the last change by the user
		//--------------------------------------------------------------------
		QString last_value();

		//--------------------------------------------------------------------
		// The text editing widget inside the wrapper
		//--------------------------------------------------------------------
		QLineEdit* GetLineEdit() const { return m_pLineEdit; }
		QPlainTextEdit* GetTextEdit() const { return m_pTextEdit; }

	private:
		void text_changed();

		QLineEdit* m_pLineEdit;			// single line
		QPlainTextEdit* m_pTextEdit;	// multiline
		bool m_bMultiline;
		QString m_LastValue;
		QString m_PrevValue;
};

//============================================================================
//============================================================================
class tqcMultilineTextBox : public tqcTextBox
{
	public:
		tqcMultilineTextBox(QWidget* i_pParent)
			:	tqcTextBox(i_pParent, QString(), true, false) {}
};

//============================================================================
//============================================================================
class tqcMultilineFixedTextBox : public tqcTextBox
{
	public:
		tqcMultilineFixedTextBox(QWidget* i_pParent)
			:	tqcTextBox(i_pParent, QString(), true, true) {}
};

#endif // USE_QT
