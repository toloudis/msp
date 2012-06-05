/*****************************************************************************
**  demG3dTestModelMaterial.cpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestModelMaterial.hpp"

#include "Core/app/appTime.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

namespace
{

}

//====================================================================
//====================================================================
demG3dTestModelMaterial::demG3dTestModelMaterial(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestModelMaterial::~demG3dTestModelMaterial()
{
}

//====================================================================
//====================================================================
void demG3dTestModelMaterial::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("These spheres use the same fragment, but have separate materials assigned."));
	
	//	make fragments
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f, 50, 50);

	//	setup fragment materials
	effPhongData* pData = NULL;
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	pData = dynamic_cast<effPhongData*>(m_Mat1.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_NameDiffuse = "lblue0132.png";
	pData->ReloadTextures(locator);
	m_Texture1 = pData->m_TextureDiffuse;

	pData = dynamic_cast<effPhongData*>(m_Mat2.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.8f, 0.2f, 0.8f, 1.0f));
	pData->m_NameDiffuse = "lgrey020.png";
	pData->ReloadTextures(locator);
	m_Texture2 = pData->m_TextureDiffuse;

	pData = dynamic_cast<effPhongData*>(m_Mat3.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.0f, 0.8f, 0.2f, 1.0f));

	//	make models
	maMatrix4x4 world_mat;

	world_mat.MakeTranslate(7, 0, 7);
	m_Sphere1 = new g3dSceneNode(m_SphereFragment, world_mat);
	m_Root->AddChild(m_Sphere1);
	m_Sphere1->SetMaterial(&m_Mat1);

	world_mat.MakeTranslate(-7, 0, 7);
	m_Sphere2 = new g3dSceneNode(m_SphereFragment, world_mat);
	m_Root->AddChild(m_Sphere2);
	m_Sphere2->SetMaterial(&m_Mat2);

	world_mat.MakeTranslate(7, 0, -7);
	m_Sphere3 = new g3dSceneNode(m_SphereFragment, world_mat);
	m_Root->AddChild(m_Sphere3);
	m_Sphere3->SetMaterial(&m_Mat3);

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
void demG3dTestModelMaterial::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	set the orientations of our cubes

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestModelMaterial::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Sphere1);
	//g3dRenderer::RemoveModel(m_Sphere2);
	//g3dRenderer::RemoveModel(m_Sphere3);

	//	cleanup fragments
	delete m_SphereFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_Texture1);
	matTextureMgr::ReleaseTexture(m_Texture2);

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
