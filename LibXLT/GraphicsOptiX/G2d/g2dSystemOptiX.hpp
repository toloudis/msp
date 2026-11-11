/*****************************************************************************
**  g2dSystemOptiX.hpp
**
**      g2dSystemOptiX creates and owns the OptiX backend's windows, and
**	installs the 2D hooks (resource counter, fonts, image creation) while
**	it exists, as g2dSystemVk does.
**
**	Stub: windows are g2dWindowOptiX, which draw nothing.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DSYSTEMOPTIX_HPP
#error g2dSystemOptiX.hpp multiply included
#endif
#define GOX_G2DSYSTEMOPTIX_HPP

#ifndef G2D_SYSTEM_HPP
#include "Graphics/G2d/g2dSystem.hpp"
#endif

#include <vector>

namespace gox
{
	class g2dFontUtilOptiX;
	class g2dImageCreateOptiX;
	class g2dResourceCounterOptiX;

	class g2dSystemOptiX : public g2dSystem
	{
		public:
			explicit g2dSystemOptiX(bool i_bHeadless);
			virtual ~g2dSystemOptiX();

			virtual g2dWindow* CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y);
			virtual g2dWindow* CreateAppWindow(void* i_Handle);
			virtual g2dWindow* CreateSubWindow(void* i_Handle);
			virtual g2dWindow* CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y);
			virtual g2dWindow* CreateFullScreen(int i_Width, int i_Height, int i_BitDepth);
			virtual g2dWindow* CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth);
			virtual void DestroyWindow(g2dWindow* i_pWindow);
			virtual void ClearManagedResources();

		private:
			g2dWindow* add_window(g2dWindow* i_pWindow);

			bool						m_bHeadless;
			std::vector<g2dWindow*>		m_Windows;
			g2dResourceCounterOptiX*	m_pResourceCounter;
			g2dFontUtilOptiX*			m_pFontUtil;
			g2dImageCreateOptiX*		m_pImageCreate;
	};
}
