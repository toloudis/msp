/*****************************************************************************
**  cmpsCompassSelect.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassSelect.hpp"

#include "Support/cmps/private/cmpsCompassObjectSelect.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassSelect::cmpsCompassSelect(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompass(i_RenderLayer)
{
	// Note: each child class should instantiate their own compass object
	// and set m_pCompassObject to point to it in this function.
	//
	m_pCompassObjectSelect = new cmpsCompassObjectSelect(i_RenderLayer);
	m_pCompassObject = m_pCompassObjectSelect;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassSelect::~cmpsCompassSelect()
{
	delete m_pCompassObject;
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassSelect::SetOrientation( const maRotation& i_Orientation )
{
	//  ignore the passed in value for Select since it's axis aligned
	//
	maRotation rot(0.0f,0.0f,0.0f);
	cmpsCompass::SetOrientation( rot );
}


