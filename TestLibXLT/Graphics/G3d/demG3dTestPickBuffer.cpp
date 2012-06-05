/*****************************************************************************
**  demG3dTestPickBuffer.hpp
**
**		This mode displays a demonstration/test of using a renderer
**	to encode what object is visible at what pixel. 
**	A way to use the GPU to do picking.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestPickBuffer.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appMouseEvent.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/cam/camCameraManipOrbit.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "GraphicsDX9/g2d/g2dDX9GlobalWin.hpp"
#include "GraphicsDX9/g2d/g2dImageDX9.hpp"
#include "GraphicsDX9/g2d/g2dWindowDX9.hpp"
#include "GraphicsDX9/g3d/g3dPickBufferRendererDX9.hpp"
#include "GraphicsDX9/mat/matRenderTargetTexture.hpp"

#include <algorithm>

namespace
{

}

//====================================================================
//====================================================================
demG3dTestPickBuffer::demG3dTestPickBuffer(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pDirLight( NULL ), m_Root(NULL), m_Scene(NULL), 
	m_pPickRenderer(NULL),
	m_oldRenderer(NULL),
	m_pTargetRenderer(NULL),
//	m_PickBuffer(NULL),
	m_X(0), m_Y(0),
	m_ObjectCode(0)
{
	this->SetEnableMouseEvents(false);
}

//====================================================================
//====================================================================
demG3dTestPickBuffer::~demG3dTestPickBuffer()
{
}

//====================================================================
//====================================================================
void demG3dTestPickBuffer::Initialize()
{
	//this->Camera().SetOrthographic(true);
	//this->Camera().SetOrthoWidth(10);

	demG3dTestMode::Initialize();

	this->SetEnableMouseEvents(true);

	// Set up the pick buffer renderer
	m_pPickRenderer = new g3dPickBufferRendererDX9();
	m_oldRenderer = m_Viewer.GetRenderer();
	//m_Viewer.SetRenderer(m_pRenderer);

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Picking with a separate render action."));
	m_Viewer.SetTextMessage(2, itString("Press 'r' for regular renderer, 'p' for pick buffer"));
	m_Viewer.SetTextMessage(3, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(4, itString("Press space to go to the next section"));

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
	pObject->GetBase()->SetName("Cube");
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( 1.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Line Cube
	pFrag = g3dPrimitiveFragmentUtil::CreateLineCube( 0.25f );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetName("Line Cube");
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( 2.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Sphere
	pFrag = g3dPrimitiveFragmentUtil::CreateSphere( 0.25f, 10, 10 );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetName("Sphere");
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( -1.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Cone
	pFrag = g3dPrimitiveFragmentUtil::CreateCone( 0.25f, 0.5f, 10 );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetName("Cone");
	pObject->GetBase()->SetFragment( pFrag );
	pObject->SetPosition( maPoint3d( -2.0f, 0.0f, 0.0f ) );
	m_Root->AddChild( pObject->GetBase() );
	m_Objects.push_back( pObject );

	// Cylinder
	pFrag = g3dPrimitiveFragmentUtil::CreateCylinder( 0.25f, 0.25f, 0.5f, 10 );
	m_Fragments.push_back( pFrag );

	pObject = new scObject();
	pObject->GetBase()->SetName("Cylinder");
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

	// Create 1x1 buffer for rendering picking codes
	//m_PickBuffer = matTextureMgr::CreateRenderTargetTexture( 1, 1, false );
	//g2dRenderTarget* pTarget = m_PickBuffer->GetRenderTargetAPI();
	// Use back buffer for rendering. We will use a viewport call to reduce the
	// rendering to a single pixel.
	m_pTargetRenderer = new g3dTargetRenderer(m_Viewer.GetTarget(), m_pPickRenderer, 
		m_Scene, &m_PickCamera );
	m_pTargetRenderer->SetBackgroundColor(g2dRGBColor(0,0,0));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestPickBuffer::Think()
{
	demG3dTestMode::Think();

	char buffer[256];
	::sprintf(buffer, "Picking x: %d y: %d, code: %d named: %s", m_X, m_Y, m_ObjectCode, m_ShapeName.c_str());
	m_Viewer.SetTextMessage(1, itString(buffer));

	float frame_time = appTime::GetTime();
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestPickBuffer::DeInitialize()
{
	this->SetEnableMouseEvents(false);

	//delete m_pRootRenderState;
	//m_pRootRenderState = NULL;

	g3dLightMgr::DestroyLight( m_pDirLight );
	m_pDirLight = NULL;

	envSTLHelpers::DeleteContainer( m_Fragments );
	envSTLHelpers::DeleteContainer( m_Objects );
	
	//matTextureMgr::ReleaseTexture(m_PickBuffer);
	envSTLHelpers::ForAll( m_Textures, matTextureMgr::ReleaseTexture );
	m_Textures.clear();
	
	envSTLHelpers::DeleteContainer( m_Materials );

	m_Viewer.ClearTextMessages();
	delete m_pTargetRenderer;
	delete m_Root;
	delete m_Scene;

	m_Viewer.SetRenderer(m_oldRenderer);
	delete m_pPickRenderer;

	demG3dTestMode::DeInitialize();

}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestPickBuffer::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('r'):
		case itString::CharType('R'):
			m_Viewer.SetRenderer(m_oldRenderer);
		break;
		case itString::CharType('p'):
		case itString::CharType('P'):
			m_Viewer.SetRenderer(m_pPickRenderer);
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}

//====================================================================
//	Override this function to get appMouseUpEvents.
//====================================================================
void demG3dTestPickBuffer::ReceiveMouseUpEvent(appMouseUpEvent& i_Event)
{

}

//====================================================================
//	Override this function to get appMouseDownEvents.
//====================================================================
void demG3dTestPickBuffer::ReceiveMouseDownEvent(appMouseDownEvent& i_Event)
{

}

//====================================================================
//	Override this function to get appMouseDownEvents.
//====================================================================
void demG3dTestPickBuffer::ReceiveMouseMoveEvent(appMouseMoveEvent& i_Event)
{
	m_X = i_Event.GetX();
	m_Y = i_Event.GetY();

	int width = 0, height = 0;
	m_Viewer.GetWindow()->GetDimensions(width, height);

	if (m_X < width && m_Y < height)
	{
		float left = -1.0f + 2.0f * (m_X / float (width));
		float right = -1.0f + 2.0f * (m_X + 1) / float (width);
		float top = -1.0f + 2.0f * (m_Y / float (height));
		float bottom = -1.0f + 2.0f * (m_Y + 1) / float (height);

		// Set up the viewport of the camera to a single pixel
		m_PickCamera = this->GetCamera();
		m_PickCamera.SetSubViewport(top, bottom, left, right);

		// Set up subviewport in render buffer also
		D3DVIEWPORT9 old_viewport;
		g2dDX9Global::g_pDevice->GetViewport(&old_viewport);
		D3DVIEWPORT9 new_viewport;
		new_viewport.X      = 0;
		new_viewport.Y      = 0;
		new_viewport.Width  = 1;
		new_viewport.Height = 1;
		new_viewport.MinZ   = 0.0f;
		new_viewport.MaxZ   = 1.0f;
		g2dDX9Global::g_pDevice->SetViewport(&new_viewport);

		// Render pick buffer
		float frame_time = appTime::GetTime();
		m_pTargetRenderer->Render(frame_time);

		// Restore the old viewport
		g2dDX9Global::g_pDevice->SetViewport(&old_viewport);

		// Get out the pixel color	
		g2dWindowDX9 *pWindow = (g2dWindowDX9*) m_Viewer.GetWindow();
		g2dD3D9SurfacePtr pSurface = pWindow->GetBackBuffer();

		//matRenderTargetTexture* pTextureD3D = (matRenderTargetTexture*)m_PickBuffer;
		//g2dD3D9TexturePtr pTextureSurface = pTextureD3D->GetTextureD3D();
		//g2dD3D9SurfacePtr pSurface = 0;
		//pTextureSurface->GetSurfaceLevel( 0, &pSurface);

		// The image class wraps the surface and will release the surface for us
		g2dImageDX9 image_wrapper(pSurface); 

		// Get color at (0,0) pixel location (the single pixel viewport set above)
		envType::UInt8 red = 0, green = 0, blue = 0;
		image_wrapper.GetPixelColor(0, 0, red, green, blue);
		m_ObjectCode = g3dPickBufferRendererDX9::ConvertColorToIndex(red, green, blue);

		const g3dSceneNode *pNode = m_Root->GetPickedNode(m_ObjectCode);
		if (pNode)
		{
			m_ShapeName = pNode->GetName();
		}
		else
		{
			m_ShapeName = "No Node";
		}
	}
}
