/****************************************************************************\
**  brshData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004-7 - All Rights Reserved
\****************************************************************************/
#include "Support/brsh/data/brshData.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
brshData::brshData()
:	m_bEnabled("Enable Paint", false),
	m_CurrentCanvas("Current Canvas", ""),
	m_Color("Brush Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_BrushSize("Size", 0.1f),
	m_Opacity("Opacity", 100.0f),
	m_BrushShape("Brush Shape", fsLocator()),
	m_SaveButton("Save")
{
}



