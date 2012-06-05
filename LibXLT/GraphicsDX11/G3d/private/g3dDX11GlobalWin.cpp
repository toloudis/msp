/****************************************************************************\
**  g3dDX11GlobalWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDX11GlobalWin.hpp"

#include "Core/app/appFlowEventHandler.hpp"
#include "Core/dbg/dbgMsg.hpp"

namespace g3dDX11Global
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Initialize()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DeInitialize()
{
}

//----------------------------------------------------------------------------
//	IsTNLHALDevice returns true if the selected D3D device supports
//	transformations in hardware.
//----------------------------------------------------------------------------
bool IsTNLHALDevice()
{
	DBG_WARNING( "Deprecated" );
	return true;
}

//----------------------------------------------------------------------------
//	GetMaxTextures returns the maximum number of textures that the
//	device can blend simultaneously.
//----------------------------------------------------------------------------
int GetMaxTextures()
{
	DBG_WARNING( "Deprecated" );
	return 8;
}

//----------------------------------------------------------------------------
//	GetMaxStages returns the maximum number of "TextureStageState" blending
//	staging that can be used.  Note that this is not the neccessarily the
//	same as the value returned by GetMaxTextures.
//----------------------------------------------------------------------------
int GetMaxStages()
{
	DBG_WARNING( "Deprecated" );
	return 32;
}

}
