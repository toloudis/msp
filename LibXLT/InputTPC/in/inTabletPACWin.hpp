/****************************************************************************\
**  inTabletPACWin.hpp
**
**      inTabletPACWin.hpp defines the PAC component of inTablet.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_TABLETPACWIN_HPP
#error inTabletPACWin.hpp multiply included
#endif
#define IN_TABLETPACWIN_HPP

#ifndef IN_TPCHEADER_HPP
#include "InputTPC/in/private/inTPCHeader.hpp"
#endif

//----------------------------------------------------------------------------
// Forward Declaration
//----------------------------------------------------------------------------
class inTPCInkCollector;
class inTPCInkOverlay;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class inTabletPAC
{
public:
	//========================================================================
	//	Constructor
	//========================================================================
	inTabletPAC();

	//========================================================================
	//	Destructor
	//========================================================================
	~inTabletPAC();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//	i_State will be updated with the current state of the mouse
	//========================================================================
	void Think(int& o_dX, int& o_dY, float& o_dZ, bool& o_bCursorDown);

	//------------------------------------------------------------------------
	// Get/Set whether or not the tablet should process events
	//------------------------------------------------------------------------
	bool IsInkEnabled();
	void SetInkEnabled(bool i_bEnabled);

	//------------------------------------------------------------------------
	// Get the current pressure of the pen on the tablet
	//------------------------------------------------------------------------
	void GetPressure(int &o_Pressure);

	//------------------------------------------------------------------------
	// Get the X, Y tilt of the pen on the tablet
	//------------------------------------------------------------------------
	void GetTilt(int &o_XTilt, int &o_YTilt);

	//------------------------------------------------------------------------
	// Get the state of the tablet pen to see if it should erase
	//------------------------------------------------------------------------
	void GetErase(bool &o_bErase);

private:

	inTPCInkCollector* m_pInkCollector;
	int m_Pressure;
	bool m_bErase;

};