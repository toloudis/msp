/*****************************************************************************
**  demG3dTestFAOgen.cpp
**
**		This mode displays a demonstration/test of pixel shaders
**
**	Gigawatt Studios
**	Copyright(C) 2000 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestFAOgen.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneTraverse.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/sc/scObject.hpp"
#include "GraphicsDX9/g3d/g3dDrawStyleUtilDX9.hpp"
#include "GraphicsDX9/shdw/GPUAOEngine.hpp"
#include "GraphicsDX9/shdw/shdwAOPreviewRendererDX9.hpp"
#include "GraphicsDX9/shdw/shdwAORendererD3D.hpp"
#include "GraphicsDX9/shdw/shdwDepthMapAO.hpp"
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
demG3dTestFAOgen::demG3dTestFAOgen(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL), 
	m_pRenderer(NULL),
	m_oldRenderer(NULL),
	m_AOTexRes(256),
	m_AODepthMapRes(512),
	m_scObj(NULL),
	m_entModel(NULL)
{
	m_pRenderer = new shdwAOPreviewRendererDX9();
}

//====================================================================
//====================================================================
demG3dTestFAOgen::~demG3dTestFAOgen()
{
}

//====================================================================
//====================================================================
void demG3dTestFAOgen::Initialize()
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

	m_SmallSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(0.1f, 4, 4);
	m_BigSphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(0.3f, 4, 4);

	effPhongData* pData = dynamic_cast<effPhongData*>(m_SmallSphereMat.GetEffectData());
	pData->m_ColorEmissive = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_SmallSphereMat.SetShaderEffect(("Simple.fx"));

	pData = dynamic_cast<effPhongData*>(m_HilightMat.GetEffectData());
	pData->m_ColorEmissive = (maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	pData->m_ColorAmbient = (maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	pData->m_ColorSpecular = (maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	m_HilightMat.SetShaderEffect(("Simple.fx"));

	g3dLightMgr::SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	g3dPrefs::CurrentPrefs().m_bEnableLitPass = false;
}

void demG3dTestFAOgen::LoadModel(std::string i_name)
{
	DestroyAOTexture destroyAOTexture;
	g3dSceneTraverse::Traverse(m_Root, &destroyAOTexture);

	// delete scene object (the model)
	delete m_scObj;

	RemoveLights();
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
void demG3dTestFAOgen::Think()
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

	ResetLights();

//	m_Light0->SetPosition(light1_pos);

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
void demG3dTestFAOgen::DeInitialize()
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

	delete m_SmallSphereFragment;
	delete m_BigSphereFragment;

	m_Viewer.SetRenderer(m_oldRenderer);
	delete m_pRenderer;
	m_pRenderer = NULL;

	g3dDrawStyleUtilDX9::DeInitialize();

	demG3dTestMode::DeInitialize();
}
//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestFAOgen::ReceiveCharEvent(appCharEvent& i_Event)
{
	maMatrix4x4 m;

	switch( i_Event.GetChar() )
	{
		case itString::CharType('e'):
			g3dPrefs::CurrentPrefs().m_nAOLights += 10;
			if (g3dPrefs::CurrentPrefs().m_nAOLights > 4018)
				g3dPrefs::CurrentPrefs().m_nAOLights = 16;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			ResetLights();
			break;
		case itString::CharType('E'):
			g3dPrefs::CurrentPrefs().m_nAOLights -= 10;
			if (g3dPrefs::CurrentPrefs().m_nAOLights < 16)
				g3dPrefs::CurrentPrefs().m_nAOLights = 16;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			ResetLights();
		break;
		case itString::CharType('r'):
			g3dPrefs::CurrentPrefs().m_AODepthBias += 0.01f;
			if (g3dPrefs::CurrentPrefs().m_AODepthBias > 1)
				g3dPrefs::CurrentPrefs().m_AODepthBias = 1.0f;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('R'):
			g3dPrefs::CurrentPrefs().m_AODepthBias -= 0.01f;
			if (g3dPrefs::CurrentPrefs().m_AODepthBias < 0)
				g3dPrefs::CurrentPrefs().m_AODepthBias = 0.0f;
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
		break;
		case itString::CharType('b'):
		case itString::CharType('B'):
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('u'):
			IncCurrentLight();
			break;
		case itString::CharType('U'):
			NoCurrentLight();
			break;
		case itString::CharType('Y'):
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('y'):
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('T'):
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('t'):
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('1'):
		break;
		case itString::CharType('2'):
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
			DBG_LOG1("Set current mesh to %d", m_CurMesh);
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
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('V'):
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;
		case itString::CharType('4'):
			{
				static int s_Res = 512;
				s_Res = s_Res*2;
				if (s_Res > 2048) s_Res = 128;
				shdwDepthMapAO::ResetDepthMap(s_Res);
				g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			}
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
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
			break;

	}
	SetDisplayInfo();

	demG3dTestMode::ReceiveCharEvent(i_Event);
}

void demG3dTestFAOgen::SetDisplayInfo()
{
	// set up display info
	static const int nLines = 8;
	std::string s[nLines];

	char fs[128];

	s[0].append("GPU AO ");
	s[0].append(" tex ");
	sprintf(fs,"%d ",m_AOTexRes);
	s[0].append(fs);
	s[0].append(" shad ");
	sprintf(fs,"%d ",m_AODepthMapRes);
	s[0].append(fs);

	s[1].append("i j k m - = move camera");
	s[2].append("w wireframe. 2 robust. 1 draw discs");
	s[3].append("3 change model. q quit");
	s[4].append("4 shadRes 5 texRes 6 LOD");
	s[5].append("b light distb'n");
	
	s[6].append("e/E lights=");
	sprintf(fs,"%d",g3dPrefs::CurrentPrefs().m_nAOLights);
	s[6].append(fs);

	s[7].append("r/R depthbias=");
	sprintf(fs,"%0.02f",g3dPrefs::CurrentPrefs().m_AODepthBias);
	s[7].append(fs);

	for (int i =0; i< nLines; i++)
		m_Viewer.SetTextMessage(i, itString(s[i].c_str()));
}

void demG3dTestFAOgen::RemoveLights()
{
	// remove old lights.
	for (int i = 0; i < m_LightNodes.size(); i++)
	{
		m_Root->RemoveChild(m_LightNodes[i]);
		delete m_LightNodes[i];
	}
	m_LightNodes.clear();
}
void demG3dTestFAOgen::ResetLights()
{
	if (m_LightNodes.size() == shdwDepthMapAO::LightArray().size())
		return;

	RemoveLights();

	// add new lights.

	shdwDepthMapAO::ResetLights();
	const std::vector<maPoint3d>& lights = shdwDepthMapAO::LightArray();
	maMatrix4x4 m;

	for (int i = 0; i < lights.size(); i++)
	{
		g3dSceneNode* node = new g3dSceneNode();
		m.MakeTranslate(lights[i]);
		node->SetTransform(m);
		if (shdwDepthMapAO::GetSingleLightIndex() == i)
		{
			node->SetFragment(m_BigSphereFragment);
			node->SetMaterial(&m_HilightMat);
		}
		else
		{
			node->SetFragment(m_SmallSphereFragment);
			node->SetMaterial(&m_SmallSphereMat);
		}
		m_Root->AddChild(node);
		m_LightNodes.push_back(node);
	}
}
void demG3dTestFAOgen::IncCurrentLight()
{
	int N = m_LightNodes.size();
	
	// undo last hilight mat.
	int i = shdwDepthMapAO::GetSingleLightIndex();
	if (i < N && i != -1)
	{
		m_LightNodes[i]->SetFragment(m_SmallSphereFragment);
		m_LightNodes[i]->SetMaterial(&m_SmallSphereMat);
	}

	shdwDepthMapAO::SetSingleLightIndex(i++);
	i = shdwDepthMapAO::GetSingleLightIndex();
	g3dPrefs::CurrentPrefs().m_RecalcAO = g3dPrefs::e_AOGlobal;

	if (i != -1)
	{
		m_LightNodes[i]->SetFragment(m_BigSphereFragment);
		m_LightNodes[i]->SetMaterial(&m_HilightMat);
	}
}
void demG3dTestFAOgen::NoCurrentLight()
{
	int N = m_LightNodes.size();
	
	// undo last hilight mat.
	int i = shdwDepthMapAO::GetSingleLightIndex();
	if (i < N && i != -1)
	{
		m_LightNodes[i]->SetFragment(m_SmallSphereFragment);
		m_LightNodes[i]->SetMaterial(&m_SmallSphereMat);
	}

	shdwDepthMapAO::SetSingleLightIndex(-1);
	g3dPrefs::CurrentPrefs().m_RecalcAO = g3dPrefs::e_AOGlobal;
}
