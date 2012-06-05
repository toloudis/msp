/*****************************************************************************
**  demG3dTestPixelShader.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestPixelShader.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "envSTLHelpers.hpp"
#include "fsLocator.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dLayer.hpp"
#include "g3dPixelShaderMgrWin.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dScene.hpp"
#include "g3dVertexShaderUtilWin.hpp"
#include "matPlainTexture.hpp"
#include "matStaticCubeTexture.hpp"
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
demG3dTestPixelShader::demG3dTestPixelShader(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestPixelShader::~demG3dTestPixelShader()
{
}

//====================================================================
//====================================================================
void demG3dTestPixelShader::Initialize()
{

	// Compile shader from our external file
	//fsLocator shader_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	//shader_loc.Push("data");
	//shader_loc.Push("CookTorr.psh");
	//std::vector<DWORD> code;
	//g3dVertexShaderUtilWin::CompileVertexShader(shader_loc, code);

	// Export cpp const array
	//shader_loc.Pop();
	//shader_loc.Push("simple.cpp");
	//g3dVertexShaderUtilWin::ExportVertexShaderCodeArray(shader_loc, code, "simple_pso_array");

	if ( !g3dPixelShaderMgrWin::GetHardwarePixelShaderSupported() )
		return;

	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Pixel shader test."));
	m_Viewer.SetTextMessage(1, itString("Uses CookTorrance 'special effect' shader compiled into engine."));
	m_Viewer.SetTextMessage(2, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(3, itString("Press space to go to the next section"));

	//	make fragments
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("bigship.mx");
	m_ShipFragment = mayImport::LoadWorldFragment(locator, fsResourceFinderDir(fsLocator()),
										m_Materials, m_Textures );
//	m_ShipFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f, 50, 50);

	//	setup fragment materials
	int i;
	for (i=0; i<c_NumShaderMats; i++)
		m_ShaderMats[i].SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	// make a nice texture
	locator.Pop();
	locator.Push("view.scm");
	m_Texture1 = dynamic_cast<matStaticCubeTexture*>(matTextureMgr::LoadTexture(locator));
	//locator.Pop();
	//locator.Push("Toonish_01.png");
	//m_Texture2 = matTextureMgr::LoadTexture(locator);

	m_ShaderMats[0].AddTextureTop(m_Texture1);
	m_ShaderMats[0].AddTextureTop(m_Texture1);
	m_ShaderMats[0].SetTextureCoordinateSource(1, 1);

	// Set pixel shaders
	m_ShaderMats[0].SetSpecialEffect(matShaderIndex::e_CookTorrance);

	//	make models
	m_Ship = new g3dSceneNode(m_ShipFragment);
	m_Root->AddChild(m_Ship);
	m_Ship->SetRenderable(true);
	m_ShipFragment->SetMaterial( &m_ShaderMats[0] );

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
void demG3dTestPixelShader::Think()
{
	if ( !g3dPixelShaderMgrWin::GetHardwarePixelShaderSupported() )
	{
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
		DBG_MESSAGE0( "Hardware pixel shaders are not supported, this demo will be skipped." );
		return;
	}

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
	maMatrix4x4& matrix1 = m_Ship->GetTransform();
	//matrix1.MakeScale(0.5f,0.5f,0.5f);
	matrix1.MakeRotate( -frame_time * 30.0f * maConstants::c_fAngleToRad,
							maVector3d(0, 1, 0));

	// Rotate material for ship model
	if (l_bRotateShader)
	{
		int cycle = int(frame_time / c_TimePerShader);
		int shader = (cycle % c_NumShaderMats);
		m_ShipFragment->SetMaterial( &m_ShaderMats[shader] );
	}

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestPixelShader::DeInitialize()
{
	if ( !g3dPixelShaderMgrWin::GetHardwarePixelShaderSupported() )
		return;

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
	matTextureMgr::ReleaseTexture(m_Texture1);
	//matTextureMgr::ReleaseTexture(m_Texture2);
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
