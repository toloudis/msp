/****************************************************************************\
**	tqcColorPicker.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcColorPicker.hpp"
#include "ToolUIQt/tqc/private/tqcColorDialog.hpp"

#include "ToolUIQt/tqc/tqcEvent.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"


#include <boost/bind.hpp>

#ifdef QT_FINISH_PORT

BEGIN_EVENT_TABLE(tqcColorPicker, wxButton)
    EVT_BUTTON(wxID_ANY, tqcColorPicker::OnButtonClick)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcColorPicker::tqcColorPicker(QWidget* i_pParent, 
					   QWidgetID i_Id,
					   const wxPoint& pos, 
					   const wxSize& size)
:	wxButton(i_pParent, i_Id, wxEmptyString, pos, size),
	m_pColorDialog(NULL)
{		
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcColorPicker::~tqcColorPicker()
{
	// if our color dialog is still up, close it
	if (m_pColorDialog && (m_pColorDialog == tqcColorDialog::Instance))
	{
		tqcColorDialog::Instance->Close();
	}

}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maFloatRGBA& tqcColorPicker::GetValue() const
{
	return m_Value;
}
void tqcColorPicker::SetValue(const maFloatRGBA& i_Value)
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
void tqcColorPicker::OnButtonClick(wxCommandEvent& i_Event)
{
	// If we already have a color dialog, then don't do anything
	if (m_pColorDialog && (m_pColorDialog == tqcColorDialog::Instance))
		return;

	// if a color picker is already up, close it
	if (tqcColorDialog::Instance)
	{
		tqcColorDialog::Instance->Close();
	}

	//DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
	m_pColorDialog = new tqcColorDialog(tqtSystem::g_pMainForm, m_Value);
	tqcColorDialog::Instance = m_pColorDialog;
	//this->Connect( m_pColorDialog->GetId(), wxEVT_VALUE_CHANGED,
	//				wxCommandEventHandler(tqcColorPicker::OnColorDialogChange) );
	m_pColorDialog->SetColorChangedCallback(boost::bind(
							&tqcColorPicker::OnColorDialogChange, this));
	m_pColorDialog->Show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcColorPicker::OnColorDialogChange()
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

#endif // USE_QT
