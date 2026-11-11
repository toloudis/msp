/*****************************************************************************
**  g2dSystemOptiX.cpp
**
**      See g2dSystemOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DSYSTEMOPTIX_HPP
#include "GraphicsOptiX/G2d/g2dSystemOptiX.hpp"
#endif
#ifndef GOX_G2DWINDOWOPTIX_HPP
#include "GraphicsOptiX/G2d/g2dWindowOptiX.hpp"
#endif
#ifndef GOX_G2DFONTUTILOPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dFontUtilOptiX.hpp"
#endif
#ifndef GOX_G2DIMAGECREATEOPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dImageCreateOptiX.hpp"
#endif
#ifndef GOX_G2DRESOURCECOUNTEROPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dResourceCounterOptiX.hpp"
#endif
#ifndef G2D_RESETHANDLER_HPP
#include "Graphics/G2d/g2dResetHandler.hpp"
#endif
#ifndef APP_APPLICATION_HPP
#include "Core/App/appApplication.hpp"
#endif

#include <algorithm>

namespace gox
{
	g2dSystemOptiX::g2dSystemOptiX(bool i_bHeadless)
		: m_bHeadless(i_bHeadless)
		, m_pResourceCounter(new g2dResourceCounterOptiX)
		, m_pFontUtil(new g2dFontUtilOptiX)
		, m_pImageCreate(new g2dImageCreateOptiX)
	{
		g2dResourceCounter::SetImplementation(m_pResourceCounter);
		g2dFontUtil::SetImplementation(m_pFontUtil);
		g2dImageCreate::SetImplementation(m_pImageCreate);
	}

	g2dSystemOptiX::~g2dSystemOptiX()
	{
		for (g2dWindow* window : m_Windows)
		{
			delete window;
		}
		m_Windows.clear();

		g2dImageCreate::SetImplementation(NULL);
		g2dFontUtil::SetImplementation(NULL);
		g2dResourceCounter::SetImplementation(NULL);
		delete m_pImageCreate;
		delete m_pFontUtil;
		delete m_pResourceCounter;

		//	The handlers Graphics registered (as DX11 and Vulkan)
		g2dResetHandler::DestroyResetHandlers();
	}

	g2dWindow* g2dSystemOptiX::add_window(g2dWindow* i_pWindow)
	{
		m_Windows.push_back(i_pWindow);
		return i_pWindow;
	}

	g2dWindow* g2dSystemOptiX::CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y)
	{
		appApplication::SetMainWindowSize(i_X, i_Y, i_Width, i_Height);
		return add_window(new g2dWindowOptiX(appApplication::GetMainWindowHandle(), i_Width, i_Height, m_bHeadless));
	}

	g2dWindow* g2dSystemOptiX::CreateAppWindow(void* i_Handle)
	{
		return add_window(new g2dWindowOptiX(i_Handle, -1, -1, m_bHeadless));
	}

	g2dWindow* g2dSystemOptiX::CreateSubWindow(void* i_Handle)
	{
		return add_window(new g2dWindowOptiX(i_Handle, -1, -1, m_bHeadless));
	}

	g2dWindow* g2dSystemOptiX::CreateSubWindow(int i_Width, int i_Height, int /*i_X*/, int /*i_Y*/)
	{
		return add_window(new g2dWindowOptiX(NULL, i_Width, i_Height, m_bHeadless));
	}

	//	Full screen isn't supported (nor by DX11 or Vulkan): a plain window
	g2dWindow* g2dSystemOptiX::CreateFullScreen(int i_Width, int i_Height, int /*i_BitDepth*/)
	{
		return CreateAppWindow(i_Width, i_Height, 0, 0);
	}

	g2dWindow* g2dSystemOptiX::CreateSubFullScreen(int i_Width, int i_Height, int /*i_BitDepth*/)
	{
		return CreateSubWindow(i_Width, i_Height, 0, 0);
	}

	void g2dSystemOptiX::DestroyWindow(g2dWindow* i_pWindow)
	{
		std::vector<g2dWindow*>::iterator it = std::find(m_Windows.begin(), m_Windows.end(), i_pWindow);
		if (it != m_Windows.end())
		{
			m_Windows.erase(it);
			delete i_pWindow;
		}
	}

	void g2dSystemOptiX::ClearManagedResources()
	{
	}
}
