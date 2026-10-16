/*****************************************************************************
**	tqcColorDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/private/tqcColorDialog.hpp"

#include "Core/dbg/dbgMsg.hpp"

#ifdef USE_QT

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
tqcColorDialog* tqcColorDialog::Instance = NULL;

namespace
{
	// Reopen the dialog where the user last left it
	QPoint l_lastPosition;
	bool l_bHaveLastPosition = false;

	int to_byte(float i_Value)
	{
		int value = (int)(255.0f * i_Value + 0.5f);
		return (value < 0) ? 0 : ((value > 255) ? 255 : value);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
QColor tqcToQColor(const maFloatRGBA& i_Color)
{
	return QColor(to_byte(i_Color.GetRed()), to_byte(i_Color.GetGreen()), to_byte(i_Color.GetBlue()));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcColorDialog::tqcColorDialog( QWidget* parent,
							    const maFloatRGBA& i_Value )
: QColorDialog( tqcToQColor(i_Value), parent ),
	m_Value(i_Value),
	m_OriginalValue(i_Value)
{
	this->setWindowTitle("Color");
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setModal(false);
	if (l_bHaveLastPosition)
		this->move(l_lastPosition);

	// Every change is applied right away
	QObject::connect(this, &QColorDialog::currentColorChanged, [this](const QColor& i_Color)
	{
		set_color(i_Color);
	});

	// Cancel restores the original color
	QObject::connect(this, &QDialog::rejected, [this]()
	{
		set_color(m_OriginalValue);
	});

	QObject::connect(this, &QDialog::finished, [this](int)
	{
		l_lastPosition = this->pos();
		l_bHaveLastPosition = true;
	});
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcColorDialog::~tqcColorDialog()
{
	if (tqcColorDialog::Instance == this)
		tqcColorDialog::Instance = NULL;
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& tqcColorDialog::GetValue() const
{
	return m_Value;
}
void tqcColorDialog::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcColorDialog::SetColorChangedCallback(const ColorChangedFunction& i_FuncPtr)
{
	m_Callback = i_FuncPtr;
}

//--------------------------------------------------------------------
// set color from the dialog, keeping the alpha value
//--------------------------------------------------------------------
void tqcColorDialog::set_color(const QColor& i_NewColor)
{
	maFloatRGBA new_value = m_Value;
	// Keep the exact original value when the dialog shows it unchanged,
	// so values outside 0-1 survive opening the dialog.
	if (i_NewColor == tqcToQColor(m_Value))
		return;
	new_value.SetRed( (float)i_NewColor.redF() );
	new_value.SetGreen( (float)i_NewColor.greenF() );
	new_value.SetBlue( (float)i_NewColor.blueF() );
	set_color(new_value);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcColorDialog::set_color(const maFloatRGBA& i_NewColor)
{
	if (!(i_NewColor == m_Value))
	{
		m_Value = i_NewColor;
		do_notify();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcColorDialog::do_notify()
{
	// trigger the callback
	if (m_Callback)
		(m_Callback)();
}

#endif // USE_QT
