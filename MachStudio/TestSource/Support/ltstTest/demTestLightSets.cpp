/*****************************************************************************
**  demTestLightSets.cpp
**
**		This mode tests the light sets code from MachStudio
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "demTestLightSets.hpp"
#include "demModeManager.hpp"

#include "ltstLightSetMgr.hpp"

#include "api3dImport.hpp"
#include "api3dLightMgr.hpp"
#include "api3dObjectSingle.hpp"
#include "api3dScene.hpp"
#include "appCharEvent.hpp"
#include "appSimTime.hpp"
#include "appTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "effPhongData.hpp"
#include "envSTLHelpers.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g3dFragment.hpp"
#include "g3dFragmentCreate.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dPointLight.hpp"
#include "gfPaths.hpp"
#include "inDeviceMgr.hpp"
#include "matMaterial.hpp"
#include "matShaderMgr.hpp"
#include "matTextureMgr.hpp"
#include "matTexture.hpp"
#include "maConstants.hpp"
#include "mdlExceptionX.hpp"
#include "nameObject.hpp"
#include "tma3dCursorMgr.hpp"


namespace
{

}

//====================================================================
//====================================================================
demTestLightSets::demTestLightSets(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer),
	m_pObject(NULL),
	m_pObjectName(NULL),
	m_pLightName(NULL),
	m_LightSetName("MyLightSet")
{
}

//====================================================================
//====================================================================
demTestLightSets::~demTestLightSets()
{
}

//====================================================================
//====================================================================
void demTestLightSets::Initialize()
{
	ltstLightSetMgr::Initialize();

	Camera().SetClip(1.0f, 16.0f);
	demTestMode::Initialize();
	
	// set up display info
	m_Viewer.SetTextMessage(0, itString("LightSet test"));
	m_Viewer.SetTextMessage(1, itString("R - removes object from light set, A q- adds it back in"));

	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("Pony.chx");


	try
	{
		// Load Model 
		m_pObject = dynamic_cast<api3dObjectSingle*>(api3dImport::LoadObject(locator));
		m_pObject->SetOrientation(maRotation(0, -75.0f * maConstants::c_fAngleToRad, 0));

		std::string loc_fname;
		fsFileUtil::LocatorToANSIFilename(locator, loc_fname);
		DBG_ASSERT1(m_pObject, "Error loading file %s", loc_fname.c_str());

		api3dScene::AddObject(m_pObject);

		// Create name and register with the light set manager
		m_pObjectName = new nameObject();
		m_pObjectName->SetName(nameString("Character"));
		ltstLightSetMgr::AddObject(m_pObjectName, m_pObject);
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("File not found: %s", filename.c_str());
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
	}
	catch( const mdlInvalidModelFileX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("Invalid Model File: %s", filename.c_str());
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
	}
	catch( ... )
	{
		DBG_WARNING0("Unknown error");
		throw;
	}


	//api3dLightMgr::SetAmbient(maFloatRGBA(0.2f, 0.2f, 0.2f, 1.0f));

	m_pDirLight = api3dLightMgr::CreateDirectionalLight();
	m_pDirLight->SetDirection(maVector3d(0.3f, -1.0f, 1.0f));
	m_pDirLight->SetIntensity(maFloatRGBA(0.6f, 0.9f, 0.6f, 0.0f));
	m_pDirLight->Enable();


	// Create name and register with the light set manager
	m_pLightName = new nameObject();
	m_pLightName->SetName(nameString("DirLight1"));
	ltstLightSetMgr::AddLight(m_pLightName, m_pDirLight);

	// Create a light set and add light to it
	ltstLightSetMgr::CreateLightSet(m_LightSetName);
	ltstLightSetMgr::AddLightToSet(m_LightSetName, m_pLightName->GetName());

	// Start with the object in the light set
	ltstLightSetMgr::AddObjectToLightSet(m_LightSetName, m_pObjectName->GetName());

}

//====================================================================
//	Think
//====================================================================
void demTestLightSets::Think()
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
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestLightSets::DeInitialize()
{	
	ltstLightSetMgr::DeleteLightSet(m_LightSetName);

	if (m_pObjectName)
	{
		ltstLightSetMgr::RemoveObject(m_pObjectName, m_pObject);
		delete m_pObjectName;
	}

	ltstLightSetMgr::RemoveLight(m_pLightName, m_pDirLight);
	delete m_pLightName;

	api3dScene::RemoveObject(m_pObject);
	delete m_pObject;

	api3dLightMgr::DestroyLight( m_pDirLight );

	demTestMode::DeInitialize();
	
	ltstLightSetMgr::DeInitialize();

}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestLightSets::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
	case itString::CharType('a'):
	case itString::CharType('A'):
		ltstLightSetMgr::AddObjectToLightSet(m_LightSetName, m_pObjectName->GetName());
		break;
	case itString::CharType('r'):
	case itString::CharType('R'):
		ltstLightSetMgr::RemoveObjectFromLightSet(m_LightSetName, m_pObjectName->GetName());
		break;
	}

	demTestMode::ReceiveCharEvent(i_Event);
}

