/*****************************************************************************
**  demG3dTestHairShader.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestHairShader.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "demModeManager.hpp"
#include "Graphics/eff/effHairData.hpp"
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
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mdl/mdlImport.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/mtr/mtrMaterialUtil.hpp"
#include "Graphics/sc/scObject.hpp"

#include "Core/gf/gfPaths.hpp"


namespace
{
const int c_NumShaderMats = 1;
const float c_TimePerShader = 2.0f;
bool l_bRotateShader = true;

struct RangedFloat
{
	float m_Min, m_Max, m_Inc;
	float m_Val;
	RangedFloat(float init, float mi, float ma, float in):m_Min(mi),m_Max(ma),m_Inc(in),m_Val(init){}
	float Dec()
	{
		m_Val -= m_Inc;
		if (m_Val < m_Min) 
			m_Val = m_Min;
		return m_Val;
	}
	float Inc()
	{
		m_Val += m_Inc;
		if (m_Val > m_Max) 
			m_Val = m_Max;
		return m_Val;
	}
};

RangedFloat g_LightRadius(25, 5, 60, 1);
RangedFloat g_SpecExp0(200, 0, 300, 10);
RangedFloat g_SpecExp1(200, 0, 300, 10);
RangedFloat g_SpecShift0(0.12f, -1, 1, 0.1f);
RangedFloat g_SpecShift1(0.66f, -1, 1, 0.1f);

RangedFloat g_AmbientLight(0.2f, 0, 1, 0.1f);

}

//====================================================================
//====================================================================
demG3dTestHairShader::demG3dTestHairShader(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_SmallSphereFragment(NULL),
	m_Light1(NULL),
	m_Light2(NULL),
	m_PointLight1(NULL),
	m_PointLight2(NULL),
	m_pStateRoot(NULL),
	m_pHairMaterial(NULL), m_pHairData(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestHairShader::~demG3dTestHairShader()
{
}

//====================================================================
//====================================================================
void demG3dTestHairShader::Initialize()
{
	demG3dTestMode::Initialize();

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Hair shader"));
	m_Viewer.SetTextMessage(1, itString("Space:go to next section"));
	m_Viewer.SetTextMessage(2, itString("[A,L]:toggle [ambient,lit] pass"));
	m_Viewer.SetTextMessage(3, itString("1,2:toggle lights  S:shadow casting lights"));
	m_Viewer.SetTextMessage(4, itString("[,]:light radius"));
	m_Viewer.SetTextMessage(5, itString("f,F,g,G:specular shift"));
	m_Viewer.SetTextMessage(6, itString("z,Z:ambient level"));

	// Force ambient color in materials to be full white
	mdlReader::SetAlwaysFullAmbient(true);

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root, 
		g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative,
		true, true)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());
	Camera().SetClip(1.0f, 255.0f);

	//	setup data path
	fsLocator locator;// = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("C:\\Projects\\Test-Data\\Data\\STOCK\\Props\\General");
	locator.Push("Models");
	fsLocator texLocator;// = gfPaths::GetPath(gfPaths::e_ExePath);
	texLocator.Push("C:\\Projects\\Test-Data\\Data\\STOCK\\Props\\General");
	texLocator.Push("Textures");

	// load the model
	locator.Push("Hair_Test.mx");
	m_entModel = entImport::LoadGeometry(locator, fsResourceFinderDir(texLocator));
	m_scObj = entImport::CreateObject(*m_entModel);
	maAxisBox b = m_scObj->GetBase()->GetWorldBox();
	m_scObj->SetPosition(b.GetCenter());
	m_Root->AddChild(m_scObj->GetBase());
	locator.Pop();

	// find a hair fragment and install the hair shader and data in it.
	int nMats = m_entModel->Materials().size();
	int nFrags = m_entModel->Fragments().size();
	std::vector<matMaterial*>& mats = m_entModel->Materials();
	for (int i = 0; i < nMats; i++)
	{
		mtrMaterialUtil::RemoveTextures(*mats[i], *m_entModel);
		mats[i]->SetHasFur(false);
		mats[i]->SetHasGlow(false);

		std::string::size_type loc = mats[i]->GetName().find( "hair", 0 );
		if (( loc != std::string::npos ) && (m_pHairMaterial == NULL))
		{
			m_pHairMaterial = mats[i];
		}
		loc = mats[i]->GetName().find( "Hair", 0 );
		if (( loc != std::string::npos ) && (m_pHairMaterial == NULL))
		{
			m_pHairMaterial = mats[i];
		}
	}
	if (m_pHairMaterial != NULL)
	{
		// setup hair shader
		effHairData data;
		data.m_NameAlpha = "HairAlpha.bmp";
		data.m_NameBase = "HairBase.bmp";
		//data.m_NameNormalMap = "hairNormal.bmp";
		data.m_NameSpecularMask = "HairSpecMask.bmp";
		data.m_NameSpecularShift = "HairShift.bmp";
		data.m_SpecularColor0 = maFloatRGBA(0, 1, 0, 1);
		data.m_SpecularColor1 = maFloatRGBA(1, 0, 0, 1);
		data.m_HairBaseColor = maFloatRGBA(0.557f, 0.238f, 0.06f, 1);
		data.m_SpecularExponent0 = g_SpecExp0.m_Val;
		data.m_SpecularExponent1 = g_SpecExp1.m_Val;
		data.m_SpecularShift0 = g_SpecShift0.m_Val;
		data.m_SpecularShift1 = g_SpecShift1.m_Val;

		m_pHairMaterial->SetHasGlow(true);
		m_pHairMaterial->GlowData().m_GlowAmount = 4.0f;
		m_pHairMaterial->GlowData().m_GlowSize = 0.1f;
		m_pHairMaterial->GlowData().m_GlowScale = maVector4d(1,1,0,0);

		m_pHairMaterial->SetShaderEffect("Hair.fx", &data);
		m_pHairMaterial->GetEffectData()->ReloadTextures(texLocator);
		m_pHairMaterial->GetEffectData()->GetTextures(m_entModel->Textures());
		m_pHairData = (effHairData*)m_pHairMaterial->GetEffectData();
	}


	// lights
	m_SmallSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1.3f, 15, 15);
	effPhongData* pData = NULL;
	pData = dynamic_cast<effPhongData*>(m_SmallSphereMat.GetEffectData());
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
	m_PointLight1->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_PointLight2->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_PointLight1->SetCastsShadow(true);
	m_PointLight2->SetCastsShadow(true);
	m_PointLight1->Enable();
	m_PointLight2->Enable();
	PositionLights();

	// ambient + lit render states
	m_pStateRoot = new g3dRenderState();
	m_pStateRoot->m_AmbientLight = maFloatRGBA(g_AmbientLight.m_Val, g_AmbientLight.m_Val, g_AmbientLight.m_Val, 1.0f);
	m_pStateRoot->m_Lights.push_back(m_PointLight1);
	m_pStateRoot->m_Lights.push_back(m_PointLight2);
	m_Root->SetRenderState(m_pStateRoot);

	// need single light rendering to do a projected light
	g3dSingleLightRendering::SetDoSingleLightRendering(true);

	// enable glow
	g3dPrefs::CurrentPrefs().m_bEnableGlow = true;
}

//====================================================================
//	Think
//====================================================================
void demG3dTestHairShader::Think()
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
void demG3dTestHairShader::DeInitialize()
{
	// restore single light rendering flag
	g3dSingleLightRendering::SetDoSingleLightRendering(false);

	// cleanup lights
	g3dLightMgr::DestroyLight(m_PointLight1);
	g3dLightMgr::DestroyLight(m_PointLight2);

	// cleanup render states
	delete m_pStateRoot;

	//	cleanup fragments
	delete m_SmallSphereFragment;

	m_Viewer.ClearTextMessages();

	// delete scene object (the model)
	delete m_scObj;

	// clean up scene graph for this scene
	delete m_Root;
	delete m_Scene;

	// delete the model data
	delete m_entModel;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestHairShader::ReceiveCharEvent(appCharEvent& i_Event)
{
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
			g_LightRadius.Dec();
			PositionLights();
		break;
		case itString::CharType(']'):
			g_LightRadius.Inc();
			PositionLights();
		break;
		case itString::CharType('f'):
			m_pHairData->m_SpecularShift0 = g_SpecShift0.Inc();
		break;
		case itString::CharType('F'):
			m_pHairData->m_SpecularShift0 = g_SpecShift0.Dec();
		break;
		case itString::CharType('g'):
			m_pHairData->m_SpecularShift1 = g_SpecShift1.Inc();
		break;
		case itString::CharType('G'):
			m_pHairData->m_SpecularShift1 = g_SpecShift1.Dec();
		break;

		case itString::CharType('v'):
			m_pHairData->m_SpecularExponent0 = g_SpecExp0.Inc();
		break;
		case itString::CharType('V'):
			m_pHairData->m_SpecularExponent0 = g_SpecExp0.Dec();
		break;
		case itString::CharType('b'):
			m_pHairData->m_SpecularExponent1 = g_SpecExp1.Inc();
		break;
		case itString::CharType('B'):
			m_pHairData->m_SpecularExponent1 = g_SpecExp1.Dec();
		break;

		case itString::CharType('z'):
			g_AmbientLight.Inc();
			m_pStateRoot->m_AmbientLight = maFloatRGBA(g_AmbientLight.m_Val, g_AmbientLight.m_Val, g_AmbientLight.m_Val, 1.0f);
		break;
		case itString::CharType('Z'):
			g_AmbientLight.Dec();
			m_pStateRoot->m_AmbientLight = maFloatRGBA(g_AmbientLight.m_Val, g_AmbientLight.m_Val, g_AmbientLight.m_Val, 1.0f);
		break;
	}

	demG3dTestMode::ReceiveCharEvent(i_Event);
}

void demG3dTestHairShader::PositionLights()
{
	float theta = 75.0f * maConstants::c_fAngleToRad;
	float phi = 0;
	float x, y, z;
	x = g_LightRadius.m_Val * sin(phi) * sin(theta);
	y = g_LightRadius.m_Val * cos(theta);
	z = g_LightRadius.m_Val * cos(phi) * sin(theta);
	maPoint3d light1_pos(x, y, z);
	phi += 90.0f * maConstants::c_fAngleToRad;
	x = g_LightRadius.m_Val * sin(phi) * sin(theta);
	y = g_LightRadius.m_Val * cos(theta);
	z = g_LightRadius.m_Val * cos(phi) * sin(theta);
	maPoint3d light2_pos(x, y, z);

	m_PointLight1->SetPosition(light1_pos);
	m_PointLight2->SetPosition(light2_pos);
	maMatrix4x4 m;
	m.MakeTranslate(light1_pos.m_X, light1_pos.m_Y, light1_pos.m_Z);
	m_Light1->SetTransform(m);
	m.MakeTranslate(light2_pos.m_X, light2_pos.m_Y, light2_pos.m_Z);
	m_Light2->SetTransform(m);
}
