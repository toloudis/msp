/****************************************************************************\
**	twcColorRGBAEdit.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcColorRGBAEdit.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorRGBAEdit::twcColorRGBAEdit(wxWindow* i_pParent)
:	wxPanel(i_pParent),
	m_Value(0,0,0,1)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_pFloatEditR = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 16,20 ));
	this->Connect( m_pFloatEditR->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBAEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditR, 1, wxALL, 5 );

	m_pFloatEditG = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 16,20 ));
	this->Connect( m_pFloatEditG->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBAEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditG, 1, wxALL, 5 );

	m_pFloatEditB = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 16,20 ));
	this->Connect( m_pFloatEditB->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBAEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditB, 1, wxALL, 5 );

	m_pFloatEditA = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 16,20 ));
	m_pFloatEditA->SetFloatValue(255);
	this->Connect( m_pFloatEditA->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBAEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditA, 1, wxALL, 5 );

	//m_pColorPicker = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxSize( 16,20 ));
	//this->Connect( m_pColorPicker->GetId(), wxEVT_COMMAND_COLOURPICKER_CHANGED,
	//				wxCommandEventHandler(twcColorRGBAEdit::OnColorChange) );
	m_pColorPicker = new twcColorPicker(this, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ));
	this->Connect( m_pColorPicker->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBAEdit::OnColorChange) );
	bSizer1->Add( m_pColorPicker, 1, wxALL , 5 );

	// float edits should act like integer values
	m_pFloatEditR->SetDecimalPlaces(0);
	m_pFloatEditG->SetDecimalPlaces(0);
	m_pFloatEditB->SetDecimalPlaces(0);
	m_pFloatEditA->SetDecimalPlaces(0);
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& twcColorRGBAEdit::GetValue() const
{
	return m_Value;
}
void twcColorRGBAEdit::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;

	int red = 255 * m_Value.GetRed();
	int green = 255 * m_Value.GetGreen();
	int blue = 255 * m_Value.GetBlue();
	int alpha = 255 * m_Value.GetAlpha();
	m_pFloatEditR->SetFloatValue(red); 
	m_pFloatEditG->SetFloatValue(green);
	m_pFloatEditB->SetFloatValue(blue);
	m_pFloatEditA->SetFloatValue(alpha);

	m_pColorPicker->SetValue(m_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorRGBAEdit::OnTextChange(wxCommandEvent& i_Event)
{
	maFloatRGBA new_value( m_pFloatEditR->GetFloatValue() / 255.0f, 
					  m_pFloatEditG->GetFloatValue() / 255.0f, 
					  m_pFloatEditB->GetFloatValue() / 255.0f,
					  m_pFloatEditA->GetFloatValue() / 255.0f );

	if (!(m_Value == new_value))
	{
		m_Value = new_value;
		m_pColorPicker->SetValue(m_Value);

		// Use one of the float edit fields to trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		m_pFloatEditR->ProcessCommand(changed_event);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorRGBAEdit::OnColorChange(wxCommandEvent& i_Event)
{
	maFloatRGBA new_value = m_pColorPicker->GetValue();
	if (!(m_Value == new_value))
	{
		m_Value = new_value;

		m_pFloatEditR->SetFloatValue(new_value.GetRed() * 255); 
		m_pFloatEditG->SetFloatValue(new_value.GetGreen() * 255);
		m_pFloatEditB->SetFloatValue(new_value.GetBlue() * 255);

		// Use one of the float edit fields to trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		m_pFloatEditR->ProcessCommand(changed_event);
	}
}


#endif // USE_WXWIDGETS
