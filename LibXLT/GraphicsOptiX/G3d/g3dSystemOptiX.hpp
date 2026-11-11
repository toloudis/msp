/*****************************************************************************
**  g3dSystemOptiX.hpp
**
**      g3dSystemOptiX reports the OptiX device, and installs the 3D hooks
**	(lights, textures, shaders, screen capture) while it exists, as
**	g3dSystemVk does.
**
**	Stub: the adapter is "OptiX (stub)" with no memory.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DSYSTEMOPTIX_HPP
#error g3dSystemOptiX.hpp multiply included
#endif
#define GOX_G3DSYSTEMOPTIX_HPP

#ifndef G3D_SYSTEM_HPP
#include "Graphics/G3d/g3dSystem.hpp"
#endif

#include <string>

namespace gox
{
	class g2dScreenCaptureUtilOptiX;
	class g3dLightMgrOptiX;
	class matShaderMgrOptiX;
	class matTextureMgrOptiX;

	class g3dSystemOptiX : public g3dSystem
	{
		public:
			g3dSystemOptiX();
			virtual ~g3dSystemOptiX();

			virtual const char* GetVideoAdapterName();
			virtual float GetVideoMemory();

		private:
			std::string					m_Name;
			g3dLightMgrOptiX*			m_pLightMgr;
			matTextureMgrOptiX*			m_pTextureMgr;
			matShaderMgrOptiX*			m_pShaderMgr;
			g2dScreenCaptureUtilOptiX*	m_pScreenCapture;
	};
}
