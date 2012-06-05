/*****************************************************************************
**  demG3dTestProjectedLights.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestProjectedLights.hpp"


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
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mdl/mdlImport.hpp"
#include "GraphicsDX9/g3d/g3dDepthMapRendererDX9.hpp"
#include "GraphicsDX9/g3d/g3dSceneRendererDX9.hpp"


namespace
{
const int c_NumShaderMats = 1;
const float c_TimePerShader = 2.0f;
bool l_bRotateShader = true;

const float c_AngleIncrement = 5.0f;
const float c_MaxAngle = 85.0f;
const float c_MinAngle = 5.0f;

const float c_ScaleIncrement = 2.0f;
const float c_MaxScale = 64.0f;
const float c_MinScale = 0.004f;

const float c_AspectIncrement = 2.0f;
const float c_MaxAspect = 64.0f;
const float c_MinAspect = 0.004f;

}

//====================================================================
//====================================================================
demG3dTestProjectedLights::demG3dTestProjectedLights(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pEffect(NULL), m_Texture1(NULL), m_FollowCamera(true),
	m_pProjLight(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestProjectedLights::~demG3dTestProjectedLights()
{
}

//====================================================================
//====================================================================
void demG3dTestProjectedLights::Initialize()
{
	demG3dTestMode::Initialize();

//	Camera().SetClip(1.0f, 16.0f);

	m_Root = new g3dSceneNode();
	//m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer
	m_Scene = new g3dScene(new g3dLayer(m_Root, 
		g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative,
		true, true)); // scene owns layer

	// parallel scene for testing/shadow casting
	m_Root2 = new g3dSceneNode();
	m_Scene2 = new g3dScene(new g3dLayer(m_Root2)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Projected light test."));
	m_Viewer.SetTextMessage(1, itString("The light is attached to the camera's position."));
	m_Viewer.SetTextMessage(2, itString("Press 'f' to toggle the attachment, then move the camera to see the shadows."));
	m_Viewer.SetTextMessage(4, itString("[/] - change angle, s/S - scale, a/A - aspect ratio of light"));
	m_Viewer.SetTextMessage(5, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(6, itString("Press space to go to the next section"));

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

	std::vector<g3dFragment*> ship2Fragments, ship2LowResFragments, ship2HighResFragments;
	mdlImport::LoadWorldFragments(locator, fsResourceFinderDir(fsLocator()),
							ship2Fragments,
							ship2LowResFragments,
							ship2HighResFragments,
							m_Materials,
							m_Textures);
	m_Ship2Fragment = ship2HighResFragments[0];

	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);
	m_RectFragment->SetReceivesShadow(true);

	//	make models
	m_Ship = new g3dSceneNode(m_ShipFragment);
	m_Root->AddChild(m_Ship);
	m_Ship->SetRenderable(true);
	m_ShipFragment->SetMaterial( &m_ShaderMat );

	m_Ship2 = new g3dSceneNode(m_Ship2Fragment);
	m_Root2->AddChild(m_Ship2);
	m_Ship2->SetRenderable(true);
	m_Ship2Fragment->SetMaterial( &m_TexMat );

	maMatrix4x4 rect_mat;
	rect_mat.MakeRotate(-maConstants::c_fPI_Div_2, maVector3d(1,0,0));
	rect_mat.TranslateBy(0,-10,0);
	m_WorldRect = new g3dSceneNode(m_RectFragment, rect_mat);
	m_Root->AddChild(m_WorldRect);
	m_RectFragment->SetMaterial( &m_ShaderMat );

	// make the render target texture
	m_ShadowMap = matTextureMgr::CreateRenderTargetTexture( 512, 512, true );
	// load texture to project
	fsLocator tex_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	tex_loc.Push("data");
	tex_loc.Push("checkerboard.png");
	m_Texture1 = matTextureMgr::LoadTexture(tex_loc);

	g3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));

	// Create projected light
	//m_pProjLight = new g3dProjectedLight();
	m_pProjLight = g3dLightMgr::CreateProjectedLight();
	m_pProjLight->Enable();
	m_pProjLight->SetTexture(m_Texture1);
	m_pProjLight->SetShadowMap(m_ShadowMap);

	// Create renderer for the texture that needs updating everyframe
	g2dRenderTarget* pTarget = m_ShadowMap->GetRenderTargetAPI();
	m_pDepthRenderer = new g3dDepthMapRendererDX9();
	m_pMapRenderer = new g3dTargetRenderer(pTarget, m_pDepthRenderer, m_Scene2, &m_ShadowCamera );
	m_pMapRenderer->SetBackgroundColor(g2dRGBColor(0xff, 0xff, 0xff));

	// need single light rendering to do a projected light
	g3dSingleLightRendering::SetDoSingleLightRendering(true);
}

//====================================================================
//	Think
//====================================================================
void demG3dTestProjectedLights::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();
	m_Scene->UpdateWorldData();
	m_Scene2->UpdateWorldData();

	if (m_FollowCamera)
	{
		m_pProjLight->SetPosition(Camera().GetPosition());
		m_pProjLight->SetTarget(Camera().GetPosition() + Camera().GetDirection());

		// Update projection light to match camera's position
		maMatrix4x4 proj_matx, cam_matx, shift_matx; 
		shift_matx.MakeScale(-0.5f, -0.5f, 1.0f);
		shift_matx.TranslateBy(0.5f, 0.5f, -0.002f);
		Camera().GetCameraMatrix(cam_matx);
		Camera().GetProjectionMatrix(proj_matx);
		m_ProjMatrix = cam_matx * proj_matx * shift_matx; 

		m_LightPos = Camera().GetPosition();

		// render to texture, capturing depth buffer
		m_pProjLight->OrientCamera(m_ShadowCamera);
		m_pMapRenderer->Render(frame_time);
	}
//	m_pEffect->SetupProjectedLight(m_pProjLight->GetTexture(), 
//		m_pProjLight->GetShadowMap(), 
//		m_pProjLight->GetTotalMatrix(), 
//		m_pProjLight->GetPosition(), 
//		m_pProjLight->GetLightSize(), 
//		m_pProjLight->GetSceneScale(),
//		m_pProjLight->GetShadowIntensity());

	//	render
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestProjectedLights::DeInitialize()
{
	// restore single light rendering flag
	g3dSingleLightRendering::SetDoSingleLightRendering(false);

	delete m_pEffect;

	//	cleanup fragments
	delete m_ShipFragment;
	delete m_RectFragment;
	delete m_Ship2Fragment;

	// delete materials
	envSTLHelpers::DeleteContainer(m_Materials);

	//	release textures
	matTextureMgr::ReleaseTexture(m_Texture1);
	matTextureMgr::ReleaseTexture(m_ShadowMap);
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture);

	delete m_Root;
	delete m_Scene;
	delete m_Root2;
	delete m_Scene2;
	delete m_pMapRenderer;
	delete m_pDepthRenderer;

	g3dLightMgr::DestroyLight( m_pProjLight );

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestProjectedLights::ReceiveCharEvent(appCharEvent& i_Event)
{
	float angle, scale, aspect;
	switch( i_Event.GetChar() )
	{
		case itString::CharType('f'):
		case itString::CharType('F'):
			m_FollowCamera = !m_FollowCamera;
		break;
		case itString::CharType('['):
			angle = m_pProjLight->GetAngle();
			angle -= c_AngleIncrement;
			if (angle < c_MinAngle) angle = c_MinAngle;
			m_pProjLight->SetAngle(angle);
		break;
		case itString::CharType(']'):
			angle = m_pProjLight->GetAngle();
			angle += c_AngleIncrement;
			if (angle > c_MaxAngle) angle = c_MaxAngle;
			m_pProjLight->SetAngle(angle);
		break;
		case itString::CharType('s'):
			scale = m_pProjLight->GetScale();
			scale /= c_ScaleIncrement;
			if (scale < c_MinScale) scale = c_MinScale;
			m_pProjLight->SetScale(scale);
		break;
		case itString::CharType('S'):
			scale = m_pProjLight->GetScale();
			scale *= c_ScaleIncrement;
			if (scale > c_MaxScale) scale = c_MaxScale;
			m_pProjLight->SetScale(scale);
		break;
		case itString::CharType('a'):
			aspect = m_pProjLight->GetAspect();
			aspect /= c_AspectIncrement;
			if (aspect < c_MinAspect) aspect = c_MinAspect;
			m_pProjLight->SetAspect(aspect);
		break;
		case itString::CharType('A'):
			aspect = m_pProjLight->GetAspect();
			aspect *= c_AspectIncrement;
			if (aspect > c_MaxAspect) aspect = c_MaxAspect;
			m_pProjLight->SetAspect(aspect);
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}