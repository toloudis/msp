/****************************************************************************\
**	g3dFogVk.hpp
**
**	The g3dFogVk sets fog settings into device
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_G3D_FOGVK_HPP
#error g3dFogVk.hpp multiply included
#endif
#define GVK_G3D_FOGVK_HPP

class maFloatRGBA;

namespace gvk
{

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

namespace g3dFogVk
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
}
