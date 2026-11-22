/*****************************************************************************
**  g3dSystemVk.hpp
**
**      g3dSystemVk reports the Vulkan device (as g3dSystemDX11 does for
**	DX11): its name, and its dedicated (device-local) memory in bytes; and
**	installs the 3D hooks (lights, textures, shaders, screen capture) while
**	it exists.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G3DSYSTEMVK_HPP
#error g3dSystemVk.hpp multiply included
#endif
#define GVK_G3DSYSTEMVK_HPP

#ifndef G3D_SYSTEM_HPP
#include "Graphics/G3d/g3dSystem.hpp"
#endif

#include <string>

namespace gvk
{
	class vkdDevice;
	class g3dLightMgrVk;
	class matTextureMgrVk;
	class effShaderArray;
	class g2dScreenCaptureUtilVk;

	class g3dSystemVk : public g3dSystem
	{
		public:
			explicit g3dSystemVk(vkdDevice& i_Device);
			virtual ~g3dSystemVk();

			virtual const char* GetVideoAdapterName();
			virtual float GetVideoMemory();

		private:
			vkdDevice&	m_Device;
			std::string	m_Name;

			//	The 3D hooks, installed for the system's lifetime (as g3dSystemDX11)
			g3dLightMgrVk*			m_pLightMgr;
			matTextureMgrVk*		m_pTextureMgr;
			effShaderArray*			m_pShaderMgr;
			g2dScreenCaptureUtilVk*	m_pScreenCapture;
	};
}
