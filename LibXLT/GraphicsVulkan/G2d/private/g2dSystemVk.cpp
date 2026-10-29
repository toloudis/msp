/*****************************************************************************
**  g2dSystemVk.cpp
**
**      See g2dSystemVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DSYSTEMVK_HPP
#include "GraphicsVulkan/G2d/g2dSystemVk.hpp"
#endif
#ifndef GVK_G2DWINDOWVK_HPP
#include "GraphicsVulkan/G2d/g2dWindowVk.hpp"
#endif
#ifndef APP_APPLICATION_HPP
#include "Core/App/appApplication.hpp"
#endif
#include <string>
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef G2D_RESETHANDLER_HPP
#include "Graphics/G2d/g2dResetHandler.hpp"
#endif

#ifndef GVK_G2DRESOURCECOUNTERVK_HPP
#include "GraphicsVulkan/G2d/g2dResourceCounterVk.hpp"
#endif
#ifndef GVK_G2DIMAGECREATEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageCreateVk.hpp"
#endif
#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif
#ifndef GVK_G2DFONTUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dFontUtilVk.hpp"
#endif

#include <algorithm>

namespace gvk
{
	g2dSystemVk::g2dSystemVk(vkdDevice& i_Device, bool i_bHeadless)
		: m_Device(i_Device)
		, m_bHeadless(i_bHeadless)
		, m_pResourceCounter(new g2dResourceCounterVk)
		, m_pFontUtil(new g2dFontUtilVk)
		, m_pImageCreate(new g2dImageCreateVk)
	{
		g2dResourceCounter::SetImplementation(m_pResourceCounter);
		g2dFontUtil::SetImplementation(m_pFontUtil);
		g2dImageCreate::SetImplementation(m_pImageCreate);

		g2dFullscreenQuad::InitFullscreenQuad();
	}

	g2dSystemVk::~g2dSystemVk()
	{
		for (g2dWindow* window : m_Windows)
		{
			delete window;
		}
		m_Windows.clear();

		g2dFullscreenQuad::CleanUpFullscreenQuad();

		g2dImageCreate::SetImplementation(NULL);
		g2dFontUtil::SetImplementation(NULL);
		g2dResourceCounter::SetImplementation(NULL);
		delete m_pImageCreate;
		delete m_pFontUtil;
		delete m_pResourceCounter;

		//	The handlers Graphics and the texture manager registered (as DX11)
		g2dResetHandler::DestroyResetHandlers();
	}

	g2dWindow* g2dSystemVk::CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y)
	{
		appApplication::SetMainWindowSize(i_X, i_Y, i_Width, i_Height);
		g2dWindow* window = new g2dWindowVk(m_Device, appApplication::GetMainWindowHandle(), false, -1, -1, m_bHeadless);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateAppWindow(void* i_Handle)
	{
		g2dWindow* window = new g2dWindowVk(m_Device, i_Handle, false, -1, -1, m_bHeadless);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateSubWindow(void* i_Handle)
	{
		g2dWindow* window = new g2dWindowVk(m_Device, i_Handle, false, -1, -1, m_bHeadless);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y)
	{
		std::string title("");
		void* handle = appApplication::CreateSubWindow(i_Width, i_Height, i_X, i_Y, title);
		g2dWindow* window = new g2dWindowVk(m_Device, handle, true, i_Width, i_Height, m_bHeadless);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateFullScreen(int, int, int)
	{
		DBG_ERROR("Fullscreen not supported.");
		return NULL;
	}

	g2dWindow* g2dSystemVk::CreateSubFullScreen(int, int, int)
	{
		DBG_ERROR("Fullscreen not supported.");
		return NULL;
	}

	void g2dSystemVk::DestroyWindow(g2dWindow* i_pWindow)
	{
		auto found = std::find(m_Windows.begin(), m_Windows.end(), i_pWindow);
		if (found != m_Windows.end())
		{
			m_Windows.erase(found);
			delete i_pWindow;
		}
	}

	void g2dSystemVk::ClearManagedResources()
	{
	}
}
