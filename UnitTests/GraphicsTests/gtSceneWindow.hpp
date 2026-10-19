/*****************************************************************************
**  gtSceneWindow.hpp
**
**      A g2dWindow on a hidden native window, made by the g2dSystem of
**      whichever backend is up (gtBackend), and what each frame showed:
**      Vulkan reads the presented swap-chain image, DX11 its back buffer
**      just before presenting.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "gtBackend.hpp"
#include "gtTarget.hpp"

class g2dWindow;

class gtSceneWindow
{
	public:
		gtSceneWindow(gtBackend::Kind i_Kind, int i_Width, int i_Height);
		~gtSceneWindow();

		g2dWindow& Window()		{ return *m_pWindow; }

		void Resize(int i_Width, int i_Height);

		//	Presents the frame and returns what it showed (RGBA, top row
		//	first; the window's size)
		gtPicture Present();

	private:
		gtSceneWindow(const gtSceneWindow&) = delete;
		gtSceneWindow& operator = (const gtSceneWindow&) = delete;

		gtPicture read_dx11();

		gtBackend::Kind	m_Kind;
		void*			m_pNative;
		g2dWindow*		m_pWindow;
};
