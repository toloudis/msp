/*****************************************************************************
**  demPrtTestCone3DParticle.cpp
**
**		This mode displays a demonstration/test of the 3D particles.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demPrtTestCone3DParticle.hpp"


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
#include "g3dDirectionalLight.hpp"
#include "g3dFragment.hpp"
#include "g3dLightManager.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dViewer.hpp"
#include "matTextureManager.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"
#include "scSceneWorld.hpp"

#include <algorithm>

namespace
{

	anKeyAnimation<float> l_Anim1(0.0f);
	anKeyAnimation<float> l_Anim2(0.0f);

	g3dFragment* l_RectFragment;
	scStaticObject* l_Rect;

	class demPrtTestRayIntersectionCheck : public scRayIntersectionCheck
	{
		public:
			virtual bool RayIntersect( const maPoint3d& i_Begin, const maPoint3d& i_End,
									   float& o_T, maVector3d* o_pNormal );
	};


	bool demPrtTestRayIntersectionCheck::RayIntersect( const maPoint3d& i_Begin,
													  const maPoint3d& i_End,
													  float& o_T,
													  maVector3d* o_pNormal )
	{
		maPoint3d v0, v1, v2;
		int tex_num = g3dFragmentManager::GetNumTextureVertices( l_RectFragment );
		unsigned char* buffer = g3dFragmentManager::Lock( l_RectFragment );
		const unsigned short* indices = g3dFragmentManager::GetIndices( l_RectFragment );
		int num = g3dFragmentManager::GetNumIndices( l_RectFragment );
		int i;
		for( i = 0 ; i < num ; i += 3 )
		{
			int idx  = indices[ i ];
			int idx1 = indices[ i + 1 ];
			int idx2 = indices[ i + 2 ];

			switch ( tex_num )
			{
				case 1:
				{
					g3dType::Tex1Vertex* vertex = reinterpret_cast<g3dType::Tex1Vertex*>(buffer);
					v0.Set(	(vertex + idx)->m_Vertex.GetX(),
				  			(vertex + idx)->m_Vertex.GetY(),
				  			(vertex + idx)->m_Vertex.GetZ() );
					v1.Set(	(vertex + idx1)->m_Vertex.GetX(),
				  			(vertex + idx1)->m_Vertex.GetY(),
				  			(vertex + idx1)->m_Vertex.GetZ() );
					v2.Set(	(vertex + idx2)->m_Vertex.GetX(),
				  			(vertex + idx2)->m_Vertex.GetY(),
				  			(vertex + idx2)->m_Vertex.GetZ() );
				}
				break;
				case 2:
				{
					g3dType::Tex2Vertex* vertex = reinterpret_cast<g3dType::Tex2Vertex*>(buffer);
					v0.Set(	(vertex + idx)->m_Vertex.GetX(),
				  			(vertex + idx)->m_Vertex.GetY(),
				  			(vertex + idx)->m_Vertex.GetZ() );
					v1.Set(	(vertex + idx1)->m_Vertex.GetX(),
				  			(vertex + idx1)->m_Vertex.GetY(),
				  			(vertex + idx1)->m_Vertex.GetZ() );
					v2.Set(	(vertex + idx2)->m_Vertex.GetX(),
				  			(vertex + idx2)->m_Vertex.GetY(),
				  			(vertex + idx2)->m_Vertex.GetZ() );
				}
				break;
				case 3:
				{
					g3dType::Tex3Vertex* vertex = reinterpret_cast<g3dType::Tex3Vertex*>(buffer);
					v0.Set(	(vertex + idx)->m_Vertex.GetX(),
				  			(vertex + idx)->m_Vertex.GetY(),
				  			(vertex + idx)->m_Vertex.GetZ() );
					v1.Set(	(vertex + idx1)->m_Vertex.GetX(),
				  			(vertex + idx1)->m_Vertex.GetY(),
				  			(vertex + idx1)->m_Vertex.GetZ() );
					v2.Set(	(vertex + idx2)->m_Vertex.GetX(),
				  			(vertex + idx2)->m_Vertex.GetY(),
				  			(vertex + idx2)->m_Vertex.GetZ() );
				}
				break;
			}

			maMatrix4x4 matrix = l_Rect->GetOrientation().GetMatrix();
			v0 = matrix * v0;
			v1 = matrix * v1;
			v2 = matrix * v2;

			if ( geoRayIntersection::IntersectLineTriangle( i_Begin,
															i_End - i_Begin,
															v0,
															v1,
															v2,
															o_T,
															o_pNormal ) )
			{
				return true;
			}
		}
		return false;
	}
}


//====================================================================
//====================================================================
demPrtTestCone3DParticle::demPrtTestCone3DParticle(g3dViewer &i_Viewer)
: m_Viewer(i_Viewer)
{
}

//====================================================================
//====================================================================
demPrtTestCone3DParticle::~demPrtTestCone3DParticle()
{
}

//====================================================================
//====================================================================
void demPrtTestCone3DParticle::Initialize()
{
	demPrtTestMode::Initialize();

	this->SetPitch(50.0f * maConstants::c_fAngleToRad);
	this->SetYaw(280.0f * maConstants::c_fAngleToRad);
	this->SetRadius(100.0f);

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(120.0f, 120.0f, 2, 4);
	l_RectFragment = m_RectFragment;

	//	setup fragment materials
	m_RectMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	//	add some textures
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("lblue0132.png");
	m_RectTexture = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture);

	locator.Pop();
	locator.Push("00lasr00.png");
	m_ParticleTexture = matTextureManager::LoadTexture(locator);

	m_UVATexture = matTextureManager::CreateUVATexture();

	const int num_pages = 8;
	char tex_name[16];
	strcpy(tex_name, "0jxp101.png");

	int i;
	for( i = 0 ; i < num_pages ; i++ )
	{
		tex_name[6] = '1' + i;
		locator.Pop();
		locator.Push(tex_name);
		m_UVATextures.push_back(matTextureManager::LoadTexture(locator));
		m_UVATexture->AddTexturePage(m_UVATextures[i]);
	}

	m_UVATexture->SetNumFrames(32);
	m_UVATexture->SetNumHeightFrames(2);
	m_UVATexture->SetNumWidthFrames(2);
	m_UVATexture->SetFrameRate(16.0f);

	//	set the materials for the fragments
	g3dFragmentManager::SetMaterial(m_RectFragment, &m_RectMat, 0);

	// create a sphere model
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1.0f, 25, 25);
	m_pSphereMat = new matMaterial;
	m_pSphereMat->SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	locator.Pop();
	locator.Push(itString("lgrey020.png"));
	m_pSphereTexture = matTextureManager::LoadTexture(locator);
	m_pSphereMat->AddTextureTop(m_pSphereTexture);
	g3dFragmentManager::SetMaterial(m_SphereFragment, m_pSphereMat, 0);

	//	make objects
	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);

	m_Rect = new scStaticObject(maPoint3d(0, 0, 0), world_rotation, maPoint3d(1, 1, 1), m_RectFragment, g3dPackage::e_World);
	l_Rect = m_Rect;
	scScene::AddObject(m_Rect);

	//	*************
	//	generator 1
	m_Generator1 = new prtConeParticleGenerator(appTime::GetTime(), g3dPackage::e_World);
	m_Generator1->SetTexture(m_UVATexture);
	m_Generator1->SetGeneratorLifetime(1000000);
	m_Generator1->SetEmitter(new scCircleEmitter(2.0f));
	m_Generator1->SetOrientation(world_rotation);
	m_Generator1->SetRenderMode(scSpriteGroupParticleGenerator::e_Additive);
	m_Generator1->SetParameter(scParticleGenerator::e_ParticleRate, 300.0f);
	m_Generator1->SetParameter(scSpriteGroupParticleGenerator::e_InitialScale, 2.0f);
	m_Generator1->SetParameter(scParticleGenerator::e_MinParticleLifetime, 0.8f);
	m_Generator1->SetParameter(scParticleGenerator::e_MaxParticleLifetime, 0.8f);
	m_Generator1->SetParameter(scParticleGenerator::e_MaxParticles, 10000);
	m_Generator1->SetParameter(scSpriteGroupParticleGenerator::e_ScaleCoeff, 2.0f);
	m_Generator1->SetScaleMode(scSpriteGroupParticleGenerator::e_Linear);
	m_Generator1->SetParameter(scSpriteGroupParticleGenerator::e_MinAngularVelocity, 0.0f);
	m_Generator1->SetParameter(scSpriteGroupParticleGenerator::e_MaxAngularVelocity, 0.0f);
	m_Generator1->SetParameter(scSpriteGroupParticleGenerator::e_MinAngularAcceleration, 0.0f);
	m_Generator1->SetParameter(scSpriteGroupParticleGenerator::e_MaxAngularAcceleration, 0.0f);
	m_Generator1->SetParameter(prtConeParticleGenerator::e_MinSpeed, 13.0f);
	m_Generator1->SetParameter(prtConeParticleGenerator::e_MaxSpeed, 18.0f);
	m_Generator1->SetParameter(prtConeParticleGenerator::e_ConeAngle, 0.05f);
	m_Generator1->SetPosition(maPoint3d(10, 10, 10));

	l_Anim1.AddKey(0.5f, 1.0f);
	l_Anim1.AddKey(1.0f, 0.0f);
	m_Generator1->SetAlphaProfile(l_Anim1);
	scScene::AddParticleGenerator(m_Generator1);

	l_Anim2.AddKey(0.5f, 1.0f);
	l_Anim2.AddKey(1.0f, 0.0f);

	//	*************
	//	generator 2
	this->make_generator2();

	//	set lights
	m_Light1 = g3dLightManager::CreateDirectionalLight();
	m_Light2 = g3dLightManager::CreateDirectionalLight();

	m_Light1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_Light2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_Light1->SetIntensity(maFloatRGBA(0.4f, 0.7f, 0.2f, 1.0f));
	m_Light2->SetIntensity(maFloatRGBA(0.3f, 0.6f, 0.8f, 1.0f));

	m_Light1->Enable();
	m_Light2->Enable();

	g3dLightManager::SetAmbient(maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f));

	m_pRayIntersectionCheck = new demPrtTestRayIntersectionCheck;
	prtCone3DParticleGenerator::SetRayIntersectionCheck( m_pRayIntersectionCheck );
}

//====================================================================
//	Think
//====================================================================
void demPrtTestCone3DParticle::Think()
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
void demPrtTestCone3DParticle::DeInitialize()
{
	delete m_pRayIntersectionCheck;

	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup objects
	scScene::RemoveObject(m_Rect);
	delete m_Rect;
	m_Scene->DestroyObject(m_Generator1);
	m_Scene->DestroyObject(m_Generator2);

	//	cleanup fragments
	delete m_RectFragment;

	//	release textures
	matTextureManager::ReleaseTexture(m_RectTexture);
	matTextureManager::ReleaseTexture(m_ParticleTexture);
	matTextureManager::ReleaseTexture(m_UVATexture);

	std::for_each(m_UVATextures.begin(), m_UVATextures.end(), matTextureManager::ReleaseTexture);

	delete m_Scene;

	demPrtTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demPrtTestCone3DParticle::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('g'):
		{
			m_Scene->DestroyObject(m_Generator2);
			m_Generator2 = NULL;

			// create a sphere model
			m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(1.0f, 25, 25);
			m_pSphereMat = new matMaterial;
			m_pSphereMat->SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
			fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
			locator.Push(itString("lgrey020.png"));
			m_pSphereTexture = matTextureManager::LoadTexture(locator);
			m_pSphereMat->AddTextureTop(m_pSphereTexture);
			g3dFragmentManager::SetMaterial(m_SphereFragment, m_pSphereMat, 0);

			this->make_generator2();
		}

		default:
			this->demPrtTestMode::ReceiveCharEvent(i_Event);
		break;
	}
}

void demPrtTestCone3DParticle::make_generator2()
{
	maRotation world_rotation(maVector3d(1, 0, 0), -maConstants::c_fPI_Div_2);
	m_Generator2 = new prtCone3DParticleGenerator(appTime::GetTime(), g3dPackage::e_World);
	m_Generator2->SetPosition(maPoint3d(-15, 0, 0));
	m_Generator2->SetGeneratorLifetime(100000);
	m_Generator2->SetEmitter(new scCircleEmitter(2.0f));
	m_Generator2->SetOrientation(world_rotation);
	m_Generator2->SetParameter(scParticleGenerator::e_ParticleRate, 2.0f);
	m_Generator2->SetParameter(scParticleGenerator::e_MinParticleLifetime, 16.0f);
	m_Generator2->SetParameter(scParticleGenerator::e_MaxParticleLifetime, 18.0f);
	m_Generator2->SetParameter(scParticleGenerator::e_MaxParticles, 20);
	m_Generator2->SetParameter(sc3DParticleGenerator::e_Scale, 2.5f);
	m_Generator2->SetParameter(prtCone3DParticleGenerator::e_MinSpeed, 18.0f);
	m_Generator2->SetParameter(prtCone3DParticleGenerator::e_MaxSpeed, 20.0f);
	m_Generator2->SetParameter(prtCone3DParticleGenerator::e_ConeAngle, 0.5f);
	m_Generator2->SetParameter(prtCone3DParticleGenerator::e_AccelerationX, 0.0f);
	m_Generator2->SetParameter(prtCone3DParticleGenerator::e_AccelerationY, -6.0f);
	m_Generator2->SetParameter(prtCone3DParticleGenerator::e_AccelerationZ, 0.0f);

	m_Generator2->SetModelType( sc3DParticleGenerator::e_Simple );

	g3dHFragment& model = m_Generator2->GetModel();
	model.SetFragment( NULL );

	std::vector<matMaterial*>& materials = m_Generator2->GetMaterials();
	materials.push_back( m_pSphereMat );

	std::vector<g3dFragment*>& frags = m_Generator2->GetFragments();
	frags.push_back( m_SphereFragment );

	std::vector<matTexture*>& textures = m_Generator2->GetTextures();
	textures.push_back( m_pSphereTexture );

	scScene::AddParticleGenerator( m_Generator2 );
}
