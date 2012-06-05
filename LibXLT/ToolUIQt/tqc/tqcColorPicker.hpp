/****************************************************************************\
**	tqcColorPicker.hpp
**
**		Button that display a color value and opens a modeless color 
**	dialog when pressed.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORPICKER_HPP
#error tqcColorPicker.hpp multiply included
#endif
#define TQC_COLORPICKER_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 

#ifdef QT_FINISH_PORT

//============================================================================
//============================================================================
class tqcColorDialog;

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed. Users should register for this event, 
//	not for the button press event.
//============================================================================

//============================================================================
//============================================================================
class tqcColorPicker : public wxButton
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorPicker(QWidget* i_pParent, 
				   QWidgetID i_Id = wxID_ANY,
				   const wxPoint& pos = wxDefaultPosition, 
				   const wxSize& size = wxDefaultSize);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		~tqcColorPicker();

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
		tqcColorDialog* m_pColorDialog;

    DECLARE_EVENT_TABLE()
};

#endif // USE_QT
