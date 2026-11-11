/*****************************************************************************
**  g3dSystemOptiX.cpp
**
**      See g3dSystemOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DSYSTEMOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSystemOptiX.hpp"
#endif
#ifndef GOX_G3DLIGHTMGROPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dLightMgrOptiX.hpp"
#endif
#ifndef GOX_MATTEXTUREMGROPTIX_HPP
#include "GraphicsOptiX/Mat/private/matTextureMgrOptiX.hpp"
#endif
#ifndef GOX_MATSHADERMGROPTIX_HPP
#include "GraphicsOptiX/Mat/private/matShaderMgrOptiX.hpp"
#endif
#ifndef GOX_G2DSCREENCAPTUREUTILOPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dScreenCaptureUtilOptiX.hpp"
#endif

namespace gox
{
	g3dSystemOptiX::g3dSystemOptiX()
		: m_Name("OptiX (stub)")
		, m_pLightMgr(new g3dLightMgrOptiX)
		, m_pTextureMgr(new matTextureMgrOptiX)
		, m_pShaderMgr(new matShaderMgrOptiX)
		, m_pScreenCapture(new g2dScreenCaptureUtilOptiX)
	{
		g3dLightMgr::SetImplementation(m_pLightMgr);
		matTextureMgr::SetImplementation(m_pTextureMgr);
		matShaderMgr::SetImplementation(m_pShaderMgr);
		g2dScreenCaptureUtil::SetImplementation(m_pScreenCapture);
	}

	g3dSystemOptiX::~g3dSystemOptiX()
	{
		g2dScreenCaptureUtil::SetImplementation(NULL);
		matShaderMgr::SetImplementation(NULL);
		matTextureMgr::SetImplementation(NULL);
		g3dLightMgr::SetImplementation(NULL);
		delete m_pScreenCapture;
		delete m_pShaderMgr;
		delete m_pTextureMgr;
		delete m_pLightMgr;
	}

	const char* g3dSystemOptiX::GetVideoAdapterName()
	{
		return m_Name.c_str();
	}

	//	As DX11 and Vulkan: dedicated video memory in bytes
	float g3dSystemOptiX::GetVideoMemory()
	{
		return 0.0f;
	}
}
