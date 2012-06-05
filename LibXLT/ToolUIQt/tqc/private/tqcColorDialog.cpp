/*****************************************************************************
**	tqcColorDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Core/dbg/dbgMsg.hpp"
#include "ToolUIQt/tqc/private/tqcColorBoxCtrl.hpp"
#include "ToolUIQt/tqc/private/tqcColorDialog.hpp"
#include "ToolUIQt/tqc/private/tqcColorUtil.hpp"


#ifdef QT_FINISH_PORT

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
tqcColorDialog* tqcColorDialog::Instance = NULL;
wxPoint l_lastPosition = wxPoint(-1, -1); // Indicate the default position, chosen by either the windowing system or wxWidgets

//--------------------------------------------------------------------
// Static bool for whether to update every change or wait for Ok/Apply
//--------------------------------------------------------------------
bool tqcColorDialog::sm_bContinuousUpdate = true;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcColorDialog::tqcColorDialog( QWidget* parent,
							    const maFloatRGBA& i_Value )
: tqcColorDialogBase( parent, wxID_ANY, wxT("Color"), l_lastPosition ),
	m_Value(i_Value),
	m_OriginalValue(i_Value),
	m_bDisableNotify(false)
{
	// Set the continuous update checkbox
	this->m_checkBox_Continuous->SetValue(sm_bContinuousUpdate);

	// Add custom controls to panels waiting for them
	wxBoxSizer* bSizerBox = new wxBoxSizer( wxVERTICAL );
	m_pColorBox = new tqcColorBoxCtrl(m_panel_2dpicker, m_Value, wxID_ANY, wxDefaultPosition, wxSize( 256,256 ));
	this->Connect( m_pColorBox->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(tqcColorDialog::ColorBox_Changed) );
	bSizerBox->Add( m_pColorBox, 1, wxEXPAND | wxALL, 0 );
	m_panel_2dpicker->SetSizer(bSizerBox);
	m_panel_2dpicker->Layout();

	wxBoxSizer* bSizerSlider = new wxBoxSizer( wxVERTICAL );
	m_pColorSlider = new tqcColorSliderCtrl(m_panel_vertSlider, m_Value, wxID_ANY, wxDefaultPosition, wxSize( 40,256 ));
	this->Connect( m_pColorSlider->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(tqcColorDialog::ColorSlider_Changed) );
	bSizerSlider->Add( m_pColorSlider, 1, wxEXPAND | wxALL, 0 );
	m_panel_vertSlider->SetSizer(bSizerSlider);
	m_panel_vertSlider->Layout();

	// Store old color into panel
	int red = 255 * m_Value.GetRed();
	int green = 255 * m_Value.GetGreen();
	int blue = 255 * m_Value.GetBlue();
	m_panel_OldColor->SetBackgroundColour( wxColour(red, green, blue) );
	
	m_bDisableNotify = true;
	m_radioBtn_HSV_H->SetValue(true);
	update_rgb();
	update_hsv();
	update_panel_NewColor();
	m_bDisableNotify = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcColorDialog::~tqcColorDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing tqcColorDialog()");
	if (tqcColorDialog::Instance == this)
		tqcColorDialog::Instance = NULL;

}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& tqcColorDialog::GetValue() const
{
	return m_Value;
}
void tqcColorDialog::SetValue(const maFloatRGBA& i_Value)
{
	m_Value = i_Value;
}

//--------------------------------------------------------------------
// Can't seem to use wxEVT_VALUE_CHANGED with a dialog 
// (wxDialog is not a wxControl), so use boost functions
//	to implement the callback.
//--------------------------------------------------------------------
void tqcColorDialog::SetColorChangedCallback(const ColorChangedFunction& i_FuncPtr)
{
	m_Callback = i_FuncPtr;
}

//--------------------------------------------------------------------
// Virtual event handlers
//--------------------------------------------------------------------
void tqcColorDialog::OnClose( wxCloseEvent& i_Event )
{
	// Modeless dialog, need to destroy it
	l_lastPosition = this->GetPosition();
	this->Destroy();
}
void tqcColorDialog::panelOld_MouseDown( wxMouseEvent& i_Event )
{
	if (!m_bDisableNotify)
	{
		// Restore original color
		set_color(m_OriginalValue);

		m_bDisableNotify = true;
		update_rgb();
		update_hsv();
		update_panel_NewColor();
		update_color_box();
		update_color_slider();
		m_bDisableNotify = false;
	}
	i_Event.Skip();
}
void tqcColorDialog::button_Cancel_Click( wxCommandEvent& i_Event )
{
	// Restore original color
	set_color(m_OriginalValue);
	
	if (!sm_bContinuousUpdate)
	{
		// force notify before closing dialog
		do_notify();
	}

	// Close the modeless dialog (can't use EndModal())
	this->Close(true);
}
void tqcColorDialog::button_Apply_Click( wxCommandEvent& i_Event )
{
	// just do the callback, nothing else needed
	do_notify();
}
void tqcColorDialog::button_Ok_Click( wxCommandEvent& i_Event )
{
	if (!sm_bContinuousUpdate)
	{
		// force notify before closing dialog
		do_notify();
	}

	// Close the modeless dialog (can't use EndModal())
	this->Close(true);
}
void tqcColorDialog::ColorBox_Changed( wxCommandEvent& i_Event )
{
	if (!m_bDisableNotify)
	{
		set_color(m_pColorBox->GetColor());

		m_bDisableNotify = true;
		update_rgb();
		update_panel_NewColor();

		// When communicating between controls that understand HSV,
		// just use those values
		double hue, saturation, value;
		m_pColorBox->GetHSV(hue, saturation, value);
		update_hsv(hue, saturation, value);
		m_pColorSlider->SetColor(m_Value, hue, saturation, value); 

		m_bDisableNotify = false;
	}
}
void tqcColorDialog::ColorSlider_Changed( wxCommandEvent& i_Event )
{
	if (!m_bDisableNotify)
	{
		set_color(m_pColorSlider->GetColor());

		m_bDisableNotify = true;
		update_rgb();
		update_panel_NewColor();

		// When communicating between controls that understand HSV,
		// just use those values
		double hue, saturation, value;
		m_pColorSlider->GetHSV(hue, saturation, value);
		update_hsv(hue, saturation, value);
		m_pColorBox->SetColor(m_Value, hue, saturation, value); 

		m_bDisableNotify = false;
	}
}
void tqcColorDialog::spinCtrl_HSV_Char( wxKeyEvent& i_Event )
{
	if (i_Event.GetKeyCode() == WXK_RETURN)
		spinCtrl_HSV_Changed();

}

void tqcColorDialog::spinCtrl_HSV_Focus(wxFocusEvent& i_Event)
{
	if(i_Event.GetEventType() == wxEVT_SET_FOCUS)
		spinCtrl_HSV_Changed();
}

void tqcColorDialog::spinCtrl_HSV_Changed( wxSpinEvent& i_Event )
{
	spinCtrl_HSV_Changed();
}
void tqcColorDialog::spinCtrl_HSV_Changed()
{
	m_spinCtrl_HSV_H->SetSelection(-1,-1);
	m_spinCtrl_HSV_S->SetSelection(-1,-1);
	m_spinCtrl_HSV_V->SetSelection(-1,-1);
	if (!m_bDisableNotify)
	{
		double hue = m_spinCtrl_HSV_H->GetValue() / 360.0;
		double saturation = m_spinCtrl_HSV_S->GetValue() / 100.0;
		double value = m_spinCtrl_HSV_V->GetValue() / 100.0;

		double red, green, blue;
		tqcColorUtil::HSV_to_RGB(hue, saturation, value,
								 red, green, blue);
		set_color(red, green, blue);

		m_bDisableNotify = true;
		update_rgb();
		update_panel_NewColor();
		
		// When communicating between controls that understand HSV,
		// just use those values
		m_pColorBox->SetColor(m_Value, hue, saturation, value); 
		m_pColorSlider->SetColor(m_Value, hue, saturation, value);

		m_bDisableNotify = false;
	}
}

void tqcColorDialog::spinCtrl_RGB_Char( wxKeyEvent& i_Event )
{
		if (i_Event.GetKeyCode() == WXK_RETURN)
		spinCtrl_RGB_Changed();

}

void tqcColorDialog::spinCtrl_RGB_Focus(wxFocusEvent& i_Event)
{
	if(i_Event.GetEventType() == wxEVT_SET_FOCUS)
		spinCtrl_RGB_Changed();
}

void tqcColorDialog::spinCtrl_RGB_Changed( wxSpinEvent& i_Event )
{
	spinCtrl_RGB_Changed();
}
void tqcColorDialog::spinCtrl_RGB_Changed()
{
	m_spinCtrl_RGB_R->SetSelection(-1,-1);
	m_spinCtrl_RGB_G->SetSelection(-1,-1);
	m_spinCtrl_RGB_B->SetSelection(-1,-1);
	if (!m_bDisableNotify)
	{
		set_color(m_spinCtrl_RGB_R->GetValue() / 255.0, 
				  m_spinCtrl_RGB_G->GetValue() / 255.0, 
				  m_spinCtrl_RGB_B->GetValue() / 255.0);

		m_bDisableNotify = true;
		update_hsv();
		update_panel_NewColor();
		update_color_box();
		update_color_slider();
		m_bDisableNotify = false;
	}
}
void tqcColorDialog::radioHSV_H_Changed( wxCommandEvent& i_Event )
{
	this->m_pColorBox->SetColorDimension(tqcColorCtrl::e_HSV_H);
	this->m_pColorSlider->SetColorDimension(tqcColorCtrl::e_HSV_H);
}
void tqcColorDialog::radioHSV_S_Changed( wxCommandEvent& i_Event )
{
	this->m_pColorBox->SetColorDimension(tqcColorCtrl::e_HSV_S);
	this->m_pColorSlider->SetColorDimension(tqcColorCtrl::e_HSV_S);
}
void tqcColorDialog::radioHSV_V_Changed( wxCommandEvent& i_Event )
{
	this->m_pColorBox->SetColorDimension(tqcColorCtrl::e_HSV_V);
	this->m_pColorSlider->SetColorDimension(tqcColorCtrl::e_HSV_V);
}
void tqcColorDialog::radioRGB_R_Changed( wxCommandEvent& i_Event )
{
	this->m_pColorBox->SetColorDimension(tqcColorCtrl::e_RGB_R);
	this->m_pColorSlider->SetColorDimension(tqcColorCtrl::e_RGB_R);
}
void tqcColorDialog::radioRGB_G_Changed( wxCommandEvent& i_Event )
{
	this->m_pColorBox->SetColorDimension(tqcColorCtrl::e_RGB_G);
	this->m_pColorSlider->SetColorDimension(tqcColorCtrl::e_RGB_G);
}
void tqcColorDialog::radioRGB_B_Changed( wxCommandEvent& i_Event )
{
	this->m_pColorBox->SetColorDimension(tqcColorCtrl::e_RGB_B);
	this->m_pColorSlider->SetColorDimension(tqcColorCtrl::e_RGB_B);
}

void tqcColorDialog::checkBox_Continuous_Changed( wxCommandEvent& i_Event )
{
	sm_bContinuousUpdate = this->m_checkBox_Continuous->GetValue();

	// If just turning it on, go ahead and notify now
	// as if Apply was pressed.
	if (sm_bContinuousUpdate)
	{
		do_notify();
	}
}
	
//--------------------------------------------------------------------
// set color value using 3 values from 0-255, and trigger callback
//--------------------------------------------------------------------
void tqcColorDialog::set_color(double i_Red, double i_Green, double i_Blue)
{
	maFloatRGBA new_value = m_Value; // keep alpha value consistent
	new_value.SetRed( i_Red );
	new_value.SetGreen( i_Green );
	new_value.SetBlue( i_Blue );
	set_color(new_value);
}
void tqcColorDialog::set_color(const maFloatRGBA& i_NewColor)
{
	if (!(i_NewColor == m_Value))
	{
		m_Value = i_NewColor;
		
		if (sm_bContinuousUpdate)
		{
			// trigger the callback
			do_notify();
		}
	}
}
void tqcColorDialog::do_notify()
{
	// trigger the callback
	if (m_Callback)
		(m_Callback)();

	// Can't seem to use wxEVT_VALUE_CHANGED with a dialog 
	// (wxDialog is not a wxControl)
	//wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	//this->ProcessCommand(changed_event);
}

//--------------------------------------------------------------------
// Update background color of new color panel
//--------------------------------------------------------------------
void tqcColorDialog::update_panel_NewColor()
{
	int red = 255 * m_Value.GetRed();
	int green = 255 * m_Value.GetGreen();
	int blue = 255 * m_Value.GetBlue();
	wxColour bg_color(red, green, blue);
	m_panel_NewColor->SetBackgroundColour(bg_color);
	m_panel_NewColor->Refresh();
}

//--------------------------------------------------------------------
// Update spin controls for RGB
//--------------------------------------------------------------------
void tqcColorDialog::update_rgb()
{
	int red = 255 * m_Value.GetRed();
	int green = 255 * m_Value.GetGreen();
	int blue = 255 * m_Value.GetBlue();
	m_spinCtrl_RGB_R->SetValue(red); 
	m_spinCtrl_RGB_G->SetValue(green);
	m_spinCtrl_RGB_B->SetValue(blue);
}
//--------------------------------------------------------------------
// Update spin controls for HSV
//--------------------------------------------------------------------
void tqcColorDialog::update_hsv()
{
	double hue, saturation, value;
	tqcColorUtil::RGB_to_HSV(m_Value.GetRed(), m_Value.GetGreen(), m_Value.GetBlue(),
			   hue, saturation, value);

	update_hsv(hue, saturation, value);
}
void tqcColorDialog::update_hsv(double i_Hsv_H, double i_Hsv_S, double i_Hsv_V)
{
	// Convert from double to our ranges:
	m_spinCtrl_HSV_H->SetValue((int)(i_Hsv_H * 360)); 
	m_spinCtrl_HSV_S->SetValue((int)(i_Hsv_S * 100));
	m_spinCtrl_HSV_V->SetValue((int)(i_Hsv_V * 100));
}
//--------------------------------------------------------------------
// Update custom color controls
//--------------------------------------------------------------------
void tqcColorDialog::update_color_box()
{
	m_pColorBox->SetColor(m_Value); 
}
void tqcColorDialog::update_color_slider()
{
	m_pColorSlider->SetColor(m_Value); 
}

#endif // USE_QT
