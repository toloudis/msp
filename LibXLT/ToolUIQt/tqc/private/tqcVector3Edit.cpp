/****************************************************************************\
**	tqcVector3Edit.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcVector3Edit.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcVector3Edit::tqcVector3Edit(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_Value(0,0,0)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(2);

	tqcFloatEdit** edits[3] = { &m_pFloatEditX, &m_pFloatEditY, &m_pFloatEditZ };
	for (int i = 0; i < 3; ++i)
	{
		*edits[i] = new tqcFloatEdit(this);
		(*edits[i])->SetValueChangedCallback([this]() { OnTextChange(); });
		bSizer1->addWidget( *edits[i], 1 );
	}
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maVector3d& tqcVector3Edit::GetValue() const
{
	return m_Value;
}
void tqcVector3Edit::SetValue(const maVector3d& i_Value)
{
	m_Value = i_Value;
	m_pFloatEditX->SetFloatValue(m_Value.m_X);
	m_pFloatEditY->SetFloatValue(m_Value.m_Y);
	m_pFloatEditZ->SetFloatValue(m_Value.m_Z);
}

//--------------------------------------------------------------------
//	DecimalPlaces - passed on to FloatEdit control
//--------------------------------------------------------------------
const short tqcVector3Edit::GetDecimalPlaces() const
{
	return m_pFloatEditX->GetDecimalPlaces();
}
void tqcVector3Edit::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_pFloatEditX->SetDecimalPlaces(i_DecimalPlaces);
	m_pFloatEditY->SetDecimalPlaces(i_DecimalPlaces);
	m_pFloatEditZ->SetDecimalPlaces(i_DecimalPlaces);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcVector3Edit::OnTextChange()
{
	maVector3d new_value( m_pFloatEditX->GetFloatValue(),
						  m_pFloatEditY->GetFloatValue(),
						  m_pFloatEditZ->GetFloatValue() );
	if (m_Value != new_value)
	{
		m_Value = new_value;
		NotifyValueChanged();
	}
}

#endif // USE_QT
