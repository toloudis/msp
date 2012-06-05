/****************************************************************************\
**	tqcGradientColorEdit.hpp
**
**		Custom control with GradientColorBase and color property window
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_GRADIENTCOLOREDIT_HPP
#error tqcGradientColorEdit.hpp multiply included
#endif
#define TQC_GRADIENTCOLOREDIT_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef TQC_GRADIENTCOLORBASE_HPP
#include "ToolUIQt/tqc/private/tqcGradientColorBase.hpp"
#endif
#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <wx/choice.h>

#ifdef QT_FINISH_PORT

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere.
//============================================================================
class tqcGradientColorEdit : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcGradientColorEdit(QWidget* i_pParent);
		virtual ~tqcGradientColorEdit();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maGradient& GetValue() const;
		void SetValue(const maGradient& i_Value);

	private:
		//--------------------------------------------------------------------
		// Callback when value change		
		//--------------------------------------------------------------------
		void OnValueChange(wxCommandEvent& i_Event);

		//wxChoice* m_choice_shape;
		/*wxChoice* m_choice_interpolation;
		tqcPictureBox* m_box_previewTexture;*/
		wxControl* m_control;	// this control is used to send the value changed event
		tqcGradientColorBase* m_gradient_gradientEditor;
};

#endif // USE_QT
