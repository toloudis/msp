/*****************************************************************************
**  demG3dTestRenderState.hpp
**
**		This mode displays a demonstration/test of using render states
**	to assign different lighting to different models.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestRenderState.hpp"

#include "Core/app/appCharEvent.hpp"
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
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//====================================================================
//====================================================================
demG3dTestRenderState::demG3dTestRenderState(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestRenderState::~demG3dTestRenderState()
{
}

//====================================================================
//====================================================================
void demG3dTestRenderState::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root, g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative, true, true)); // scene owns layers
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Render state test"));
	m_Viewer.SetTextMessage(1, itString("The large spheres are lit differently. One side sphere gets the red point light, the other the blue."));
	m_Viewer.SetTextMessage(2, itString("The center sphere uses none of the point lights, but has a brighter ambient setting."));
	m_Viewer.SetTextMessage(3, itString("The white directional light is in the root render state, so it lights all of the objects."));
	m_Viewer.SetTextMessage(4, itString("'t' sets large sphere to transparent, 'o' back to opaque"));
	m_Viewer.SetTextMessage(5,  itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(6,  itString("Press space to go to the next section"));

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateRectangle(50.0f, 50.0f, 40, 40);
	m_RectFragment->SetReceivesShadow(true);
	m_BigSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(4.0f, 100, 100);
	m_BigSphereFragment->SetCastsShadow(true);
	m_BigSphereFragment->SetReceivesShadow(true);
	m_SmallSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1.3f, 15, 15);
	m_SmallSphereFragment->SetCastsShadow(true);
	m_SmallSphereFragment->SetReceivesShadow(true);

	//	setup fragment materials
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");

	effPhongData* pData = NULL;

	pData = dynamic_cast<effPhongData*>(m_RectMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(0.8f, 0.8f, 0.8f, 1.0f));
	m_RectMat.SetHasSpecular(false);
	m_RectMat.SetShaderEffect(("Simple.fx"));

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
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 0.75f));
	pData->m_ColorSpecular = (maFloatRGBA(0.6f, 0.6f, 0.6f, 1.0f));
	m_SmallSphereMat.SetHasSpecular(true);
	pData->m_SpecularPower = (50);
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

	m_Sphere1 = new g3dSceneNode();
	m_Sphere1->SetFragment(m_BigSphereFragment);
	m.MakeTranslate(0, 7, 0);
	m_Sphere1->SetTransform(m);
	m_Root->AddChild(m_Sphere1);

	m_Sphere2 = new g3dSceneNode();
	m_Sphere2->SetFragment(m_BigSphereFragment);
	m.MakeTranslate(-8, 7, -6);
	m_Sphere2->SetTransform(m);
	m_Root->AddChild(m_Sphere2);

	m_Sphere3 = new g3dSceneNode();
	m_Sphere3->SetFragment(m_BigSphereFragment);
	m.MakeTranslate(8, 7, 6);
	m_Sphere3->SetTransform(m);
	m_Root->AddChild(m_Sphere3);

	m_Light1 = new g3dSceneNode();
	m_Light1->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light1);
	m_Light2 = new g3dSceneNode();
	m_Light2->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light2);

	//	set lights
	m_DirectionalLight = g3dLightMgr::CreateDirectionalLight();

	m_DirectionalLight->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_DirectionalLight->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	m_PointLight1 = g3dLightMgr::CreatePointLight();
	m_PointLight2 = g3dLightMgr::CreatePointLight();
	m_PointLight2->SetCastsShadow(true);

	m_PointLight1->SetIntensity(maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	m_PointLight2->SetIntensity(maFloatRGBA(0.0f, 0.0f, 1.0f, 1.0f));

	m_DirectionalLight->Enable();
	m_PointLight1->Enable();
	m_PointLight2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));

	// Organize lights into render states
	m_pStateRoot = new g3dRenderState();
	m_pStateRoot->m_Lights.push_back(m_DirectionalLight);
	m_Root->SetRenderState(m_pStateRoot);

	// allow both lights to hit base rectange
	m_pStateLights = new g3dRenderState();
	m_pStateLights->m_Lights.push_back(m_PointLight1);
	m_pStateLights->m_Lights.push_back(m_PointLight2);
	m_BaseRect->SetRenderState(m_pStateLights);

	// middle light has more ambient, no point lights
	m_pStateAmbient= new g3dRenderState();
	m_pStateAmbient->m_AmbientLight.Set(0.4f, 0.4f, 0.4f, 1.0f);
	m_Sphere1->SetRenderState(m_pStateAmbient);

	// one side sphere gets blue light
	m_pStateBlue = new g3dRenderState();
	m_pStateBlue->m_Lights.push_back(m_PointLight2);
	m_Sphere2->SetRenderState(m_pStateBlue);

	// other side sphere gets red light
	m_pStateRed = new g3dRenderState();
	m_pStateRed->m_Lights.push_back(m_PointLight1);
	m_Sphere3->SetRenderState(m_pStateRed);

}

//====================================================================
//	Think
//====================================================================
void demG3dTestRenderState::Think()
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

	m_PointLight1->SetPosition(light1_pos);
	m_PointLight2->SetPosition(light2_pos);

	maMatrix4x4 m;
	m.MakeTranslate(light1_pos.m_X, light1_pos.m_Y, light1_pos.m_Z);
	m_Light1->SetTransform(m);
	m.MakeTranslate(light2_pos.m_X, light2_pos.m_Y, light2_pos.m_Z);
	m_Light2->SetTransform(m);

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestRenderState::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_DirectionalLight);
	g3dLightMgr::DestroyLight(m_PointLight1);
	g3dLightMgr::DestroyLight(m_PointLight2);

	// destroy render states
	delete m_pStateRoot;
	delete m_pStateBlue;
	delete m_pStateRed;
	delete m_pStateAmbient;

	//	cleanup fragments
	delete m_RectFragment;
	delete m_BigSphereFragment;
	delete m_SmallSphereFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_SphereTexture);

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestRenderState::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		// Opaque
		case itString::CharType('o'):
		case itString::CharType('O'):
			m_BigSphereFragment->SetMaterial(&m_BigSphereMat);
			break;
		// Transparent
		case itString::CharType('t'):
		case itString::CharType('T'):
			m_BigSphereFragment->SetMaterial(&m_SmallSphereMat);
			break;
		// Shadows
		case itString::CharType('s'):
		case itString::CharType('S'):
			g3dSingleLightRendering::SetDoSingleLightRendering(!g3dSingleLightRendering::GetDoSingleLightRendering());
			break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}