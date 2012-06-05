/*****************************************************************************
**  demG3dTestCubeMap.cpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "envPlatform.hpp"

#include "demG3dTestCubeMap.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightManager.hpp"
#include "g3dLayer.hpp"
//#include "matPlainTexture.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "matStaticCubeTexture.hpp"
#include "matTextureManager.hpp"
#include "matTextureManagerPAC.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

namespace
{
}

//====================================================================
//====================================================================
demG3dTestCubeMap::demG3dTestCubeMap(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL),
	m_CubeTexture(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestCubeMap::~demG3dTestCubeMap()
{
}

//====================================================================
//====================================================================
void demG3dTestCubeMap::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	//	make fragments
	m_CubeFragment = g3dPrimitiveFragmentUtil::CreateCube(3.0f);
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateSphere(5.0f, 50, 50);
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);

	//	setup fragment materials
	m_CubeMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_SphereMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_RectMat.SetDiffuse(maFloatRGBA(0.0f, 0.8f, 0.8f, 1.0f));

	// make a nice texture
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("lblue0132.png"));
	if( matTextureManager::GetStaticCubeMapSupported() )
	{
		fsLocator base = gfPaths::GetGamePath(gfPaths::e_ExePath);
		base.Push("data");
		fsLocator files[6];
		files[0] = files[1] = files[2] = files[3] = files[4] = files[5] = base;
		files[0].Push("View1.png");
		files[1].Push("View3.png");
		files[2].Push("View5.png");
		files[3].Push("View6.png");
		files[4].Push("View2.png");
		files[5].Push("View4.png");
		m_CubeTexture = matTextureManagerPAC::LoadStaticCubeTexture(files);
		m_CubeMat.AddTextureTop(m_CubeTexture);
		m_SphereMat.AddTextureTop(m_CubeTexture);
	}

	m_RectTexture = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture);

	m_CubeFragment->SetMaterial(&m_CubeMat);
	m_SphereFragment->SetMaterial(&m_SphereMat);
	m_RectFragment->SetMaterial(&m_RectMat);

	//	make models
	maMatrix4x4 world_mat;

	m_CubeModel = new g3dSceneNode(m_CubeFragment);
	m_Root->AddChild(m_CubeModel);
	m_SphereModel = new g3dSceneNode(m_SphereFragment);
	m_Root->AddChild(m_SphereModel);
	m_RectModel = new g3dSceneNode(m_RectFragment);
	m_Root->AddChild(m_RectModel);

	m_RectModel->GetTransform().MakeRotateX(-maConstants::c_fPI_Div_2);
	m_CubeModel->GetTransform().MakeTranslate(5, 10, 5);
	m_SphereModel->GetTransform().MakeTranslate(-5, 10, 5);

	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestCubeMap::Think()
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
void demG3dTestCubeMap::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_RectModel);
	//g3dRenderer::RemoveModel(m_CubeModel);
	//g3dRenderer::RemoveModel(m_SphereModel);

	//	cleanup fragments
	delete m_CubeFragment;
	delete m_SphereFragment;
	delete m_RectFragment;

	//	release textures
	if( m_CubeTexture )
		matTextureManagerPAC::DestroyTexture(m_CubeTexture);

	matTextureManager::ReleaseTexture(m_RectTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
