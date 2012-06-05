/*****************************************************************************
**  demTestAnimsMode.cpp
**
**		This mode tests properties of entity animations
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demTestAnimsMode.hpp"

#include "demModeManager.hpp"

#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "entAnimation.hpp"
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


enum Anims
{
	e_First,
	e_Second,
	e_FirstSlow,
	e_SecondShort,
	e_NumAnims
};

}


//====================================================================
//====================================================================
demTestAnimsMode::demTestAnimsMode(g3dViewer &i_Viewer)
: demViewerMode(i_Viewer), m_Viewer(i_Viewer),
	m_Entity(NULL),
	m_EntityTemplate(NULL)
{

}

//====================================================================
//====================================================================
demTestAnimsMode::~demTestAnimsMode()
{
}

//====================================================================
//====================================================================
void demTestAnimsMode::Initialize()
{
	demViewerMode::Initialize();

	m_Scene = new entSceneWorld();
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_Viewer.SetTextMessage(0, itString("Anims Test"));
	m_Viewer.SetTextMessage(1, itString("1 - Idle"));
	m_Viewer.SetTextMessage(2, itString("2 - Roar"));
	m_Viewer.SetTextMessage(3, itString("3 - Idle slow"));
	m_Viewer.SetTextMessage(4, itString("4 - Roar sub anim clip"));

	this->SetPitch(30.0f * maConstants::c_fAngleToRad);
	this->SetYaw(270.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	// Init for file selection dialog
	m_DataPath = gfPaths::GetGamePath(gfPaths::e_ExePath);
	m_DataPath.Push(itString("Data"));

	// all textures are in data directory
	fsResourceFinderDir finder(m_DataPath);

	// Load dragon model
	//
	fsLocator locator = m_DataPath;
	locator.Push(itString("dr1dr.jnx"));

	entModelTemplate *mdl_template = entImport::LoadGeometry(locator, finder);
	m_EntityTemplate = dynamic_cast<entEntityTemplate*>(mdl_template);

	m_Entity = new entEntity(*m_EntityTemplate);
	m_Scene->AddEntity(m_Entity);

	// Load dragon anim keys
	//
	locator.Pop();
	locator.Push(itString("dr1dr100.jna"));
	entAnimKeys *key_set1 = entImport::LoadAnimKeys(locator);
	m_EntityTemplate->AddAnimKeys(key_set1, locator);

	locator.Pop();
	locator.Push(itString("dr1dr200.jna"));
	entAnimKeys *key_set2 = entImport::LoadAnimKeys(locator);
	m_EntityTemplate->AddAnimKeys(key_set2, locator);

	// Setup dragon animations
	//
	entAnimation *anim = NULL;
	m_EntityTemplate->AddAnimation(entImport::CreateAnimation(*key_set1), e_First);
	m_EntityTemplate->AddAnimation(entImport::CreateAnimation(*key_set2), e_Second);

	anim = entImport::CreateAnimation(*key_set1);
	anim->SetFrameRate(4.0f);	// 24fps is default
	m_EntityTemplate->AddAnimation(anim, e_FirstSlow);

	anim = entImport::CreateAnimation(*key_set2);
	anim->SetStartFrame(50.0f);
	anim->SetEndFrame(100.0f);
	m_EntityTemplate->AddAnimation(anim, e_SecondShort);


	// Set active animation
	m_Entity->SetAnimation(e_First, appTime::GetTime());

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
void demTestAnimsMode::Think()
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
void demTestAnimsMode::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	delete m_Entity;
	m_Entity = NULL;
	delete m_EntityTemplate;
	m_EntityTemplate = NULL;

	m_Viewer.ClearTextMessages();
	delete m_Scene;

	demViewerMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestAnimsMode::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('r'):
		case itString::CharType('R'):
			this->reset_camera();
		break;

		case itString::CharType('1'):
			m_Entity->SetAnimation(e_First, appTime::GetTime());
		break;
		case itString::CharType('2'):
			m_Entity->SetAnimation(e_Second, appTime::GetTime());
		break;
		case itString::CharType('3'):
			m_Entity->SetAnimation(e_FirstSlow, appTime::GetTime());
		break;
		case itString::CharType('4'):
			m_Entity->SetAnimation(e_SecondShort, appTime::GetTime());
		break;

		default:
			demViewerMode::ReceiveCharEvent(i_Event);
			break;
	}
}

//====================================================================
//====================================================================
void demTestAnimsMode::reset_camera()
{
	if (!m_Entity)
	{
		this->SetTarget( maPoint3d(0,0,0) );
		this->SetRadius( 20 );
		return;
	}

	maAxisBox bbox;
	m_Entity->ComputeWorldBox(bbox);
	this->SetTarget( bbox.GetCenter() );
	float diameter = (bbox.GetBoxPoint(0) - bbox.GetBoxPoint(7)).Length();
	this->SetRadius( diameter * 2.0f );
}
