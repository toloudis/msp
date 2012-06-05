/*****************************************************************************
**  mspLighting.cpp
**
**      see .hpp
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mspLighting.hpp"

#include "Core/Fs/fsResourceFinderDir.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dProjectedLightWrapper.hpp"
#include "Tool/api3d/api3dScene.hpp"

//
//	namespace
//
namespace
{
	const int c_DepthMapResolution = 1024; //2048;
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
mspLighting::mspLighting()
:	m_PointLight(NULL), 
	m_ProjectedLight1(NULL), 
	m_ProjectedLightWrapper1(NULL), 
	m_ProjectedLight2(NULL), 
	m_ProjectedLightWrapper2(NULL), 
	m_LightTexture(NULL), 
	m_StateRoot(NULL), 
	m_AmbientEnvironment(NULL)
{

	m_PointLight = g3dLightMgr::CreatePointLight();
	m_PointLight->SetPosition(100.0f * maPoint3d(0.603f, 0.156f, 0.176f));
	m_PointLight->SetIntensity(maFloatRGBA(32.0f/255.0f, 28.0f/255.0f, 26.0f/255.0f, 1.0f));
	m_PointLight->SetDiffuseEnabled(true);
	m_PointLight->SetSpecularEnabled(true);
	m_PointLight->SetAffectsGlow(true);
	m_PointLight->SetCastsShadow(true);
	m_PointLight->SetFalloff0(1.00f);
	m_PointLight->SetFalloff1(1.00f);
	m_PointLight->SetFalloff2(0.00f);
	m_PointLight->SetFalloff3(0.00f);
	m_PointLight->SetFalloffStart(0.00f);
	m_PointLight->SetRange(100 * 0.01f);
	m_PointLight->SetIntensityFactor(102.3f);
	m_PointLight->Enable();

	// create the projection texture for both projection lights
	fsLocator texture_loc;
	texture_loc.Push("Data");
	texture_loc.Push("Shader_Ball_projection.dds");
	m_LightTexture = matTextureMgr::LoadTexture(texture_loc);

	m_ProjectedLight1 = g3dLightMgr::CreateProjectedLight();
	m_ProjectedLight1->SetPosition(100.0f * maPoint3d(-0.199f, 0.482f, 1.010f));
	m_ProjectedLight1->SetTarget(100.0f * maPoint3d(-0.086f, 1.224f, -0.286f));
	m_ProjectedLight1->SetIntensity(maFloatRGBA(207.0f/255.0f, 222.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f));
	m_ProjectedLight1->SetIntensityFactor(1.0f);
	m_ProjectedLight1->SetAngle(45.0f);
	m_ProjectedLight1->SetScale(100 * 2.0f);
	m_ProjectedLight1->SetRange(100 * 2.0f);
	m_ProjectedLight1->SetAspect(1.0f);
	m_ProjectedLight1->SetDiffuseEnabled(true);
	m_ProjectedLight1->SetSpecularEnabled(true);
	m_ProjectedLight1->SetAffectsGlow(true);
	m_ProjectedLight1->SetCastsShadow(true);
	m_ProjectedLight1->SetDepthBias(0.005f);
	m_ProjectedLight1->SetShadowQuality(g3dProjectedLight::SQ_HIGH);
	m_ProjectedLight1->SetShadowIntensity(1.0f);
	m_ProjectedLight1->SetShadowColor(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_ProjectedLight1->SetLightSize(0.001f);
	m_ProjectedLight1->SetTexture(m_LightTexture);
	m_ProjectedLight1->Enable();
	// Wrapper handles the shadow map for the projected light
	m_ProjectedLightWrapper1 = new api3dProjectedLightWrapper(*m_ProjectedLight1, c_DepthMapResolution, api3dScene::GetScene());

	m_ProjectedLight2 = g3dLightMgr::CreateProjectedLight();
	m_ProjectedLight2->SetPosition(100.0f * maPoint3d(0.669f, 1.278f, 0.089f));
	m_ProjectedLight2->SetTarget(100.0f * maPoint3d(-0.086f, 0.511f, -0.374f));
	m_ProjectedLight2->SetIntensity(maFloatRGBA(255.0f/255.0f, 242.0f/255.0f, 220.0f/255.0f, 255.0f/255.0f));
	m_ProjectedLight2->SetIntensityFactor(1.2f);
	m_ProjectedLight2->SetAngle(34.01f);
	m_ProjectedLight2->SetScale(100 * 2.0f);
	m_ProjectedLight2->SetRange(100 * 2.0f);
	m_ProjectedLight2->SetAspect(1.0f);
	m_ProjectedLight2->SetDiffuseEnabled(true);
	m_ProjectedLight2->SetSpecularEnabled(true);
	m_ProjectedLight2->SetAffectsGlow(true);
	m_ProjectedLight2->SetCastsShadow(true);
	m_ProjectedLight2->SetDepthBias(0.005f);
	m_ProjectedLight2->SetShadowQuality(g3dProjectedLight::SQ_HIGH);
	m_ProjectedLight2->SetShadowIntensity(1.0f);
	m_ProjectedLight2->SetShadowColor(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_ProjectedLight2->SetLightSize(0.001f);
	m_ProjectedLight2->SetTexture(m_LightTexture);
	m_ProjectedLight2->Enable();
	// Wrapper handles the shadow map for the projected light
	m_ProjectedLightWrapper2 = new api3dProjectedLightWrapper(*m_ProjectedLight2, c_DepthMapResolution, api3dScene::GetScene());

	// Organize light into render state
	m_StateRoot = new g3dRenderState();
	m_StateRoot->m_Lights.push_back(m_PointLight);
	m_StateRoot->m_Lights.push_back(m_ProjectedLight1);
	m_StateRoot->m_Lights.push_back(m_ProjectedLight2);

	api3dScene::GetRoot(api3dScene::WorldLayerIndex())->SetRenderState(m_StateRoot);

	m_AmbientEnvironment = new g3dAmbientEnvState();
	m_AmbientEnvironment->m_DiffuseColor = maFloatRGBA(0.5f,0.5f,0.5f,1.0f);
	m_AmbientEnvironment->m_DiffuseFactor = 0.2f;

	api3dScene::GetRoot(api3dScene::WorldLayerIndex())->SetEnvironment(m_AmbientEnvironment);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mspLighting::~mspLighting()
{		
	// cleanup lights
	if (m_PointLight)
		g3dLightMgr::DestroyLight(m_PointLight);
	m_PointLight = NULL;
	if (m_ProjectedLight1)
		g3dLightMgr::DestroyLight(m_ProjectedLight1);
	m_ProjectedLight1 = NULL;
	if (m_ProjectedLight2)
		g3dLightMgr::DestroyLight(m_ProjectedLight2);
	m_ProjectedLight2 = NULL;

	// delete projected light wrappers and texture
	delete m_ProjectedLightWrapper1;
	m_ProjectedLightWrapper1 = NULL;
	delete m_ProjectedLightWrapper2;
	m_ProjectedLightWrapper2 = NULL;
	if (m_LightTexture)
		matTextureMgr::ReleaseTexture(m_LightTexture);
	m_LightTexture = NULL;

	// destroy render states
	delete m_StateRoot;
	m_StateRoot = NULL;
	delete m_AmbientEnvironment;
	m_AmbientEnvironment = NULL;
	
}

//--------------------------------------------------------------------
// Set up projected light shadow buffers
//--------------------------------------------------------------------
void mspLighting::OrientLights()
{		
	// Render the depth maps first
	m_ProjectedLightWrapper1->OrientCamera();
	m_ProjectedLightWrapper1->RenderDepthMap(1.0f);
	m_ProjectedLightWrapper2->OrientCamera();
	m_ProjectedLightWrapper2->RenderDepthMap(1.0f);
}
