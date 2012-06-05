/*****************************************************************************
**  demG3dTestTextureCompression.hpp
**
**		This mode displays a demonstration/test of the compressed textures 
**	functionality available in the engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestTextureCompression.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "fsFileUtil.hpp"
#include "g2dFontUtil.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightManager.hpp"
//#include "matMipTexture.hpp"
#include "g3dLayer.hpp"
//#include "matPlainTexture.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "matTextureManager.hpp"
#include "matUVATexture.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

#include <algorithm>

//====================================================================
//====================================================================
demG3dTestTextureCompression::demG3dTestTextureCompression(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestTextureCompression::~demG3dTestTextureCompression()
{
}

//====================================================================
//====================================================================
void demG3dTestTextureCompression::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	g3dLayer *screen_layer = new g3dLayer(m_Root, g3dLayer::e_ZSort, g3dLayer::e_Screen);
	m_Scene = new g3dScene(screen_layer); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.9f, 0.9f, 1, 1);
	m_DDSRectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.9f, 0.9f, 1, 1);

	//	setup fragment materials
	m_RectMat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_DDSRectMat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));

	m_RectMat.SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_DDSRectMat.SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));


	//	add some textures
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("DDSTexture.png"));
	m_RectTexture = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture);
	m_RectMat.SetRotation(maConstants::c_fPI, 0);

	locator.Pop();
	locator.Push(itString("DDSTexture.dds"));
	m_DDSRectTexture = matTextureManager::LoadTexture(locator);
	m_DDSRectMat.AddTextureTop(m_DDSRectTexture);
	m_DDSRectMat.SetRotation(maConstants::c_fPI, 0);


	//	set the materials for the fragments
	m_RectFragment->SetMaterial(&m_RectMat);
	m_DDSRectFragment->SetMaterial(&m_DDSRectMat);

	//	make models
	maMatrix4x4 world_mat;
	world_mat.MakeTranslate(-0.5f, 0, 0);

	m_Rect = new g3dSceneNode(m_RectFragment, world_mat);
	m_Root->AddChild(m_Rect);
	m_Rect->SetRenderable(true);

	world_mat.MakeTranslate(0.5f, 0, 0);
	m_DDSRect = new g3dSceneNode(m_DDSRectFragment, world_mat);
	m_Root->AddChild(m_DDSRect);
	m_DDSRect->SetRenderable(true);
	
	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestTextureCompression::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestTextureCompression::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Rect);
	//g3dRenderer::RemoveModel(m_DDSRect);

	//	cleanup fragments
	delete m_RectFragment;
	delete m_DDSRectFragment;

	//	release textures
	matTextureManager::ReleaseTexture(m_RectTexture);
	matTextureManager::ReleaseTexture(m_DDSRectTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
