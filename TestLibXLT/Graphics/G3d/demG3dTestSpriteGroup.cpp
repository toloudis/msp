/*****************************************************************************
**  demG3dTestSpriteGroup.cpp
**
**		This mode displays a demonstration/test of the g3dSpriteGroupModel.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestSpriteGroup.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dFontUtil.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightManager.hpp"
#include "g3dLayer.hpp"
#include "matPlainTexture.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dSpriteGroupModel.hpp"
#include "matTextureManager.hpp"
#include "matUVATexture.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

#include <algorithm>

//====================================================================
//====================================================================
demG3dTestSpriteGroup::demG3dTestSpriteGroup(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestSpriteGroup::~demG3dTestSpriteGroup()
{
}

//====================================================================
//====================================================================
void demG3dTestSpriteGroup::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(30.0f, 30.0f, 10, 10);

	//	setup material
	m_Mat1.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_RectMat.SetDiffuse(maFloatRGBA(0.1f, 0.1f, 0.1f, 1.0f));
	m_RectMat.SetSpecular(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_RectMat.SetHasSpecular(true);
	m_RectMat.SetSpecularPower(100.0f);
	m_RectFragment->SetMaterial(&m_RectMat);

	// make a nice texture
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("test.png"));
	m_PlainTexture = matTextureManager::LoadTexture(locator);

	{
		const int num_frames = 32;
		const int num_height_frames = 2;
		const int num_width_frames = 2;
		const float frame_rate = 24.0f;
		const int num_pages = 8;

		m_UVATexture = matTextureManager::CreateUVATexture();

		char tex_name[16];
		strcpy(tex_name, "0jxp201.png");

		int i;
		for( i = 0 ; i < num_pages ; i++ )
		{
			tex_name[6] = '1' + i;
			locator.Pop();
			locator.Push(tex_name);
			m_UVABlockTextures.push_back(matTextureManager::LoadTexture(locator));
			m_UVATexture->AddTexturePage(m_UVABlockTextures[i]);
		}

		m_UVATexture->SetNumFrames(num_frames);
		m_UVATexture->SetNumHeightFrames(num_height_frames);
		m_UVATexture->SetNumWidthFrames(num_width_frames);
		m_UVATexture->SetFrameRate(frame_rate);
	}

	m_Mat1.AddTextureTop(m_PlainTexture);
	m_Mat2.AddTextureTop(m_UVATexture);
//	m_RectMat.AddTextureTop(m_PlainTexture);

	//	make models
	maMatrix4x4 world_mat;
	world_mat.MakeRotateX( -maConstants::c_fPI_Div_2 );

	m_WorldRect = new g3dSceneNode(m_RectFragment, world_mat, demG3dLayerSpec::e_World);
	m_WorldRect->SetRenderable(true);

	//	sprite group 1 - plain texture
	m_Model1 = g3dRenderer::AddSpriteGroupModel(demG3dLayerSpec::e_World);
	m_Model1->SetRenderable(true);

	const int num_sprites = 10;
	const float radius = 5.0f;
	m_Sprites1.resize(num_sprites);
	g3dSpriteData* last = NULL;
	int i;
	for( i = 0 ; i < num_sprites ; i++ )
	{
		g3dSpriteData& cur_sprite = m_Sprites1[i];
		float angle = float(i) / float(num_sprites) * maConstants::c_fPI_Times_2;
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		cur_sprite.SetPosition(maPoint3d(x, y, 0));
		cur_sprite.SetAlpha(1.0f - (float(i) / float(num_sprites) * 0.5f));
		cur_sprite.SetPrev(last);
		cur_sprite.SetScale(1.0f);

		if( i != num_sprites-1 )
			cur_sprite.SetNext(&(m_Sprites1[i+1]));

		last = &cur_sprite;
	}

	m_Model1->SetHead(&(m_Sprites1[0]));
	m_Model1->SetMaterial(&m_Mat1);
	m_Model1->SetPosition(maPoint3d(0, 0, 0));

	//	sprite group 2 - uva texture
	m_Model2 = g3dRenderer::AddSpriteGroupModel(demG3dLayerSpec::e_World);
	m_Model2->SetRenderable(true);

	m_Model2->SetRenderType(g3dSpriteGroupModel::e_Additive);

	m_Sprites2.resize(num_sprites);
	last = NULL;
	for( i = 0 ; i < num_sprites ; i++ )
	{
		g3dSpriteData& cur_sprite = m_Sprites2[i];
		float angle = float(i) / float(num_sprites) * maConstants::c_fPI_Times_2;
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		cur_sprite.SetPosition(maPoint3d(x, y+10, 10));
		cur_sprite.SetAlpha(1.0f - (float(i) / float(num_sprites) * 0.5f));
		cur_sprite.SetPrev(last);
		cur_sprite.SetScale(1.0f);
		cur_sprite.SetUVAFrame(i);

		if( i != num_sprites-1 )
			cur_sprite.SetNext(&(m_Sprites2[i+1]));

		last = &cur_sprite;
	}

	m_Model2->SetHead(&(m_Sprites2[0]));
	m_Model2->SetMaterial(&m_Mat2);
	m_Model1->SetPosition(maPoint3d(0, 10, 10));

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
}

//====================================================================
//	Think
//====================================================================
void demG3dTestSpriteGroup::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	draw some text stuff
/*	const int num_text_lines = 6;
	itString text[num_text_lines];
	text[0] = itString("This slide shows two sprite group models.");
	text[1] = itString("The one with the test image shows a regular texture (notice the correct alignment");
	text[2] = itString("and the one with the fire shows a UVA texture displaying various different frames.");
	text[3] = itString("The one with the fire frames is using additive rendering mode.");
	text[4] = itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)");
	text[5] = itString("Press space to go to the next section");
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
*/
	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestSpriteGroup::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_WorldRect);
	//g3dRenderer::RemoveModel(m_Model1);
	//g3dRenderer::RemoveModel(m_Model2);

	delete m_RectFragment;

	std::for_each(m_UVABlockTextures.begin(), m_UVABlockTextures.end(), matTextureManager::ReleaseTexture);

	//	release textures
	matTextureManager::ReleaseTexture(m_PlainTexture);
	matTextureManager::ReleaseTexture(m_UVATexture);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
