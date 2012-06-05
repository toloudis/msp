/****************************************************************************\
**  g3dDX11GlobalWin.hpp
**
**      g3dDX11GlobalWin.hpp contains some D3D stuff that many components
**	in the Windows PAC for g3d might need.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_DX11GLOBALWIN_HPP
#error g3dDX11GlobalWin.hpp multiply included
#endif
#define G3D_DX11GLOBALWIN_HPP

namespace g3dDX11Global
{

//----------------------------------------------------------------------------
//	Initialize causes this component to gather information about the device
//	selected, including the preferred texture formats and whether it's a
//	TNLHAL device.  This must be done after the d3d device is created.
//----------------------------------------------------------------------------
void Initialize();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DeInitialize();

//----------------------------------------------------------------------------
//	IsTNLHALDevice returns true if the selected D3D device supports
//	transformations in hardware.
//----------------------------------------------------------------------------
bool IsTNLHALDevice();

//----------------------------------------------------------------------------
//	GetMaxTextures returns the maximum number of textures that the
//	device can blend simultaneously.
//----------------------------------------------------------------------------
int GetMaxTextures();

//----------------------------------------------------------------------------
//	GetMaxStages returns the maximum number of "TextureStageState" blending
//	staging that can be used.  Note that this is not the neccessarily the
//	same as the value returned by GetMaxTextures.
//----------------------------------------------------------------------------
int GetMaxStages();

}


