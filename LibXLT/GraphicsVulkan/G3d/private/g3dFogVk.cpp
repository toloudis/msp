/****************************************************************************\
**	g3dFogVk.cpp
**
**	See g3dFogVk.hpp. Both functions are empty, as in GraphicsDX11: its
**	bodies are commented out there (they set Direct3D 9 fixed-function fog
**	states, under ENABLE_LEGACYSTATES, which nothing defines).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
#endif

namespace gvk
{

//----------------------------------------------------------------------------
//	EnableFog - temporarily turn on/off fog during rendering process
//----------------------------------------------------------------------------
void g3dFogVk::EnableFog( bool i_bEnable )
{
}

//----------------------------------------------------------------------------
//	SetFog sets all fog parameters
//----------------------------------------------------------------------------
void g3dFogVk::SetFog(	int i_nMode,
						const maFloatRGBA& i_Color,
						float i_fStart,
						float i_fEnd,
						float i_fDensity )
{
}

}
