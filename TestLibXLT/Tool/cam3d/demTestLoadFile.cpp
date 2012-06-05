/*****************************************************************************
**  demTestLoadFile.cpp
**
**		This mode tests loading files through api3dImport
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "demTestLoadFile.hpp"

#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Tool/cam3d/cam3dAnimKeys.hpp"
#include "Tool/cam3d/cam3dImport.hpp"
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
demTestLoadFile::demTestLoadFile(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pObject(NULL),
	m_pCameraAnimKeys(NULL)
{
}

//====================================================================
//====================================================================
demTestLoadFile::~demTestLoadFile()
{
}

//====================================================================
//====================================================================
void demTestLoadFile::Initialize()
{
	demTestMode::Initialize();

	fsLocator model_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	model_loc.Push("data");
	model_loc.Push("camera_test.mx");

	fsLocator scrip_loc = gfPaths::GetPath(gfPaths::e_ExePath);
	scrip_loc.Push("data");
	scrip_loc.Push("camera_test.cam");


	try
	{
		// Load Model 
		m_pObject = api3dImport::LoadObject(model_loc);

		std::string loc_fname;
		fsFileUtil::LocatorToANSIFilename(model_loc, loc_fname);
		DBG_ASSERT1(m_pObject, "Error loading file %s", loc_fname.c_str());

		api3dScene::AddObject(m_pObject);

		// Load camera script file
		m_pCameraAnimKeys = cam3dImport::LoadAnimation(scrip_loc);
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
void demTestLoadFile::Think()
{
	demTestMode::Think();

	float frame_time = appTime::GetTime();

	inDeviceMgr::Think();
	tma3dCursorMgr::Think();
	
	// Execute camera script
	if (m_pCameraAnimKeys)
	{
		const float c_FPS = 30.0f;
		float frame = frame_time * c_FPS;
		maPoint3d pos = m_pCameraAnimKeys->GetPosition(frame);
		maVector3d view = m_pCameraAnimKeys->GetViewVector(frame);
		cam3dMgr::SetManipPositionAndTarget( pos, pos + view );

		if (m_pCameraAnimKeys->HasFieldOfViewAnimation())
		{
			float fov = m_pCameraAnimKeys->GetFieldOfView(frame);
			cam3dMgr::SetCameraFOV(cam3dMgr::GetCamera(), fov);
		}
	}


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
void demTestLoadFile::DeInitialize()
{	
	delete m_pCameraAnimKeys;

	api3dScene::RemoveObject(m_pObject);
	delete m_pObject;

	api3dLightMgr::DestroyLight( m_pDirLight );

	demTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestLoadFile::ReceiveCharEvent(appCharEvent& i_Event)
{
	//switch( i_Event.GetChar() )
	//{
	//case itString::CharType('f'):
	//case itString::CharType('F'):
	//	break;
	//}

	demTestMode::ReceiveCharEvent(i_Event);
}

