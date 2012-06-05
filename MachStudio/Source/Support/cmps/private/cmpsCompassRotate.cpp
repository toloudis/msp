/*****************************************************************************
**  cmpsCompassRotate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassRotate.hpp"

#include "Support/cmps/private/cmpsCompassObjectRotate.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassRotate::cmpsCompassRotate(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompass(i_RenderLayer)
{
	// Note: each child class should instantiate their own compass object
	// and set m_pCompassObject to point to it in this function.
	//
	m_pCompassObjectRotate = new cmpsCompassObjectRotate(i_RenderLayer);
	m_pCompassObject = m_pCompassObjectRotate;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassRotate::~cmpsCompassRotate()
{
	delete m_pCompassObject;
}

