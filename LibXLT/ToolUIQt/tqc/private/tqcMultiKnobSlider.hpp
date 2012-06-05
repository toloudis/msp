/****************************************************************************\
**	tqcMultiKnobSlider.hpp
**
**		Custom control with a slider that contains multiple knobs
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_MULTIKNOBSLIDER_HPP
#error tqcMultiKnobSlider.hpp multiply included
#endif
#define TQC_MULTIKNOBSLIDER_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <vector>
#include <wx/panel.h>
#include <wx/sizer.h>

#ifdef QT_FINISH_PORT

class tqcColorDialog;

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere, along with the tqcKnob object 
//	itself in the client data. Users should register for this event.
//============================================================================

//============================================================================
//============================================================================
class tqcKnob : public wxButton
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
public:
    tqcKnob(QWidget* i_parent, const float i_percentage, const wxSize& i_size, 
		const wxString& i_label, int i_parentWidth, int i_parentHeight, maFloatRGBA& i_color);
    
	//--------------------------------------------------------------------
	//  Get the position of the knob, value from 0~1
	//--------------------------------------------------------------------
	float GetPos();

	//--------------------------------------------------------------------
	//  Set the position of the knob, value from 0~1
	//--------------------------------------------------------------------
    void SetPos(float i_percent);
	
	//--------------------------------------------------------------------
	//  Set the position of the knob in pixel
	//--------------------------------------------------------------------
	void SetPos(int i_x);

	//--------------------------------------------------------------------
	//  Set the color of the knob using maFloatRGBA
	//--------------------------------------------------------------------
	void SetColor(maFloatRGBA& i_color);

	//--------------------------------------------------------------------
	//  Set the color of the knob in wxColour
	//--------------------------------------------------------------------
	void SetColor(wxColour& i_color);

	//--------------------------------------------------------------------
	//  Get the color of the knob
	//--------------------------------------------------------------------
	maFloatRGBA& GetColor();

private:
	//--------------------------------------------------------------------
	//  Callback: Color change from tqcColorDialog
	//--------------------------------------------------------------------
	void OnColorChanged();
	
	//--------------------------------------------------------------------
	//  Callback: position change
	//--------------------------------------------------------------------
	void OnMouseOver(wxMouseEvent& i_event);

	//--------------------------------------------------------------------
	//  Callback: right click on knob
	//--------------------------------------------------------------------
	void OnMouseRightClick(wxMouseEvent& i_event);

	//--------------------------------------------------------------------
	//  Send out value change event for this control
	//--------------------------------------------------------------------
	void SendValueChangeEvent();

    int m_width;
    int m_height;
    int m_parentWidth;
	int m_parentHeight;
    int m_curPos;
	float m_curPercentage;

	maFloatRGBA m_color;
	tqcColorDialog* m_pColorDialog;
	clock_t m_lastEventTime;

    DECLARE_EVENT_TABLE()
}; 

/////////////////////////////////////
class tqcSliderBackground : public wxPanel
{
public:
	//--------------------------------------------------------------------
	// Background of the slider
	//--------------------------------------------------------------------
    tqcSliderBackground(QWidget* i_parent, const wxPoint& i_pos, const wxSize& i_size);

    int m_width;
	int m_height;

};

///////////////////////////////////
class tqcMultiKnobSlider : public wxPanel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
    tqcMultiKnobSlider(QWidget* i_parent, const wxPoint& i_pos, 
		const wxSize& i_backgroundSize, const wxSize& i_knobSize);

	//--------------------------------------------------------------------
	// Create a new knob on the slider
	//--------------------------------------------------------------------
    tqcKnob* AddKnob(float i_percentage, maFloatRGBA& i_color);
	tqcKnob* AddKnob(int i_position, maFloatRGBA& i_color);

	//--------------------------------------------------------------------
	//	Value
	//--------------------------------------------------------------------
	void SetValue(const maGradient& i_value);

	//--------------------------------------------------------------------
	// Set current knob according to i_index
	//--------------------------------------------------------------------
	void SetCurrentKnobByIndex(int i_index);

	//--------------------------------------------------------------------
	// Set current knob to i_knob
	//--------------------------------------------------------------------
	void SetCurrentKnob(tqcKnob* i_knob);

	//--------------------------------------------------------------------
	// Delete current knob
	//--------------------------------------------------------------------
	void DeleteCurrentKnob();

	//--------------------------------------------------------------------
	// Get the number of knobs on the slider
	//--------------------------------------------------------------------
	int GetKnobsNum();

	//--------------------------------------------------------------------
	// Get knob list
	//--------------------------------------------------------------------
	void GetKnobList(std::vector<tqcKnob*>& o_knobList);

	//--------------------------------------------------------------------
	// Force to sort knob list
	//--------------------------------------------------------------------
	void SortKnobList();

	//--------------------------------------------------------------------
	// Set current knob position in percentage
	//--------------------------------------------------------------------
	void SetCurrentKnobPos(float i_percentage);

	//--------------------------------------------------------------------
	// Set current knob color
	//--------------------------------------------------------------------
	void SetCurrentKnobColor(maFloatRGBA& i_color);

	//--------------------------------------------------------------------
	// Get current knob position in percentage
	//--------------------------------------------------------------------
	float GetCurrentKnobPos();

	//--------------------------------------------------------------------
	// Get current knob color
	//--------------------------------------------------------------------
	void GetCurrentKnobColor(maFloatRGBA& o_color);


private:
	//--------------------------------------------------------------------
	//  Gather knob value change event and send one to parent
	//--------------------------------------------------------------------
	void OnValueChange(wxCommandEvent& i_Event);

	//--------------------------------------------------------------------
	//  Send out value change event for this control
	//--------------------------------------------------------------------
	void SendValueChangeEvent(tqcKnob* i_knob);

	//--------------------------------------------------------------------
	//  Convert position on slider to percentage
	//--------------------------------------------------------------------
	float PositionToPercentage(int i_pos);

	wxSize m_backgroundSize;
	wxSize m_knobSize;
    tqcSliderBackground* m_sliderBackground; //pointer to sliderBackground within panel

	std::vector<tqcKnob*> m_knobList;
	tqcKnob* m_currentKnob;
}; 

#endif // USE_QT
