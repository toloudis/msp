/*****************************************************************************
**  g3dLightMgrOptiX.hpp
**
**      g3dLightMgrOptiX creates the OptiX backend's lights (g3dLightMgr).
**
**	Lights are Graphics' own light classes (as GraphicsVulkan's), not yet
**	used for rendering; the ambient color and headlight settings are kept.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DLIGHTMGROPTIX_HPP
#error g3dLightMgrOptiX.hpp multiply included
#endif
#define GOX_G3DLIGHTMGROPTIX_HPP

#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif

namespace gox
{
	class g3dLightMgrOptiX : public g3dLightMgrImpl
	{
		public:
			g3dLightMgrOptiX();

			virtual g3dPointLight* CreatePointLight();
			virtual g3dDirectionalLight* CreateDirectionalLight();
			virtual g3dSpotLight* CreateSpotLight();
			virtual g3dProjectedLight* CreateProjectedLight();
			virtual void DestroyLight(g3dLight* i_Light);
			virtual void SetAmbient(const maFloatRGBA& i_Color);
			virtual int GetMaxLights();
			virtual void EnableHeadlight(bool i_bEnable);
			virtual bool IsHeadlightEnabled() const;
			virtual void SetHeadlightDirection(const maVector3d& i_LightDir);

		private:
			maFloatRGBA	m_Ambient;
			bool		m_bHeadlight;
			maVector3d	m_HeadlightDir;
	};
}
