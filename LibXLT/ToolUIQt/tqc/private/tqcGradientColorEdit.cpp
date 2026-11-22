/****************************************************************************\
**	tqcGradientColorEdit.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcGradientColorEdit.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcGradientColorEdit::tqcGradientColorEdit(QWidget* i_pParent)
:	QWidget(i_pParent)
{
	QVBoxLayout* bSizer1 = new QVBoxLayout(this);
	bSizer1->setContentsMargins(5, 5, 5, 5);

	m_gradient_gradientEditor = new tqcGradientColorBase(this);
	m_gradient_gradientEditor->SetValueChangedCallback([this]() { NotifyValueChanged(); });
	bSizer1->addWidget(m_gradient_gradientEditor);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcGradientColorEdit::~tqcGradientColorEdit()
{
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maGradient& tqcGradientColorEdit::GetValue() const
{
	return m_gradient_gradientEditor->GetValue();
}
void tqcGradientColorEdit::SetValue(const maGradient& i_Value)
{
	m_gradient_gradientEditor->SetValue(i_Value);
}

#endif // USE_QT
