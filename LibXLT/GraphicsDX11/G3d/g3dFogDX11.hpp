/****************************************************************************\
**	g3dFogDX11.hpp
**
**	The g3dFogDX11 sets fog settings into device
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_FOGDX11_HPP
#error g3dFogDX11.hpp multiply included
#endif
#define G3D_FOGDX11_HPP

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class maFloatRGBA;

namespace g3dFogDX11
{

	//------------------------------------------------------------------------
	//	EnableFog - temporarily turn on/off fog during rendering process
	//------------------------------------------------------------------------
	void EnableFog( bool i_bEnable );

	//------------------------------------------------------------------------
	//	SetFog sets all fog parameters
	//------------------------------------------------------------------------
	void SetFog( int i_nMode,
				 const maFloatRGBA& i_Color,
				 float i_fStart,
				 float i_fEnd,
				 float i_fDensity );

}
