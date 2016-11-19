/****************************************************************************\
**	twcVector3Edit.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcVector3Edit.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcVector3Edit::twcVector3Edit(wxWindow* i_pParent)
:	wxPanel(i_pParent),
	m_Value(0,0,0)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_pFloatEditX = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize( 40,20 )));
	this->Connect( m_pFloatEditX->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcVector3Edit::OnTextChange) );
	bSizer1->Add( m_pFloatEditX, 1, wxALL, 2 );

	m_pFloatEditY = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize( 40,20 )));
	this->Connect( m_pFloatEditY->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcVector3Edit::OnTextChange) );
	bSizer1->Add( m_pFloatEditY, 1, wxALL, 2 );

	m_pFloatEditZ = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize( 40,20 )));
	this->Connect( m_pFloatEditZ->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcVector3Edit::OnTextChange) );
	bSizer1->Add( m_pFloatEditZ, 1, wxALL, 2 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maVector3d& twcVector3Edit::GetValue() const
{
	return m_Value;
}
void twcVector3Edit::SetValue(const maVector3d& i_Value)
{
	m_Value = i_Value;

	m_pFloatEditX->SetFloatValue(m_Value.m_X); 
	m_pFloatEditY->SetFloatValue(m_Value.m_Y);
	m_pFloatEditZ->SetFloatValue(m_Value.m_Z);
}

//--------------------------------------------------------------------
//	DecimalPlaces - passed on to FloatEdit control
//--------------------------------------------------------------------
const short twcVector3Edit::GetDecimalPlaces() const
{
	return m_pFloatEditX->GetDecimalPlaces();
}
void twcVector3Edit::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_pFloatEditX->SetDecimalPlaces(i_DecimalPlaces);
	m_pFloatEditY->SetDecimalPlaces(i_DecimalPlaces);
	m_pFloatEditZ->SetDecimalPlaces(i_DecimalPlaces);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcVector3Edit::OnTextChange(wxCommandEvent& i_Event)
{
	maVector3d new_value( m_pFloatEditX->GetFloatValue(), 
					  m_pFloatEditY->GetFloatValue(), 
					  m_pFloatEditZ->GetFloatValue() );

	if (m_Value != new_value)
	{
		m_Value = new_value;

		// Use one of the float edit fields to trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		m_pFloatEditX->ProcessCommand(changed_event);
	}
}


#endif // USE_WXWIDGETS
