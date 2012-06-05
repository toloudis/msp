/*****************************************************************************
**  demG3dTestSubWindow.hpp
**
**		This mode displays a demonstration/test of prelit textured and
**		non-textured fragments
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestSubWindow.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/cam/camCameraManipOrbit.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"

#include <algorithm>

namespace
{

}

//====================================================================
//====================================================================
demG3dTestSubWindow::demG3dTestSubWindow(g2dSystem &i_System, g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_System(i_System), m_pSubWindow(NULL),
	m_pDirLight( NULL ), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestSubWindow::~demG3dTestSubWindow()
{
}

//====================================================================
//====================================================================
void demG3dTestSubWindow::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Sub window test."));
	m_Viewer.SetTextMessage(1, itString("Both windows are rendering the same scene."));
	m_Viewer.SetTextMessage(2, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(3, itString("Press space to go to the next section"));

	// Camera
	this->SetPitch( maConstants::c_fAngleToRad * 45.0f );
	this->SetYaw( 0.2f );
	this->SetRadius( 4.0f );

//	fsLocator tex_loc( sgPaths::e_RootData );

	scObject *pObject;
	g3dFragment *pFrag;

	//matMaterial& def_mat =  g3dPrimitiveFragmentUtil::DefaultMaterial();
	//def_mat.SetEmissive(0,1,0,1);

	// Cube
	pFrag = g3dPrimitiveFragmentUtil::CreateCube( 0.25f );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( 1.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Line Cube
	pFrag = g3dPrimitiveFragmentUtil::CreateLineCube( 0.25f );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( 2.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Sphere
	pFrag = g3dPrimitiveFragmentUtil::CreateSphere( 0.25f, 10, 10 );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( -1.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Cone
	pFrag = g3dPrimitiveFragmentUtil::CreateCone( 0.25f, 0.5f, 10 );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( -2.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Cylinder
	pFrag = g3dPrimitiveFragmentUtil::CreateCylinder( 0.25f, 0.25f, 0.5f, 10 );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( 0.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Set up lights
	m_pDirLight = g3dLightMgr::CreateDirectionalLight();
	m_pDirLight->SetDirection( maVector3d( 0.5f, -1.0f, -0.25f ) );
	m_pDirLight->SetIntensity( maFloatRGBA( 0.5f, 0.5f, 0.5f, 1.0f ) ); 
	m_pDirLight->Enable();

	g3dLightMgr::SetAmbient( maFloatRGBA(0.25f, 0.25f, 0.5f, 1.0f ) );

	// Set up root render state
	//m_pRootRenderState = new g3dRenderState();
	//m_pRootRenderState->m_Lights.push_back( m_pDirLight );
	//m_pRootRenderState->m_AmbientLight.Set( 0.25f, 0.25f, 0.5f, 1.0f );
	//pRoot->SetRenderState( m_pRootRenderState );

	//----------------------------------------
	// Set up second window
	//

	int width = 200, height = 200;
	int xpos = 200, ypos = 200;
	m_pSubWindow = m_System.CreateSubWindow(width, height, xpos, ypos);
	
	// set up renderer and viewer for this window
	m_pSubViewer = new g3dViewer(m_pSubWindow, m_Viewer.GetRenderer());
	m_pSubViewer->SetBackgroundColor( g2dRGBColor(0x50, 0x20, 0x30) );

	m_pSubViewer->SetScene(m_Scene);
	m_pSubViewer->SetCamera(&Camera());
}

//====================================================================
//	Think
//====================================================================
void demG3dTestSubWindow::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	// Subwindow test
	m_pSubViewer->Render(frame_time);
//	m_pSubWindow->Clear(g2dRGBColor(0x50, 0x20, 0x30));
//	m_pSubWindow->DrawText(50, 50, m_FontLarge, itString("2"), g2dRGBColor(0xf0, 0x90, 0x20));
//	m_pSubWindow->EndScene();
	m_pSubViewer->Present();


	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestSubWindow::DeInitialize()
{
	delete m_pSubViewer;

	if (m_pSubWindow)
		m_System.DestroyWindow(m_pSubWindow);
	m_pSubWindow = NULL;

	//delete m_pRootRenderState;
	//m_pRootRenderState = NULL;

	g3dLightMgr::DestroyLight( m_pDirLight );
	m_pDirLight = NULL;

	envSTLHelpers::DeleteContainer( m_Fragments );
	envSTLHelpers::DeleteContainer( m_Objects );
	
	envSTLHelpers::ForAll( m_Textures, matTextureMgr::ReleaseTexture );
	m_Textures.clear();
	
	envSTLHelpers::DeleteContainer( m_Materials );

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();

}
