#include "CColourPicker.h"
#include "resource.h"

EXPORT CColourPicker::CColourPicker(HWND hParentWindow, std::string title)
{
	m_pCallback = NULL;
	SetRGB(0, 0, 0);
	SetAlpha(100);
	OldCol = CurrCol;

	UseAlpha = CP_USE_ALPHA;

	m_hParent = hParentWindow;
	m_Title = title;
}
 
EXPORT CColourPicker::CColourPicker(HWND hParentWindow, std::string title, unsigned short r, unsigned short g, unsigned short b, 
							 unsigned short a, bool IsRGB)
{
	m_pCallback = NULL;
	if (IsRGB)
		SetRGB(r, g, b);
	else
		SetHSV(r, g, b);
	SetAlpha(a);
	OldCol = CurrCol;
	
	m_hParent = hParentWindow;
	m_Title = title;
}

EXPORT void CColourPicker::CreatecolourPicker(short AlphaUsage,
											  IColorChangeCallback* i_pCallback /*= NULL*/)
{
	UseAlpha = AlphaUsage;
	m_IsModeless = false;
	m_pCallback = i_pCallback;
	
	switch(UseAlpha)
	{
	case CP_NO_ALPHA:
		DialogBoxParam(GetModuleHandle("CPicker.dll"), MAKEINTRESOURCE(IDD_COLORPICKERNOALPHA), m_hParent, 
			(DLGPROC)ColourPickerDlgProc, (LPARAM)this);
		break;

	case CP_USE_ALPHA:
	case CP_DISABLE_ALPHA:
		DialogBoxParam(GetModuleHandle("CPicker.dll"), MAKEINTRESOURCE(IDD_COLORPICKER), m_hParent, 
			(DLGPROC)ColourPickerDlgProc, (LPARAM)this);
		break;

	default:
		CreatecolourPicker(CP_USE_ALPHA);
		break;
	}
}

EXPORT void CColourPicker::CreatecolourPickerModeless(short AlphaUsage,
													  IColorChangeCallback* i_pCallback /*= NULL*/)
{
	UseAlpha = AlphaUsage;
	m_IsModeless = true;
	m_pCallback = i_pCallback;
	
	switch(UseAlpha)
	{
	case CP_NO_ALPHA:
		m_hDlg = CreateDialogParam(GetModuleHandle("CPicker.dll"), MAKEINTRESOURCE(IDD_COLORPICKERNOALPHA), m_hParent, 
			(DLGPROC)ColourPickerDlgProc, (LPARAM)this);
		::ShowWindow(m_hDlg, TRUE);
		break;

	case CP_USE_ALPHA:
	case CP_DISABLE_ALPHA:
		m_hDlg = CreateDialogParam(GetModuleHandle("CPicker.dll"), MAKEINTRESOURCE(IDD_COLORPICKER), m_hParent, 
			(DLGPROC)ColourPickerDlgProc, (LPARAM)this);
		::ShowWindow(m_hDlg, TRUE);
		break;

	default:
		CreatecolourPickerModeless(CP_USE_ALPHA);
		break;
	}
}

EXPORT void CColourPicker::SetRGB(unsigned short r, unsigned short g, unsigned short b)
{
	// Clamp colour values to 255
	CurrCol.r = min(r, 255);
	CurrCol.g = min(g, 255);
	CurrCol.b = min(b, 255);

	CurrCol.UpdateHSV();
	if (m_pCallback)
		m_pCallback->ColorChanged(CurrCol);
}

EXPORT void CColourPicker::SetHSV(unsigned short h, unsigned short s, unsigned short v)
{
	// Clamp hue values to 359, sat and val to 100
	CurrCol.h = min(h, 359);
	CurrCol.s = min(s, 100);
	CurrCol.v = min(v, 100);

	CurrCol.UpdateRGB();
	if (m_pCallback)
		m_pCallback->ColorChanged(CurrCol);
}

EXPORT void CColourPicker::SetAlpha(unsigned short a)
{
	if (UseAlpha == CP_USE_ALPHA)
		{
		// Clamp alpha values to 100
		CurrCol.a = min(a, 100);
		}
	else
		CurrCol.a = 100;

	if (m_pCallback)
		m_pCallback->ColorChanged(CurrCol);
}

EXPORT SColour CColourPicker::GetCurrentColour()
{
	return CurrCol;
}

EXPORT SColour CColourPicker::GetOldColour()
{
	return OldCol;
}

short CColourPicker::GetAlphaUsage()
{
	return UseAlpha;
}

void CColourPicker::UpdateOldColour()
{
	OldCol = CurrCol;
}

void CColourPicker::Revert()
{
	CurrCol = OldCol;
	if (m_pCallback)
		m_pCallback->ColorChanged(CurrCol);
}

// Updates the RGB color from the HSV
void SColour::UpdateRGB()
{
	int conv;
	double hue, sat, val;
	int base;

	hue = (float)h / 100.0f;
	sat = (float)s / 100.0f;
	val = (float)v / 100.0f;

	if ((float)s == 0) // Acromatic color (gray). Hue doesn't mind.
		{
		conv = (unsigned short) (255.0f * val);
		r = b = g = conv;
		return;
		}
	
	base = (unsigned short)(255.0f * (1.0 - sat) * val);

	switch ((unsigned short)((float)h/60.0f))
	{
	case 0:
		r = (unsigned short)(255.0f * val);
		g = (unsigned short)((255.0f * val - base) * (h/60.0f) + base);
		b = base;
		break;

	case 1:
		r = (unsigned short)((255.0f * val - base) * (1.0f - ((h%60)/ 60.0f)) + base);
		g = (unsigned short)(255.0f * val);
		b = base;
		break;

	case 2:
		r = base;
		g = (unsigned short)(255.0f * val);
		b = (unsigned short)((255.0f * val - base) * ((h%60)/60.0f) + base);
		break;
	
	case 3:
		r = base;
		g = (unsigned short)((255.0f * val - base) * (1.0f - ((h%60) / 60.0f)) + base);
		b = (unsigned short)(255.0f * val);
		break;
	
	case 4:
		r = (unsigned short)((255.0f * val - base) * ((h%60) / 60.0f) + base);
		g = base;
		b = (unsigned short)(255.0f * val);
		break;
	
	case 5:
		r = (unsigned short)(255.0f * val);
		g = base;
		b = (unsigned short)((255.0f * val - base) * (1.0f - ((h%60) / 60.0f)) + base);
		break;
	}
}

#define MAX(a, b, c) ((a)>(b)? ((a)>(c)?(a):(c)) : ((b)>(c)?(b):(c)))
#define MIN(a, b, c) ((a)<(b)? ((a)<(c)?(a):(c)) : ((b)<(c)?(b):(c)))
// Updates the HSV color from the RGB
void SColour::UpdateHSV()
{
	unsigned short max, min, delta;
	short temp;
    
	max = MAX(r, g, b);
	min = MIN(r, g, b);
	delta = max-min;

    if (max == 0)
	{
		s = h = v = 0;
		return;
	}
    
	v = (unsigned short) ((double)max/255.0*100.0);
	s = (unsigned short) (((double)delta/max)*100.0);

	if (r == max)
		temp = (short)(60 * ((g-b) / (double) delta));
	else if (g == max)
		temp = (short)(60 * (2.0 + (b-r) / (double) delta));
	else
		temp = (short)(60 * (4.0 + (r-g) / (double) delta));
	
	if (temp<0)
		h = temp + 360;
	else
		h = temp;
}
