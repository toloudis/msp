/****************************************************************************\
**	twcColorPicker.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcColorPicker.hpp"
#include "ToolUIWx/twc/private/twcColorDialog.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


#ifdef USE_WXWIDGETS

BEGIN_EVENT_TABLE(twcColorPicker, wxButton)
    EVT_BUTTON(wxID_ANY, twcColorPicker::OnButtonClick)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorPicker::twcColorPicker(wxWindow* i_pParent, 
					   wxWindowID i_Id,
					   const wxPoint& pos, 
					   const wxSize& size)
:	wxButton(i_pParent, i_Id, wxEmptyString, pos, size),
	m_pColorDialog(NULL)
{		
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorPicker::~twcColorPicker()
{
	// if our color dialog is still up, close it
	if (m_pColorDialog && (m_pColorDialog == twcColorDialog::Instance))
	{
		twcColorDialog::Instance->Close();
	}

}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& twcColorPicker::GetValue() const
{
	return m_Value;
}
void twcColorPicker::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;

	int red = 255 * m_Value.GetRed();
	int green = 255 * m_Value.GetGreen();
	int blue = 255 * m_Value.GetBlue();
	wxColour color(red, green, blue);
	this->SetBackgroundColour(color);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorPicker::OnButtonClick(wxCommandEvent& i_Event)
{
	// If we already have a color dialog, then don't do anything
	if (m_pColorDialog && (m_pColorDialog == twcColorDialog::Instance))
		return;

	// if a color picker is already up, close it
	if (twcColorDialog::Instance)
	{
		twcColorDialog::Instance->Close();
	}

	//DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
	m_pColorDialog = new twcColorDialog(twxSystem::g_pMainForm, m_Value);
	twcColorDialog::Instance = m_pColorDialog;
	//this->Connect( m_pColorDialog->GetId(), wxEVT_VALUE_CHANGED,
	//				wxCommandEventHandler(twcColorPicker::OnColorDialogChange) );
	m_pColorDialog->SetColorChangedCallback(std::bind(
							&twcColorPicker::OnColorDialogChange, this));
	m_pColorDialog->Show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorPicker::OnColorDialogChange()
{
	if (m_pColorDialog)
	{
		maFloatRGBA new_value = m_pColorDialog->GetValue();
		if (!(new_value == m_Value))
		{
			this->SetValue( new_value );

			// trigger the callback
			wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
			this->ProcessCommand(changed_event);
		}
	}
}

#endif // USE_WXWIDGETS
