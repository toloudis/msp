/****************************************************************************\
**	twcGradientColorEdit.hpp
**
**		Custom control with GradientColorBase and color property window
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_GRADIENTCOLOREDIT_HPP
#error twcGradientColorEdit.hpp multiply included
#endif
#define TWC_GRADIENTCOLOREDIT_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef TWC_GRADIENTCOLORBASE_HPP
#include "ToolUIWx/twc/private/twcGradientColorBase.hpp"
#endif

#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif

//#ifndef TWC_PICTUREBOX_HPP
//#include "ToolUIWx/twc/private/twcPictureBox.hpp"
//#endif

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <wx/choice.h>

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere.
//============================================================================
class twcGradientColorEdit : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcGradientColorEdit(wxWindow* i_pParent);
		virtual ~twcGradientColorEdit();

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
		twcPictureBox* m_box_previewTexture;*/
		wxControl* m_control;	// this control is used to send the value changed event
		twcGradientColorBase* m_gradient_gradientEditor;
};

#endif // USE_WXWIDGETS
