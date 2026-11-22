/****************************************************************************\
**	tqcTextBox.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcTextBox.hpp"

#ifdef USE_QT

namespace
{
	//------------------------------------------------------------------------
	// Plain text editor that reports when it loses the focus
	//------------------------------------------------------------------------
	class FocusOutTextEdit : public QPlainTextEdit
	{
	public:
		FocusOutTextEdit(QWidget* i_pParent, std::function<void()> i_FocusOut)
			: QPlainTextEdit(i_pParent), m_FocusOut(i_FocusOut) {}

	protected:
		virtual void focusOutEvent(QFocusEvent* i_pEvent)
		{
			QPlainTextEdit::focusOutEvent(i_pEvent);
			if (m_FocusOut)
				m_FocusOut();
		}

	private:
		std::function<void()> m_FocusOut;
	};

	// Size of the fixed size multiline text box
	const int c_FixedMultilineSize = 123;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcTextBox::tqcTextBox(QWidget* i_pParent,
					   const QString& i_Value,
					   bool i_bMultiline,
					   bool i_bMultilineFixed)
:	QWidget(i_pParent),
	m_pLineEdit(NULL),
	m_pTextEdit(NULL),
	m_LastValue(i_Value),
	m_bMultiline(i_bMultiline)
{
	QHBoxLayout* pLayout = new QHBoxLayout(this);
	pLayout->setContentsMargins(0, 0, 0, 0);

	if (m_bMultiline)
	{
		m_pTextEdit = new FocusOutTextEdit(this, [this]() { text_changed(); });
		m_pTextEdit->setPlainText(i_Value);
		if (i_bMultilineFixed)
			m_pTextEdit->setFixedSize(c_FixedMultilineSize, c_FixedMultilineSize);
		pLayout->addWidget(m_pTextEdit);
		this->setFocusProxy(m_pTextEdit);
	}
	else
	{
		m_pLineEdit = new QLineEdit(i_Value, this);
		pLayout->addWidget(m_pLineEdit);
		this->setFocusProxy(m_pLineEdit);

		// editingFinished comes on ENTER and when the control loses focus
		QObject::connect(m_pLineEdit, &QLineEdit::editingFinished, [this]() { text_changed(); });

		// After Enter is pressed, select the full text
		// in order to make it easier to try out new values
		QObject::connect(m_pLineEdit, &QLineEdit::returnPressed, [this]() { m_pLineEdit->selectAll(); });
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QString tqcTextBox::GetValue() const
{
	return m_pLineEdit ? m_pLineEdit->text() : m_pTextEdit->toPlainText();
}

//----------------------------------------------------------------------------
// Override SetValue so that we can track the current value
//	in order to know when it changes.
//----------------------------------------------------------------------------
//virtual
void tqcTextBox::SetValue(const QString& i_Value)
{
	m_LastValue = i_Value;
	ChangeValue(i_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::ChangeValue(const QString& i_Value)
{
	if (GetValue() == i_Value)
		return;
	if (m_pLineEdit)
	{
		m_pLineEdit->setText(i_Value);
		m_pLineEdit->setCursorPosition(0);
	}
	else
		m_pTextEdit->setPlainText(i_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::Clear()
{
	SetValue(QString());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::AppendText(const QString& i_Text)
{
	if (m_pLineEdit)
		m_pLineEdit->setText(m_pLineEdit->text() + i_Text);
	else
	{
		m_pTextEdit->moveCursor(QTextCursor::End);
		m_pTextEdit->insertPlainText(i_Text);
		m_pTextEdit->moveCursor(QTextCursor::End);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::SetEditable(bool i_bEditable)
{
	if (m_pLineEdit)
		m_pLineEdit->setReadOnly(!i_bEditable);
	else
		m_pTextEdit->setReadOnly(!i_bEditable);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::text_changed()
{
	QString text = this->GetValue();

	// We need to store the last value to restore when used in the filePicker and is partially deleted.
	m_PrevValue = m_LastValue;

	if (text != m_LastValue)
	{
		m_LastValue = text;

		// trigger the callback
		NotifyValueChanged();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QString tqcTextBox::last_value()
{
	return m_PrevValue;
}

#endif // USE_QT
