/*****************************************************************************
**  demG3dTestPivot.hpp
**
**		This mode displays a demonstration/test of transformations around 
**	a pivot point.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestPivot.hpp"

#include "Core/app/appCharEvent.hpp"
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
#include "Graphics/sc/scObject.hpp"

//====================================================================
//====================================================================
demG3dTestPivot::demG3dTestPivot(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestPivot::~demG3dTestPivot()
{
}

//====================================================================
//====================================================================
void demG3dTestPivot::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layers
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("This slide demonstrates pivot points."));
	m_Viewer.SetTextMessage(1, itString("Choose a pivot point for the model with the keys 1,2,3"));
	m_Viewer.SetTextMessage(2, itString("Keys 4,5,6 change the pivot without preserving the transform."));
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
	m_Model = new scObject();
	m_Model->GetBase()->SetFragment(m_BaseFragment);
	m_Root->AddChild(m_Model->GetBase());

	g3dSceneNode* child1 = new g3dSceneNode();
	child1->SetFragment(m_Arm1Fragment);
	m_Model->GetBase()->AddChild(child1);

	g3dSceneNode* child2 = new g3dSceneNode();
	child2->SetFragment(m_Arm2Fragment);
	child1->AddChild(child2);

	g3dSceneNode* child3 = new g3dSceneNode();
	child3->SetFragment(m_Arm2Fragment);
	child1->AddChild(child3);

	// Set internal transformations
	maMatrix4x4 child_matrix11 = child2->GetTransform();
	child_matrix11.Identity();
	child_matrix11.RotateBy( -maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
	child_matrix11.TranslateBy(0.0f, 0.0f, 11.0f);
	child2->SetTransform(child_matrix11);

	maMatrix4x4 child_matrix12 = child3->GetTransform();
	child_matrix12.Identity();
	child_matrix12.RotateBy( -maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
	child_matrix12.TranslateBy(0.0f, 0.0f, -11.0f);
	child3->SetTransform(child_matrix12);

	// Set the initial pivot point to be one of the arms
	m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, -11.0f));

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

	// view the model from the side so the arms stretch left to right
	this->SetYaw( maConstants::c_fPI_Div_2 );
}

//====================================================================
//	Think
//====================================================================
void demG3dTestPivot::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	animate the orientation of our model
	m_Model->SetOrientation( maRotation(maVector3d(1,0,0), frame_time * 60.0f * maConstants::c_fAngleToRad) );

	// scale animation also should be affected by pivot point
	//float scale = 1.0f + 0.25f * sinf(frame_time);
	//m_Model->SetScale( maVector3d(scale, scale, scale) );

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestPivot::DeInitialize()
{

	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	delete m_Model;

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

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestPivot::ReceiveCharEvent(appCharEvent& i_Event)
{
	const bool preserve_transform = true;
	const bool dont_preserve_transform = false;

	switch( i_Event.GetChar() )
	{
		case itString::CharType('1'):
			m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, 11.0f), preserve_transform);
		break;
		case itString::CharType('2'):
			m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, 0.0f), preserve_transform);
		break;
		case itString::CharType('3'):
			m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, -11.0f), preserve_transform);
		break;
		case itString::CharType('4'):
			m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, 11.0f), dont_preserve_transform);
		break;
		case itString::CharType('5'):
			m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, 0.0f), dont_preserve_transform);
		break;
		case itString::CharType('6'):
			m_Model->SetPivotPoint(maPoint3d(0.0f, 0.0f, -11.0f), dont_preserve_transform);
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}