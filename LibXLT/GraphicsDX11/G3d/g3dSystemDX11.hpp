/****************************************************************************\
**	g3dSystemDX11.hpp
**
**	The g3dSystemDX11 class sets up the D3D implementations of g3d
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_SYSTEMDX11_HPP
#error g3dSystemDX11.hpp multiply included
#endif
#define G3D_SYSTEMDX11_HPP

#ifndef G3D_SYSTEM_HPP
#include "Graphics/g3d/g3dSystem.hpp"
#endif

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <string>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class effShaderArray;
class g2dScreenCaptureUtilDX11;
class g3dLightMgrDX11;
class matTextureMgrDX11;

class g3dSystemDX11 : public g3dSystem
{
public:
	//--------------------------------------------------------------------
	//  Create and initialize system
	//--------------------------------------------------------------------
	g3dSystemDX11();

	//--------------------------------------------------------------------
	// Clean up and destroy system
	//--------------------------------------------------------------------
	virtual ~g3dSystemDX11();

	//------------------------------------------------------------------------
	//	GetVideoAdapterName returns some kind of ANSI C string uniquely
	//	identifying the type of video hardware in the system.  This function
	//	can be called only after calling Init().
	//------------------------------------------------------------------------
	const char* GetVideoAdapterName();

	//------------------------------------------------------------------------
	// return KB
	//------------------------------------------------------------------------
	float GetVideoMemory();

private:
	std::string m_VideoName;
	g3dLightMgrDX11 *m_pLightMgrImpl;
	matTextureMgrDX11 *m_pTextureMgrImpl;
	g2dScreenCaptureUtilDX11 *m_pScreenCaptureImpl;
	effShaderArray* m_pShaderImpl;
};
