/****************************************************************************\
**	twcColorRGBEdit.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcColorRGBEdit.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorRGBEdit::twcColorRGBEdit(wxWindow* i_pParent)
:	wxPanel(i_pParent),
	m_Value(0,0,0,1)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_pFloatEditR = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ));
	this->Connect( m_pFloatEditR->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditR, 1, wxALL, 2 );

	m_pFloatEditG = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ));
	this->Connect( m_pFloatEditG->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditG, 1, wxALL, 2 );

	m_pFloatEditB = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ));
	this->Connect( m_pFloatEditB->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBEdit::OnTextChange) );
	bSizer1->Add( m_pFloatEditB, 1, wxALL, 2 );

	//m_pColorPicker = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxSize( 20,20 ));
	//this->Connect( m_pColorPicker->GetId(), wxEVT_COMMAND_COLOURPICKER_CHANGED,
	//				wxCommandEventHandler(twcColorRGBEdit::OnColorChange) );
	m_pColorPicker = new twcColorPicker(this, wxID_ANY, wxDefaultPosition, wxSize( 20,20 ));
	this->Connect( m_pColorPicker->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcColorRGBEdit::OnColorChange) );
	bSizer1->Add( m_pColorPicker, 1, wxALL, 2 );

	// float edits should act like integer values
	m_pFloatEditR->SetDecimalPlaces(0);
	m_pFloatEditG->SetDecimalPlaces(0);
	m_pFloatEditB->SetDecimalPlaces(0);
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& twcColorRGBEdit::GetValue() const
{
	return m_Value;
}
void twcColorRGBEdit::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;

	int red = 255 * m_Value.GetRed();
	int green = 255 * m_Value.GetGreen();
	int blue = 255 * m_Value.GetBlue();
	m_pFloatEditR->SetFloatValue(red); 
	m_pFloatEditG->SetFloatValue(green);
	m_pFloatEditB->SetFloatValue(blue);

	//wxColour color(red, green, blue);
	//m_pColorPicker->SetColour(color);
	m_pColorPicker->SetValue(i_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorRGBEdit::OnTextChange(wxCommandEvent& i_Event)
{
	maFloatRGBA new_value( m_pFloatEditR->GetFloatValue() / 255.0f, 
					  m_pFloatEditG->GetFloatValue() / 255.0f, 
					  m_pFloatEditB->GetFloatValue() / 255.0f,
					  1.0f );

	if (!(m_Value == new_value))
	{
		m_Value = new_value;

		//wxColour color(m_pFloatEditR->GetFloatValue(), 
		//			   m_pFloatEditG->GetFloatValue(), 
		//			   m_pFloatEditB->GetFloatValue());
		//m_pColorPicker->SetColour(color);
		m_pColorPicker->SetValue(m_Value);

		// Use one of the float edit fields to trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		m_pFloatEditR->ProcessCommand(changed_event);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorRGBEdit::OnColorChange(wxCommandEvent& i_Event)
{
	maFloatRGBA new_value = m_pColorPicker->GetValue();

	if (!(m_Value == new_value))
	{
		m_Value = new_value;

		int red = 255 * m_Value.GetRed();
		int green = 255 * m_Value.GetGreen();
		int blue = 255 * m_Value.GetBlue();
		m_pFloatEditR->SetFloatValue(red); 
		m_pFloatEditG->SetFloatValue(green);
		m_pFloatEditB->SetFloatValue(blue);

		// Use one of the float edit fields to trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		m_pFloatEditR->ProcessCommand(changed_event);
	}
}


#endif // USE_WXWIDGETS
