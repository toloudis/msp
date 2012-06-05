/*****************************************************************************
**  demTestShadowShapes.cpp
**
**		This mode tests shadowing through projected lights
**	with shapes created through api3dShape.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "demTestShadowShapes.hpp"



#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dProjectedLightWrapper.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appModeMgr.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"

#include "GraphicsDX9/mat/matTextureMgrDX9.hpp"
#include "GraphicsDX9/mat/matPlainTexture.hpp"

namespace
{
	const int c_DepthMapSize = 512;
}

//====================================================================
//====================================================================
demTestShadowShapes::demTestShadowShapes(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pShape1(NULL)
{
}

//====================================================================
//====================================================================
demTestShadowShapes::~demTestShadowShapes()
{
}

//====================================================================
//====================================================================
void demTestShadowShapes::Initialize()
{
	Camera().SetClip(0.4f, 160.0f);
	demTestMode::Initialize();

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Shadow map test - api3dShape"));
	m_Viewer.SetTextMessage(1, itString("Meshes created by api3dShape do not cast/receive shadows by default"));
	m_Viewer.SetTextMessage(2, itString("the assumption is that they are used as icons"));
	m_Viewer.SetTextMessage(3, itString("'s' toggles shadow flags on fragments, 'f' - sets light to camera view"));
	
	// Shader array (needed for projected lights) needs the tangent info in bumpTriMeshBumpFrag, but
	// api3dShape only makes regular tmeshRegFrag fragments. So, this example will not light.
	//TODO [bga] - To fix the test, we could use mayFragCreate::GetBumpFragChoice to control
	// if g3dPrimitiveFragmentUtil creates tmesh or bump fragments.   
	m_Viewer.SetTextMessage(5, itString("TEST IS BROKEN - shader array doesn't work on tmeshRegFrag, only bumpTriMeshBumpFrag"));
	
	// Create shapes
	m_pShape1 = api3dShape::CreateRectangle(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f), 30, 30, 8, 8, false);
	effPhongData* pPhongData = dynamic_cast<effPhongData*>(m_pShape1->Material()->GetEffectData());
	DBG_ASSERT0(pPhongData != NULL, "SimpleShape not using effPhong");
	pPhongData->m_ColorDiffuse = maFloatRGBA(0.5f, 0.3f, 0.7f, 1.0f);
	m_pShape1->SetOrientation(maRotation(maVector3d(1,0,0), -maConstants::c_fPI_Div_2));
	m_pShape2 = api3dShape::CreateSphere(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f), 4, 16, 16);
	pPhongData = dynamic_cast<effPhongData*>(m_pShape2->Material()->GetEffectData());
	DBG_ASSERT0(pPhongData != NULL, "SimpleShape not using effPhong");
	pPhongData->m_ColorDiffuse = maFloatRGBA(0.3f, 0.5f, 0.9f, 1.0f);
	m_pShape2->SetPosition(maPoint3d(0,4,0));

	// add to scene
	api3dScene::AddObject(m_pShape1);
	api3dScene::AddObject(m_pShape2);

	// Create g3d light
	m_pLight = api3dLightMgr::CreateProjectedLight();
	m_pLight->SetPosition(maPoint3d(12, 10, -10));
	m_pLight->SetTarget(maPoint3d(0,0,0));
	m_pLight->SetIntensity(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_pLight->SetCastsShadow(true);
	m_pLight->Enable();

	fsLocator tex_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	tex_loc.Push("data");
	//tex_loc.Push("projection.bmp");
	tex_loc.Push("Projection.dds");

	try
	{
		// load texture to project (have to make sure this isn't a mip-map
		// texture, or else you get a line at the light's plane.)
		matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc, TEXTURE_TYPE_2D, false);
		//matTexture *pTexture = matTextureMgrDX9::LoadPlainTexture(tex_loc, 0,0);

		m_pPrjLightWrapper = new api3dProjectedLightWrapper(*m_pLight, c_DepthMapSize);
		m_pPrjLightWrapper->SetTexture(pTexture); // ownership of texture passes to wrapper
		m_pPrjLightWrapper->OrientCamera();
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("File not found: %s", filename.c_str());
		this->SetTerminateCondition( appMode::e_TerminateAndDestroy );
	}
	catch( const mdlInvalidModelFileX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("Invalid Model File: %s", filename.c_str());
		this->SetTerminateCondition( appMode::e_TerminateAndDestroy );
	}
	catch( ... )
	{
		DBG_WARNING0("Unknown error");
		throw;
	}


	//api3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));

	// hmm, shader array doesn't run on regular fragments anymore.
	g3dSingleLightRendering::SetDoSingleLightRendering(true);
}

//====================================================================
//	Think
//====================================================================
void demTestShadowShapes::Think()
{
	demTestMode::Think();

	float frame_time = appTime::GetTime();

	inDeviceMgr::Think();
	tma3dCursorMgr::Think();

	api3dScene::Think( frame_time );
	cam3dMgr::Think();
	
	if (g3dSingleLightRendering::GetDoSingleLightRendering())
		api3dTargetRendererMgr::RenderTargets( frame_time );

	//	render
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( appMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestShadowShapes::DeInitialize()
{	
	g3dSingleLightRendering::SetDoSingleLightRendering(false);

	api3dScene::RemoveObject(m_pShape1);
	delete m_pShape1;
	api3dScene::RemoveObject(m_pShape2);
	delete m_pShape2;

	api3dLightMgr::DestroyLight( m_pLight );
	delete m_pPrjLightWrapper;

	m_Viewer.ClearTextMessages();

	demTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestShadowShapes::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
	// set proj light to camera's position	
	case itString::CharType('F'):
	case itString::CharType('f'):
		m_pLight->SetPosition(cam3dMgr::GetCamera().GetPosition());
		m_pPrjLightWrapper->OrientCamera();
		break;	
	// toggle shadow flags
	case itString::CharType('S'):
	case itString::CharType('s'):
		m_pShape1->Fragment()->SetReceivesShadow(!m_pShape1->GetFragment()->GetReceivesShadow());
		m_pShape2->Fragment()->SetCastsShadow(!m_pShape2->GetFragment()->GetCastsShadow());
		break;	
	}

	demTestMode::ReceiveCharEvent(i_Event);
}

