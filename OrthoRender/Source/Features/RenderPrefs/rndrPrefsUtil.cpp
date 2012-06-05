/****************************************************************************\
**	rndrPrefsUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"

#include "Graphics/G3d/g3dSingleLightRendering.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"

//--------------------------------------------------------------------
//	SetMultipassRendering - Turn on or off multipass lighting.
//--------------------------------------------------------------------
void rndrPrefsUtil::SetMultipassRendering(bool i_bShadows)
{
	g3dSingleLightRendering::SetDoSingleLightRendering( i_bShadows );

	// Also sets the g3dPreference for the headlight state
	// opposite to the shadows boolean.
	g3dPrefs::CurrentPrefs().m_bHeadlightOn = !i_bShadows;
}
