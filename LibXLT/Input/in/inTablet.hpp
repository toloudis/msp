/*****************************************************************************
**  inTablet.hpp
**
**		inTablet uses the tablet hardware interface to report back various 
**		stroke, click, etc events caught from the tablet pen or mouse.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_TABLET_HPP
#error inTablet.hpp multiply included
#endif
#define IN_TABLET_HPP

#ifndef IN_DEVICE_HPP
#include "Input/in/inDevice.hpp"
#endif

//========================================================================
//	Forward References
//========================================================================
class inTabletPAC;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class inTablet : public inDevice
{
public:
	//========================================================================
	//	Constructor
	//========================================================================
	inTablet();

	//========================================================================
	//	Destructor
	//========================================================================
	~inTablet();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//========================================================================
	void Think();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void GetPosition( int& o_dX, int &o_dY);
	void GetPosition( int& o_dX, int &o_dY, float &o_dZ);

	//------------------------------------------------------------------------
	// Is the pen on the tablet, or the mouse left button pressed within the tablet window
	//------------------------------------------------------------------------
	bool IsCursorDown();

	//------------------------------------------------------------------------
	// Get/Set whether or not the tablet should process events
	//------------------------------------------------------------------------
	bool GetInkEnabled();
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
	// Get the erase state of the pen
	//------------------------------------------------------------------------
	void GetErase(bool &o_bErase);

private:
	inTabletPAC* m_pPAC;
	int m_X;
	int m_Y;
	float m_Z;
	bool m_bCursorDown;

}; // end class inTablet