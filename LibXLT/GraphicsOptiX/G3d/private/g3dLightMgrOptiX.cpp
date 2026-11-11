/*****************************************************************************
**  g3dLightMgrOptiX.cpp
**
**      See g3dLightMgrOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DLIGHTMGROPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dLightMgrOptiX.hpp"
#endif

#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/G3d/g3dPointLight.hpp"
#endif
#ifndef G3D_DIRECTIONALLIGHT_HPP
#include "Graphics/G3d/g3dDirectionalLight.hpp"
#endif
#ifndef G3D_SPOTLIGHT_HPP
#include "Graphics/G3d/g3dSpotLight.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif

#include <cstddef>

namespace gox
{
	g3dLightMgrOptiX::g3dLightMgrOptiX()
		: m_Ambient(0.0f, 0.0f, 0.0f, 1.0f)
		, m_bHeadlight(false)
		, m_HeadlightDir(0.0f, 0.0f, 1.0f)
	{
	}

	g3dPointLight* g3dLightMgrOptiX::CreatePointLight()
	{
		return new g3dPointLight;
	}

	g3dDirectionalLight* g3dLightMgrOptiX::CreateDirectionalLight()
	{
		return new g3dDirectionalLight;
	}

	g3dSpotLight* g3dLightMgrOptiX::CreateSpotLight()
	{
		return new g3dSpotLight;
	}

	g3dProjectedLight* g3dLightMgrOptiX::CreateProjectedLight()
	{
		return new g3dProjectedLight;
	}

	void g3dLightMgrOptiX::DestroyLight(g3dLight* i_Light)
	{
		delete i_Light;
	}

	void g3dLightMgrOptiX::SetAmbient(const maFloatRGBA& i_Color)
	{
		m_Ambient = i_Color;
	}

	int g3dLightMgrOptiX::GetMaxLights()
	{
		return 0;
	}

	void g3dLightMgrOptiX::EnableHeadlight(bool i_bEnable)
	{
		m_bHeadlight = i_bEnable;
	}

	bool g3dLightMgrOptiX::IsHeadlightEnabled() const
	{
		return m_bHeadlight;
	}

	void g3dLightMgrOptiX::SetHeadlightDirection(const maVector3d& i_LightDir)
	{
		m_HeadlightDir = i_LightDir;
	}
}
