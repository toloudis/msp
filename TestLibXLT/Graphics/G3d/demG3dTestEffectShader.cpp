/*****************************************************************************
**  demG3dTestEffectShader.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestEffectShader.hpp"


#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "demModeManager.hpp"
#include "effShaderUtilWin.hpp"
#include "envSTLHelpers.hpp"
#include "fsLocator.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dLayer.hpp"
#include "g3dPointLight.hpp"
#include "g3dScene.hpp"
#include "matTextureMgr.hpp"
#include "maConstants.hpp"
#include "mayImport.hpp"
#include "gfPaths.hpp"


namespace
{
const int c_NumShaderMats = 1;
const float c_TimePerShader = 2.0f;
bool l_bRotateShader = true;

}

//====================================================================
//====================================================================
demG3dTestEffectShader::demG3dTestEffectShader(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pEffect(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestEffectShader::~demG3dTestEffectShader()
{
}

//====================================================================
//====================================================================
void demG3dTestEffectShader::Initialize()
{
	demG3dTestMode::Initialize();

	// set short clipping for depth rendering tests
	//Camera().SetClip(1.0f, 16.0f);

	// effect load test
	fsLocator eff_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	eff_loc.Push("data");
	eff_loc.Push("Multi.fx");
	//eff_loc.Push("Simple.fx");
	//eff_loc.Push("Depth.fx");
	//eff_loc.Push("velvety.fx");
	m_pEffect = effShaderUtilWin::CompileEffect(eff_loc);
	m_ShaderMat.SetShaderEffect( m_pEffect );

	// Set some specular power in order to test specular 
	// lighting in shader also
	// FIX TO USE SHADER DATA CLASS
	//m_ShaderMat.SetSpecularPower(50.0f);


	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Effects Shader."));
	m_Viewer.SetTextMessage(1, itString("This model is shaded from an .fx file loaded from disk."));
	m_Viewer.SetTextMessage(2, itString("Press 's' to toggle the shader on/off (but they might match if written correctly)"));
	m_Viewer.SetTextMessage(3, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(4, itString("Press space to go to the next section"));

	//	make fragments
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("bigship.mx");
	std::vector<g3dFragment*> shipFragments, shipLowResFragments, shipHighResFragments;
	mayImport::LoadWorldFragments(locator, fsResourceFinderDir(fsLocator()),
							shipFragments,
							shipLowResFragments,
							shipHighResFragments,
							m_Materials,
							m_Textures);
	m_ShipFragment = shipHighResFragments[0];

	//	make models
	m_Ship = new g3dSceneNode(m_ShipFragment);
	m_Root->AddChild(m_Ship);
	m_Ship->SetRenderable(true);
	m_ShipFragment->SetMaterial( &m_ShaderMat );

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();
	m_Light3 = g3dLightMgr::CreatePointLight();
	m_Light4 = g3dLightMgr::CreatePointLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));
	m_Light3->SetPosition(maVector3d(0, -5, 0));
	m_Light4->SetPosition(maVector3d(0, 5, 0));

	m_Light1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));
	m_Light3->SetIntensity(maFloatRGBA(0.9f, 0.1f, 0.1f, 1.0f));
	m_Light4->SetIntensity(maFloatRGBA(0.1f, 0.9f, 0.1f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();
	m_Light3->Enable();
	m_Light4->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestEffectShader::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	set the orientations of our lights
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

	m_Light3->SetPosition(light1_pos);
	m_Light4->SetPosition(light2_pos);

	//	set the orientations of our objects
	//maMatrix4x4& matrix1 = m_Ship->GetTransform();
	////matrix1.MakeScale(0.5f,0.5f,0.5f);
	//matrix1.MakeRotate( -frame_time * 30.0f * maConstants::c_fAngleToRad,
	//						maVector3d(0, 1, 0));


	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestEffectShader::DeInitialize()
{
	delete m_pEffect;

	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);
	g3dLightMgr::DestroyLight(m_Light3);
	g3dLightMgr::DestroyLight(m_Light4);

	//	cleanup models
	//g3dScene::RemoveModel(m_Ship);

	//	cleanup fragments
	delete m_ShipFragment;

	// delete materials
	envSTLHelpers::DeleteContainer(m_Materials);

	//	release textures
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestEffectShader::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('s'):
		case itString::CharType('S'):
			if (m_ShaderMat.GetShaderEffect() != NULL)
				m_ShaderMat.SetShaderEffect( NULL );
			else
				m_ShaderMat.SetShaderEffect( m_pEffect );
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}