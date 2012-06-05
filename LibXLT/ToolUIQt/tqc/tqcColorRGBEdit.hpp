/****************************************************************************\
**	tqcColorRGBEdit.hpp
**
**		Custom control with slider and flaot edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORRGBEDIT_HPP
#error tqcColorRGBEdit.hpp multiply included
#endif
#define TQC_COLORRGBEDIT_HPP

#ifdef QT_FINISH_PORT

#ifndef TQC_FLOATEDIT_HPP
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef TQC_COLORPICKER_HPP
#include "ToolUIQt/tqc/tqcColorPicker.hpp"
#endif 

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcColorRGBEdit : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorRGBEdit(QWidget* i_pParent);

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

		tqcFloatEdit* m_pFloatEditR;
		tqcFloatEdit* m_pFloatEditG;
		tqcFloatEdit* m_pFloatEditB;
		tqcColorPicker* m_pColorPicker;
		
		maFloatRGBA m_Value;
};

#endif // USE_QT
