/****************************************************************************\
**	twcVector3EditUpDown.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcVector3EditUpDown.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcVector3EditUpDown::twcVector3EditUpDown(wxWindow* i_pParent)
:	wxPanel(i_pParent),
	m_Value(0,0,0)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_pNumericUpDownX = new twcNumericUpDown(this);
	this->Connect( m_pNumericUpDownX->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcVector3EditUpDown::OnTextChange) );
	bSizer1->Add( m_pNumericUpDownX, 1, wxALL, 2 );

	m_pNumericUpDownY = new twcNumericUpDown(this);
	this->Connect( m_pNumericUpDownY->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcVector3EditUpDown::OnTextChange) );
	bSizer1->Add( m_pNumericUpDownY, 1, wxALL, 2 );

	m_pNumericUpDownZ = new twcNumericUpDown(this);
	this->Connect( m_pNumericUpDownZ->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcVector3EditUpDown::OnTextChange) );
	bSizer1->Add( m_pNumericUpDownZ, 1, wxALL, 2 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maVector3d& twcVector3EditUpDown::GetValue() const
{
	return m_Value;
}
void twcVector3EditUpDown::SetValue(const maVector3d& i_Value)
{
	m_Value = i_Value;

	m_pNumericUpDownX->SetValue(m_Value.m_X); 
	m_pNumericUpDownY->SetValue(m_Value.m_Y);
	m_pNumericUpDownZ->SetValue(m_Value.m_Z);
}

//--------------------------------------------------------------------
//	Increment
//--------------------------------------------------------------------
maVector3d twcVector3EditUpDown::GetIncrement() const
{
	return maVector3d(	m_pNumericUpDownX->GetIncrement(),
						m_pNumericUpDownY->GetIncrement(),
						m_pNumericUpDownZ->GetIncrement() );
}
void twcVector3EditUpDown::SetIncrement(const maVector3d& i_Increment)
{
	m_pNumericUpDownX->SetIncrement(i_Increment.m_X);
	m_pNumericUpDownY->SetIncrement(i_Increment.m_Y);
	m_pNumericUpDownZ->SetIncrement(i_Increment.m_Z);
}
void twcVector3EditUpDown::SetIncrement(const float i_IncX, const float i_IncY, const float i_IncZ)
{
	m_pNumericUpDownX->SetIncrement(i_IncX);
	m_pNumericUpDownY->SetIncrement(i_IncY);
	m_pNumericUpDownZ->SetIncrement(i_IncZ);
}

//--------------------------------------------------------------------
//	Maximum
//--------------------------------------------------------------------
maVector3d twcVector3EditUpDown::GetMaximum() const
{
	return maVector3d(	m_pNumericUpDownX->GetMaximum(),
						m_pNumericUpDownY->GetMaximum(),
						m_pNumericUpDownZ->GetMaximum() );
}
void twcVector3EditUpDown::SetMaximum(const maVector3d& i_Maximum)
{
	m_pNumericUpDownX->SetMaximum(i_Maximum.m_X);
	m_pNumericUpDownY->SetMaximum(i_Maximum.m_Y);
	m_pNumericUpDownZ->SetMaximum(i_Maximum.m_Z);
}
void twcVector3EditUpDown::SetMaximum(const float i_ValX, const float i_ValY, const float i_ValZ)
{
	m_pNumericUpDownX->SetMaximum(i_ValX);
	m_pNumericUpDownY->SetMaximum(i_ValY);
	m_pNumericUpDownZ->SetMaximum(i_ValZ);
}

//--------------------------------------------------------------------
//	Minimum
//--------------------------------------------------------------------
maVector3d twcVector3EditUpDown::GetMinimum() const
{
	return maVector3d(	m_pNumericUpDownX->GetMinimum(),
						m_pNumericUpDownY->GetMinimum(),
						m_pNumericUpDownZ->GetMinimum() );
}
void twcVector3EditUpDown::SetMinimum(const maVector3d& i_Minimum)
{
	m_pNumericUpDownX->SetMinimum(i_Minimum.m_X);
	m_pNumericUpDownY->SetMinimum(i_Minimum.m_Y);
	m_pNumericUpDownZ->SetMinimum(i_Minimum.m_Z);
}
void twcVector3EditUpDown::SetMinimum(const float i_ValX, const float i_ValY, const float i_ValZ)
{
	m_pNumericUpDownX->SetMinimum(i_ValX);
	m_pNumericUpDownY->SetMinimum(i_ValY);
	m_pNumericUpDownZ->SetMinimum(i_ValZ);
}


//--------------------------------------------------------------------
//	DecimalPlaces - passed on to NumericUpDown control
//--------------------------------------------------------------------
const short twcVector3EditUpDown::GetDecimalPlaces() const
{
	return m_pNumericUpDownX->GetDecimalPlaces();
}
void twcVector3EditUpDown::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_pNumericUpDownX->SetDecimalPlaces(i_DecimalPlaces);
	m_pNumericUpDownY->SetDecimalPlaces(i_DecimalPlaces);
	m_pNumericUpDownZ->SetDecimalPlaces(i_DecimalPlaces);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcVector3EditUpDown::OnTextChange(wxCommandEvent& i_Event)
{
	maVector3d new_value( m_pNumericUpDownX->GetValue(), 
					  m_pNumericUpDownY->GetValue(), 
					  m_pNumericUpDownZ->GetValue() );

	if (m_Value != new_value)
	{
		m_Value = new_value;

		// Use one of the float edit fields to trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		m_pNumericUpDownX->ProcessCommand(changed_event);
	}
}


#endif // USE_WXWIDGETS
