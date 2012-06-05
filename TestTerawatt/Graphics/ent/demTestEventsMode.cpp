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
#include "entParticleEvent.hpp"
#include "entSoundEvent.hpp"
#include "envSTLHelpers.hpp"
#include "g2dRGBColor.hpp"
#include "g2dScreen.hpp"
#include "g2dScreenDrawUtil.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightManager.hpp"
#include "g3dRenderer.hpp"
#include "g3dPackage.hpp"
#include "gfPaths.hpp"
#include "gfPlatformFileDialog.hpp"
#include "maConstants.hpp"
#include "scGeneratorUtil.hpp"
#include "scMovableObject.hpp"
#include "scParticleGeneratorTemplate.hpp"
#include "scScene.hpp"
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
demTestEventsMode::demTestEventsMode()
:	m_Entity(NULL),
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

	this->SetPitch(30.0f * maConstants::c_fAngleToRad);
	this->SetYaw(270.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	// Init for file selection dialog
	m_DataPath = gfPaths::GetGamePath(gfPaths::e_ExePath);
	m_DataPath.Push(itString("Data"));

	// Load dragon model
	//
	fsLocator locator = m_DataPath;
	locator.Push(itString("dr1dr.jnx"));

	m_EntityTemplate = new entEntityTemplate(m_DataPath, e_NumAnims);
	entImport::LoadGeometry(locator, *m_EntityTemplate);

	m_Entity = new entEntity(*m_EntityTemplate, g3dPackage::e_World);

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

	entSoundEvent* sound_event = new entSoundEvent(sound);
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

	sound_event = new entSoundEvent(sound);
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

	sound_event = new entSoundEvent(sound);
	sound_event->SetNodeName("joint_drneck1");
	sound_event->SetDistance(100.0f);
	sound_event->SetFalloff(500.0f);
	anim2->AddAnimEvent(sound_event, 60.0f);

	// Set up particle event - fire breath
	scParticleGeneratorTemplate* gen_template = new scParticleGeneratorTemplate;
	locator.Pop();
	locator.Push(itString("dragonbreath2.tpr"));
	scGeneratorUtil::Read(locator, *gen_template);
	locator.Pop();
	locator.Push(gen_template->GetTextureLocator());
	gen_template->SetTextureLocator(locator);
	gen_template->MakeTexture();
	entParticleEvent *par_event = new entParticleEvent(gen_template); // ownership passes to event
	par_event->SetNodeName("joint_drhead");
	par_event->SetLifetime(3.5f);
	par_event->SetPositionOffset(maVector3d(0.5f,1,0));
	par_event->SetDirectionOffset(maVector3d(2,0,0));
	anim2->AddAnimEvent(par_event, 64.0f);

	// Set active animation
	m_Entity->SetAnimation(e_First, appTime::GetTime());

	reset_camera();

	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.65f, 0.65f, 0.6f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.6f, 0.6f, 0.65f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demTestEventsMode::Think()
{
	demViewerMode::Think();

	float frame_time = appTime::GetTime();

	// Give Think to Entities in order to activate events
	m_Entity->Think(frame_time);

	// Sound mgr think
	snSoundManager::Think(frame_time);
	snSoundManager::SetSoundJobsListenerPosition(this->GetCamera().GetPosition());

	g2dScreenDrawUtil::Clear(g2dRGBColor(0x30, 0x80, 0xa0));

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
		g2dScreenDrawUtil::DrawText(	10,
										(i * 17) + 10,
										this->GetFont(),
										text[i],
										text_color);

		text_color.SetRed(text_color.GetRed() + 10);
	}

	//	view frustrum cull
	maMatrix4x4 camera;
	maMatrix4x4 projection;
	this->GetCamera().GetCameraMatrix(camera);
	this->GetCamera().GetProjectionMatrix(projection);
	scScene::Cull(camera * projection);

	scScene::PreRender(frame_time);

	//	render
	g3dRenderer::RenderModels(frame_time);

	//	flip
	g2dScreen::EndScene();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demTestEventsMode::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

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
