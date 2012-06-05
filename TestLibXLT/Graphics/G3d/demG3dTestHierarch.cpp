/*****************************************************************************
**  demG3dTestHierarch.hpp
**
**		This mode displays a demonstration/test of a hierarchical model
**	used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestHierarch.hpp"

#include "Core/app/appTime.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//====================================================================
//====================================================================
demG3dTestHierarch::demG3dTestHierarch(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestHierarch::~demG3dTestHierarch()
{
}

//====================================================================
//====================================================================
void demG3dTestHierarch::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layers
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("This slide shows a hierarchical model."));
	m_Viewer.SetTextMessage(1, itString("Each level of the model has a transformation to the level above it and"));
	m_Viewer.SetTextMessage(2, itString("is rendered using forward kinematics."));
	m_Viewer.SetTextMessage(4, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(5, itString("Press space to go to the next section"));

	//	make fragments
	m_BaseFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(6.0f, 20, 20);
	m_Arm1Fragment = g3dPrimitiveFragmentUtil::CreateTexturedBlock(2, 2, 20);
	m_Arm2Fragment = g3dPrimitiveFragmentUtil::CreateTexturedBlock(2, 2, 10);

	//	setup fragment materials
	effPhongData* pData = NULL;
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	pData = dynamic_cast<effPhongData*>(m_Mat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_NameDiffuse = "lgren041.png";
	pData->ReloadTextures(locator);
	m_Texture = pData->m_TextureDiffuse;

	m_BaseFragment->SetMaterial(&m_Mat);
	m_Arm1Fragment->SetMaterial(&m_Mat);
	m_Arm2Fragment->SetMaterial(&m_Mat);

	//	make models
	m_Model = new g3dSceneNode();
	m_Model->SetFragment(m_BaseFragment);
	m_Root->AddChild(m_Model);

	g3dSceneNode* child1 = new g3dSceneNode();
	child1->SetFragment(m_Arm1Fragment);
	m_Model->AddChild(child1);

	g3dSceneNode* child2 = new g3dSceneNode();
	child2->SetFragment(m_Arm2Fragment);
	child1->AddChild(child2);

	child2 = new g3dSceneNode();
	child2->SetFragment(m_Arm2Fragment);
	child1->AddChild(child2);

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestHierarch::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	set the orientations of our model
	maMatrix4x4 child_matrix1 = m_Model->GetChild(0)->GetTransform();
	child_matrix1.Identity();
	child_matrix1.RotateBy( frame_time * 30.0f * maConstants::c_fAngleToRad, maVector3d(0, 1, 0));
	m_Model->GetChild(0)->SetTransform(child_matrix1);

	maMatrix4x4 child_matrix11 = m_Model->GetChild(0)->GetChild(0)->GetTransform();
	child_matrix11.Identity();
	child_matrix11.RotateBy( -maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
	child_matrix11.RotateBy( -frame_time * 90.0f * maConstants::c_fAngleToRad, maVector3d(0, 0, 1) );
	child_matrix11.TranslateBy(0.0f, 0.0f, 11.0f);
	m_Model->GetChild(0)->GetChild(0)->SetTransform(child_matrix11);

	maMatrix4x4 child_matrix12 = m_Model->GetChild(0)->GetChild(1)->GetTransform();
	child_matrix12.Identity();
	child_matrix12.RotateBy( -maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
	child_matrix12.RotateBy( frame_time * 90.0f * maConstants::c_fAngleToRad, maVector3d(0, 0, 1) );
	child_matrix12.TranslateBy(0.0f, 0.0f, -11.0f);
	m_Model->GetChild(0)->GetChild(1)->SetTransform(child_matrix12);

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestHierarch::DeInitialize()
{

	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Model);

	//	cleanup fragments
	delete m_BaseFragment;
	delete m_Arm1Fragment;
	delete m_Arm2Fragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_Texture);

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
