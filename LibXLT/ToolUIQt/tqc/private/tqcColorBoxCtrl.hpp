/****************************************************************************\
**	tqcColorBoxCtrl.hpp
**
**		Control for choosing a color from a 2D box whose dimensions
**	represent some axes from RGB or HSV space.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORBOXCTRL_HPP
#error tqcColorBoxCtrl.hpp multiply included
#endif
#define TQC_COLORBOXCTRL_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 

#ifdef QT_FINISH_PORT

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed. 
//============================================================================

//============================================================================
// Base class color control: has color value, value changed callback,
//	enumeration for dimension of color space to display.
//============================================================================
class tqcColorCtrl : public wxControl
{
	public:
		enum ColorDimension
		{
			e_HSV_H,
			e_HSV_S,
			e_HSV_V,
			e_RGB_R,
			e_RGB_G,
			e_RGB_B,
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorCtrl(QWidget* i_pParent, 
					 const maFloatRGBA &i_Color,
				     QWidgetID i_Id = wxID_ANY,
				     const wxPoint& pos = wxDefaultPosition, 
				     const wxSize& size = wxDefaultSize);

		//--------------------------------------------------------------------
		//	Color
		//--------------------------------------------------------------------
		const maFloatRGBA& GetColor() const;
		void SetColor(const maFloatRGBA& i_Color);

		//--------------------------------------------------------------------
		//	HSV variation of color
		//--------------------------------------------------------------------
		void GetHSV(double &o_Hsv_H, double &o_Hsv_S, double &o_Hsv_V) const;
		void SetColor(const maFloatRGBA& i_Color,
					  double i_Hsv_H, double i_Hsv_S, double i_Hsv_V);

		//--------------------------------------------------------------------
		//	ColorDimension
		//--------------------------------------------------------------------
		ColorDimension GetColorDimension() const;
		void SetColorDimension(ColorDimension i_ColorDimension);

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void set_color(const maFloatRGBA& i_Color, bool i_bUpdateHSV);

		maFloatRGBA m_Color;
		double m_Hue, m_Saturation, m_Value;
		ColorDimension m_ColorDimension;

};

//============================================================================
//============================================================================
class tqcColorBoxCtrl : public tqcColorCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorBoxCtrl(QWidget* i_pParent, 
					    const maFloatRGBA &i_Color,
						QWidgetID i_Id = wxID_ANY,
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &WXUNUSED(event));
		void OnMouseDown(wxMouseEvent &i_Event);
		void OnMouseUp(wxMouseEvent &i_Event);
		void OnMouseMove(wxMouseEvent &i_Event);
		void OnCaptureLost(wxMouseCaptureChangedEvent &i_Event);

    DECLARE_EVENT_TABLE()
};

//============================================================================
//============================================================================
class tqcColorSliderCtrl : public tqcColorCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorSliderCtrl(QWidget* i_pParent, 
					       const maFloatRGBA &i_Color,
						   QWidgetID i_Id = wxID_ANY,
						   const wxPoint& pos = wxDefaultPosition, 
						   const wxSize& size = wxDefaultSize);


	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &WXUNUSED(event));
		void OnMouseDown(wxMouseEvent &i_Event);
		void OnMouseUp(wxMouseEvent &i_Event);
		void OnMouseMove(wxMouseEvent &i_Event);

    DECLARE_EVENT_TABLE()
};

#endif // USE_QT
