/****************************************************************************\
**	tqcGradientColorEdit.hpp
**
**		Custom control with GradientColorBase and color property window
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcGradientColorEdit.hpp"

#include "ToolUIQt/tqc/tqcEvent.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcGradientColorEdit::tqcGradientColorEdit(QWidget* i_pParent)
:	wxPanel(i_pParent)
{		
	m_control = new wxControl(this, wxID_ANY);
	m_control->Hide();

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;	// Contain shape, interpolation and preview texture
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_gradient_gradientEditor = new tqcGradientColorBase(this);
	bSizer2->Add(m_gradient_gradientEditor, 0, wxALL | wxEXPAND, 0);

	bSizer1->Add(bSizer2, 0, wxALL | wxEXPAND, 5);
	
	this->Connect(m_gradient_gradientEditor->GetId(), wxEVT_VALUE_CHANGED, 
		wxCommandEventHandler(tqcGradientColorEdit::OnValueChange), NULL, this);
	this->SetSizer( bSizer1 );
	this->Layout();
}

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

//--------------------------------------------------------------------
// Callback when value change		
//--------------------------------------------------------------------
void tqcGradientColorEdit::OnValueChange(wxCommandEvent& i_Event)
{
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	m_control->ProcessCommand(changed_event);
}


#endif // USE_QT
