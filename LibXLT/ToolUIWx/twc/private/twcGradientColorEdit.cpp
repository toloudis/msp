/****************************************************************************\
**	twcGradientColorEdit.hpp
**
**		Custom control with GradientColorBase and color property window
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcGradientColorEdit.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcGradientColorEdit::twcGradientColorEdit(wxWindow* i_pParent)
:	wxPanel(i_pParent)
{		
	m_control = new wxControl(this, wxID_ANY);
	m_control->Hide();

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;	// Contain shape, interpolation and preview texture
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_gradient_gradientEditor = new twcGradientColorBase(this);
	bSizer2->Add(m_gradient_gradientEditor, 0, wxALL | wxEXPAND, 0);

	bSizer1->Add(bSizer2, 0, wxALL | wxEXPAND, 5);
	
	this->Connect(m_gradient_gradientEditor->GetId(), wxEVT_VALUE_CHANGED, 
		wxCommandEventHandler(twcGradientColorEdit::OnValueChange), NULL, this);
	this->SetSizer( bSizer1 );
	this->Layout();
}

twcGradientColorEdit::~twcGradientColorEdit()
{
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maGradient& twcGradientColorEdit::GetValue() const
{
	return m_gradient_gradientEditor->GetValue();
}

void twcGradientColorEdit::SetValue(const maGradient& i_Value)
{
	m_gradient_gradientEditor->SetValue(i_Value);
}

//--------------------------------------------------------------------
// Callback when value change		
//--------------------------------------------------------------------
void twcGradientColorEdit::OnValueChange(wxCommandEvent& i_Event)
{
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	m_control->ProcessCommand(changed_event);
}


#endif // USE_WXWIDGETS
