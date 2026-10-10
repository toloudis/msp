/****************************************************************************\
**	tqcFloatEdit.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"

#include <sstream>

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcFloatEdit::tqcFloatEdit(QWidget* i_pParent)
:	QLineEdit("0", i_pParent),
	m_FloatValue(0),
	m_DecimalPlaces(-1)
{
	this->setMinimumWidth(40);

	// editingFinished comes on ENTER and when the control loses focus
	QObject::connect(this, &QLineEdit::editingFinished, [this]() { text_changed(); });

	// After Enter is pressed, select the full text
	// in order to make it easier to try out new values
	QObject::connect(this, &QLineEdit::returnPressed, [this]() { this->selectAll(); });
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const float tqcFloatEdit::GetFloatValue() const
{
	return m_FloatValue;
}
void tqcFloatEdit::SetFloatValue(const float i_Value)
{
	m_FloatValue = i_Value;
	std::ostringstream str;
	// Default (-1) uses 6 digits of precision in natural stream method.
	// If m_DecimalPlaces has been set, then switch to fixed format and
	// limit the number of decimal places.
	if (m_DecimalPlaces >= 0)
	{
		str.setf(std::ios_base::fixed, std::ios_base::floatfield);
		str.precision(m_DecimalPlaces);
	}
	str << m_FloatValue;
	this->ChangeValue(tqtQString(str.str()));
}

//--------------------------------------------------------------------
//	DecimalPlaces
//--------------------------------------------------------------------
const short tqcFloatEdit::GetDecimalPlaces() const
{
	return m_DecimalPlaces;
}
void tqcFloatEdit::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_DecimalPlaces = i_DecimalPlaces;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcFloatEdit::ChangeValue(const QString& i_Text)
{
	if (this->text() != i_Text)
	{
		this->setText(i_Text);
		this->setCursorPosition(0);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFloatEdit::text_changed()
{
	float old_value = m_FloatValue;
	std::istringstream str(tqtStdString(this->text()));
	str >> m_FloatValue;
	if (m_FloatValue != old_value)
	{
		// trigger the callback
		NotifyValueChanged();
	}
}

#endif // USE_QT
