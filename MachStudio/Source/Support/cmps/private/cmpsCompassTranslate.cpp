/*****************************************************************************
**  cmpsCompassTranslate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassTranslate.hpp"

#include "Support/cmps/private/cmpsCompassObjectTranslate.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassTranslate::cmpsCompassTranslate(cmpsRenderLayer::RenderLayer i_RenderLayer,
										   bool i_bAxisAligned)
:	cmpsCompass(i_RenderLayer),
	m_bAxisAligned(i_bAxisAligned)
{
	// Note: each child class should instantiate their own compass object
	// and set m_pCompassObject to point to it in this function.
	//
	m_pCompassObjectTranslate = new cmpsCompassObjectTranslate(i_RenderLayer);
	m_pCompassObject = m_pCompassObjectTranslate;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassTranslate::~cmpsCompassTranslate()
{
	delete m_pCompassObject;
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassTranslate::SetOrientation( const maRotation& i_Orientation )
{
	if (this->m_bAxisAligned)
	{
		//  ignore the passed in value for translate since it's axis aligned
		//
		maRotation rot(0.0f,0.0f,0.0f);
		cmpsCompass::SetOrientation( rot );
	}
	else
	{
		cmpsCompass::SetOrientation( i_Orientation );
	}
}

