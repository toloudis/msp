/*****************************************************************************
**  demTestOrthographic.cpp
**
**		This mode tests orthographic camera settings
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "demTestOrthographic.hpp"

#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
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
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Core/gf/gfPaths.hpp"
#include "InputDI/in/inDeviceMgr.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"


namespace
{

}

//====================================================================
//====================================================================
demTestOrthographic::demTestOrthographic(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pObject(NULL)
{
}

//====================================================================
//====================================================================
demTestOrthographic::~demTestOrthographic()
{
}

//====================================================================
//====================================================================
void demTestOrthographic::Initialize()
{
	// Create orthographic camera, use ortho manipulator
	m_OrthoCamera.LookAt(maPoint3d(0,0,-50), maPoint3d(0,0,0), maVector3d(0,1,0));
	m_OrthoCamera.SetOrthographic(true);
	m_OrthoCamera.SetOrthoWidth(10.0f);
	cam3dMgr::UseScriptedCamera(&m_OrthoCamera, cam3dMgr::e_OrthoPan);
	m_Viewer.SetCamera(&m_OrthoCamera);

	demTestMode::Initialize();

	fsLocator model_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	model_loc.Push("data");
	model_loc.Push("camera_test.mx");

	try
	{
		// Load Model 
		m_pObject = api3dImport::LoadObject(model_loc);
		m_pObject->SetWireframe(true);

		std::string loc_fname;
		fsFileUtil::LocatorToANSIFilename(model_loc, loc_fname);
		DBG_ASSERT1(m_pObject, "Error loading file %s", loc_fname.c_str());

		api3dScene::AddObject(m_pObject);
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

	m_pDirLight = api3dLightMgr::CreateDirectionalLight();
	m_pDirLight->SetDirection(maVector3d(0.0f, -1.0f, -1.0f));
	m_pDirLight->Enable();



}

//====================================================================
//	Think
//====================================================================
void demTestOrthographic::Think()
{
	demTestMode::Think();

	float frame_time = appTime::GetTime();

	inDeviceMgr::Think();
	tma3dCursorMgr::Think();


	api3dScene::Think( frame_time );
	cam3dMgr::Think();

	//	render
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( appMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestOrthographic::DeInitialize()
{	
	// Return to regular editor camera
	cam3dMgr::EndScriptedCamera();
	m_Viewer.SetCamera(&cam3dMgr::GetCamera());

	api3dScene::RemoveObject(m_pObject);
	delete m_pObject;

	api3dLightMgr::DestroyLight( m_pDirLight );

	demTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestOrthographic::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
	case itString::CharType('f'):
	case itString::CharType('F'):
		cam3dMgr::FocusCamera(m_pObject->GetWorldBox());
		break;
	case itString::CharType('p'):
	case itString::CharType('P'):
		cam3dMgr::GetCamera().SetOrthographic(false);
		cam3dMgr::UseScriptedCamera(&m_OrthoCamera, cam3dMgr::e_MayaOrbit);
		break;
	case itString::CharType('o'):
	case itString::CharType('O'):
		cam3dMgr::GetCamera().SetOrthographic(true);
		cam3dMgr::UseScriptedCamera(&m_OrthoCamera, cam3dMgr::e_OrthoPan);
		break;
	case itString::CharType('x'):
	case itString::CharType('X'):
		cam3dMgr::GetCamera().SetOrthographic(true);
		m_OrthoCamera.LookAt(maPoint3d(-50,0,0), maPoint3d(0,0,0), maVector3d(0,1,0));
		cam3dMgr::UseScriptedCamera(&m_OrthoCamera, cam3dMgr::e_OrthoPan);
		break;
	case itString::CharType('y'):
	case itString::CharType('Y'):
		cam3dMgr::GetCamera().SetOrthographic(true);
		m_OrthoCamera.LookAt(maPoint3d(0,-50,0), maPoint3d(0,0,0), maVector3d(0,0,1));
		cam3dMgr::UseScriptedCamera(&m_OrthoCamera, cam3dMgr::e_OrthoPan);
		break;
	case itString::CharType('z'):
	case itString::CharType('Z'):
		cam3dMgr::GetCamera().SetOrthographic(true);
		m_OrthoCamera.LookAt(maPoint3d(0,0,-50), maPoint3d(0,0,0), maVector3d(0,1,0));
		cam3dMgr::UseScriptedCamera(&m_OrthoCamera, cam3dMgr::e_OrthoPan);
		break;
	}

	demTestMode::ReceiveCharEvent(i_Event);
}

