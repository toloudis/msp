/****************************************************************************\
**	twcNumericUpDown.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcNumericUpDown.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"

#ifdef USE_WXWIDGETS

BEGIN_EVENT_TABLE(twcNumericUpDown, wxPanel)
	EVT_SPIN_UP(wxID_ANY, twcNumericUpDown::OnScrollUp)
	EVT_SPIN_DOWN(wxID_ANY, twcNumericUpDown::OnScrollDown)
	EVT_LEFT_DOWN(twcNumericUpDown::OnMouseClick)
	EVT_LEFT_UP(twcNumericUpDown::OnMouseUp)
	EVT_MOTION(twcNumericUpDown::OnMouseDrag)
END_EVENT_TABLE()

twcNumericUpDown* twcNumericUpDown::Instance = NULL;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcNumericUpDown::twcNumericUpDown(wxWindow* i_pParent)
:	wxPanel(i_pParent, wxID_ANY),
	m_Maximum(100),
	m_bRestrictValue(false),
	m_Increment(1), 
	new_value(0.0),
	m_yInitial(0.0),
	m_yOffset(0)
{		
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	m_fValue = 0.0;
	m_Minimum = 0.0;
	m_bspinButtonClicked = false;
	m_pFloatEdit = new twcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 40,20 ));
	this->Connect( m_pFloatEdit->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(twcNumericUpDown::OnTextChange) );
	
	bSizer1->Add( m_pFloatEdit, 1, wxALL, 0 );
	
	m_pSpinner = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,20 ), wxSL_VERTICAL );
	m_pSpinner->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler(twcNumericUpDown::OnMouseClick), NULL, this );
	m_pSpinner->Connect(wxEVT_MOTION, wxMouseEventHandler(twcNumericUpDown::OnMouseDrag), NULL, this );
	m_pSpinner->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(twcNumericUpDown::OnMouseUp), NULL, this );
	
	//wxWindow* parent = FindWindowById(m_pSpinner->GetId(), this); 
	// This is so that the mouse can be made to move outside the buttons onto the parent window.
	this->Connect( wxEVT_MOTION, wxMouseEventHandler(twcNumericUpDown::OnMouseDrag), NULL, this );
	this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler(twcNumericUpDown::OnMouseUp), NULL, this );
	
	bSizer1->Add( m_pSpinner, 0, wxALL, 0 );
	this->SetSizer( bSizer1 );
	this->Layout();
}

twcNumericUpDown::~twcNumericUpDown()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing twcNumericUpDown()");
	if (twcNumericUpDown::Instance == this)
	{
		twcNumericUpDown::Instance = NULL;
		
	}

}
//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const float twcNumericUpDown::GetValue() const
{
	return m_fValue;
}
void twcNumericUpDown::SetValue(const float i_Value)
{
	m_fValue = i_Value;
	m_pFloatEdit->SetFloatValue(m_fValue);
}

//--------------------------------------------------------------------
//	Minimum
//--------------------------------------------------------------------
const float twcNumericUpDown::GetMinimum() const
{
	return m_Minimum;
}
void twcNumericUpDown::SetMinimum(const float i_Minimum)
{
	m_Minimum = i_Minimum;
}

//--------------------------------------------------------------------
//	Maximum
//--------------------------------------------------------------------
const float twcNumericUpDown::GetMaximum() const
{
	return m_Maximum;
}
void twcNumericUpDown::SetMaximum(const float i_Maximum)
{
	m_Maximum = i_Maximum;
}

//--------------------------------------------------------------------
// Functions to check if the flag restricting the ranges are set
//--------------------------------------------------------------------

const bool twcNumericUpDown::GetRestrictFlag() const
{
	return m_bRestrictValue;
}

void twcNumericUpDown::SetRestrictFlag( const bool i_bRestrictValue)
{
	m_bRestrictValue = i_bRestrictValue;
}

//--------------------------------------------------------------------
//	DecimalPlaces - passed on to FloatEdit control
//--------------------------------------------------------------------
const short twcNumericUpDown::GetDecimalPlaces() const
{
	return m_pFloatEdit->GetDecimalPlaces();
}
void twcNumericUpDown::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_pFloatEdit->SetDecimalPlaces(i_DecimalPlaces);
}

//--------------------------------------------------------------------
//	Increment
//--------------------------------------------------------------------
const float twcNumericUpDown::GetIncrement() const
{
	return m_Increment;
}
void twcNumericUpDown::SetIncrement(const float i_Increment)
{
	m_Increment = i_Increment;
}

//--------------------------------------------------------------------
// Calls the callback and appropriate event handlers
//--------------------------------------------------------------------
bool twcNumericUpDown::ProcessCommand(wxCommandEvent& i_Event)
{
	// Use the float edit field to trigger the callback
	return m_pFloatEdit->ProcessCommand(i_Event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::OnTextChange(wxCommandEvent& i_Event)
{
	
	wxString str;
	m_fValue = m_pFloatEdit->GetFloatValue();

	if(GetRestrictFlag())
	{
		if(m_fValue < GetMinimum()) 
		{
			str << (int)GetMinimum();
			m_pFloatEdit->ChangeValue(str); // This changes the Value on the control.
			m_fValue = GetMinimum();
		}
		else if(m_fValue > GetMaximum()) 
		{
			str << (int)GetMaximum();
			m_pFloatEdit->ChangeValue(str); // This changes the Value on the control.
			m_fValue = GetMaximum();
		}
	}
	notify_callback();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::OnScrollUp(wxSpinEvent& i_Event)
{

	m_bspinButtonClicked = true ;
	new_value = m_fValue + m_Increment;
	
	if(GetRestrictFlag())
	{
		if (new_value > GetMaximum())
			new_value = GetMaximum();
	}
	if (new_value != m_fValue)
	{
		m_fValue = new_value;
		m_pFloatEdit->SetFloatValue(m_fValue);
		notify_callback();
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::OnScrollDown(wxSpinEvent& i_Event)
{
	m_bspinButtonClicked = true ;
	new_value = m_fValue - m_Increment;
	if(GetRestrictFlag())
	{
		if (new_value < GetMinimum())
			new_value = GetMinimum();
	}
	
	if (new_value != m_fValue)
	{
		m_fValue = new_value;
		m_pFloatEdit->SetFloatValue(m_fValue);
		notify_callback();
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::OnMouseClick(wxMouseEvent& i_Event)
{
	i_Event.Skip();
	if(i_Event.LeftDown())
	{
		m_yInitial = i_Event.m_y; 
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::OnMouseDrag(wxMouseEvent& i_Event)
{
	i_Event.Skip();
	float temp = 0.0;
	
	// If the mouse is dragged without the buttons being clicked bail out
	if(m_bspinButtonClicked == false) return;

	if(i_Event.Dragging())
	{
		temp = m_yOffset;
		if(m_yInitial < 0 ) return;
		m_yOffset = i_Event.m_y - m_yInitial;

		// This checks if the mouse is moving down or up.	
		if(temp > m_yOffset )
			new_value = m_fValue + abs(m_yOffset * GetIncrement());
		else 
			new_value = m_fValue - abs(m_yOffset * GetIncrement());
		if(GetRestrictFlag())
		{
			if (new_value > GetMaximum() ) new_value = GetMaximum();
			if(new_value < GetMinimum()) new_value = GetMinimum();
		}
		if(new_value != m_fValue)
		{
			m_fValue = new_value;
			if(m_pFloatEdit)
			{
				m_pFloatEdit->SetFloatValue(m_fValue);
				notify_callback();
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::OnMouseUp(wxMouseEvent& i_Event)
{
	i_Event.Skip();
	m_bspinButtonClicked = false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcNumericUpDown::notify_callback()
{
	// trigger the callback
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	m_pFloatEdit->ProcessCommand(changed_event);
}
#endif // USE_WXWIDGETS
