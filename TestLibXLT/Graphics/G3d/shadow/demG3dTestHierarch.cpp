/*****************************************************************************
**  demG3dTestHierarch.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestHierarch.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dFontUtil.hpp"
#include "g2dRGBColor.hpp"
#include "g2dScreen.hpp"
#include "g2dScreenDrawUtil.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dDynamicModel.hpp"
#include "g3dFragmentManager.hpp"
#include "g3dLightManager.hpp"
#include "g3dPackage.hpp"
#include "matPlainTexture.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dRenderer.hpp"
#include "g3dStaticModel.hpp"
#include "matTextureManager.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

//====================================================================
//====================================================================
demG3dTestHierarch::demG3dTestHierarch()
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

	//	make fragments
	m_BaseFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(6.0f, 20, 20);
	m_Arm1Fragment = g3dPrimitiveFragmentUtil::CreateTexturedBlock(2, 2, 20);
	m_Arm2Fragment = g3dPrimitiveFragmentUtil::CreateTexturedBlock(2, 2, 10);
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(140.0f, 140.0f, 2, 2, 1);

	//	setup fragment materials
	m_Mat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	// make a nice texture
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("lgren041.png"));
	m_Texture = matTextureManager::LoadTexture(locator);

	m_Mat.AddTextureTop(m_Texture);

	g3dFragmentManager::SetMaterial(m_BaseFragment, &m_Mat, 0);
	g3dFragmentManager::SetMaterial(m_Arm1Fragment, &m_Mat, 0);
	g3dFragmentManager::SetMaterial(m_Arm2Fragment, &m_Mat, 0);
	g3dFragmentManager::SetMaterial(m_RectFragment, &m_Mat, 0);

	//	make models
	g3dHFragment hfrag(m_BaseFragment);
	hfrag.GetTransform().TranslateBy(0, 10, 0);

	g3dHFragment* child1 = new g3dHFragment(m_Arm1Fragment);
	hfrag.AddChild(child1);

	g3dHFragment* child2 = new g3dHFragment(m_Arm2Fragment);
	child1->AddChild(child2);

	child2 = new g3dHFragment(m_Arm2Fragment);
	child1->AddChild(child2);

	m_Model = g3dRenderer::AddDynamicModel(hfrag, g3dPackage::e_World);
	m_Model->SetRenderable(true);
	m_Model->SetCastsShadow(true);

	maMatrix4x4 world_mat;
	world_mat.MakeRotateX(-maConstants::c_fPI_Div_2);
	m_Rect = g3dRenderer::AddStaticModel(m_RectFragment, world_mat, g3dPackage::e_World);
	m_Rect->SetRenderable(true);

	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();
	m_Light1->SetCastsShadow(true);
	m_Light2->SetCastsShadow(true);

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.2f, 0.7f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.3f, 0.5f, 0.6f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(0.3f, 0.3f, 0.3f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestHierarch::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	g2dScreenDrawUtil::Clear(g2dRGBColor(0x30, 0x80, 0xa0));

	//	draw some text stuff
/*	const int num_text_lines = 8;
	itString text[num_text_lines];
	text[0] = itString("This slide shows a hierarchical model.");
	text[1] = itString("Each level of the model has a transformation to the level above it and");
	text[2] = itString("is rendered using forward kinematics.");
	text[3] = itString("The implementation of Terawatt minimizes D3D state changes, so since the same");
	text[4] = itString("material is used for this entire model, no texture or material parameter changes");
	text[5] = itString("are made while rendering.  This is true between different models also.");
	text[6] = itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)");
	text[7] = itString("Press space to go to the next section");
	g2dRGBColor text_color(0x50, 0x50, 0x90);

	int i;
	for( i = 0 ; i < num_text_lines ; i++ )
	{
		g2dScreenDrawUtil::DrawText(	10,
										(i * 17) + 10,
										this->GetFont(),
										text[i],
										text_color);

		text_color.SetRed(text_color.GetRed() + 10);
	}
*/
	//	set the orientations of our model
	maMatrix4x4& child_matrix1 = m_Model->GetHead()->GetChild(0)->GetTransform();
	child_matrix1.Identity();
	child_matrix1.RotateBy( frame_time * 30.0f * maConstants::c_fAngleToRad, maVector3d(0, 1, 0));
	//child_matrix1.TranslateBy(0, 10, 0);

	maMatrix4x4& child_matrix11 = m_Model->GetHead()->GetChild(0)->GetChild(0)->GetTransform();
	child_matrix11.Identity();
	child_matrix11.RotateBy( -maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
	child_matrix11.RotateBy( -frame_time * 90.0f * maConstants::c_fAngleToRad, maVector3d(0, 0, 1) );
	child_matrix11.TranslateBy(0.0f, 0.0f, 11.0f);

	maMatrix4x4& child_matrix12 = m_Model->GetHead()->GetChild(0)->GetChild(1)->GetTransform();
	child_matrix12.Identity();
	child_matrix12.RotateBy( -maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
	child_matrix12.RotateBy( frame_time * 90.0f * maConstants::c_fAngleToRad, maVector3d(0, 0, 1) );
	child_matrix12.TranslateBy(0.0f, 0.0f, -11.0f);


	//	render
	g3dRenderer::RenderModels(frame_time);

	//	flip
	g2dScreen::EndScene();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestHierarch::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	g3dRenderer::RemoveModel(m_Model);
	g3dRenderer::RemoveModel(m_Rect);

	//	cleanup fragments
	g3dFragmentManager::Destroy(m_BaseFragment);
	g3dFragmentManager::Destroy(m_RectFragment);
	g3dFragmentManager::Destroy(m_Arm1Fragment);
	g3dFragmentManager::Destroy(m_Arm2Fragment);

	//	release textures
	matTextureManager::ReleaseTexture(m_Texture);

	demG3dTestMode::DeInitialize();
}
