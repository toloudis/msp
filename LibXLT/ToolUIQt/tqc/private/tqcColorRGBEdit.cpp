/****************************************************************************\
**	tqcColorRGBEdit.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcColorRGBEdit.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcColorRGBEdit::tqcColorRGBEdit(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_Value(0,0,0,1)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(2);

	tqcFloatEdit** edits[3] = { &m_pFloatEditR, &m_pFloatEditG, &m_pFloatEditB };
	for (int i = 0; i < 3; ++i)
	{
		*edits[i] = new tqcFloatEdit(this);
		(*edits[i])->setMinimumWidth(20);
		// float edits should act like integer values
		(*edits[i])->SetDecimalPlaces(0);
		(*edits[i])->SetValueChangedCallback([this]() { OnTextChange(); });
		bSizer1->addWidget( *edits[i], 1 );
	}

	m_pColorPicker = new tqcColorPicker(this);
	m_pColorPicker->SetValueChangedCallback([this]() { OnColorChange(); });
	bSizer1->addWidget( m_pColorPicker, 1 );
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& tqcColorRGBEdit::GetValue() const
{
	return m_Value;
}
void tqcColorRGBEdit::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;
	update_edits();
	m_pColorPicker->SetValue(i_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorRGBEdit::update_edits()
{
	m_pFloatEditR->SetFloatValue((float)(int)(255 * m_Value.GetRed()));
	m_pFloatEditG->SetFloatValue((float)(int)(255 * m_Value.GetGreen()));
	m_pFloatEditB->SetFloatValue((float)(int)(255 * m_Value.GetBlue()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorRGBEdit::OnTextChange()
{
	maFloatRGBA new_value( m_pFloatEditR->GetFloatValue() / 255.0f,
						   m_pFloatEditG->GetFloatValue() / 255.0f,
						   m_pFloatEditB->GetFloatValue() / 255.0f,
						   1.0f );
	if (!(m_Value == new_value))
	{
		m_Value = new_value;
		m_pColorPicker->SetValue(m_Value);
		NotifyValueChanged();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorRGBEdit::OnColorChange()
{
	maFloatRGBA new_value = m_pColorPicker->GetValue();
	if (!(m_Value == new_value))
	{
		m_Value = new_value;
		update_edits();
		NotifyValueChanged();
	}
}

#endif // USE_QT
