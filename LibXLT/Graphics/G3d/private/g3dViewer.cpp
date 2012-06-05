/****************************************************************************\
**	g3dViewer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dViewer.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"

#include <sstream>
#include <iomanip>


//----------------------------------------------------------------------------
// The root node is not owned by the layer, just pointed to
//----------------------------------------------------------------------------
g3dViewer::g3dViewer()
:	g3dTargetRenderer(NULL, NULL, NULL, NULL),
	m_pWindow(NULL),
	m_fCalculatedFrameRate(0.0f)
{
	//	make our font
	m_Font = g2dFontUtil::LoadFont(itString("Arial"), 16);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dViewer::g3dViewer(g2dWindow* i_pWindow, g3dSceneRenderer* i_pRenderer)
:	g3dTargetRenderer(i_pWindow, i_pRenderer, NULL, NULL),
	m_pWindow(i_pWindow),
	m_fCalculatedFrameRate(0.0f)
{
	//	make our font
	m_Font = g2dFontUtil::LoadFont(itString("Arial"), 16);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dViewer::g3dViewer(g2dWindow* i_pWindow, g3dSceneRenderer* i_pRenderer,
					 g3dScene* i_pScene, camCamera* i_pCamera)
:	g3dTargetRenderer(i_pWindow, i_pRenderer, i_pScene, i_pCamera),
	m_pWindow(i_pWindow),
	m_fCalculatedFrameRate(0.0f)
{
	//	make our font
	m_Font = g2dFontUtil::LoadFont(itString("Arial"), 16);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dViewer::~g3dViewer()
{
	//	release our font
	g2dFontUtil::ReleaseFont(m_Font);
}

//----------------------------------------------------------------------------
// The window is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
g2dWindow* g3dViewer::GetWindow() const
{
	return m_pWindow;
}
void g3dViewer::SetWindow(g2dWindow* i_pWindow)
{
	g3dTargetRenderer::SetTarget(i_pWindow);
	m_pWindow = i_pWindow;
}

//----------------------------------------------------------------------------
// render anything needed after renderer is done
//----------------------------------------------------------------------------
void g3dViewer::RenderOverlay()
{
	//char num_tris_str[64];
	//sprintf(num_tris_str, "num prims: %d", this->GetNumPrimitivesRendered());

	//	DISPLAY: Number of primitives
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss <<"Num of Prims: "<<this->GetNumPrimitivesRendered();
	std::string num_tris_str(oss.str());

	m_pWindow->SetDebugInfo(6, num_tris_str.c_str());

	//	DISPLAY: Render FPS
	std::ostringstream oss2;
	oss2.clear();
	oss2.setf(std::ios::fixed, std::ios::floatfield);
	oss2 << "Render FPS: " << std::setprecision(2) << (1.0f / m_fCalculatedFrameRate) << " " << std::setprecision(1) << std::setw(3) << 1000.0f * m_fCalculatedFrameRate << " ms";
	std::string render_FPS_str(oss2.str());

	m_pWindow->SetDebugInfo(8, render_FPS_str.c_str());

/*
	if( g3dPrefs::CurrentPrefs().m_bEnableTransparent && g3dPrefs::CurrentPrefs().m_TransparencyMode == 1 )
	{
		char num_pass_str[64];
		sprintf(num_pass_str, "transparency depth Peel passes: %d", this->GetMaxDepthPeelPasses());
		m_pWindow->SetDebugInfo(3, num_pass_str );
	}
*/

	// Display text messages
	g2dRGBColor text_color(0x00,0xff,0x00);
	std::vector<itString>::const_iterator it, end = m_Messages.end();
	int offset = 0; 
	for ( it = m_Messages.begin(); it != end; ++it, ++offset )
	{
		int y = (offset * 15) + 30;
		int x = 10;

		m_pWindow->DrawText( x, y, m_Font, (*it), text_color );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dViewer::Present()
{
	m_pWindow->Present();
}

//----------------------------------------------------------------------------
// Set text string to display on given line.
//----------------------------------------------------------------------------
void g3dViewer::SetTextMessage(int i, const itString &i_Msg)
{
	if (i >= m_Messages.size())
		m_Messages.resize(i+1, itString(""));

	m_Messages[i] = i_Msg;
}

//----------------------------------------------------------------------------
// Remove any text messages
//----------------------------------------------------------------------------
void g3dViewer::ClearTextMessages()
{
	m_Messages.clear();
}

//----------------------------------------------------------------------------
// Add TargetRenderer to management
//----------------------------------------------------------------------------
void g3dViewer::AddTargetRenderer( g3dTargetRenderer *i_pTargetRenderer )
{
	m_TargetRenderers.push_back(i_pTargetRenderer);
}

//----------------------------------------------------------------------------
// Remove TargetRenderer from management
//----------------------------------------------------------------------------
void g3dViewer::RemoveTargetRenderer( g3dTargetRenderer *i_pTargetRenderer )
{
	bool removed = envSTLHelpers::RemoveOneValue(m_TargetRenderers, i_pTargetRenderer);
}

//------------------------------------------------------------------------
//	This is an externally calculated frame rate that should encapsulate
//	only the time the app is rendering.  It should not include time for
//	UI updating, File I/O, etc.
//	This value will be displayed in the debug display ('~').
//------------------------------------------------------------------------
void g3dViewer::SetFrameRateRunningAverage( float i_FPS )
{
	m_fCalculatedFrameRate = i_FPS;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dViewer::RenderTargets( float i_fSimTime )
{
	int nTris = 0;
	int nTargets = m_TargetRenderers.size();
	for (int i=0; i<nTargets; i++)
	{
		m_TargetRenderers[i]->SetCamera(GetCamera());
		m_TargetRenderers[i]->Render(i_fSimTime);
		nTris += m_TargetRenderers[i]->GetNumPrimitivesRendered();
	}
	m_NumPrimitivesRendered += nTris;
}

