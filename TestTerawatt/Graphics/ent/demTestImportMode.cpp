/*****************************************************************************
**  demTestImportMode.cpp
**
**		This mode tests the entImport namespace
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demTestImportMode.hpp"

#include "demModeManager.hpp"

#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "entEntity.hpp"
#include "entEntityTemplate.hpp"
#include "entImport.hpp"
#include "envSTLHelpers.hpp"
#include "fsResourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g2dWindow.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dViewer.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"
#include "entSceneWorld.hpp"

#include <algorithm>

namespace
{

}


//====================================================================
//====================================================================
demTestImportMode::demTestImportMode(g3dViewer &i_Viewer)
: demViewerMode(i_Viewer), m_Viewer(i_Viewer)
{

}

//====================================================================
//====================================================================
demTestImportMode::~demTestImportMode()
{
}

//====================================================================
//====================================================================
void demTestImportMode::Initialize()
{
	demViewerMode::Initialize();

	m_Scene = new entSceneWorld();
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_Viewer.SetTextMessage(0, itString("Import Test"));

	this->SetPitch(30.0f * maConstants::c_fAngleToRad);
	this->SetYaw(270.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	// Init for file selection dialog
	m_DataPath = gfPaths::GetGamePath(gfPaths::e_ExePath);
	m_DataPath.Push(itString("Data"));

	// all textures are in data directory
	fsResourceFinderDir finder(m_DataPath);

	// Load single-skin model
	//
	fsLocator locator = m_DataPath;
	locator.Push(itString("dr1dr.jnx"));

	entModelTemplate* ent_template = entImport::LoadGeometry(locator, finder);
	DBG_ASSERT0(ent_template, "Can't load template for dr1dr.jnx");
	m_Templates.push_back(ent_template);

	entEntity* entity = new entEntity(*ent_template);
	entity->SetPosition(maPoint3d(40,0,0));
	m_Entities.push_back(entity);
	m_Scene->AddEntity(entity);
	

	// Load hierarchical model
	locator.Pop();
	locator.Push(itString("dr1ht.mhx"));

	ent_template = entImport::LoadGeometry(locator, finder);
	DBG_ASSERT0(ent_template, "Can't load template for dr1ht.mhx");
	m_Templates.push_back(ent_template);

	entity = new entEntity(*ent_template);
	entity->SetPosition(maPoint3d(-40,0,0));
	m_Entities.push_back(entity);
	m_Scene->AddEntity(entity);

	// Load single fragment .mx
	locator.Pop();
	locator.Push(itString("carlead.mx"));

	ent_template = entImport::LoadGeometry(locator, finder);
	DBG_ASSERT0(ent_template, "Can't load template for carlead.mhx");
	m_Templates.push_back(ent_template);

	entity = new entEntity(*ent_template);
	entity->SetPosition(maPoint3d(0,0,-40));
	m_Entities.push_back(entity);
	m_Scene->AddEntity(entity);

	// Load multiple fragment .mx
	locator.Pop();
	locator.Push(itString("loaddock.mx"));

	ent_template = entImport::LoadGeometry(locator, finder);
	DBG_ASSERT0(ent_template, "Can't load template for loaddock.mhx");
	m_Templates.push_back(ent_template);

	entity = new entEntity(*ent_template);
	entity->SetPosition(maPoint3d(0,0,40));
	m_Entities.push_back(entity);
	m_Scene->AddEntity(entity);

	reset_camera();

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.65f, 0.65f, 0.6f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.6f, 0.6f, 0.65f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demTestImportMode::Think()
{
	demViewerMode::Think();

	float frame_time = appTime::GetTime();

	// animate
	m_Scene->Think(frame_time);

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestImportMode::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	envSTLHelpers::DeleteContainer(m_Entities);
	envSTLHelpers::DeleteContainer(m_Templates);

	m_Viewer.ClearTextMessages();
	delete m_Scene;

	demViewerMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestImportMode::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('r'):
		case itString::CharType('R'):
			this->reset_camera();
		break;

		default:
			demViewerMode::ReceiveCharEvent(i_Event);
			break;
	}
}

//====================================================================
//====================================================================
void demTestImportMode::reset_camera()
{
	maAxisBox bbox, total_bbox;
	for (int i=0; i<m_Entities.size(); i++)
	{
		m_Entities[i]->ComputeWorldBox(bbox);
		total_bbox.Union(bbox);
	}

	this->SetTarget( total_bbox.GetCenter() );
	float radius = total_bbox.GetRadius();
	this->SetRadius( radius * 2.0f );
}
