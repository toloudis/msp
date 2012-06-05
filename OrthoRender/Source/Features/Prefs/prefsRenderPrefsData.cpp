/****************************************************************************\
**  prefsRenderPrefsData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/prefsRenderPrefsData.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
prefsRenderPrefsData::prefsRenderPrefsData()
:	m_bEnableDOF("Render DOF",true),
	m_bEnableFur("Render Fur",true),
	m_bEnableGlow("Render Glow",true),
	m_bMatteMode("Render Matte",false),
	m_bEnableOutline("Render Outline",true)
{
}


