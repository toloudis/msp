/****************************************************************************\
**	inTabletPACWin.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "InputTPC/in/inTabletPACWin.hpp"

#include "Core/app/appApplication.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "InputTPC/in/inTPCInfo.hpp"
#include "InputTPC/in/private/inTPCInkCollector.hpp"
#include "InputTPC/in/private/inTPCInkOverlay.hpp"


//============================================================================
//============================================================================
namespace 
{
	//property data that can be grabbed later by the tablet object
	int l_Pressure, l_XTilt, l_YTilt = 0;
}


//------------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------------
inTabletPAC::inTabletPAC()
{
	//inTPCInfo::LogTabletInfo();
	m_pInkCollector = NULL;
	if(inTPCInfo::IsTabletSystem())
	{
		m_pInkCollector = new inTPCInkCollector();
		if( m_pInkCollector )
			m_pInkCollector->Init( (HWND)inDeviceMgr::GetWindowHandle() );
	}
}

//------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------
inTabletPAC::~inTabletPAC()
{
	if( m_pInkCollector )
	{
		delete m_pInkCollector;
		m_pInkCollector = NULL;
	}
}

//------------------------------------------------------------------------
//	Think gives the device a chance to update its state once per frame
//	i_State will be updated with the current state of the mouse
//------------------------------------------------------------------------
void inTabletPAC::Think(int& o_dX, int& o_dY, float& o_dZ, bool& o_bCursorDown)
{
	if( m_pInkCollector )
	{
		m_pInkCollector->UpdateState();

		o_dX = m_pInkCollector->m_Data.m_X;
		o_dY = m_pInkCollector->m_Data.m_Y;
		o_dZ = m_pInkCollector->m_Data.m_Z;

		o_bCursorDown = m_pInkCollector->m_Data.m_bCursorDown;
		m_bErase = m_pInkCollector->m_Data.m_bErase;
		if(o_bCursorDown)
		{
			m_pInkCollector->GetDesiredPackets(m_pInkCollector->e_Pressure, l_Pressure);
			m_pInkCollector->GetDesiredPackets(m_pInkCollector->e_XTilt, l_XTilt);
			m_pInkCollector->GetDesiredPackets(m_pInkCollector->e_YTilt, l_YTilt);
		}
	}
}

//------------------------------------------------------------------------
// Get/Set whether or not the tablet should process events
//------------------------------------------------------------------------
bool inTabletPAC::IsInkEnabled()
{
	bool bEnabled = false;
	if( m_pInkCollector )
		bEnabled = m_pInkCollector->GetEnabled();

	return bEnabled;
}
void inTabletPAC::SetInkEnabled(bool i_bEnabled)
{
	if( m_pInkCollector )
		m_pInkCollector->SetEnabled((VARIANT_BOOL)i_bEnabled);
}

//------------------------------------------------------------------------
// Get the current pressure of the pen on the tablet
//------------------------------------------------------------------------
void inTabletPAC::GetPressure(int &o_Pressure)
{
	o_Pressure = l_Pressure;
}

//------------------------------------------------------------------------
// Get the X, Y tilt of the pen on the tablet
//------------------------------------------------------------------------
void inTabletPAC::GetTilt(int &o_XTilt, int &o_YTilt)
{
	o_XTilt = l_XTilt;
	o_YTilt = l_YTilt;
}

//------------------------------------------------------------------------
// Get the state of the tablet pen to see if it should erase
//------------------------------------------------------------------------
void inTabletPAC::GetErase(bool &o_bErase)
{
	o_bErase = m_bErase;
}

