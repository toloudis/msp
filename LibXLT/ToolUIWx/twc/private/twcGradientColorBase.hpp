/****************************************************************************\
**	twcGradientColorBase.hpp
**
**		Custom control with gradient image and multiknobslider
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_GRADIENTCOLORBASE_HPP
#error twcGradientColorBase.hpp multiply included
#endif
#define TWC_GRADIENTCOLORBASE_HPP


#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif

#ifndef TWC_MULTIKNOBSLIDER_HPP
#include "ToolUIWx/twc/private/twcMultiKnobSlider.hpp"
#endif

#ifndef TWC_PICTUREBOX_HPP
#include "ToolUIWx/twc/private/twcPictureBox.hpp"
#endif

#ifndef TWC_COLORRGBEDIT_HPP
#include "ToolUIWx/twc/twcColorRGBAEdit.hpp"
#endif

#ifndef TWC_RANGEDFLOAT_HPP
#include "ToolUIWx/twc/twcRangedFloat.hpp"
#endif

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <vector>
#include <wx/choice.h>

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
class twcGradientColorBase : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcGradientColorBase(wxWindow* i_pParent);
		virtual ~twcGradientColorBase();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maGradient& GetValue() const;
		void SetValue(const maGradient& i_value);

	private:
		//--------------------------------------------------------------------
		// callback when adds knob
		//--------------------------------------------------------------------
		void OnAddKnob(wxMouseEvent& i_Event);

		//--------------------------------------------------------------------
		// callback any property of knob changes
		//--------------------------------------------------------------------
		void KnobValueChange(wxCommandEvent& i_Event);

		//--------------------------------------------------------------------
		// modify m_value according to the knob position/color
		//--------------------------------------------------------------------
		void WriteIntoValue();

		//--------------------------------------------------------------------
		// callback when position slider changes in knob property window
		//--------------------------------------------------------------------
		void OnPositionSliderChange(wxCommandEvent& i_Event);

		//--------------------------------------------------------------------
		// callback when deletes button press in knob property window
		//--------------------------------------------------------------------
		void OnDeleteKnobClick(wxMouseEvent& i_Event);

		//--------------------------------------------------------------------
		// callback function when paints gradient panel
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent& i_Event);

		//--------------------------------------------------------------------
		// force to paint gradient panel
		//--------------------------------------------------------------------
		void PaintNow();

		//--------------------------------------------------------------------
		// paint gradient panel
		//--------------------------------------------------------------------
		void PaintGradient(wxDC& dc);
		
		//--------------------------------------------------------------------
		// Initialize
		//--------------------------------------------------------------------
		void Initialize();

		//--------------------------------------------------------------------
		// Initialize knobs
		//--------------------------------------------------------------------
		void InitializeKnobs(twcMultiKnobSlider* i_slider);

		//--------------------------------------------------------------------
		// Initialize knobs
		//--------------------------------------------------------------------
		void ShowKnobProperty(twcKnob* knob);

		//--------------------------------------------------------------------
		// Get drawable rect of gradient panel
		//--------------------------------------------------------------------
		wxRect GetGradientPanelDrawable();

		//--------------------------------------------------------------------
		// Draw knob indicator
		//--------------------------------------------------------------------
		void DrawKnobIndicator(twcKnob* knob);


		//--------------------------------------------------------------------
		// Custom event handler for erase background event
		// Note: this function does nothing. It is used to achieve 
		// double buffering when painting gradient panel
		//--------------------------------------------------------------------
		void OnEraseBackground(wxPaintEvent& i_event);


		wxPanel* m_panel_gradientPanel;
		wxStaticText* m_text_colorPosition;
		//wxSlider* m_slider_colorPosition;
		twcRangedFloat* m_colorPosition;
		twcColorRGBAEdit* m_knobColorEdit;
		wxButton* m_button_colorDelete;
		twcMultiKnobSlider* m_slider_colorSlider;

		maGradient m_value;
		//std::vector<twcKnob*> m_colorKnobs;
		//twcKnob* m_CurrentKnob;

		// Constant panel properties
		const int m_knobSize;
		const int m_gradientPanelWidth;
		const int m_gradientPanelHeight;
		const int m_gradientPanelHeightOffset;

		// variables for double buffering
		wxBitmap m_buffer;

};

#endif // USE_WXWIDGETS
