/*****************************************************************************
**  inTablet.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Input/in/inTablet.hpp"
#include "Input/in/private/inTabletPAC.hpp"

//========================================================================
//	Constructor
//========================================================================
inTablet::inTablet()
: m_pPAC(new inTabletPAC)
{
	//initially turn off tablet stroke recognition
	SetInkEnabled(false);
}


//========================================================================
//	Destructor
//========================================================================
inTablet::~inTablet()
{
	delete m_pPAC;
}

//========================================================================
//	Think gives the device a chance to update its state once per frame
//========================================================================
void inTablet::Think()
{
	if( !this->IsEnabled() )
		return;

	m_X = 0;
	m_Y = 0;
	m_Z = 0;
	m_bCursorDown = false;
	
	m_pPAC->Think(m_X, m_Y, m_Z, m_bCursorDown);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void inTablet::GetPosition( int& o_dX, int &o_dY)
{
	if( !this->IsEnabled() )
	{
		o_dX = 0;
		o_dY = 0;
	}
	else
	{
		o_dX = m_X;
		o_dY = m_Y;
	}

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void inTablet::GetPosition( int& o_dX, int &o_dY, float &o_dZ)
{
	if( !this->IsEnabled() )
	{
		o_dX = 0;
		o_dY = 0;
		o_dZ = 0;
	}
	else
	{
		o_dX = m_X;
		o_dY = m_Y;
		o_dZ = m_Z;
	}

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool inTablet::IsCursorDown()
{
	if( !this->IsEnabled() )
		return false;

	return m_bCursorDown;

}

//------------------------------------------------------------------------
// Get/Set whether or not the tablet should process events
//------------------------------------------------------------------------
bool inTablet::GetInkEnabled()
{
	if( !this->IsEnabled() )
		return false;
	return m_pPAC->IsInkEnabled();
}
void inTablet::SetInkEnabled(bool i_bEnabled)
{
	if( !this->IsEnabled() )
		return;
	m_pPAC->SetInkEnabled(i_bEnabled);
}

//------------------------------------------------------------------------
// Get the current pressure of the pen on the tablet
//------------------------------------------------------------------------
void inTablet::GetPressure(int &o_Pressure)
{
	m_pPAC->GetPressure(o_Pressure);
}

//------------------------------------------------------------------------
// Get the X, Y tilt of the pen on the tablet
//------------------------------------------------------------------------
void inTablet::GetTilt(int &o_XTilt, int &o_YTilt)
{
	m_pPAC->GetTilt(o_XTilt, o_YTilt);
}

//------------------------------------------------------------------------
// Get the erase state of the pen
//------------------------------------------------------------------------
void inTablet::GetErase(bool &o_bErase)
{
	m_pPAC->GetErase(o_bErase);
}