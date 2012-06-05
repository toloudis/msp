/****************************************************************************\
**	twcColorRGBAEdit.hpp
**
**		Custom control with slider and flaot edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_COLORRGBAEDIT_HPP
#error twcColorRGBAEdit.hpp multiply included
#endif
#define TWC_COLORRGBAEDIT_HPP

#ifndef TWC_FLOATEDIT_HPP
#include "ToolUIWx/twc/twcFloatEdit.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef TWC_COLORPICKER_HPP
#include "ToolUIWx/twc/twcColorPicker.hpp"
#endif 

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class twcColorRGBAEdit : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcColorRGBAEdit(wxWindow* i_pParent);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maFloatRGBA& GetValue() const;
		void SetValue(const maFloatRGBA& i_Value);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnColorChange(wxCommandEvent& i_Event);

		twcFloatEdit* m_pFloatEditR;
		twcFloatEdit* m_pFloatEditG;
		twcFloatEdit* m_pFloatEditB;
		twcFloatEdit* m_pFloatEditA;
		twcColorPicker* m_pColorPicker;
		
		maFloatRGBA m_Value;
};

#endif // USE_WXWIDGETS
