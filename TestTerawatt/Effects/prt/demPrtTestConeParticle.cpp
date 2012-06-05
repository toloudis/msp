/*****************************************************************************
**  demPrtTestConeParticle.cpp
**
**		This mode displays a demonstration/test of the scStaticObject
**	and scSimpleMovableObject.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demPrtTestConeParticle.hpp"

#include "prtCircleEmitter.hpp"
#include "prtConeParticleGenerator.hpp"
#include "prtBlockEmitter.hpp"
#include "prtPointEmitter.hpp"

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsFileUtil.hpp"
#include "fsLocator.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dFragment.hpp"
#include "g3dLightMgr.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dViewer.hpp"
#include "matTextureMgr.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"
#include "matUVATexture.hpp"
#include "scSceneWorld.hpp"

#include <algorithm>

namespace
{

	anKeyAnimation<float> l_Anim1(0.0f);
	anKeyAnimation<float> l_Anim2(0.0f);

}

//====================================================================
//====================================================================
demPrtTestConeParticle::demPrtTestConeParticle(g3dViewer &i_Viewer)
: m_Viewer(i_Viewer)
{
}

//====================================================================
//====================================================================
demPrtTestConeParticle::~demPrtTestConeParticle()
{
}

//====================================================================
//====================================================================
void demPrtTestConeParticle::Initialize()
{
	demPrtTestMode::Initialize();

	m_Scene = new scSceneWorld();
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_Viewer.SetTextMessage(0,itString("This slide shows a some prtConeParticleGenerators."));
	m_Viewer.SetTextMessage(1,itString("The moving one uses a small cone angle and a moving prtCircleEmitter."));
	m_Viewer.SetTextMessage(2,itString("The stationary one uses a larger cone angle.  Both use the same UVA."));
	m_Viewer.SetTextMessage(3,itString("The moving one uses additive rendering mode, the stationary one multiplicative."));
	m_Viewer.SetTextMessage(4,itString("Each uses an anKeyAnimation to determine its alpha channel."));
	m_Viewer.SetTextMessage(5,itString("('i', 'j', 'k', 'm', 'a', 'd', 'w', 'x', 'e', 'c', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(6,itString("Press space to go to the next section"));

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

	locator.Pop();
	locator.Push("00lasr00.png");
	m_ParticleTexture = matTextureMgr::LoadTexture(locator);

	m_UVATexture = matTextureMgr::CreateUVATexture();

	const int num_pages = 8;
	char tex_name[16];
	strcpy(tex_name, "0jxp101.png");

	int i;
	for( i = 0 ; i < num_pages ; i++ )
	{
		tex_name[6] = '1' + i;
		locator.Pop();
		locator.Push(tex_name);
		m_UVATextures.push_back(matTextureMgr::LoadTexture(locator));
		m_UVATexture->AddTexturePage(m_UVATextures[i]);
	}

	m_UVATexture->SetNumFrames(32);
	m_UVATexture->SetNumHeightFrames(2);
	m_UVATexture->SetNumWidthFrames(2);
	m_UVATexture->SetFrameRate(16.0f);

	//	set the materials for the fragments
	m_RectFragment->SetMaterial(&m_RectMat);

	//	make objects
	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);

	m_Rect = new scObject(m_RectFragment);
	m_Scene->AddObject(m_Rect);

	//	*************
	//	generator 1
	m_Generator1 = new prtConeParticleGenerator(appTime::GetTime());
	m_Generator1->SetTexture(m_UVATexture);
	m_Generator1->SetGeneratorLifetime(1000000);
	m_Generator1->SetEmitter(new prtCircleEmitter(2.0f));
	m_Generator1->SetOrientation(world_rotation);
	m_Generator1->SetRenderMode(prtSpriteGroupParticleGenerator::e_Additive);
	m_Generator1->SetParameter(prtParticleGenerator::e_ParticleRate, 300.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_InitialScale, 2.0f);
	m_Generator1->SetParameter(prtParticleGenerator::e_MinParticleLifetime, 0.8f);
	m_Generator1->SetParameter(prtParticleGenerator::e_MaxParticleLifetime, 0.8f);
	m_Generator1->SetParameter(prtParticleGenerator::e_MaxParticles, 10000);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_ScaleCoeff, 2.0f);
	m_Generator1->SetScaleMode(prtSpriteGroupParticleGenerator::e_Linear);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity, 0.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, 0.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, 0.0f);
	m_Generator1->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, 0.0f);
	m_Generator1->SetParameter(prtConeParticleGenerator::e_MinSpeed, 13.0f);
	m_Generator1->SetParameter(prtConeParticleGenerator::e_MaxSpeed, 18.0f);
	m_Generator1->SetParameter(prtConeParticleGenerator::e_ConeAngle, 0.05f);
	m_Generator1->SetPosition(maPoint3d(10, 10, 10));

	l_Anim1.AddKey(0.5f, 1.0f);
	l_Anim1.AddKey(1.0f, 0.0f);
	m_Generator1->SetAlphaProfile(l_Anim1);
	m_Scene->AddObject(m_Generator1);

	//	*************
	//	generator 2
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
void demPrtTestConeParticle::Think()
{
	demPrtTestMode::Think();

	float frame_time = appTime::GetTime();

	m_Generator1->SetOrientation(maRotation(maVector3d(0, 1, 0), frame_time * 6));
	m_Generator1->SetPosition(maPoint3d(	10 + 5 * cos(frame_time*6),
								10 + 5 * sin(frame_time*6),
								10));



	// animate
	m_Scene->Think(frame_time);

	//	render
	m_Viewer.Render(frame_time);


	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demPrtTestConeParticle::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup objects
	m_Scene->DestroyObject(m_Rect);
	m_Scene->DestroyObject(m_Generator1);
	m_Scene->DestroyObject(m_Generator2);

	//	cleanup fragments
	delete m_RectFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_RectTexture);
	matTextureMgr::ReleaseTexture(m_ParticleTexture);
	matTextureMgr::ReleaseTexture(m_UVATexture);

	std::for_each(m_UVATextures.begin(), m_UVATextures.end(), matTextureMgr::ReleaseTexture);

	m_Viewer.ClearTextMessages();
	delete m_Scene;

	demPrtTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demPrtTestConeParticle::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('g'):
		{
			m_Scene->DestroyObject(m_Generator2);
			m_Generator2 = NULL;
			this->make_generator2();
		}

		default:
			this->demPrtTestMode::ReceiveCharEvent(i_Event);
		break;
	}
}

void demPrtTestConeParticle::make_generator2()
{
	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);
	m_Generator2 = new prtConeParticleGenerator(appTime::GetTime());
	m_Generator2->SetTexture(m_UVATexture);
	m_Generator2->SetPosition(maPoint3d(-15, 0, 0));
	m_Generator2->SetGeneratorLifetime(100000);
	m_Generator2->SetEmitter(new prtCircleEmitter(2.0f));
	m_Generator2->SetOrientation(world_rotation);
	m_Generator2->SetRenderMode(prtSpriteGroupParticleGenerator::e_Multiplicative);
	m_Generator2->SetParameter(prtParticleGenerator::e_ParticleRate, 100.0f);
	m_Generator2->SetParameter(prtParticleGenerator::e_MinParticleLifetime, 1.2f);
	m_Generator2->SetParameter(prtParticleGenerator::e_MaxParticleLifetime, 1.8f);
	m_Generator2->SetParameter(prtParticleGenerator::e_MaxParticles, 10000);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_InitialScale, 2.5f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_ScaleCoeff, 2.5f);
	m_Generator2->SetScaleMode(prtSpriteGroupParticleGenerator::e_Exponential);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_InitialScale, 2.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MinStartAngle, -maConstants::c_fPI);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MaxStartAngle, maConstants::c_fPI);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity, -1.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity, 1.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration, -2.0f);
	m_Generator2->SetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration, 2.0f);
	m_Generator2->SetParameter(prtConeParticleGenerator::e_MinSpeed, 13.0f);
	m_Generator2->SetParameter(prtConeParticleGenerator::e_MaxSpeed, 18.0f);
	m_Generator2->SetParameter(prtConeParticleGenerator::e_ConeAngle, 0.5f);

	l_Anim2.AddKey(0.5f, 1.0f);
	l_Anim2.AddKey(1.0f, 0.0f);
	m_Generator2->SetAlphaProfile(l_Anim2);
	m_Scene->AddObject(m_Generator2);
}
