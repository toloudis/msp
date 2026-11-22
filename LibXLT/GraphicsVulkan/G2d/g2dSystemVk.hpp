/*****************************************************************************
**  g2dSystemVk.hpp
**
**      g2dSystemVk creates and owns the Vulkan windows (g2dWindowVk), as
**	g2dSystemDX11 does for DX11, and installs the 2D hooks (resource
**	counter, fonts, image creation and saving) while it exists.
**	Full-screen windows aren't supported, exactly as in DX11. Headless
**	(g3dGfxOptions::m_bHeadless, RenderNode), its windows have no swap
**	chain.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DSYSTEMVK_HPP
#error g2dSystemVk.hpp multiply included
#endif
#define GVK_G2DSYSTEMVK_HPP

#ifndef G2D_SYSTEM_HPP
#include "Graphics/G2d/g2dSystem.hpp"
#endif

#include <vector>

namespace gvk
{
	class vkdDevice;
	class g2dResourceCounterVk;
	class g2dFontUtilVk;
	class g2dImageCreateVk;

	class g2dSystemVk : public g2dSystem
	{
		public:
			g2dSystemVk(vkdDevice& i_Device, bool i_bHeadless);
			virtual ~g2dSystemVk();

			virtual g2dWindow* CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y);
			virtual g2dWindow* CreateAppWindow(void* i_Handle);
			virtual g2dWindow* CreateSubWindow(void* i_Handle);
			virtual g2dWindow* CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y);
			virtual g2dWindow* CreateFullScreen(int i_Width, int i_Height, int i_BitDepth);
			virtual g2dWindow* CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth);
			virtual void DestroyWindow(g2dWindow* i_pWindow);
			virtual void ClearManagedResources();

		private:
			vkdDevice&				m_Device;
			bool					m_bHeadless;
			std::vector<g2dWindow*>	m_Windows;

			//	The 2D hooks, installed for the system's lifetime (as g2dSystemDX11)
			g2dResourceCounterVk*	m_pResourceCounter;
			g2dFontUtilVk*			m_pFontUtil;
			g2dImageCreateVk*		m_pImageCreate;
	};
}
