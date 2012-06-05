/*****************************************************************************
**  demTestEventsMode.cpp
**
**		This mode test events attached to animation frames
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demTestEventsMode.hpp"

#include "demModeManager.hpp"

#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "entAnimation.hpp"
#include "entEntity.hpp"
#include "entEntityTemplate.hpp"
#include "entImport.hpp"
#include "entSceneWorld.hpp"
#include "envSTLHelpers.hpp"
#include "evtParticleEvent.hpp"
#include "evtSoundEvent.hpp"
#include "fsREsourceFinderDir.hpp"
#include "g2dRGBColor.hpp"
#include "g2dWindow.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dViewer.hpp"
#include "gfPaths.hpp"
#include "gfPlatformFileDialog.hpp"
#include "maConstants.hpp"
#include "matTextureMgr.hpp"
#include "prtCircleEmitter.hpp"
#include "prtGeneratorUtil.hpp"
#include "prtParticleGeneratorTemplate.hpp"
#include "prtSpiralParticleGenerator.hpp"
#include "snSoundJob2D.hpp"
#include "snSoundManager.hpp"

#include <algorithm>

namespace
{

enum Anims
{
	e_First,
	e_Second,
	e_NumAnims
};

}


//====================================================================
//====================================================================
demTestEventsMode::demTestEventsMode(g3dViewer &i_Viewer)
: demViewerMode(i_Viewer), m_Viewer(i_Viewer),
	m_Entity(NULL),
	m_EntityTemplate(NULL)
{

}

//====================================================================
//====================================================================
demTestEventsMode::~demTestEventsMode()
{
}

//====================================================================
//====================================================================
void demTestEventsMode::Initialize()
{
	demViewerMode::Initialize();

	m_Scene = new entSceneWorld();
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_Viewer.SetTextMessage(0, itString("Anim Event Tests"));
	m_Viewer.SetTextMessage(1, itString("1 - Idle"));
	m_Viewer.SetTextMessage(2, itString("2 - Roar"));

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
	entAnimation *anim1 = entImport::CreateAnimation(*key_set1);
	m_EntityTemplate->AddAnimation(anim1, e_First);
	entAnimation *anim2 = entImport::CreateAnimation(*key_set2);
	m_EntityTemplate->AddAnimation(anim2, e_Second);

	// Set up sound event 1
	snSoundJob2D *sound = new snSoundJob2D;
	locator.Pop();
	locator.Push(itString("shorten.wav"));
	sound->SetFilename(locator);
	sound->SetTypeMask( snSoundManager::SOUNDTYPE_EFFECT );

	// Preload sound
	sound->Load();
	snSoundManager::AddPreloadSound(sound);

	evtSoundEvent* sound_event = new evtSoundEvent(sound);
	anim1->AddAnimEvent(sound_event, 40.0f);

	// Set up sound event 2 - looping noise
	sound = new snSoundJob2D;
	locator.Pop();
	locator.Push(itString("jv1zp.wav"));
	sound->SetFilename(locator);
	sound->SetTypeMask( snSoundManager::SOUNDTYPE_EFFECT );

	// Preload sound
	sound->Load();
	snSoundManager::AddPreloadSound(sound);

	sound_event = new evtSoundEvent(sound);
	sound_event->SetLooping(true);
	anim1->AddAnimEvent(sound_event, 0.0f);

	// Set up sound event 3 - roar
	sound = new snSoundJob2D;
	locator.Pop();
	locator.Push(itString("drdr200.wav"));
	sound->SetFilename(locator);
	sound->SetTypeMask( snSoundManager::SOUNDTYPE_EFFECT );

	// Preload sound
	sound->Load();
	snSoundManager::AddPreloadSound(sound);

	sound_event = new evtSoundEvent(sound);
	sound_event->SetNodeName("joint_drneck1");
	sound_event->SetDistance(100.0f);
	sound_event->SetFalloff(500.0f);
	anim2->AddAnimEvent(sound_event, 60.0f);

	// Set up particle event - fire breath
	fsLocator sub_locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	sub_locator.Push("00lasr00.png");
	
	prtParticleGeneratorTemplate* gen_template = new prtParticleGeneratorTemplate;
	locator.Pop();
	locator.Push(itString("dragonbreath2.tpr"));
	prtGeneratorUtil::Read(locator, *gen_template);
	//locator.Pop();
	//locator.Push(gen_template->GetTextureLocator());
	//gen_template->SetTextureLocator(locator);
	gen_template->SetTextureLocator(sub_locator);
	gen_template->MakeTexture();
	evtParticleEvent *par_event = new evtParticleEvent(gen_template); // ownership passes to event
	par_event->SetNodeName("joint_drhead");
	par_event->SetLifetime(3.5f);
	par_event->SetPositionOffset(maVector3d(0.5f,1,0));
	par_event->SetDirectionOffset(maVector3d(2,0,0));
	anim2->AddAnimEvent(par_event, 64.0f);

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
void demTestEventsMode::Think()
{
	demViewerMode::Think();

	float frame_time = appTime::GetTime();

	// Sound mgr think
	snSoundManager::Think(frame_time);
	snSoundManager::SetSoundJobsListenerPosition(this->GetCamera().GetPosition());

	//	draw some text stuff
	const int num_text_lines = 3;
	itString text[num_text_lines];

	text[0] = itString("Anim Event Tests");
	text[1] = itString("1 - Idle");
	text[2] = itString("2 - Roar");


	g2dRGBColor text_color(0x50, 0x50, 0x90);

	int i;
	for( i = 0 ; i < num_text_lines ; i++ )
	{
		m_Viewer.GetWindow()->DrawText(	10,
										(i * 17) + 10,
										this->GetFont(),
										text[i],
										text_color);

		text_color.SetRed(text_color.GetRed() + 10);
	}

	// animate
	m_Scene->Think(frame_time);

	//	render
	m_Viewer.Render(frame_time);


	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestEventsMode::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	delete m_Entity;
	m_Entity = NULL;
	delete m_EntityTemplate;
	m_EntityTemplate = NULL;


	demViewerMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demTestEventsMode::ReceiveCharEvent(appCharEvent& i_Event)
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

		default:
			demViewerMode::ReceiveCharEvent(i_Event);
			break;
	}
}

//====================================================================
//====================================================================
void demTestEventsMode::reset_camera()
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
