/*****************************************************************************
**  cmpsCompassScale.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassScale.hpp"

#include "Support/cmps/private/cmpsCompassObjectScale.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassScale::cmpsCompassScale(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompass(i_RenderLayer)
{
	// Note: each child class should instantiate their own compass object
	// and set m_pCompassObject to point to it in this function.
	//
	m_pCompassObjectScale = new cmpsCompassObjectScale(i_RenderLayer);
	m_pCompassObject = m_pCompassObjectScale;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassScale::~cmpsCompassScale()
{
	delete m_pCompassObject;
}


