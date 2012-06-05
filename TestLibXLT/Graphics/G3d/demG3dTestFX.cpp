/*****************************************************************************
**  demG3dTestFX.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestFX.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "demG3dTestRenderer.hpp"
#include "demModeManager.hpp"
#include "GraphicsDX9/eff/effShaderArray.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Core/gf/gfPaths.hpp"

namespace
{
struct RangedFloat
{
	float m_Min, m_Max, m_Inc;
	RangedFloat(float mi, float ma, float in):m_Min(mi),m_Max(ma),m_Inc(in){}
	float Dec(float val) const
	{
		val -= m_Inc;
		if (val < m_Min) 
			val = m_Min;
		return val;
	}
	float Inc(float val) const
	{
		val += m_Inc;
		if (val > m_Max) 
			val = m_Max;
		return val;
	}
};

}

//====================================================================
//====================================================================
demG3dTestFX::demG3dTestFX(g3dViewer &i_Viewer, demG3dTestRenderer* i_Renderer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pRenderer(i_Renderer),
	m_oldRenderer(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestFX::~demG3dTestFX()
{
}

//====================================================================
//====================================================================
void demG3dTestFX::Initialize()
{
	demG3dTestMode::Initialize();

	m_oldRenderer = m_Viewer.GetRenderer();
	m_Viewer.SetRenderer(m_pRenderer);
	m_Viewer.GetWindow()->ToggleDebugOverlay();

	// set up display info
//	m_Viewer.SetTextMessage(0, itString("Ambient Env Map test."));

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene();

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());
	Camera().SetClip(1.0f, 255.0f);
}

//====================================================================
//	Think
//====================================================================
void demG3dTestFX::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestFX::DeInitialize()
{
	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;
	m_Viewer.SetRenderer(m_oldRenderer);
	m_Viewer.GetWindow()->ToggleDebugOverlay();
	delete m_pRenderer;
	m_pRenderer = NULL;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestFX::ReceiveCharEvent(appCharEvent& i_Event)
{
	static bool bShown = false;
	static int iMode = 0;

	m_pRenderer->HandleChar(i_Event.GetChar());

	demG3dTestMode::ReceiveCharEvent(i_Event);
}
