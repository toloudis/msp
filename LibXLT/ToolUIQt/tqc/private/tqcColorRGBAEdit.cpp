/****************************************************************************\
**	tqcColorRGBAEdit.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcColorRGBAEdit.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcColorRGBAEdit::tqcColorRGBAEdit(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_Value(0,0,0,1)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(2);

	tqcFloatEdit** edits[4] = { &m_pFloatEditR, &m_pFloatEditG, &m_pFloatEditB, &m_pFloatEditA };
	for (int i = 0; i < 4; ++i)
	{
		*edits[i] = new tqcFloatEdit(this);
		(*edits[i])->setMinimumWidth(16);
		// float edits should act like integer values
		(*edits[i])->SetDecimalPlaces(0);
		(*edits[i])->SetValueChangedCallback([this]() { OnTextChange(); });
		bSizer1->addWidget( *edits[i], 1 );
	}
	m_pFloatEditA->SetFloatValue(255);

	m_pColorPicker = new tqcColorPicker(this);
	m_pColorPicker->SetValueChangedCallback([this]() { OnColorChange(); });
	bSizer1->addWidget( m_pColorPicker, 1 );
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& tqcColorRGBAEdit::GetValue() const
{
	return m_Value;
}
void tqcColorRGBAEdit::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;
	update_edits();
	m_pColorPicker->SetValue(i_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorRGBAEdit::update_edits()
{
	m_pFloatEditR->SetFloatValue((float)(int)(255 * m_Value.GetRed()));
	m_pFloatEditG->SetFloatValue((float)(int)(255 * m_Value.GetGreen()));
	m_pFloatEditB->SetFloatValue((float)(int)(255 * m_Value.GetBlue()));
	m_pFloatEditA->SetFloatValue((float)(int)(255 * m_Value.GetAlpha()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorRGBAEdit::OnTextChange()
{
	maFloatRGBA new_value( m_pFloatEditR->GetFloatValue() / 255.0f,
						   m_pFloatEditG->GetFloatValue() / 255.0f,
						   m_pFloatEditB->GetFloatValue() / 255.0f,
						   m_pFloatEditA->GetFloatValue() / 255.0f );
	if (!(m_Value == new_value))
	{
		m_Value = new_value;
		m_pColorPicker->SetValue(m_Value);
		NotifyValueChanged();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorRGBAEdit::OnColorChange()
{
	// The color picker doesn't change alpha
	maFloatRGBA new_value = m_pColorPicker->GetValue();
	new_value.SetAlpha(m_Value.GetAlpha());
	if (!(m_Value == new_value))
	{
		m_Value = new_value;
		update_edits();
		NotifyValueChanged();
	}
}

#endif // USE_QT
