/****************************************************************************\
**	tqcGradientColorBase.hpp
**
**		Custom control with gradient image and multiknobslider
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_GRADIENTCOLORBASE_HPP
#error tqcGradientColorBase.hpp multiply included
#endif
#define TQC_GRADIENTCOLORBASE_HPP


#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif
#ifndef TQC_MULTIKNOBSLIDER_HPP
#include "ToolUIQt/tqc/private/tqcMultiKnobSlider.hpp"
#endif
#ifndef TQC_PICTUREBOX_HPP
#include "ToolUIQt/tqc/private/tqcPictureBox.hpp"
#endif
#ifndef TQC_COLORRGBEDIT_HPP
#include "ToolUIQt/tqc/tqcColorRGBAEdit.hpp"
#endif
#ifndef TQC_RANGEDFLOAT_HPP
#include "ToolUIQt/tqc/tqcRangedFloat.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <vector>
#include <wx/choice.h>

#ifdef QT_FINISH_PORT


//============================================================================
//============================================================================
class tqcGradientColorBase : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcGradientColorBase(QWidget* i_pParent);
		virtual ~tqcGradientColorBase();

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
		void InitializeKnobs(tqcMultiKnobSlider* i_slider);

		//--------------------------------------------------------------------
		// Initialize knobs
		//--------------------------------------------------------------------
		void ShowKnobProperty(tqcKnob* knob);

		//--------------------------------------------------------------------
		// Get drawable rect of gradient panel
		//--------------------------------------------------------------------
		wxRect GetGradientPanelDrawable();

		//--------------------------------------------------------------------
		// Draw knob indicator
		//--------------------------------------------------------------------
		void DrawKnobIndicator(tqcKnob* knob);


		//--------------------------------------------------------------------
		// Custom event handler for erase background event
		// Note: this function does nothing. It is used to achieve 
		// double buffering when painting gradient panel
		//--------------------------------------------------------------------
		void OnEraseBackground(wxPaintEvent& i_event);


		wxPanel* m_panel_gradientPanel;
		wxStaticText* m_text_colorPosition;
		//wxSlider* m_slider_colorPosition;
		tqcRangedFloat* m_colorPosition;
		tqcColorRGBAEdit* m_knobColorEdit;
		wxButton* m_button_colorDelete;
		tqcMultiKnobSlider* m_slider_colorSlider;

		maGradient m_value;
		//std::vector<tqcKnob*> m_colorKnobs;
		//tqcKnob* m_CurrentKnob;

		// Constant panel properties
		const int m_knobSize;
		const int m_gradientPanelWidth;
		const int m_gradientPanelHeight;
		const int m_gradientPanelHeightOffset;

		// variables for double buffering
		wxBitmap m_buffer;

};

#endif // USE_QT
