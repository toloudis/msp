/*****************************************************************************
**  demG3dTestDrawStyle.cpp
**
**		This mode displays a demonstration/test of the draw style enumeration
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestDrawStyle.hpp"


#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlImport.hpp"


namespace
{
}

//====================================================================
//====================================================================
demG3dTestDrawStyle::demG3dTestDrawStyle(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestDrawStyle::~demG3dTestDrawStyle()
{
}

//====================================================================
//====================================================================
void demG3dTestDrawStyle::Initialize()
{
	demG3dTestMode::Initialize();

	// set short clipping for depth rendering tests
	//Camera().SetClip(1.0f, 16.0f);

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Draw Style Test"));
	m_Viewer.SetTextMessage(1, itString("The test demonstrates the drawing styles for a model"));
	m_Viewer.SetTextMessage(2, itString("Press 's' to change the drawing style"));
	m_Viewer.SetTextMessage(3, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(4, itString("Press space to go to the next section"));

	//	make fragments
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("bigship.mx");
	std::vector<g3dFragment*> shipFragments, shipLowResFragments, shipHighResFragments;
	mdlImport::LoadWorldFragments(locator, fsResourceFinderDir(fsLocator()),
							shipFragments,
							shipLowResFragments,
							shipHighResFragments,
							m_Materials,
							m_Textures);
	m_ShipFragment = shipHighResFragments[0];

	//	make models
	m_Ship = new g3dSceneNode(m_ShipFragment);
	m_Root->AddChild(m_Ship);
	m_Ship->SetRenderable(true);
	m_Ship->SetDrawStyle(g3dSceneNode::e_Wireframe);

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestDrawStyle::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestDrawStyle::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	//g3dScene::RemoveModel(m_Ship);

	//	cleanup fragments
	delete m_ShipFragment;

	// delete materials
	envSTLHelpers::DeleteContainer(m_Materials);

	//	release textures
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestDrawStyle::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('s'):
		case itString::CharType('S'):
			if (m_Ship->GetDrawStyle() == g3dSceneNode::e_Wireframe)
				m_Ship->SetDrawStyle(g3dSceneNode::e_LitWireframe);
			else if (m_Ship->GetDrawStyle() == g3dSceneNode::e_LitWireframe)
				m_Ship->SetDrawStyle(g3dSceneNode::e_Solid);
			else if (m_Ship->GetDrawStyle() == g3dSceneNode::e_Solid)
				m_Ship->SetDrawStyle(g3dSceneNode::e_Wireframe);
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}