/****************************************************************************\
**	tqcGradientColorBase.hpp
**
**		Gradient editor: a preview of the gradient, a slider with a knob
**	for each color node, and controls for the selected node's color and
**	position. Clicking the preview adds a node there.
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

#ifndef TQC_COLORRGBAEDIT_HPP
#include "ToolUIQt/tqc/tqcColorRGBAEdit.hpp"
#endif

#ifndef TQC_RANGEDFLOAT_HPP
#include "ToolUIQt/tqc/tqcRangedFloat.hpp"
#endif

#ifdef USE_QT

class tqcGradientPreview;

//============================================================================
//============================================================================
class tqcGradientColorBase : public QWidget, public tqcValueChangedNotifier
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
		friend class tqcGradientPreview;

		// callback when the preview is clicked, to add a knob
		void OnAddKnob(int i_X);
		// callback any property of a knob changes
		void KnobValueChange(tqcKnob* i_knob);
		// callback the color edit changes
		void ColorEditChange();
		// modify m_value according to the knob position/color
		void WriteIntoValue();
		// callback when position slider changes in knob property window
		void OnPositionSliderChange();
		// callback when delete button pressed in knob property window
		void OnDeleteKnobClick();
		// paint gradient panel
		void PaintGradient(QPainter& i_Painter);
		// show the properties of the knob
		void ShowKnobProperty(tqcKnob* knob);
		// Get drawable rect of gradient panel
		QRect GetGradientPanelDrawable();

		tqcGradientPreview* m_panel_gradientPanel;
		tqcRangedFloat* m_colorPosition;
		tqcColorRGBAEdit* m_knobColorEdit;
		QPushButton* m_button_colorDelete;
		tqcMultiKnobSlider* m_slider_colorSlider;

		maGradient m_value;
		bool m_bSettingValue;

		// Constant panel properties
		const int m_knobSize;
		const int m_gradientPanelWidth;
		const int m_gradientPanelHeight;
		const int m_gradientPanelHeightOffset;
};

#endif // USE_QT
