/*****************************************************************************
**  demG3dTestRenderToTexture.cpp
**
**		This mode displays a demonstration/test of rendering to a texture.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestRenderToTexture.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX9/g2d/g2dDX9GlobalWin.hpp"
#include "GraphicsDX9/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX9/shdw/shdwShadowLayerRendererDX9.hpp"

#include <algorithm>

namespace
{
	const maFloatRGBA c_FogColor(1.0f, 0.5f, 0.5f, 0.5f);
	const float c_FogDensity = 0.05f;

	float l_ZoomScale = 0.50f;
}

//====================================================================
//====================================================================
demG3dTestRenderToTexture::demG3dTestRenderToTexture(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_WorldRoot(NULL), m_Scene(NULL),
	m_pRenderer(NULL)

{
	m_pZoomTexture = NULL;
}

//====================================================================
//====================================================================
demG3dTestRenderToTexture::~demG3dTestRenderToTexture()
{
}

//====================================================================
//====================================================================
void demG3dTestRenderToTexture::Initialize()
{
	demG3dTestMode::Initialize();

	m_WorldRoot = new g3dSceneNode();
	m_ScreenRoot = new g3dSceneNode();
	std::vector<g3dLayer*> layers;
	layers.push_back(new g3dLayer(m_WorldRoot, g3dLayer::e_ZBuffer,
								g3dLayer::e_World));
	//layers.push_back(new g3dLayer(m_CameraRoot, g3dLayer::e_ZBuffer,
	//							g3dLayer::e_Camera));
	layers.push_back(new g3dLayer(m_ScreenRoot, g3dLayer::e_ZBuffer,
								g3dLayer::e_Screen));
	m_Scene = new g3dScene(layers); // scene owns layers


	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Render to target test"));
	m_Viewer.SetTextMessage(4, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(5, itString("Press space to go to the next section"));


	//	make fragments
	m_CubeFragment = g3dPrimitiveFragmentUtil::CreateTexturedCube(3.0f);
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);
	m_ZoomFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.5f, 0.5f, 1, 1);


	//	setup fragment materials
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	effPhongData* pData = NULL;

	pData = dynamic_cast<effPhongData*>(m_CubeMat.GetEffectData());
    pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_NameDiffuse = "lblue0132.png";
	pData->ReloadTextures(locator);
	m_CubeTexture = pData->m_TextureDiffuse;
	m_CubeMat.SetShaderEffect(("Phong.fx"));

	pData = dynamic_cast<effPhongData*>(m_RectMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.0f, 0.8f, 0.8f, 1.0f));

	pData = dynamic_cast<effPhongData*>(m_ZoomMat.GetEffectData());
	m_ZoomMat.SetShaderEffect(("Phong.fx"));
	// make the render target texture
	m_pZoomTexture = matTextureMgr::CreateRenderTargetTexture( 256, 256 );
	pData->m_TextureDiffuse = m_pZoomTexture;

	m_CubeFragment->SetMaterial( &m_CubeMat );
	m_RectFragment->SetMaterial( &m_RectMat );
	m_ZoomFragment->SetMaterial( &m_ZoomMat );

	//	make models
	m_WorldRect = new g3dSceneNode(m_RectFragment);
	m_WorldRoot->AddChild( m_WorldRect ); 

	m_ZoomRect = new g3dSceneNode(m_ZoomFragment);
	m_ScreenRoot->AddChild( m_ZoomRect ); 

	maMatrix4x4 world_mat;
	world_mat.MakeRotateX( -maConstants::c_fPI_Div_2 );

	const int num_z = 20;
	const int num_x = 20;
	const float z_delta = 5.0f;
	const float x_delta = 5.0f;
	const float first_z = -(num_z / 2) * z_delta;
	const float first_x = -(num_x / 2) * x_delta;

	int x_num, z_num;

	for( z_num = 0 ; z_num < num_z ; z_num++ )
	{
		float zval = float(z_num) * z_delta + first_z;

		for( x_num = 0 ; x_num < num_x ; x_num++ )
		{
			float xval = float(x_num) * x_delta + first_x;

			world_mat.MakeTranslate(xval, 5.0f, zval);
			g3dSceneNode* new_model = new g3dSceneNode(m_CubeFragment, world_mat);
			m_WorldRoot->AddChild(new_model);
			m_Cubes.push_back(new_model);
		}
	}

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

	m_Scene->SetFog( g3dType::e_FogModeLinear, c_FogColor, 
		10.0f, 50.0f, c_FogDensity);

	// Create renderer for the texture that needs updating everyframe
	g2dRenderTarget* pTarget = m_pZoomTexture->GetRenderTargetAPI();
	m_pRenderer = new shdwShadowLayerRendererDX9();
	m_pZoomRenderer = new g3dTargetRenderer(pTarget, /*m_pRenderer*/m_Viewer.GetRenderer(), m_Scene, &Camera() );
	m_pZoomRenderer->SetBackgroundColor(g2dRGBColor(0x50, 0x20, 0x30));

}

//====================================================================
//	Think
//====================================================================
void demG3dTestRenderToTexture::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();
	m_Scene->UpdateWorldData();

	// setup the zoom camera
	float fov = Camera().GetFOV();
	Camera().SetFOV(fov * l_ZoomScale);
	
	
	// temp, try clearing depth buffer
	/*HRESULT op_result = g2dDX9Global::g_pDevice->Clear(	0,
														NULL,
														D3DCLEAR_ZBUFFER,
														D3DCOLOR_XRGB(0x00, 0x00, 0x00),
														0.0f,
														0);
	if( op_result != D3D_OK )
	{
		g2dDX9Global::PrintDXError(op_result);
		DBG_ASSERT0(op_result != D3D_OK, "Clear Failed");
		return;
	}*/

	// render to texture
	m_pZoomRenderer->Render(frame_time);

	// restore the main camera's zoom
	Camera().SetFOV(fov);

	//	render main scene
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );

	/*  -----------------------------------------
	//	draw some text stuff
	char text[256];
	float fov = camCamera::GetFOV();
	sprintf(text, "FOV: %.0f", 	(fov * l_ZoomScale) / maConstants::c_fAngleToRad);
 	std::vector<itString> text_strings;
	text_strings.push_back(itString(text));
	text_strings.push_back(itString("[, ] change fov"));

	g2dRGBColor text_color(0x50, 0x50, 0x90);

	int i;
	for( i = 0 ; i < text_strings.size() ; i++ )
	{
		g2dScreenDrawUtil::DrawText(	10,
										(i * 17) + 10,
										this->GetFont(),
										text_strings[i],
										text_color);

		text_color.SetRed(text_color.GetRed() + 10);
	}
		*/
}

//====================================================================
//====================================================================
void demG3dTestRenderToTexture::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup fragments
	delete m_CubeFragment;
	delete m_RectFragment;
	delete m_ZoomFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_CubeTexture);
	matTextureMgr::ReleaseTexture(m_pZoomTexture);
//	matTextureMgr::ReleaseTexture(m_pMaskTexture);

	demG3dTestMode::DeInitialize();

	m_Viewer.ClearTextMessages();

	delete m_WorldRoot;
	delete m_ScreenRoot;
	delete m_Scene;
	delete m_pZoomRenderer;
	delete m_pRenderer;

}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestRenderToTexture::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType(']'):
			l_ZoomScale += 0.1f;
			if( l_ZoomScale > 2.0f )
				l_ZoomScale = 2.0f;
		break;

		case itString::CharType('['):
			l_ZoomScale -= 0.1f;
			if( l_ZoomScale < 0.2f )
				l_ZoomScale = 0.2f;
		break;

		default:
			this->demG3dTestMode::ReceiveCharEvent(i_Event);
		break;
	}
}
