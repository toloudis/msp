/*****************************************************************************
**  demG3dTestLights.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestLights.hpp"

#include "Core/app/appTime.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//====================================================================
//====================================================================
demG3dTestLights::demG3dTestLights(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestLights::~demG3dTestLights()
{
}

//====================================================================
//====================================================================
void demG3dTestLights::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layers
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("This slide shows some lighting features of the Terawatt engine."));
	m_Viewer.SetTextMessage(1, itString("The moving spheres are coincident with the positions of two point lights.  The scene is"));
	m_Viewer.SetTextMessage(2, itString("also illuminated by a directional light.  The ground material and the sphere material"));
	m_Viewer.SetTextMessage(3, itString("both show specular reflections, whereas the moving spheres only have diffuse reflection."));
	m_Viewer.SetTextMessage(4,  itString("Also, the moving spheres show a non-textured translucent material."));
	m_Viewer.SetTextMessage(5, itString("Also, the texture on the rectangle is mip-mapped.  (Try moving the camera forward and back.)"));
	m_Viewer.SetTextMessage(6,  itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(7,  itString("Press space to go to the next section"));

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(50.0f, 50.0f, 40, 40);
	m_BigSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(6.0f, 100, 100);
	m_SmallSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1.3f, 15, 15);


	effPhongData* pData = NULL;

	//	setup fragment materials

	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");

	pData = dynamic_cast<effPhongData*>(m_RectMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_ColorSpecular = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_RectMat.SetHasSpecular(true);
	pData->m_SpecularPower = (70);
	pData->m_UV.m_UScale = 4;
	pData->m_UV.m_VScale = 4;
	pData->m_NameDiffuse = "lblue013.png";
	pData->ReloadTextures(locator);
	m_RectTexture = pData->m_TextureDiffuse;
	m_RectMat.SetShaderEffect(("Phong.fx"));

	pData = dynamic_cast<effPhongData*>(m_BigSphereMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.6f, 0.6f, 0.6f, 1.0f));
	pData->m_ColorSpecular = (maFloatRGBA(0.6f, 0.6f, 0.6f, 1.0f));
	m_BigSphereMat.SetHasSpecular(true);
	pData->m_SpecularPower = (50);
	pData->m_NameDiffuse = "lgrey020.png";
	pData->ReloadTextures(locator);
	m_SphereTexture = pData->m_TextureDiffuse;
	m_BigSphereMat.SetShaderEffect(("Phong.fx"));

	pData = dynamic_cast<effPhongData*>(m_SmallSphereMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 0.5f));
//	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
//	m_SmallSphereMat.SetHasSpecular(true);
//	m_SmallSphereMat.SetSpecularPower(100);
	m_SmallSphereMat.SetShaderEffect(("Simple.fx"));


	m_RectFragment->SetMaterial(&m_RectMat);
	m_BigSphereFragment->SetMaterial(&m_BigSphereMat);
	m_SmallSphereFragment->SetMaterial(&m_SmallSphereMat);

	maMatrix4x4 m;

	//	make models
	m_BaseRect = new g3dSceneNode();
	m_BaseRect->SetFragment(m_RectFragment);
	m.MakeRotateX( -maConstants::c_fPI_Div_2 );
	m_BaseRect->SetTransform(m);
	m_Root->AddChild(m_BaseRect);

	m_Sphere = new g3dSceneNode();
	m_Sphere->SetFragment(m_BigSphereFragment);
	m.MakeTranslate(0, 7, 0);
	m_Sphere->SetTransform(m);
	m_Root->AddChild(m_Sphere);

	m_Light1 = new g3dSceneNode();
	m_Light1->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light1);
	m_Light2 = new g3dSceneNode();
	m_Light2->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light2);
	m_Light3 = new g3dSceneNode();
	m_Light3->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light3);

	//	set lights
	m_DirectionalLight = g3dLightMgr::CreateDirectionalLight();

	m_DirectionalLight->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_DirectionalLight->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	m_PointLight1 = g3dLightMgr::CreatePointLight();
	m_PointLight2 = g3dLightMgr::CreatePointLight();
	m_PointLight3 = g3dLightMgr::CreatePointLight();

	m_PointLight1->SetIntensity(maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	m_PointLight2->SetIntensity(maFloatRGBA(0.0f, 0.0f, 1.0f, 1.0f));
	m_PointLight3->SetIntensity(maFloatRGBA(0.0f, 1.0f, 0.0f, 1.0f));

	m_PointLight3->SetRange(8.0f);

	m_DirectionalLight->Enable();
	m_PointLight1->Enable();
	m_PointLight2->Enable();
	m_PointLight3->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestLights::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	set the orientations of our lights and light spheres
	float theta = frame_time * maConstants::c_fAngleToRad * 30.0f;
	float phi = frame_time * maConstants::c_fAngleToRad * 60.0f;

	float x, y, z;

	x = 15.0f * sin(theta);
	y = 10.0f + 4.0f * sin(phi);
	z = 15.0f * cos(theta);

	maPoint3d light1_pos(x, y, z);

	x = 15.0f * sin(theta + maConstants::c_fPI);
	y = 8.0f + 4.0f * sin(phi + maConstants::c_fPI);
	z = 15.0f * cos(theta + maConstants::c_fPI);

	maPoint3d light2_pos(x, y, z);

	x = 12.0f * sin(theta + maConstants::c_fPI_Div_2) + 5.0f * sin(phi + maConstants::c_fPI_Div_2);
	y = 8.0f + 4.0f * sin(phi + maConstants::c_fPI_Div_2);
	z = 12.0f * cos(theta + maConstants::c_fPI_Div_2) + 5.0f * sin(phi + maConstants::c_fPI_Div_2);;

	maPoint3d light3_pos(x, y, z);

	m_PointLight1->SetPosition(light1_pos);
	m_PointLight2->SetPosition(light2_pos);
	m_PointLight3->SetPosition(light3_pos);

	maMatrix4x4 m;
	m.MakeTranslate(light1_pos.m_X, light1_pos.m_Y, light1_pos.m_Z);
	m_Light1->SetTransform(m);
	m.MakeTranslate(light2_pos.m_X, light2_pos.m_Y, light2_pos.m_Z);
	m_Light2->SetTransform(m);
	m.MakeTranslate(light3_pos.m_X, light3_pos.m_Y, light3_pos.m_Z);
	m_Light3->SetTransform(m);

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestLights::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_DirectionalLight);
	g3dLightMgr::DestroyLight(m_PointLight1);
	g3dLightMgr::DestroyLight(m_PointLight2);
	g3dLightMgr::DestroyLight(m_PointLight3);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_BaseRect);
	//g3dRenderer::RemoveModel(m_Sphere);
	//g3dRenderer::RemoveModel(m_Light1);
	//g3dRenderer::RemoveModel(m_Light2);
	//g3dRenderer::RemoveModel(m_Light3);

	//	cleanup fragments
	delete m_RectFragment;
	delete m_BigSphereFragment;
	delete m_SmallSphereFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_RectTexture);
	matTextureMgr::ReleaseTexture(m_SphereTexture);

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
