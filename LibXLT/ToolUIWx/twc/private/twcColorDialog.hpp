/*****************************************************************************
**	twcColorDialog.hpp
**
**	Dialog for choosing a color modelessly
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_COLORDIALOG_HPP
#error twcColorDialog.hpp multiply included
#endif
#define TWC_COLORDIALOG_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <functional>

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "ToolUIWx/twc/private/twcColorDialogBase.h"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class twcColorBoxCtrl;
class twcColorSliderCtrl;

//----------------------------------------------------------------------------
// Class twcColorDialog
//----------------------------------------------------------------------------
class twcColorDialog : public twcColorDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static twcColorDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcColorDialog( wxWindow* parent, const maFloatRGBA& i_Value );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~twcColorDialog();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maFloatRGBA& GetValue() const;
		void SetValue(const maFloatRGBA& i_Value);

		//--------------------------------------------------------------------
		// Can't seem to use wxEVT_VALUE_CHANGED with a dialog 
		// (wxDialog is not a wxControl), so use boost functions
		//	to implement the callback.
		//--------------------------------------------------------------------
		typedef std::function<void()> ColorChangedFunction;
		void SetColorChangedCallback(const ColorChangedFunction& i_FuncPtr);

	private:
		//--------------------------------------------------------------------
		// Virtual event handlers
		//--------------------------------------------------------------------
		virtual void OnClose( wxCloseEvent& i_Event );
		virtual void panelOld_MouseDown( wxMouseEvent& i_Event );
		virtual void button_Cancel_Click( wxCommandEvent& i_Event );
		virtual void button_Apply_Click( wxCommandEvent& i_Event );
		virtual void button_Ok_Click( wxCommandEvent& i_Event );
		virtual void ColorBox_Changed( wxCommandEvent& i_Event );
		virtual void ColorSlider_Changed( wxCommandEvent& i_Event );
		virtual void spinCtrl_HSV_Char( wxKeyEvent& i_Event );
		virtual void spinCtrl_HSV_Focus( wxFocusEvent& i_Event );
		virtual void spinCtrl_HSV_Changed( wxSpinEvent& i_Event );
		virtual void spinCtrl_HSV_Changed();
		virtual void spinCtrl_RGB_Char( wxKeyEvent& i_Event );
		virtual void spinCtrl_RGB_Focus( wxFocusEvent& i_Event );
		virtual void spinCtrl_RGB_Changed( wxSpinEvent& i_Event );
		virtual void spinCtrl_RGB_Changed();
		virtual void radioHSV_H_Changed( wxCommandEvent& i_Event );
		virtual void radioHSV_S_Changed( wxCommandEvent& i_Event );
		virtual void radioHSV_V_Changed( wxCommandEvent& i_Event );
		virtual void radioRGB_R_Changed( wxCommandEvent& i_Event );
		virtual void radioRGB_G_Changed( wxCommandEvent& i_Event );
		virtual void radioRGB_B_Changed( wxCommandEvent& i_Event );
		virtual void checkBox_Continuous_Changed( wxCommandEvent& i_Event );

		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		void set_color(double i_Red, double i_Green, double i_Blue);
		void set_color(const maFloatRGBA& i_NewColor);
		void do_notify();
		void update_panel_NewColor();
		void update_rgb();
		void update_hsv();
		void update_hsv(double i_Hsv_H, double i_Hsv_S, double i_Hsv_V);
		void update_color_box();
		void update_color_slider();

		static bool sm_bContinuousUpdate;

		maFloatRGBA m_Value;
		maFloatRGBA m_OriginalValue;
		bool m_bDisableNotify;
		ColorChangedFunction m_Callback;

		// Custom color controls
		twcColorBoxCtrl *m_pColorBox;
		twcColorSliderCtrl *m_pColorSlider;
};

#endif // USE_WXWIDGETS
