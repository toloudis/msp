/*****************************************************************************
**  demG3dTestAnims.hpp
**
**		This mode displays a demonstration/test of the different texturing
**	options available in the Terawatt 3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestAnims.hpp"

#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/an/anKeyAnimation.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//====================================================================
//====================================================================
demG3dTestAnims::demG3dTestAnims(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestAnims::~demG3dTestAnims()
{
}

//====================================================================
//====================================================================
void demG3dTestAnims::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("This slide shows some material animation features of the Terawatt engine."));
	m_Viewer.SetTextMessage(1, itString("The texture on the tetrahedron is rotating.  The diffuse color of the sphere is being animated, from cyan"));
	m_Viewer.SetTextMessage(2, itString("to green to blue and back again.  The water plane has two textures, one with a translucent alpha.  Each is"));
	m_Viewer.SetTextMessage(3, itString("rotated and scaled in different directions, and the texture translation is animating."));
	m_Viewer.SetTextMessage(4, itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)"));
	m_Viewer.SetTextMessage(5, itString("Press space to go to the next section"));

	this->SetPitch(50.0f * maConstants::c_fAngleToRad);
	this->SetYaw(280.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	DBG_LOG0("demG3dTestAnims::Initialize");

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(140.0f, 140.0f, 1, 1);
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(7.0f, 25, 25);
	m_TetraFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(7.0f, 2, 4);

	//	setup fragment materials
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	effPhongData* pData = NULL;
	pData = dynamic_cast<effPhongData*>(m_RectMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_UV.m_UScale = 3;
	pData->m_UV.m_VScale = 3;
	pData->m_NameDiffuse = "water006.png";
	pData->ReloadTextures(locator);
	m_RectTexture1 = pData->m_TextureDiffuse;


	pData = dynamic_cast<effPhongData*>(m_SphereMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_NameDiffuse = "lgrey020.png";
	pData->ReloadTextures(locator);
	m_SphereTexture = pData->m_TextureDiffuse;


	pData = dynamic_cast<effPhongData*>(m_TetraMat.GetEffectData());
	pData->m_ColorDiffuse = (maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	pData->m_NameDiffuse = "lblue0132.png";
	pData->ReloadTextures(locator);
	m_TetraTexture = pData->m_TextureDiffuse;

	//	add some textures
	// e_TexLayerAlphaBlend and SetRotation no longer supported.
//	locator.Push(itString("water005.png"));
//	m_RectTexture2 = matTextureMgr::LoadTexture(locator);
//	m_RectMat.AddTextureTop(m_RectTexture2, matMaterial::e_TexLayerAlphaBlend);
//	m_RectMat.SetScale(maVector2d(4.0, 4.0), 1);
//	m_RectMat.SetRotation(2.0f, 0);
//	m_RectMat.SetRotation(1.0f, 1);

	//	set the materials for the fragments
	m_RectFragment->SetMaterial(&m_RectMat);
	m_SphereFragment->SetMaterial(&m_SphereMat);
	m_TetraFragment->SetMaterial(&m_TetraMat);

	//	make models
	maMatrix4x4 world_mat;
	world_mat.MakeRotateX( -maConstants::c_fPI_Div_2 );

	m_Rect = new g3dSceneNode(m_RectFragment, world_mat);
	m_Root->AddChild(m_Rect);
	m_Rect->SetRenderable(true);

	world_mat.MakeTranslate(10, 6, 0);

	m_Sphere = new g3dSceneNode(m_SphereFragment, world_mat);
	m_Root->AddChild(m_Sphere);
	m_Sphere->SetRenderable(true);

	world_mat.MakeTranslate(-10, 6, 0);

	m_Tetra = new g3dSceneNode(m_TetraFragment, world_mat);
	m_Root->AddChild(m_Tetra);
	m_Tetra->SetRenderable(true);

	//	add some material animations
	this->setup_anims();

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

	DBG_LOG0("finished demG3dTestAnims::Initialize");
}

//====================================================================
//	Think
//====================================================================
void demG3dTestAnims::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestAnims::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_Light1);
	g3dLightMgr::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Rect);
	//g3dRenderer::RemoveModel(m_Sphere);
	//g3dRenderer::RemoveModel(m_Tetra);

	//	cleanup fragments
	delete m_RectFragment;
	delete m_SphereFragment;
	delete m_TetraFragment;

	//	release textures
	matTextureMgr::ReleaseTexture(m_RectTexture1);
//	matTextureMgr::ReleaseTexture(m_RectTexture2);
	matTextureMgr::ReleaseTexture(m_SphereTexture);
	matTextureMgr::ReleaseTexture(m_TetraTexture);


	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}

void demG3dTestAnims::setup_anims()
{
	float start_time = appTime::GetTime();

	anKeyAnimation<maFloatRGBA>* color_anim = new anKeyAnimation<maFloatRGBA>(maFloatRGBA(0, 1, 1, 1));
	color_anim->AddKey(2.0f, maFloatRGBA(0, 1, 0, 1));
	color_anim->AddKey(4.0f, maFloatRGBA(0, 0, 1, 1));
	color_anim->SetLooping(true);
	color_anim->SetReversing(true);
	m_ColorAnim = new matMatAnim(	color_anim,
									matMatParamIndex::e_Diffuse,
									start_time);

	m_SphereMat.AddMatAnim( m_ColorAnim );

	an2StateAnimation<float>* rotate_anim = new an2StateAnimation<float>(0, maConstants::c_fPI_Times_2, 20.0f);
	rotate_anim->SetLooping(true);
	m_RotateAnim = new matMatAnim(	rotate_anim,
									matMatParamIndex::e_TextureRotation0,
									start_time);

//	m_TetraMat.AddMatAnim( m_RotateAnim );


	an2StateAnimation<maVector2d>* texture_pos_anim1 = new an2StateAnimation<maVector2d>(maVector2d(0, 0), maVector2d(4, 0), 180.0f);
	texture_pos_anim1->SetLooping(true);
	m_TexturePosAnim1 = new matMatAnim(	texture_pos_anim1,
										matMatParamIndex::e_TextureTranslation1,
										start_time);
//	m_RectMat.AddMatAnim( m_TexturePosAnim1 );


	an2StateAnimation<maVector2d>* texture_pos_anim2 = new an2StateAnimation<maVector2d>(maVector2d(0, 0), maVector2d(3, 0), 112.0f);
	texture_pos_anim2->SetLooping(true);
	m_TexturePosAnim2 = new matMatAnim(	texture_pos_anim2,
										matMatParamIndex::e_TextureTranslation0,
										start_time);
//	m_RectMat.AddMatAnim( m_TexturePosAnim2 );
}
