/****************************************************************************\
**	twcColorBoxCtrl.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/private/twcColorBoxCtrl.hpp"
#include "ToolUIWx/twc/private/twcColorUtil.hpp"

#include "Core/Ma/maFunctions.hpp"

#ifdef USE_WXWIDGETS

#include <wx/dcbuffer.h>

namespace
{
	const int c_SliderEdgeSpace = 8;
	const int c_MarkerRadius = 3;

	void get_HSV(const maFloatRGBA& i_Color,
				 double &o_Hue, 
				 double &o_Saturation, 
				 double &o_Value)
	{	 
		twcColorUtil::RGB_to_HSV(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(),
								 o_Hue, o_Saturation, o_Value);
	}
	void set_HSV(maFloatRGBA& o_Color,
				 double i_Hue, 
				 double i_Saturation, 
				 double i_Value)
	{	 
		double r,g,b;
		twcColorUtil::HSV_to_RGB(i_Hue, i_Saturation, i_Value, r, g, b);
		o_Color.SetRed(r);
		o_Color.SetGreen(g);
		o_Color.SetBlue(b);
	}
	void set_HSV(wxColour& o_Color,
				 double i_Hue, 
				 double i_Saturation, 
				 double i_Value)
	{	 
		double r,g,b;
		twcColorUtil::HSV_to_RGB(i_Hue, i_Saturation, i_Value, r, g, b);
		o_Color.Set(r * 255, g * 255, b * 255);
	}
}

//============================================================================
// twcColorCtrl
//============================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorCtrl::twcColorCtrl(wxWindow* i_pParent, 
						   const maFloatRGBA &i_Color,
						   wxWindowID i_Id,
						   const wxPoint& pos, 
						   const wxSize& size)
:	wxControl(i_pParent, i_Id, pos, size),
	m_Color(i_Color),
	m_ColorDimension(e_HSV_H)
{		
	// Store hue, saturation and value in order to
	// be able to modify just certain dimensions without
	// having to compute them again
	get_HSV(m_Color, m_Hue, m_Saturation, m_Value);
}

//--------------------------------------------------------------------
//	Color
//--------------------------------------------------------------------
const maFloatRGBA& twcColorCtrl::GetColor() const
{
	return m_Color;
}
void twcColorCtrl::SetColor(const maFloatRGBA& i_Color)
{
	const bool update_HSV = true;
	set_color(i_Color, update_HSV);
}

//--------------------------------------------------------------------
//	HSV variation of color
//--------------------------------------------------------------------
void twcColorCtrl::GetHSV(double &o_Hsv_H, double &o_Hsv_S, double &o_Hsv_V) const
{
	o_Hsv_H = m_Hue; 
	o_Hsv_S = m_Saturation;
	o_Hsv_V = m_Value;
}
void twcColorCtrl::SetColor(const maFloatRGBA& i_Color,
							double i_Hsv_H, double i_Hsv_S, double i_Hsv_V)
{
	m_Hue = i_Hsv_H; 
	m_Saturation = i_Hsv_S;
	m_Value = i_Hsv_V;

	const bool update_HSV = false;
	set_color(i_Color, update_HSV);
}

//--------------------------------------------------------------------
//	ColorDimension
//--------------------------------------------------------------------
twcColorCtrl::ColorDimension twcColorCtrl::GetColorDimension() const
{
	return m_ColorDimension;
}
void twcColorCtrl::SetColorDimension(ColorDimension i_ColorDimension)
{
	m_ColorDimension = i_ColorDimension;
	this->Refresh();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcColorCtrl::set_color(const maFloatRGBA& i_Color, bool i_bUpdateHSV)
{
	m_Color = i_Color;

	if (i_bUpdateHSV)
	{
		// Store hue, saturation and value in order to
		// be able to modify just certain dimensions without
		// having to compute them again
		get_HSV(m_Color, m_Hue, m_Saturation, m_Value);
	}

	this->Refresh();
}


//============================================================================
// twcColorBoxCtrl
//============================================================================

BEGIN_EVENT_TABLE(twcColorBoxCtrl, twcColorCtrl)
    EVT_PAINT(twcColorBoxCtrl::OnPaint)
	EVT_LEFT_DOWN(twcColorBoxCtrl::OnMouseDown)
	EVT_LEFT_UP(twcColorBoxCtrl::OnMouseUp)
	EVT_MOTION(twcColorBoxCtrl::OnMouseMove)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorBoxCtrl::twcColorBoxCtrl(wxWindow* i_pParent, 
						   const maFloatRGBA &i_Color,
						   wxWindowID i_Id,
						   const wxPoint& pos, 
						   const wxSize& size)
:	twcColorCtrl(i_pParent, i_Color, i_Id, pos, size)
{		
	// Since we are drawing the background ourself, we can set the custom flag
	// to prevent flickering:
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorBoxCtrl::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    //wxPaintDC pdc(this);
	wxAutoBufferedPaintDC pdc(this);
    wxDC &dc = pdc;
    PrepareDC(dc);

	// Get current value as components
	int red = 255 * m_Color.GetRed();
	int green = 255 * m_Color.GetGreen();
	int blue = 255 * m_Color.GetBlue();

	// Fill box with gradient vertical lines
	wxSize size = this->GetClientSize(); 
	int width = size.GetWidth();
	int height = size.GetHeight();
	wxColour color1, color2;
	double pct;
	for (int i=0; i<width; i++)
	{
		pct = i / (double)(width-1);
		switch(m_ColorDimension)
		{
		default:
		case e_HSV_H:
			set_HSV(color1, m_Hue, pct, 1);
			set_HSV(color2, m_Hue, pct, 0);
			break;
		case e_HSV_S:
			set_HSV(color1, pct, m_Saturation, 1);
			set_HSV(color2, pct, m_Saturation, 0);
			break;
		case e_HSV_V:
			set_HSV(color1, pct, 1, m_Value);
			set_HSV(color2, pct, 0, m_Value);
			break;
		case e_RGB_R:
			color1.Set(red,255,(int)(255 * pct));
			color2.Set(red,0,(int)(255 * pct));
			break;
		case e_RGB_G:
			color1.Set(255, green, (int)(255 * pct));
			color2.Set(0, green, (int)(255 * pct));
			break;
		case e_RGB_B:
			color1.Set((int)(255 * pct),255, blue);
			color2.Set((int)(255 * pct),0, blue);
			break;
		}
		dc.GradientFillLinear(wxRect(i,0,1,height), color1, color2, wxSOUTH);
	}

	// Locate current color within the 2D box
	double x_pct, y_pct;
	switch(m_ColorDimension)
	{
	default:
	case e_HSV_H:
		x_pct = m_Saturation;
		y_pct = m_Value;
		break;
	case e_HSV_S:
		x_pct = m_Hue;
		y_pct = m_Value;
		break;
	case e_HSV_V:
		x_pct = m_Hue;
		y_pct = m_Saturation;
		break;
	case e_RGB_R:
		x_pct = m_Color.GetBlue();
		y_pct = m_Color.GetGreen();
		break;
	case e_RGB_G:
		x_pct = m_Color.GetBlue();
		y_pct = m_Color.GetRed();
		break;
	case e_RGB_B:
		x_pct = m_Color.GetRed();
		y_pct = m_Color.GetGreen();
		break;
	}

	int x_pos = (int)(x_pct * (width-1));
	int y_pos = (int)((1.0 - y_pct) * (height-1));

	wxColor pen_col = (m_Value > 0.5) ? *wxBLACK : *wxWHITE;
	dc.SetPen( wxPen(pen_col, 1, wxSOLID) );
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill
	dc.DrawCircle(x_pos, y_pos, c_MarkerRadius);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorBoxCtrl::OnMouseDown(wxMouseEvent &i_Event)
{
	this->CaptureMouse();
	this->OnMouseMove(i_Event);
	i_Event.Skip();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorBoxCtrl::OnMouseUp(wxMouseEvent &i_Event)
{
	this->ReleaseMouse();
	i_Event.Skip();
}
//----------------------------------------------------------------------------
// Called when capture is lost because of reason besides mouse up
//----------------------------------------------------------------------------
void twcColorBoxCtrl::OnCaptureLost(wxMouseCaptureChangedEvent &i_Event)
{
	// nothing to do in this case, but handling event is required 
	// if you capture mouse events.
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorBoxCtrl::OnMouseMove(wxMouseEvent &i_Event)
{
	if (i_Event.LeftIsDown())
	{
		wxSize size = this->GetClientSize(); 
		double x_pct = i_Event.GetX() / (double)(size.GetWidth()-1);
		maFunctions::Clamp(x_pct, 0.0, 1.0);
		double y_pct = 1.0 - (i_Event.GetY() / (double)(size.GetHeight()-1));
		maFunctions::Clamp(y_pct, 0.0, 1.0);

		double hue, saturation, value;
		get_HSV(m_Color, hue, saturation, value);

		maFloatRGBA new_value = m_Color;
		bool bUpdateHSV = true;
		switch(m_ColorDimension)
		{
		default:
		case e_HSV_H:
			m_Saturation = x_pct;
			m_Value = y_pct;
			set_HSV(new_value, m_Hue, m_Saturation, m_Value);
			bUpdateHSV = false;
			break;
		case e_HSV_S:
			m_Hue = x_pct;
			m_Value = y_pct;
			set_HSV(new_value, m_Hue, m_Saturation, m_Value);
			bUpdateHSV = false;
			break;
		case e_HSV_V:
			m_Hue = x_pct;
			m_Saturation = y_pct;
			set_HSV(new_value, m_Hue, m_Saturation, m_Value);
			bUpdateHSV = false;
			break;
		case e_RGB_R:
			new_value.SetBlue(x_pct);
			new_value.SetGreen(y_pct);
			break;
		case e_RGB_G:
			new_value.SetBlue(x_pct);
			new_value.SetRed(y_pct);
			break;
		case e_RGB_B:
			new_value.SetRed(x_pct);
			new_value.SetGreen(y_pct);
			break;
		}

		// Always notify
		//if (!(new_value == m_Color))
		{
			this->set_color( new_value, bUpdateHSV );

			// trigger the callback
			wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
			this->ProcessCommand(changed_event);
		}
	}
}


//============================================================================
// twcColorSliderCtrl
//============================================================================

BEGIN_EVENT_TABLE(twcColorSliderCtrl, twcColorCtrl)
    EVT_PAINT(twcColorSliderCtrl::OnPaint)
	EVT_LEFT_DOWN(twcColorSliderCtrl::OnMouseDown)
	EVT_LEFT_UP(twcColorSliderCtrl::OnMouseUp)
	EVT_MOTION(twcColorSliderCtrl::OnMouseMove)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcColorSliderCtrl::twcColorSliderCtrl(wxWindow* i_pParent, 
						   const maFloatRGBA &i_Color,
						   wxWindowID i_Id,
						   const wxPoint& pos, 
						   const wxSize& size)
:	twcColorCtrl(i_pParent, i_Color, i_Id, pos, size)
{		
	// Since we are double-buffering, we can set the custom flag
	// to prevent flickering:
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorSliderCtrl::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    //wxPaintDC pdc(this);
	wxAutoBufferedPaintDC pdc(this);
    wxDC &dc = pdc ;
    PrepareDC(dc);
    dc.Clear();

	// Fill slider with horizontal lines of varying color
	wxSize size = this->GetClientSize(); 
	wxColour color;

	int red = 255 * m_Color.GetRed();
	int green = 255 * m_Color.GetGreen();
	int blue = 255 * m_Color.GetBlue();

	int height = size.GetHeight();
	double inv_height = 1.0 / (double)height;
	double pct;
	for (int i=0; i<height; i++)
	{
		pct = inv_height * (height - i - 1);
		switch(m_ColorDimension)
		{
		default:
		case e_HSV_H:
			// Hue should always be fully saturated
			set_HSV(color, pct, 1, 1);
			//set_HSV(color, pct, m_Saturation, m_Value);
			break;
		case e_HSV_S:
			set_HSV(color, m_Hue, pct, m_Value);
			break;
		case e_HSV_V:
			set_HSV(color, m_Hue, m_Saturation, pct);
			break;
		case e_RGB_R:
			color.Set((int)(255 * pct),green,blue);
			break;
		case e_RGB_G:
			color.Set(red,(int)(255 * pct),blue);
			break;
		case e_RGB_B:
			color.Set(red,green,(int)(255 * pct));
			break;
		}
		dc.SetPen( wxPen( color, 1, wxSOLID ) );
		dc.DrawLine(c_SliderEdgeSpace,i,size.GetWidth()-c_SliderEdgeSpace,i);
	}

	double marker_pct;
	switch(m_ColorDimension)
	{
	default:
	case e_HSV_H:
		marker_pct = m_Hue;
		break;
	case e_HSV_S:
		marker_pct = m_Saturation;
		break;
	case e_HSV_V:
		marker_pct = m_Value;
		break;
	case e_RGB_R:
		marker_pct = m_Color.GetRed();
		break;
	case e_RGB_G:
		marker_pct = m_Color.GetGreen();
		break;
	case e_RGB_B:
		marker_pct = m_Color.GetBlue();
		break;
	}
	
	int marker_pos = (int)((1.0 - marker_pct) * (height-1));
	dc.SetPen( wxPen(*wxBLACK, 1, wxSOLID) );
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill
	const int edge_space = 2;
	dc.DrawRectangle(edge_space, marker_pos-c_MarkerRadius, 
					 size.GetWidth()-2*edge_space, 2*c_MarkerRadius);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorSliderCtrl::OnMouseDown(wxMouseEvent &i_Event)
{
	this->CaptureMouse();
	this->OnMouseMove(i_Event);
	i_Event.Skip();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorSliderCtrl::OnMouseUp(wxMouseEvent &i_Event)
{
	this->ReleaseMouse();
	i_Event.Skip();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void twcColorSliderCtrl::OnMouseMove(wxMouseEvent &i_Event)
{
	if (i_Event.LeftIsDown())
	{
		wxSize size = this->GetClientSize(); 
		double marker_pct = 1.0 - (i_Event.GetY() / (double)(size.GetHeight()-1));
		maFunctions::Clamp(marker_pct, 0.0, 1.0);

		maFloatRGBA new_value = m_Color;
		bool bUpdateHSV = true;
		switch(m_ColorDimension)
		{
		default:
		case e_HSV_H:
			m_Hue = marker_pct;
			set_HSV(new_value, m_Hue, m_Saturation, m_Value);
			bUpdateHSV = false;
			break;
		case e_HSV_S:
			m_Saturation = marker_pct;
			set_HSV(new_value, m_Hue, m_Saturation, m_Value);
			bUpdateHSV = false;
			break;
		case e_HSV_V:
			m_Value = marker_pct;
			set_HSV(new_value, m_Hue, m_Saturation, m_Value);
			bUpdateHSV = false;
			break;
		case e_RGB_R:
			new_value.SetRed(marker_pct);
			break;
		case e_RGB_G:
			new_value.SetGreen(marker_pct);
			break;
		case e_RGB_B:
			new_value.SetBlue(marker_pct);
			break;
		}

		// Always notify
		//if (!(new_value == m_Color))
		{
			this->set_color( new_value, bUpdateHSV );

			// trigger the callback
			wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
			this->ProcessCommand(changed_event);
		}
	}
}

#endif // USE_WXWIDGETS
