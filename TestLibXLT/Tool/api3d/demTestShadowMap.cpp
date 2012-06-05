/*****************************************************************************
**  demTestShadowMap.cpp
**
**		This mode tests shadowing through projected lights
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "demTestShadowMap.hpp"

#include "Core/app/appModeMgr.hpp"


#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dProjectedLightWrapper.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
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
demTestShadowMap::demTestShadowMap(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pObject(NULL)
{
}

//====================================================================
//====================================================================
demTestShadowMap::~demTestShadowMap()
{
}

//====================================================================
//====================================================================
void demTestShadowMap::Initialize()
{
	Camera().SetClip(1.0f, 16.0f);
	demTestMode::Initialize();

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Shadow map test"));
	m_Viewer.SetTextMessage(1, itString("Projected light cast onto shadow test from Maya"));
	m_Viewer.SetTextMessage(2, itString("Flags for casting/receiving shadows vary per mesh"));
	m_Viewer.SetTextMessage(3, itString("'t' - toggles transparent version, 'f' - sets light to camera view"));
	m_Viewer.SetTextMessage(4, itString("Alt+mouse moves the camera"));
	m_Viewer.SetTextMessage(5, itString("Press space to go to the next section"));

	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	fsLocator locator2 = locator;
	locator.Push("shadowTest.mx");
	locator2.Push("shadowTest-transp.mx");

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
		// Load Model 
		m_pObject = api3dImport::LoadObject(locator);

		std::string loc_fname;
		fsFileUtil::LocatorToANSIFilename(locator, loc_fname);
		DBG_ASSERT1(m_pObject, "Error loading file %s", loc_fname.c_str());

		m_pObject2 = api3dImport::LoadObject(locator2);
		fsFileUtil::LocatorToANSIFilename(locator2, loc_fname);
		DBG_ASSERT1(m_pObject2, "Error loading file %s", loc_fname.c_str());
		m_pObject2->SetRenderable(false);

		// add to scene
		api3dScene::AddObject(m_pObject);
		api3dScene::AddObject(m_pObject2);

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

	g3dSingleLightRendering::SetDoSingleLightRendering(true);
}

//====================================================================
//	Think
//====================================================================
void demTestShadowMap::Think()
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
void demTestShadowMap::DeInitialize()
{	
	g3dSingleLightRendering::SetDoSingleLightRendering(false);

	api3dScene::RemoveObject(m_pObject);
	delete m_pObject;
	api3dScene::RemoveObject(m_pObject2);
	delete m_pObject2;

	api3dLightMgr::DestroyLight( m_pLight );
	delete m_pPrjLightWrapper;

	m_Viewer.ClearTextMessages();

	demTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestShadowMap::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		// swap opaque and transparent versions
	case itString::CharType('T'):
	case itString::CharType('t'):
		m_pObject->SetRenderable(!m_pObject->GetRenderable());
		m_pObject2->SetRenderable(!m_pObject2->GetRenderable());
		break;
		
	case itString::CharType('F'):
	case itString::CharType('f'):
		m_pLight->SetPosition(cam3dMgr::GetCamera().GetPosition());
		m_pPrjLightWrapper->OrientCamera();
		break;
	}

	demTestMode::ReceiveCharEvent(i_Event);
}

