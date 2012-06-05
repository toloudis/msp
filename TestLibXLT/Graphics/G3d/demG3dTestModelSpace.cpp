/*****************************************************************************
**  demG3dTestModelSpace.cpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestModelSpace.hpp"

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
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

namespace
{

g3dFragment* make_line_tetra(const maPoint3d& i_Offset)
{
	maPoint3d vertices[4];
	maPoint3d normals[4];
	unsigned short indices[12];

	const float sqrt3d2 = sqrtf(3.0) / 2.0f;
	const float sqrt2 = sqrtf(2.0);

	vertices[0].Set(0, 0, sqrt3d2 / 2.0f);
	vertices[1].Set(0.5, 0, -sqrt3d2 / 2.0f);
	vertices[2].Set(-0.5, 0, -sqrt3d2 / 2.0f);
	vertices[3].Set(0, sqrt3d2, 0);

	int i;
	for( i = 0 ; i < 4 ; i++ )
		vertices[i] += i_Offset;

	normals[0].Set(0, 0, 1);
	normals[1].Set(1.0f / sqrt2, 0, 1.0f / -sqrt2);
	normals[2].Set(1.0f / -sqrt2, 0, 1.0f / -sqrt2);
	normals[3].Set(0, 1, 0);

	indices[0] = 0;
	indices[1] = 1;

	indices[2] = 1;
	indices[3] = 2;

	indices[4] = 2;
	indices[5] = 0;

	indices[6] = 0;
	indices[7] = 3;

	indices[8] = 1;
	indices[9] = 3;

	indices[10] = 2;
	indices[11] = 3;

	matMaterial* material = &(g3dPrimitiveFragmentUtil::DefaultMaterial());

	return g3dFragmentCreate::CreateLineList(	vertices,
												normals,
												4,
												indices,
												12,
												material,
												false);
}

}

//====================================================================
//====================================================================
demG3dTestModelSpace::demG3dTestModelSpace(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestModelSpace::~demG3dTestModelSpace()
{
}

//====================================================================
//====================================================================
void demG3dTestModelSpace::Initialize()
{
	demG3dTestMode::Initialize();

	m_WorldRoot = new g3dSceneNode();
	m_CameraRoot = new g3dSceneNode();
	m_ScreenRoot = new g3dSceneNode();
	std::vector<g3dLayer*> layers;
	layers.push_back(new g3dLayer(m_WorldRoot, g3dLayer::e_ZBuffer,
								g3dLayer::e_World));
	layers.push_back(new g3dLayer(m_CameraRoot, g3dLayer::e_ZBuffer,
								g3dLayer::e_Camera));
	layers.push_back(new g3dLayer(m_ScreenRoot, g3dLayer::e_ZBuffer,
								g3dLayer::e_Screen));
	m_Scene = new g3dScene(layers); // scene owns layers

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("This slide shows models rendering in three different spaces - screen, camera, and world space."));
	m_Viewer.SetTextMessage(1, itString("Notice that the cubes, rendered in camera space, always maintain their orientation to the camera."));
	m_Viewer.SetTextMessage(2, itString("Likewise the ugly green triangle, which is rendered in screen space."));
	m_Viewer.SetTextMessage(3, itString("Also shown are two fragment types available in Terawatt - triangle list and line list."));
	m_Viewer.SetTextMessage(4, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(5, itString("Press space to go to the next section"));
	

	//	make fragments
	m_CubeFragment = g3dPrimitiveFragmentUtil::CreateTexturedCube(3.0f);
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f, 50, 50);
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);
	m_TetraFragment = g3dPrimitiveFragmentUtil::CreateLineCube(3.0f);

	{
		// make the screen rect fragment
		matMaterial* mat_ptr = &m_RectMat;

		maPoint3d vertices[3];
		maPoint3d normals[3];

		vertices[0].Set( -0.6f, 0.5f, 0.0f );
		vertices[1].Set( -0.3f,-0.0f, 0.0f );
		vertices[2].Set(-0.6f,-0.0f, 0.0f );

		normals[0].Set(0, 0, 1);
		normals[1].Set(0, 0, 1);
		normals[2].Set(0, 0, 1);

		unsigned short indices[3];
		indices[0] = 2;
		indices[1] = 1;
		indices[2] = 0;

		m_ScreenRectFragment = g3dFragmentCreate::CreateFragment(	vertices,
															normals,
															3,
															indices,
															3,
															mat_ptr,
															false);
	}

	//	setup fragment materials
	effPhongData* pData = NULL;
	// make a nice texture
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");

	pData = dynamic_cast<effPhongData*>(m_CubeMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_NameDiffuse = "lblue0132.png";
	pData->ReloadTextures(locator);
	m_CubeTexture = pData->m_TextureDiffuse;
	m_CubeMat.SetShaderEffect(("Phong.fx"));

	pData = dynamic_cast<effPhongData*>(m_SphereMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.8f, 0.8f, 0.8f, 1.0f));

	pData = dynamic_cast<effPhongData*>(m_RectMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.0f, 0.8f, 0.8f, 1.0f));


	m_CubeFragment->SetMaterial(&m_CubeMat);
	m_SphereFragment->SetMaterial(&m_SphereMat);
	m_RectFragment->SetMaterial(&m_RectMat);
	m_TetraFragment->SetMaterial(&m_RectMat);
	m_ScreenRectFragment->SetMaterial(&m_RectMat);

	//	make world models
	g3dSceneNode *model = new g3dSceneNode();
	model->SetFragment(m_SphereFragment);
	m_WorldRoot->AddChild(model);

	maMatrix4x4 m;

	model = new g3dSceneNode();
	model->SetFragment(m_RectFragment);
	m.MakeRotateX( -maConstants::c_fPI_Div_2 );
	model->SetTransform(m);
	m_WorldRoot->AddChild(model);

	model = new g3dSceneNode();
	model->SetFragment(m_TetraFragment);
	m.MakeScale(2.0f, 2.0f, 2.0f);
	m.TranslateBy(0, 10, 0);
	model->SetTransform(m);
	m_WorldRoot->AddChild(model);

	// make camera space models
	m_Cube1 = new g3dSceneNode();
	m_Cube1->SetFragment(m_CubeFragment);
	m_CameraRoot->AddChild(m_Cube1);

	m_Cube2 = new g3dSceneNode();
	m_Cube2->SetFragment(m_CubeFragment);
	m_CameraRoot->AddChild(m_Cube2);

	// screen space models
	model = new g3dSceneNode();
	model->SetFragment(m_ScreenRectFragment);
	m_ScreenRoot->AddChild(model);

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
void demG3dTestModelSpace::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	set the orientations of our cubes
	maMatrix4x4 cube_matrix1 = m_Cube1->GetTransform();
	cube_matrix1.MakeScale(0.5f, 0.5f, 0.5f);
	cube_matrix1.RotateBy( frame_time * 30.0f * maConstants::c_fAngleToRad, maVector3d(0, 1, 0));
	cube_matrix1.TranslateBy(5.0f, 5.0f, 15.0f);
	m_Cube1->SetTransform(cube_matrix1);

	maMatrix4x4 cube_matrix2 = m_Cube2->GetTransform();
	cube_matrix2.MakeScale(0.5f, 0.5f, 0.5f);
	cube_matrix2.RotateBy( -frame_time * 30.0f * maConstants::c_fAngleToRad, maVector3d(0, 1, 0) );
	cube_matrix2.TranslateBy(-5.0f, -5.0f, 15.0f);
	m_Cube2->SetTransform(cube_matrix2);

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestModelSpace::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_WorldRect);
	//g3dRenderer::RemoveModel(m_WorldSphere);
	//g3dRenderer::RemoveModel(m_CameraCube1);
	//g3dRenderer::RemoveModel(m_CameraCube2);
	//g3dRenderer::RemoveModel(m_WorldTetra);
	//g3dRenderer::RemoveModel(m_ScreenRect);

	//	cleanup fragments
	delete m_CubeFragment;
	delete m_SphereFragment;
	delete m_RectFragment;
	delete m_TetraFragment;
	delete m_ScreenRectFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_CubeTexture);

	m_Viewer.ClearTextMessages();
	delete m_WorldRoot;
	delete m_CameraRoot;
	delete m_ScreenRoot;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
