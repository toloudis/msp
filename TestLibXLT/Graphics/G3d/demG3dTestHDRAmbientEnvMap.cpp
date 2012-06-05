/*****************************************************************************
**  demG3dTestHDRAmbientEnvMap.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestHDRAmbientEnvMap.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "demModeManager.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "GraphicsDX9/eff/effShaderArray.hpp"
#include "GraphicsDX9/eff/effShaderUtilWin.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "GraphicsDX9/g3d/g3dSceneRendererDX9.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mdl/mdlImport.hpp"
#include "Graphics/mdl/mdlReader.hpp"

#include "Graphics/sc/scObject.hpp"
#include "GraphicsDX9/shdw/shdwHDRRendererDX9.hpp"
#include "GraphicsDX9/shdw/shdwShadowLayerRendererDX9.hpp"

namespace
{
struct RangedFloat
{
	float m_Min, m_Max, m_Inc;
	RangedFloat(float mi, float ma, float in):m_Min(mi),m_Max(ma),m_Inc(in){}
	float Dec(float val) const
	{
		val -= m_Inc;
		if (val < m_Min) 
			val = m_Min;
		return val;
	}
	float Inc(float val) const
	{
		val += m_Inc;
		if (val > m_Max) 
			val = m_Max;
		return val;
	}
};

const RangedFloat m_AmbScaleRange(0.0f, 1.0f, 0.1f);
const RangedFloat m_MipRange(0.0f, 8.0f, 1.0f);
const RangedFloat m_AmbAngleRange(0.0f, 180.0f, 5.0f);

const float c_AspectIncrement = 2.0f;
const float c_MaxAspect = 64.0f;
const float c_MinAspect = 0.004f;

}

//====================================================================
//====================================================================
demG3dTestHDRAmbientEnvMap::demG3dTestHDRAmbientEnvMap(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pRenderer(NULL),
	m_oldRenderer(NULL),
	m_Texture1(NULL), m_RectFragment(NULL),
	m_SmallSphereFragment(NULL),
	m_Light1(NULL),
	m_Light2(NULL),
	m_PointLight1(NULL),
	m_PointLight2(NULL),
	m_AmbEnvMap(NULL),
	m_pStateRoot(NULL),
	m_pEnvRoot(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestHDRAmbientEnvMap::~demG3dTestHDRAmbientEnvMap()
{
}

void SetEnvMap(g3dSceneNode* node, matTexture* tex, float scale, float bias);

//====================================================================
//====================================================================
void demG3dTestHDRAmbientEnvMap::Initialize()
{
	demG3dTestMode::Initialize();

	m_pRenderer = new shdwHDRRendererDX9();
	m_oldRenderer = m_Viewer.GetRenderer();
	m_Viewer.SetRenderer(m_pRenderer);

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Ambient Env Map test."));
	m_Viewer.SetTextMessage(1, itString("Press space to go to the next section"));
	m_Viewer.SetTextMessage(2, itString("Press [A,L] to toggle [ambient,lit] pass"));
	m_Viewer.SetTextMessage(3, itString("1,2 - toggle lights  S - shadow casting lights"));
	m_Viewer.SetTextMessage(4, itString("o,p - mip level    [,] - amb env multiplier"));

	// Force ambient color in materials to be full white
	mdlReader::SetAlwaysFullAmbient(true);

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root, 
		g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative,
		true, true)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());
	Camera().SetClip(1.0f, 255.0f);

	g3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));

	//	setup data path
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	fsLocator texLocator = gfPaths::GetPath(gfPaths::e_ExePath);
	texLocator.Push("data");

	//	make models
	static char* names[NMODELS] = {
		"test_sphere_BD.mx",
		"test_sphere_D.mx",
		"test_sphere_E.mx",
		"test_sphere_EBD.mx",
		"test_sphere_ED.mx",
		"test_sphere_SBD.mx",
		"test_sphere_SD.mx",
		"test_sphere_SEBD.mx",
		"test_sphere_SED.mx"
	};
	static float pos[3*NMODELS] = {
		20,		0,	-20,
		0,		0,	-20,
		-20,	0,	-20,
		20,		0,	0,
		0,		0,	0,
		-20,	0,	0,
		20,		0,	20,
		0,		0,	20,
		-20,	0,	20,
	};

	for (int i = 0; i < NMODELS; i++)
	{
		locator.Push(names[i]);
		m_entModels[i] = entImport::LoadGeometry(locator, fsResourceFinderDir(texLocator));
		m_scObjs[i] = entImport::CreateObject(*m_entModels[i]);
		m_scObjs[i]->SetPosition(maPoint3d(pos[i*3], pos[i*3+1], pos[i*3+2]));
		m_Root->AddChild(m_scObjs[i]->GetBase());
		locator.Pop();
	}

	// lights
	m_SmallSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1.3f, 15, 15);
	effPhongData* pData = dynamic_cast<effPhongData*>(m_SmallSphereMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 0.5f));
	m_SmallSphereFragment->SetMaterial(&m_SmallSphereMat);
	m_Light1 = new g3dSceneNode();
	m_Light1->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light1);
	m_Light2 = new g3dSceneNode();
	m_Light2->SetFragment(m_SmallSphereFragment);
	m_Root->AddChild(m_Light2);
	m_PointLight1 = g3dLightMgr::CreatePointLight();
	m_PointLight2 = g3dLightMgr::CreatePointLight();
	m_PointLight1->SetIntensity(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
	m_PointLight2->SetIntensity(maFloatRGBA(0.2f, 0.2f, 0.7f, 1.0f));
	m_PointLight1->SetCastsShadow(true);
	m_PointLight2->SetCastsShadow(true);
	m_PointLight1->Enable();
	m_PointLight2->Enable();
	float theta = 0;
	float phi = 0;
	float x, y, z;
	x = 45.0f * sin(theta);
	y = 30.0f + 4.0f * sin(phi);
	z = 45.0f * cos(theta);
	maPoint3d light1_pos(x, y, z);
	x = 45.0f * sin(theta + maConstants::c_fPI_Div_2);
	y = 30.0f + 4.0f * sin(phi + maConstants::c_fPI_Div_2);
	z = 45.0f * cos(theta + maConstants::c_fPI_Div_2);
	maPoint3d light2_pos(x, y, z);
	m_PointLight1->SetPosition(light1_pos);
	m_PointLight2->SetPosition(light2_pos);
	maMatrix4x4 m;
	m.MakeTranslate(light1_pos.m_X, light1_pos.m_Y, light1_pos.m_Z);
	m_Light1->SetTransform(m);
	m.MakeTranslate(light2_pos.m_X, light2_pos.m_Y, light2_pos.m_Z);
	m_Light2->SetTransform(m);

	// ambient + lit render states
	m_pStateRoot = new g3dRenderState();
	m_pStateRoot->m_AmbientLight = maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f);
	m_pStateRoot->m_Lights.push_back(m_PointLight1);
	m_pStateRoot->m_Lights.push_back(m_PointLight2);
	m_AmbEnvMap = matTextureMgr::LoadTexture(texLocator, itString("Sky128.dds"));
	m_pEnvRoot = new g3dAmbientEnvState();
	m_pEnvRoot->m_DiffuseMap = m_AmbEnvMap;
	m_pEnvRoot->m_DiffuseFactor = 0.5;
	m_Root->SetRenderState(m_pStateRoot);
	m_Root->SetEnvironment(m_pEnvRoot);

	// need single light rendering to do a projected light
	g3dSingleLightRendering::SetDoSingleLightRendering(true);
}

//====================================================================
//	Think
//====================================================================
void demG3dTestHDRAmbientEnvMap::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestHDRAmbientEnvMap::DeInitialize()
{
	// restore single light rendering flag
	g3dSingleLightRendering::SetDoSingleLightRendering(false);

	// cleanup lights
	g3dLightMgr::DestroyLight(m_PointLight1);
	g3dLightMgr::DestroyLight(m_PointLight2);

	// cleanup render states
	delete m_pStateRoot;
	delete m_pEnvRoot;

	//	cleanup fragments
	delete m_RectFragment;
	delete m_SmallSphereFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_AmbEnvMap);
	matTextureMgr::ReleaseTexture(m_Texture1);

	m_Viewer.ClearTextMessages();
	for (int i = 0; i < NMODELS; i++)
	{
		delete m_scObjs[i];
	}
	delete m_Root;
	delete m_Scene;
	m_Viewer.SetRenderer(m_oldRenderer);
	delete m_pRenderer;

	for (int i = 0; i < NMODELS; i++)
	{
		delete m_entModels[i];
	}

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestHDRAmbientEnvMap::ReceiveCharEvent(appCharEvent& i_Event)
{
	static bool bShown = false;
	static int iMode = 0;
	switch( i_Event.GetChar() )
	{
		case itString::CharType('a'):
		case itString::CharType('A'):
			g3dPrefs::CurrentPrefs().m_bEnableAmbientPass = ! g3dPrefs::CurrentPrefs().m_bEnableAmbientPass;
		break;
		case itString::CharType('l'):
		case itString::CharType('L'):
			g3dPrefs::CurrentPrefs().m_bEnableLitPass = ! g3dPrefs::CurrentPrefs().m_bEnableLitPass;
		break;
		case itString::CharType('s'):
		case itString::CharType('S'):
			m_PointLight1->SetCastsShadow(!m_PointLight1->GetCastsShadow());
			m_PointLight2->SetCastsShadow(!m_PointLight2->GetCastsShadow());
		break;
		case itString::CharType('1'):
			if (m_PointLight1->IsEnabled())
				m_PointLight1->Disable();
			else
				m_PointLight1->Enable();
		break;
		case itString::CharType('2'):
			if (m_PointLight2->IsEnabled())
				m_PointLight2->Disable();
			else
				m_PointLight2->Enable();
		break;
		case itString::CharType('['):
			m_pEnvRoot->m_DiffuseFactor = m_AmbScaleRange.Dec(m_pEnvRoot->m_DiffuseFactor);
		break;
		case itString::CharType(']'):
			m_pEnvRoot->m_DiffuseFactor = m_AmbScaleRange.Inc(m_pEnvRoot->m_DiffuseFactor);
		break;
		case itString::CharType('f'):
			m_pEnvRoot->m_DiffuseAngle = m_AmbAngleRange.Dec(m_pEnvRoot->m_DiffuseAngle);
		break;
		case itString::CharType('g'):
			m_pEnvRoot->m_DiffuseAngle = m_AmbAngleRange.Inc(m_pEnvRoot->m_DiffuseAngle);
		break;
		case itString::CharType('h'):
		case itString::CharType('H'):
			g3dPrefs::CurrentPrefs().m_HDRDebugMode = (++iMode) % 8;
		break;

	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}
