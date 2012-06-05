/*****************************************************************************
**  demG3dTestVertexShader.cpp
**
**		This mode displays a demonstration/test of verex shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestVertexShader.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "g3dLayer.hpp"

#include "envSTLHelpers.hpp"
#include "fsLocator.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dSceneNode.hpp"
#include "g3dVertexShaderMgrWin.hpp"
#include "g3dVertexShaderUtilWin.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"
#include "matPlainTexture.hpp"
#include "matStaticCubeTexture.hpp"
#include "matTextureMgr.hpp"
#include "mayImport.hpp"


namespace
{
const int c_NumShaderMats = 5;
const float c_TimePerShader = 2.0f;
bool l_bRotateShader = true;

}

//====================================================================
//====================================================================
demG3dTestVertexShader::demG3dTestVertexShader(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestVertexShader::~demG3dTestVertexShader()
{
}

//====================================================================
//====================================================================
void demG3dTestVertexShader::Initialize()
{
	if ( !g3dVertexShaderMgrWin::GetHardwareVertexShaderSupported() )
		return;

	demG3dTestMode::Initialize();

	//temp, bring in back clip plane to test depth vertex shader
	//Camera().SetClip(1.0f, 100.0f);	

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Vertex shader test."));
	m_Viewer.SetTextMessage(1, itString("Rotates through multiple 'special effect' shaders compiled into engine."));
	m_Viewer.SetTextMessage(2, itString("Anisotropic, Membrane, Rainbow, ReflectRefract, Toon, (sphere has user defined shader)"));
	m_Viewer.SetTextMessage(3, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(4, itString("Press space to go to the next section"));

	//	make fragments
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f, 50, 50);
	m_SmallSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(0.4f, 7, 7);

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
	m_Mat2.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Mat3.SetDiffuse(maFloatRGBA(0.8f, 1.0f, 0.8f, 1.0f));

	// make a nice texture
	locator.Pop();
	locator.Push("aniso2.png");
	m_Texture1 = matTextureMgr::LoadTexture(locator);
	locator.Pop();
	locator.Push("Toonish_01.png");
	//locator.Push("PurpleGreenBright.png");
	m_Texture2 = matTextureMgr::LoadTexture(locator);
	locator.Pop();
	locator.Push("colors1.png");
	m_Texture3 = matTextureMgr::LoadTexture(locator);
	locator.Pop();
	locator.Push("lgrey020.png");
	m_Texture4 = matTextureMgr::LoadTexture(locator);
	locator.Pop();
	locator.Push("colors2.png");
	m_Texture5 = matTextureMgr::LoadTexture(locator);
	locator.Pop();
	locator.Push("view.scm");
	//locator.Push("chrome_fender.scm");
	m_Texture6 = dynamic_cast<matStaticCubeTexture*>(matTextureMgr::LoadTexture(locator));
	locator.Pop();
	locator.Push("64shade.bmp");
	m_Texture7 = matTextureMgr::LoadTexture(locator);
	locator.Pop();
	locator.Push("edgeimage.bmp");
	m_Texture8 = matTextureMgr::LoadTexture(locator);

	m_ShaderMats[0].AddTextureTop(m_Texture1);
	m_ShaderMats[0].SetTextureRepeat(0, matMaterial::e_Mirror);
	m_ShaderMats[1].AddTextureTop(m_Texture2);
	m_ShaderMats[1].SetTextureRepeat(0, matMaterial::e_Mirror);
	m_ShaderMats[2].AddTextureTop(m_Texture3);
	m_ShaderMats[2].AddTextureTop(m_Texture5);	// rainbow effect needs two layers
	m_ShaderMats[2].SetTextureRepeat(0, matMaterial::e_Mirror);
	m_ShaderMats[2].SetTextureRepeat(1, matMaterial::e_Mirror);
	//m_ShaderMats[2].SetTextureLayerType(1, matMaterial::e_TexLayerAddSigned);
	m_ShaderMats[2].SetTextureCoordinateSource(1, 1);
	m_ShaderMats[3].AddTextureTop(m_Texture6);
	m_ShaderMats[3].AddTextureTop(m_Texture6);
	m_ShaderMats[3].SetDiffuse(1, 1, 1, 0.5f); // make transparent to turn on alpha blending
	m_ShaderMats[3].SetTextureCoordinateSource(1, 1);
	m_ShaderMats[4].AddTextureTop(m_Texture7);
	m_ShaderMats[4].SetTextureRepeat(0, matMaterial::e_Clamp);
	m_ShaderMats[4].AddTextureTop(m_Texture8);
	m_ShaderMats[4].SetTextureRepeat(1, matMaterial::e_Clamp);
	m_ShaderMats[4].SetTextureLayerType(1, matMaterial::e_TexLayerModulate);
	m_ShaderMats[4].SetTextureCoordinateSource(1, 1);


	//m_Mat2.AddTextureTop(m_Texture4);

	// Set up user-defined shader to our external file
	//fsLocator shader_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	//shader_loc.Push("data");
	//shader_loc.Push("simple9.vsh");
	//std::vector<DWORD> code;
	//g3dVertexShaderUtilWin::CompileVertexShader(shader_loc, code);

	// Import binary code output from nvasm
	fsLocator shader_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	shader_loc.Push("data");
	shader_loc.Push("simple9.vso");
	std::vector<DWORD> code;
	g3dVertexShaderUtilWin::ImportVertexShaderBinary(shader_loc, code);

	// Export binary code
	//shader_loc.Pop();
	//shader_loc.Push("simple9.vso");
	//g3dVertexShaderUtilWin::ExportVertexShaderBinary(shader_loc, code);

	// Export cpp const array
	//shader_loc.Pop();
	//shader_loc.Push("simple.cpp");
	//g3dVertexShaderUtilWin::ExportVertexShaderCodeArray(shader_loc, code, "simple_vso_array");

	//const DWORD decl_array[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3), // position
	//	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3), // normal
	//	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2), // texture coords 0
	//	D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2), // texture coords 1
	//	D3DVSD_END()
	//};

	g3dVertexShaderMgrWin::SetUserDefinedShader(&code[0]);

	// Set vertex shaders
	m_ShaderMats[0].SetSpecialEffect(matShaderIndex::e_Anisotropic);
	m_ShaderMats[1].SetSpecialEffect(matShaderIndex::e_Membrane);
	m_ShaderMats[2].SetSpecialEffect(matShaderIndex::e_Rainbow);
	m_ShaderMats[3].SetSpecialEffect(matShaderIndex::e_ReflectRefract);
	m_ShaderMats[4].SetSpecialEffect(matShaderIndex::e_Toon);
	m_Mat2.SetSpecialEffect(matShaderIndex::e_UserDefined);

	//	make models
	maMatrix4x4 world_mat;

	//world_mat.MakeTranslate(7,0,7);
	m_Sphere1 = new g3dSceneNode(m_ShipFragment);
	m_Root->AddChild(m_Sphere1);
	//m_ShipFragment->SetMaterial(&m_ShaderMats[4]);
	m_ShipFragment->SetMaterial(&m_ShaderMats[0]);

	world_mat.MakeTranslate(7, 0, -7);
	m_Sphere2 = new g3dSceneNode(m_SphereFragment, world_mat);
	m_Root->AddChild(m_Sphere2);
	m_Sphere2->SetMaterial(&m_Mat2);

	world_mat.MakeTranslate(-7, 0, 7);
	m_Sphere3 = new g3dSceneNode(m_SphereFragment, world_mat);
	m_Root->AddChild(m_Sphere3);
	m_Sphere3->SetMaterial(&m_Mat3);

	// Point light spheres
	m_LightBall1 = new g3dSceneNode(m_SmallSphereFragment);
	m_Root->AddChild(m_LightBall1);
	m_LightBall2 = new g3dSceneNode(m_SmallSphereFragment);
	m_Root->AddChild(m_LightBall2);


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
void demG3dTestVertexShader::Think()
{
	if ( !g3dVertexShaderMgrWin::GetHardwareVertexShaderSupported() )
	{
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
		DBG_MESSAGE0( "Hardware vertex shaders are not supported, this demo will be skipped." );
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
	m_LightBall1->GetTransform().MakeTranslate(light1_pos.m_X, light1_pos.m_Y, light1_pos.m_Z);
	m_LightBall2->GetTransform().MakeTranslate(light2_pos.m_X, light2_pos.m_Y, light2_pos.m_Z);

	//	set the orientations of our objects
	maMatrix4x4& matrix1 = m_Sphere1->GetTransform();
	matrix1.MakeScale(0.5f,0.5f,0.5f);
	matrix1.RotateBy( -frame_time * 30.0f * maConstants::c_fAngleToRad,
							maVector3d(0, 1, 0));
	matrix1.TranslateBy(7, 0, 7);

	// Rotate material for ship model
	if (l_bRotateShader)
	{
		int cycle = int(frame_time / c_TimePerShader);
		int shader = (cycle % c_NumShaderMats);
		m_ShipFragment->SetMaterial(&m_ShaderMats[shader]);
	}

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestVertexShader::DeInitialize()
{
	if ( !g3dVertexShaderMgrWin::GetHardwareVertexShaderSupported() )
		return;

	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);
	g3dLightMgr::DestroyLight(m_Light3);
	g3dLightMgr::DestroyLight(m_Light4);

	//	cleanup models
	//g3dScene::RemoveModel(m_Sphere1);
	//g3dScene::RemoveModel(m_Sphere2);
	//g3dScene::RemoveModel(m_Sphere3);
	//g3dScene::RemoveModel(m_LightBall1);
	//g3dScene::RemoveModel(m_LightBall2);

	//	cleanup fragments
	delete m_SphereFragment;
	delete m_SmallSphereFragment;
	delete m_ShipFragment;

	// delete materials
	envSTLHelpers::DeleteContainer(m_Materials);

	//	release textures
	matTextureMgr::ReleaseTexture(m_Texture1);
	matTextureMgr::ReleaseTexture(m_Texture2);
	matTextureMgr::ReleaseTexture(m_Texture3);
	matTextureMgr::ReleaseTexture(m_Texture4);
	matTextureMgr::ReleaseTexture(m_Texture5);
	matTextureMgr::ReleaseTexture(m_Texture6);
	matTextureMgr::ReleaseTexture(m_Texture7);
	matTextureMgr::ReleaseTexture(m_Texture8);
	envSTLHelpers::ForAll(m_Textures, matTextureMgr::ReleaseTexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
