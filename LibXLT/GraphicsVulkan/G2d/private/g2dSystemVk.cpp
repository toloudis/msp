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
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif
#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif

#include <algorithm>

namespace gvk
{
	g2dSystemVk::g2dSystemVk(vkdDevice& i_Device)
		: m_Device(i_Device)
		, m_pResourceCounter(new g2dResourceCounterVk)
		, m_pFontUtil(new g2dFontUtilVk)
		, m_pImageCreate(new g2dImageCreateVk)
		, m_pImageSave(new g2dImageSaveVk)
	{
		g2dResourceCounter::SetImplementation(m_pResourceCounter);
		g2dFontUtil::SetImplementation(m_pFontUtil);
		g2dImageCreate::SetImplementation(m_pImageCreate);
		g2dImageSave::SetImplementation(m_pImageSave);

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

		g2dImageSave::SetImplementation(NULL);
		g2dImageCreate::SetImplementation(NULL);
		g2dFontUtil::SetImplementation(NULL);
		g2dResourceCounter::SetImplementation(NULL);
		delete m_pImageSave;
		delete m_pImageCreate;
		delete m_pFontUtil;
		delete m_pResourceCounter;
	}

	g2dWindow* g2dSystemVk::CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y)
	{
		appApplication::SetMainWindowSize(i_X, i_Y, i_Width, i_Height);
		g2dWindow* window = new g2dWindowVk(m_Device, appApplication::GetMainWindowHandle(), false);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateAppWindow(void* i_Handle)
	{
		g2dWindow* window = new g2dWindowVk(m_Device, i_Handle, false);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateSubWindow(void* i_Handle)
	{
		g2dWindow* window = new g2dWindowVk(m_Device, i_Handle, false);
		m_Windows.push_back(window);
		return window;
	}

	g2dWindow* g2dSystemVk::CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y)
	{
		itString title("");
		void* handle = appApplication::CreateSubWindow(i_Width, i_Height, i_X, i_Y, title);
		g2dWindow* window = new g2dWindowVk(m_Device, handle, true, i_Width, i_Height);
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
