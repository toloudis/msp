/****************************************************************************\
**  prefsRenderPrefsData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/prefsRenderPrefsData.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
prefsRenderPrefsData::prefsRenderPrefsData()
:	m_bEnableDOF("Render DOF",true),
	m_bEnableGlow("Render Glow",true),
	m_bMatteMode("Render Alpha",false),
	m_bEnableOutline("Render Outline",false),
	m_bEnableMotionBlur("Enable Motion Blur",false)
{
}


