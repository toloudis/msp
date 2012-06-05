/*****************************************************************************
**  demG3dTestGPUAmbientOcclusion.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Gigawatt Studios
**	Copyright(C) 2000 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestGPUAmbientOcclusion.hpp"

//#include "AOSurfaceElement.h"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "GraphicsDX9/g2d/g2dDX9GlobalWin.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "GraphicsDX9/g3d/g3dDrawStyleUtilDX9.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneTraverse.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfPaths.hpp"
#include "GraphicsDX9/shdw/GPUAOEngine.hpp"
#include "Core/ma/maConstants.hpp"
#include "GraphicsDX9/mat/matRenderTargetTexture.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlImport.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

#include "GraphicsDX9/shdw/shdwShadowLayerRendererDX9.hpp"
#include "GraphicsDX9/shdw/shdwAOPreviewRendererDX9.hpp"
/*
//////////////////////////////
// how to bake vtx interplated data:
// stick this in vtx shader:

    float2 nuPos = float2(IN.UV.x,1-IN.UV.y);
    nuPos = 2.0*(nuPos-0.5);
    OUT.HPosition = float4(nuPos,1.0,1.0);
    OUT.UV = IN.UV.xy;

	// vtx shader will output position to render to texture map
	// that corresponds to UVs of model.
*/




namespace
{
	std::vector<std::string> l_Models;
}


class InitFragments : public g3dSceneNodeProcessor
{
public:
	virtual bool ProcessNode(g3dSceneNode* i_pNode)
	{
		g3dFragment* frag = i_pNode->GetFragment();
		if (frag)
		{
			frag->SetCastsOcclusion(true);
			frag->SetReceivesOcclusion(true);
		}
		return true;
	}
};
class UpdateAOTexture : public g3dSceneNodeProcessor
{
public:
	int m_res;
	UpdateAOTexture(int res):m_res(res){}

	virtual bool ProcessNode(g3dSceneNode* i_pNode)
	{
		g3dFragment* frag = i_pNode->GetFragment();
		if (frag)
		{
			effOcclusionData* odata = frag->GetOcclusionData();

			odata->m_Resolution = m_res;

			bool rebuild = false;
			if ((odata->m_TextureDiffuse == NULL))
			{
				rebuild = true;
			}
			else
			{
				int w,h;
				w = odata->m_TextureDiffuse->GetWidth();
				h = odata->m_TextureDiffuse->GetHeight();
				if ((w != m_res)||(h != m_res))
					rebuild = true;
			}

			if (rebuild)
			{
				matTextureMgr::ReleaseTexture(odata->m_TextureDiffuse);
				matTextureMgr::ReleaseTexture(odata->TMP);
				odata->RemoveTextures();

				fsLocator texDir;

				odata->ReloadTextures(texDir);

				odata->m_bAOInvalid = true;
			}
		}
		return true;
	}
};
class DestroyAOTexture : public g3dSceneNodeProcessor
{
public:
	virtual bool ProcessNode(g3dSceneNode* i_pNode)
	{
		g3dFragment* frag = i_pNode->GetFragment();
		if (frag)
		{
			effOcclusionData* odata = frag->GetOcclusionData();
			matTextureMgr::ReleaseTexture(odata->m_TextureDiffuse);
			matTextureMgr::ReleaseTexture(odata->TMP);
			odata->RemoveTextures();
		}
		return true;
	}
};

//====================================================================
//====================================================================
demG3dTestGPUAmbientOcclusion::demG3dTestGPUAmbientOcclusion(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pRenderer(NULL),
	m_oldRenderer(NULL),
	m_AOTexRes(256),
	m_scObj(NULL),
	m_entModel(NULL)
{
	m_pRenderer = new shdwAOPreviewRendererDX9();
}

//====================================================================
//====================================================================
demG3dTestGPUAmbientOcclusion::~demG3dTestGPUAmbientOcclusion()
{
}

//====================================================================
//====================================================================
void demG3dTestGPUAmbientOcclusion::Initialize()
{
	demG3dTestMode::Initialize();

	// zoom in with camera by default
	this->SetRadius(5.0f);

	// install the AO renderer
	m_oldRenderer = m_Viewer.GetRenderer();
	m_Viewer.SetRenderer(m_pRenderer);
	// Force ambient color in materials to be full white
//	mayReader::SetAlwaysFullAmbient(true);
	g3dDrawStyleUtilDX9::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	SetDisplayInfo();

	// Make kdTree sink to grab triangles
	//geoKDTree kdTree;
	//mayFragInfoSinkKDTree tree_sink(kdTree);

	std::string name;
	fsLocator cfgLocator = gfPaths::GetPath(gfPaths::e_ExePath);
	cfgLocator.Push("data");
	cfgLocator.Push("AOModel.txt");
	gfFileTxt modelFile(cfgLocator, fsFileStream::e_ReadOnly);
	while(modelFile.ReadLine(name))
		l_Models.push_back(name);

	m_CurModel = 0;
	m_CurMesh = 0;
	LoadModel(l_Models[m_CurModel]);

	//	set lights
	m_Light0 = g3dLightMgr::CreateDirectionalLight();
	m_Light0->SetDirection(maVector3d(0,-1,0));
//	m_Light0->SetPosition(maVector3d(0, -10, 0));
	m_Light0->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Light0->Enable();

	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light1->SetDirection(maVector3d(0,1,0));
//	m_Light1->SetPosition(maVector3d(0, -10, 0));
	m_Light1->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Light1->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	g3dPrefs::CurrentPrefs().m_bEnableLitPass = false;
}
void demG3dTestGPUAmbientOcclusion::LoadModel(std::string i_name)
{
	DestroyAOTexture destroyAOTexture;
	g3dSceneTraverse::Traverse(m_Root, &destroyAOTexture);

	// delete scene object (the model)
	delete m_scObj;

	m_Root->DestroyChildren();

	// delete the model data
	delete m_entModel;

//	m_pRenderer->GetAO()->Clear();

	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(i_name.c_str());

	gfPaths::SetMaterialLibraryPath(locator);
	matTextureMgr::SetAllowNullTextures(true);

	m_entModel = entImport::LoadGeometry(locator, fsResourceFinderDir(fsLocator()));
	m_scObj = entImport::CreateObject(*m_entModel);
	maAxisBox b = m_scObj->GetBase()->GetWorldBox();
	m_scObj->SetPosition(b.GetCenter());
	m_Root->AddChild(m_scObj->GetBase());

	InitFragments initFragments;
	g3dSceneTraverse::Traverse(m_Root, &initFragments);

	// setup the AO textures
	UpdateAOTexture updateAOTextures(m_AOTexRes);
	g3dSceneTraverse::Traverse(m_Root, &updateAOTextures);

	g3dPrefs::CurrentPrefs().m_bEnableAO = true;
	g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;

	DBG_LOG1("Loaded %s", i_name.c_str());
	//m_pRenderer->GetAO()->CreateElements();
	//m_pRenderer->GetAO()->SelectNode(NULL);
}

//====================================================================
//	Think
//====================================================================
void demG3dTestGPUAmbientOcclusion::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	set the orientations of our lights
	float theta = frame_time * maConstants::c_fAngleToRad * 30.0f;
	float phi = frame_time * maConstants::c_fAngleToRad * 60.0f;

	float x, y, z;

	x = 3.75f * sin(theta);
	y = 2.5f + 2.0f * sin(phi);
	z = 3.75f * cos(theta);
	maPoint3d light1_pos(x, y, z);

//	m_Light0->SetPosition(light1_pos);

	//	set the orientations of our objects
	//maMatrix4x4& matrix1 = m_Ship->GetTransform();
	////matrix1.MakeScale(0.5f,0.5f,0.5f);
	//matrix1.MakeRotate( -frame_time * 30.0f * maConstants::c_fAngleToRad,
	//						maVector3d(0, 1, 0));

	//	render
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestGPUAmbientOcclusion::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light0);
	g3dLightMgr::DestroyLight(m_Light1);

	//	cleanup models
	//g3dScene::RemoveModel(m_Ship);

	DestroyAOTexture destroyAOTexture;
	g3dSceneTraverse::Traverse(m_Root, &destroyAOTexture);

	// delete scene object (the model)
	delete m_scObj;

	delete m_Root;
	delete m_Scene;

	// delete the model data
	delete m_entModel;

	m_Viewer.SetRenderer(m_oldRenderer);
	delete m_pRenderer;
	m_pRenderer = NULL;

	g3dDrawStyleUtilDX9::DeInitialize();

	demG3dTestMode::DeInitialize();
}
//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestGPUAmbientOcclusion::ReceiveCharEvent(appCharEvent& i_Event)
{
	//AOEngine* AO = m_pRenderer->GetAO();
	maMatrix4x4 m;

	switch( i_Event.GetChar() )
	{
		case itString::CharType('e'):
		case itString::CharType('E'):
//			AO->m_DrawElements = !AO->m_DrawElements;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
		break;
		case itString::CharType('r'):
		case itString::CharType('R'):
//			m_pRenderer->GPUParams().m_DoDiscOcclusion = !m_pRenderer->GPUParams().m_DoDiscOcclusion;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG0("Compute Disc Occlusion");
		break;
		case itString::CharType('b'):
		case itString::CharType('B'):
//			m_pRenderer->GPUParams().m_DoWeightedMin = !m_pRenderer->GPUParams().m_DoWeightedMin;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG0("Compute Weighted Min Occlusion");
			break;
		case itString::CharType('U'):
			g3dPrefs::CurrentPrefs().m_AOTriAtten += 0.1f;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG1("Triangle Attenuation %f", g3dPrefs::CurrentPrefs().m_AOTriAtten);
			break;
		case itString::CharType('u'):
			g3dPrefs::CurrentPrefs().m_AOTriAtten -= 0.1f;
			g3dPrefs::CurrentPrefs().m_AOTriAtten = max(0.01f, g3dPrefs::CurrentPrefs().m_AOTriAtten);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG1("Triangle Attenuation %f", g3dPrefs::CurrentPrefs().m_AOTriAtten);
			break;
		case itString::CharType('Y'):
			g3dPrefs::CurrentPrefs().m_AOEpsilon += 0.1f;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG1("Epsilon %f", g3dPrefs::CurrentPrefs().m_AOEpsilon);
			break;
		case itString::CharType('y'):
			g3dPrefs::CurrentPrefs().m_AOEpsilon -= 0.1f;
			g3dPrefs::CurrentPrefs().m_AOEpsilon = max(0.1f, g3dPrefs::CurrentPrefs().m_AOEpsilon);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG1("Epsilon %f", g3dPrefs::CurrentPrefs().m_AOEpsilon);
			break;
		case itString::CharType('T'):
			g3dPrefs::CurrentPrefs().m_AODistAtten += 0.1f;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG1("Distance Attenuation %f", g3dPrefs::CurrentPrefs().m_AODistAtten );
			break;
		case itString::CharType('t'):
			g3dPrefs::CurrentPrefs().m_AODistAtten -= 0.1f;
			g3dPrefs::CurrentPrefs().m_AODistAtten = max(0.0f, g3dPrefs::CurrentPrefs().m_AODistAtten);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			DBG_LOG1("Distance Attenuation %f", g3dPrefs::CurrentPrefs().m_AODistAtten );
			break;
		case itString::CharType('1'):
//			m_pRenderer->m_NodeHt++;
		break;
		case itString::CharType('2'):
//			m_pRenderer->GPUParams().m_bRobust = !m_pRenderer->GPUParams().m_bRobust;
//			DBG_LOG1("Using robust shader: %s", m_pRenderer->GPUParams().m_bRobust?"true":"false");
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
		break;
		case itString::CharType('w'):
			m_Root->SetDrawStyle((m_Root->GetDrawStyle() == g3dSceneNode::e_Wireframe) 
				? g3dSceneNode::e_Solid : g3dSceneNode::e_Wireframe);
		break;
		case itString::CharType('3'):
			m_CurModel = (++m_CurModel) % l_Models.size();
			LoadModel(l_Models[m_CurModel]);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
		break;

		case itString::CharType('x')://change cur mesh; 0 is no mesh.
			m_CurMesh = (++m_CurMesh) % (m_Root->GetNumChildren()+1);
			if (m_CurMesh != 0)
;//				AO->SelectNode(m_Root->GetChild(m_CurMesh-1));
			else
;//				AO->SelectNode(NULL);
			break;
		case itString::CharType('c'):// toggle cur mesh from AO, and hide/show it.
			if (m_CurMesh != 0)
			{
				if (m_Root->GetChild(m_CurMesh-1)->GetRenderable())
				{
//					AO->RemoveNode(m_Root->GetChild(m_CurMesh-1));
					m_Root->GetChild(m_CurMesh-1)->SetRenderable(false);
				}
				else
				{
//					AO->AddNode(m_Root->GetChild(m_CurMesh-1));
					m_Root->GetChild(m_CurMesh-1)->SetRenderable(true);
				}
//				AO->CreateElements();
			}
		break;
		case itString::CharType('v'):
			g3dPrefs::CurrentPrefs().m_AOZoneRad += 0.01f;
			DBG_LOG1("Zone Radius %f", g3dPrefs::CurrentPrefs().m_AOZoneRad);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('V'):
			g3dPrefs::CurrentPrefs().m_AOZoneRad -= 0.01f;
			g3dPrefs::CurrentPrefs().m_AOZoneRad = max(0.01f, g3dPrefs::CurrentPrefs().m_AOZoneRad);
			DBG_LOG1("Zone Radius %f", g3dPrefs::CurrentPrefs().m_AOZoneRad);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('4'):
//			m_pRenderer->GPUParams().m_GenUVMap = !m_pRenderer->GPUParams().m_GenUVMap;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('5'):
			{
			m_AOTexRes = m_AOTexRes*2;
			if (m_AOTexRes > 1024) m_AOTexRes = 256;
			UpdateAOTexture updateAOTextures(m_AOTexRes);
			g3dSceneTraverse::Traverse(m_Root, &updateAOTextures);
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			}
			break;
		case itString::CharType('6'):
//			m_pRenderer->GPUParams().m_TexLOD = (m_pRenderer->GPUParams().m_TexLOD+1)%4;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;

	}
	SetDisplayInfo();

	demG3dTestMode::ReceiveCharEvent(i_Event);
}

void demG3dTestGPUAmbientOcclusion::SetDisplayInfo()
{
	// set up display info
	static const int nLines = 10;
	std::string s[nLines];

	char fs[128];

	GPUAOParams p;

	s[0].append("GPU AO ");
	s[0].append((p.m_bRobust) ? "robust" : "basic");
	if (p.m_GenUVMap)
	{
		s[0].append(" textured ");
		sprintf(fs,"%d ",m_AOTexRes);
		s[0].append(fs);
		sprintf(fs,"LOD %d ",p.m_TexLOD);
		s[0].append(fs);
	}

	s[1].append("R disc occlusion. B Weighted min");
	s[2].append("i j k m - = move camera");
	s[3].append("w wireframe. 2 robust. 1 draw discs");
	s[4].append("3 change model. q quit");
	s[5].append("4 textured 5 texRes 6 LOD");
	
	s[6].append("y/Y epsilon=");
	sprintf(fs,"%0.02f",g3dPrefs::CurrentPrefs().m_AOEpsilon);
	s[6].append(fs);

	s[7].append("u/U tri atten=");
	sprintf(fs,"%0.02f",g3dPrefs::CurrentPrefs().m_AOTriAtten);
	s[7].append(fs);

	s[8].append("v/V zone radius=");
	sprintf(fs,"%0.02f",g3dPrefs::CurrentPrefs().m_AOZoneRad);
	s[8].append(fs);

	s[9].append("t/T dist atten=");
	sprintf(fs,"%0.02f",g3dPrefs::CurrentPrefs().m_AODistAtten);
	s[9].append(fs);

	for (int i =0; i< nLines; i++)
		m_Viewer.SetTextMessage(i, itString(s[i].c_str()));
}


