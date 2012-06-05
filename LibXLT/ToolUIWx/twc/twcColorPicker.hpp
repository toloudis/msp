/****************************************************************************\
**	twcColorPicker.hpp
**
**		Button that display a color value and opens a modeless color 
**	dialog when pressed.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_COLORPICKER_HPP
#error twcColorPicker.hpp multiply included
#endif
#define TWC_COLORPICKER_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//#ifndef TWC_EVENT_HPP
//#include "ToolUIWx/twc/twcEvent.hpp"
//#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class twcColorDialog;

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed. Users should register for this event, 
//	not for the button press event.
//============================================================================

//============================================================================
//============================================================================
class twcColorPicker : public wxButton
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcColorPicker(wxWindow* i_pParent, 
				   wxWindowID i_Id = wxID_ANY,
				   const wxPoint& pos = wxDefaultPosition, 
				   const wxSize& size = wxDefaultSize);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		~twcColorPicker();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maFloatRGBA& GetValue() const;
		void SetValue(const maFloatRGBA& i_Value);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnButtonClick(wxCommandEvent& i_Event);
		void OnColorDialogChange();

		maFloatRGBA m_Value;
		twcColorDialog* m_pColorDialog;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
