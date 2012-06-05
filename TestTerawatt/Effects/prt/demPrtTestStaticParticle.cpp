/*****************************************************************************
**  demPrtTestStaticParticle.cpp
**
**		This mode displays a demonstration/test of the scStaticObject
**	and scSimpleMovableObject.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demPrtTestStaticParticle.hpp"

#include "prtCircleEmitter.hpp"
#include "prtBlockEmitter.hpp"
#include "prtGeneratorUtil.hpp"
#include "prtStaticParticleGenerator.hpp"

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsFileUtil.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dFragment.hpp"
#include "g3dLightMgr.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dViewer.hpp"
#include "matTextureMgr.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"
#include "scSceneWorld.hpp"

#include <algorithm>

//====================================================================
//====================================================================
demPrtTestStaticParticle::demPrtTestStaticParticle(g3dViewer &i_Viewer)
: m_Viewer(i_Viewer)
{
}

//====================================================================
//====================================================================
demPrtTestStaticParticle::~demPrtTestStaticParticle()
{
}

//====================================================================
//====================================================================
void demPrtTestStaticParticle::Initialize()
{
	demPrtTestMode::Initialize();

	m_Scene = new scSceneWorld();
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_Viewer.SetTextMessage(0,itString("This slide shows a some prtStaticParticleGenerators."));
	m_Viewer.SetTextMessage(1,itString("The bright one uses a single texture and a prtCircleEmitter."));
	m_Viewer.SetTextMessage(2,itString("The bright one uses additive rendering mode, which is good for particles that appear emissive."));
	m_Viewer.SetTextMessage(3,itString("The darker one has a generator lifetime of 10 seconds.  It uses a cube emitter and multiplicative"));
	m_Viewer.SetTextMessage(4,itString("rendering mode.  It has a animated alpha channel which goes from 0 to maximum to 0 alpha over the "));
	m_Viewer.SetTextMessage(5,itString("lifetime of a particle.  (The other generator goes from maximum to 0)."));
	m_Viewer.SetTextMessage(6,itString("To restart the square generator, press 'g'"));
	m_Viewer.SetTextMessage(7,itString("('i', 'j', 'k', 'm', 'a', 'd', 'w', 'x', 'e', 'c', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(8,itString("Press space to go to the next section"));


	this->SetPitch(50.0f * maConstants::c_fAngleToRad);
	this->SetYaw(280.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(60.0f, 60.0f, 2, 4);

	//	setup fragment materials
	m_RectMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	//	add some textures
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("lblue0132.png");
	m_RectTexture = matTextureMgr::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture);

	//	set the materials for the fragments
	m_RectFragment->SetMaterial(&m_RectMat);

	//	make objects
	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);

	m_Rect = new scObject(m_RectFragment);
	m_Scene->AddObject(m_Rect);

	this->make_generator1();
	this->make_generator2();

	//	set lights
	m_Light1 = g3dLightMgr::CreateDirectionalLight();
	m_Light2 = g3dLightMgr::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demPrtTestStaticParticle::Think()
{
	demPrtTestMode::Think();

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
void demPrtTestStaticParticle::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup objects
	m_Scene->RemoveObject(m_Rect);
	delete m_Rect;
	m_Scene->RemoveObject(m_Generator1);
	delete m_Generator1;
	m_Scene->RemoveObject(m_Generator2);
	delete m_Generator2;

	//	cleanup fragments
	delete m_RectFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_RectTexture);

	m_Viewer.ClearTextMessages();
	delete m_Scene;

	demPrtTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demPrtTestStaticParticle::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('g'):
		{
			if( m_Generator2 )
			{
				m_Scene->RemoveObject(m_Generator2);
				delete m_Generator2;
				m_Generator2 = NULL;
			}

			this->make_generator2();
		}

		default:
			this->demPrtTestMode::ReceiveCharEvent(i_Event);
		break;
	}
}

//====================================================================
//====================================================================
void demPrtTestStaticParticle::GeneratorDestroyed(prtParticleGenerator* i_Generator)
{
	if( i_Generator == m_Generator1 )
		m_Generator1 = NULL;
	else if( i_Generator == m_Generator2 )
		m_Generator2 = NULL;
}

void demPrtTestStaticParticle::make_generator1()
{
	fsLocator particle_texture_locator = gfPaths::GetPath(gfPaths::e_ExePath);
	particle_texture_locator.Push("00lasr00.png");

	//	to test the reading and writing of the templates, we'll make
	//	a generator, make a template from it, write the template, read it back
	//	in, and finally remake the particle generator from it.
	m_Generator1 = new prtStaticParticleGenerator(appTime::GetTime());
	m_Generator1->SetPosition(maPoint3d(15, 10, 0));
	m_Generator1->SetGeneratorLifetime(1000000);
	m_Generator1->SetEmitter(new prtCircleEmitter(10.0f));
	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);
	m_Generator1->SetOrientation(world_rotation);
	m_Generator1->SetRenderMode(prtSpriteGroupParticleGenerator::e_Additive);
	m_Generator1->SetParameter(prtParticleGenerator::e_ParticleRate, 12.0f);
	m_Generator1->SetParameter(prtParticleGenerator::e_MinParticleLifetime, 3.0f);
	m_Generator1->SetParameter(prtParticleGenerator::e_MaxParticleLifetime, 3.0f);
	m_Generator1->SetParameter(prtParticleGenerator::e_MaxParticles, 10000);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_ScaleCoeff, 2.0f);
	m_Generator1->SetScaleMode(prtSpriteGroupParticleGenerator::e_Linear);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity, 0.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, 0.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, 0.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, 0.0f);

	an2StateAnimation<float> anim(1.0f, 0.0f, 1.0f);
	m_Generator1->SetAlphaProfile(anim);

	fsLocator template_locator = gfPaths::GetPath(gfPaths::e_ExePath);
	template_locator.Push("particletest1.tpr");

	{
		if( fsFileUtil::FileExists(template_locator) )
			fsFileUtil::DeleteFile(template_locator);

		fsFileUtil::CreateFile(template_locator);

		prtParticleGeneratorTemplate template1;
		prtGeneratorUtil::MakeTemplateFromGenerator(template1, *m_Generator1);
		template1.SetTextureLocator(particle_texture_locator);
		prtGeneratorUtil::Write(template_locator, template1);
	}

	delete m_Generator1;
	m_Generator1 = NULL;

	{
		prtGeneratorUtil::Read(template_locator, m_Template1);
		m_Template1.SetTextureLocator(particle_texture_locator);
		m_Template1.MakeTexture();

		m_Generator1 =  dynamic_cast<prtStaticParticleGenerator*>(
								prtGeneratorUtil::MakeGenerator(	m_Template1,	
								appTime::GetTime() ) );

		m_Generator1->SetGeneratorLifetime(10000.0f);
		m_Generator1->SetPosition(maPoint3d(15, 10, 0));
		m_Generator1->SetOrientation(world_rotation);
		//scScene::RegisterGeneratorEventHandler(this, m_Generator1);
		m_Scene->AddObject(m_Generator1);
	}
}

void demPrtTestStaticParticle::make_generator2()
{
	fsLocator particle_texture_locator = gfPaths::GetPath(gfPaths::e_ExePath);
	particle_texture_locator.Push("smoke.tuv");

	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);
	m_Generator2 = new prtStaticParticleGenerator(appTime::GetTime());
	m_Generator2->SetPosition(maPoint3d(-15, 15, 0));
	m_Generator2->SetGeneratorLifetime(10000);
	m_Generator2->SetEmitter(new prtBlockEmitter(15.0f));
	m_Generator2->SetOrientation(world_rotation);
	m_Generator2->SetRenderMode(prtSpriteGroupParticleGenerator::e_Multiplicative);
	m_Generator2->SetParameter(prtParticleGenerator::e_ParticleRate, 850.0f);
	m_Generator2->SetParameter(prtParticleGenerator::e_MinParticleLifetime, 0.5f);
	m_Generator2->SetParameter(prtParticleGenerator::e_MaxParticleLifetime, 0.8f);
	m_Generator2->SetParameter(prtParticleGenerator::e_MaxParticles, 10000);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_ScaleCoeff, 2.5f);
	m_Generator2->SetScaleMode(prtSpriteGroupParticleGenerator::e_Exponential);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_InitialScale, 2.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MinStartAngle, -maConstants::c_fPI);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MaxStartAngle, maConstants::c_fPI);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity, -1.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, 1.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, -1.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, 1.0f);

	anKeyAnimation<float> anim2(0.0f);
	anim2.AddKey(0.5f, 1.0f);
	anim2.AddKey(1.0f, 0.0f);
	m_Generator2->SetAlphaProfile(anim2);

	fsLocator template_locator = gfPaths::GetPath(gfPaths::e_ExePath);
	template_locator.Push("particletest2.tpr");

	{
		if( fsFileUtil::FileExists(template_locator) )
			fsFileUtil::DeleteFile(template_locator);

		fsFileUtil::CreateFile(template_locator);

		prtParticleGeneratorTemplate template1;
		prtGeneratorUtil::MakeTemplateFromGenerator(template1, *m_Generator2);
		template1.SetTextureLocator(particle_texture_locator);
		prtGeneratorUtil::Write(template_locator, template1);
	}

	delete m_Generator2;
	m_Generator2 = NULL;

	{
		prtGeneratorUtil::Read(template_locator, m_Template2);
		m_Template2.SetTextureLocator(particle_texture_locator);
		m_Template2.MakeTexture();

		m_Generator2 = dynamic_cast<prtStaticParticleGenerator*>(
							prtGeneratorUtil::MakeGenerator( m_Template2,	
														appTime::GetTime() ));

		m_Generator2->SetPosition(maPoint3d(-15, 15, 0));
		m_Generator2->SetGeneratorLifetime(10000);
		m_Generator2->SetOrientation(world_rotation);
		//scScene::RegisterGeneratorEventHandler(this, m_Generator2);
		m_Scene->AddObject(m_Generator2);
	}
}
