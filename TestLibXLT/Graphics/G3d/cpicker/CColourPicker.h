#ifndef __CPICKERDLL_H
#define __CPICKERDLL_H

#include "PixelBuffer.h"

#include <windows.h>
#include <string>

// Just some useful macros...
#define EXPORT /*_declspec (dllexport)*/ 

// No alpha slider displayed
#define CP_NO_ALPHA			0
// Alpha slider displayed
#define CP_USE_ALPHA		1
// Alpha slider displayed but unusable
#define CP_DISABLE_ALPHA	2

struct SColour
{
	unsigned short r, g, b;		// Red, green and blue
	unsigned short h, s, v;		// Hue, saturation and value
	unsigned short a;			// Alpha

	void UpdateRGB ();			// Updates RGB from HSV
	void UpdateHSV ();			// Updates HSV from RGB
};


class IColorChangeCallback
{
public:
	virtual void ColorChanged(SColour color) = 0;
};

class CColourPicker
{
public:
	// Constructor
	EXPORT CColourPicker(HWND hParentWindow, std::string title);
	// Use true for IsRGB if passing RGBA or false if passing HSVA 
	EXPORT CColourPicker(HWND hParentWindow, std::string title, unsigned short r, unsigned short g, unsigned short b, 
		unsigned short a, bool IsRGB);

	// Creates the colour picker dialog
	EXPORT void CreatecolourPicker(short AlphaUsage, 
		IColorChangeCallback* i_pCallback = NULL);
	EXPORT void CreatecolourPickerModeless(short AlphaUsage, 
		IColorChangeCallback* i_pCallback = NULL);

	// Functions to set the colour components
	// NOTE: SetRGB automatically updates HSV and viceversa
	EXPORT void SetRGB(unsigned short r, unsigned short g, unsigned short b);
	EXPORT void SetHSV(unsigned short h, unsigned short s, unsigned short v);
	EXPORT void SetAlpha(unsigned short a);
	
	// Some easy functions to retrieve the colour components
	EXPORT SColour GetCurrentColour();
	EXPORT SColour GetOldColour();

	// Returns CP_NO_ALPHA if not using alpha, CP_USE_ALPHA if using or 
	// CP_DISABLE_ALPHA if alpha is not used but the slider is displayed
	short GetAlphaUsage();
	void Revert();
	EXPORT void UpdateOldColour();
	
	void SetParent(HWND _parent){ m_hParent=_parent;}
	void SetDlg(HWND _dlg){ m_hDlg = _dlg; }
	EXPORT bool IsModeless() {return m_IsModeless;}
	std::string GetTitle() {return m_Title;}

private:
	// The current selected colour and the previous selected one
	SColour CurrCol, OldCol;
	short UseAlpha;
	bool m_IsModeless;
	
	HWND m_hParent;
	HWND m_hDlg;
	IColorChangeCallback* m_pCallback;
	std::string m_Title;

};

EXPORT void DrawCheckedRect(HWND hWnd, int r, int g, int b, int a, int cx, int cy);

LRESULT CALLBACK ColourPickerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateValues(HWND hDlg, struct SColour col);

#endif


// USAGE:

/*
CColourPicker *cpicker;
class MyCallback : public IColorChangeCallback
{
public:
	virtual void ColorChanged(SColour col)
	{
	// do something.
	}
};
MyCallback callback;
cpicker = new CColourPicker(hDlg, r, g, b, a, true);
cpicker->CreatecolourPickerModeless(flag, &callback);
*/
