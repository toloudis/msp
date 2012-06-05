/*****************************************************************************
**  demG3dTestTextures.hpp
**
**		This mode displays a demonstration/test of the different texturing
**	options available in the Terawatt 3D engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestTextures.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "fsFileUtil.hpp"
#include "g2dFontUtil.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dFragmentCreate.hpp"
#include "g3dLayer.hpp"
#include "g3dLightManager.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "gfPaths.hpp"
#include "maConstants.hpp"
#include "matTextureManager.hpp"

//#include "matPlainTexture.hpp"
#include "matUVATexture.hpp"

#include <algorithm>

//====================================================================
//====================================================================
demG3dTestTextures::demG3dTestTextures(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestTextures::~demG3dTestTextures()
{
}

//====================================================================
//====================================================================
void demG3dTestTextures::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layers
	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_TextureFont = g2dFontUtil::LoadFont(itString("Arial"), 64);

	this->SetPitch(65.0f * maConstants::c_fAngleToRad);
	this->SetYaw(212.0f * maConstants::c_fAngleToRad);
	this->SetRadius(60.0f);

	//	make fragments
	m_RectFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(100.0f, 100.0f, 10, 10);
	m_CubeFragment = g3dPrimitiveFragmentUtil::CreateTexturedCube(6.0f);
	m_TextBlockFragment = g3dPrimitiveFragmentUtil::CreateTexturedBlock(20.0f, 10.0f, 10.0f);
	m_UVABlockFragment = g3dPrimitiveFragmentUtil::CreateTexturedBlock(10.0f, 20.0f, 10.0f);

	//	setup fragment materials
	m_GroundMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_CubeMat1.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 0.5f));
	m_CubeMat2.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_TextBlockMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_UVABlockMat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_UVABlockMat.SetAmbient(maFloatRGBA(5.0f, 5.0f, 5.0f, 5.0f));

	//	add some textures
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("base.png"));
	m_GroundBaseTexture = matTextureManager::LoadTexture(locator);
	m_GroundMat.AddTextureTop(m_GroundBaseTexture);

	locator.Pop();
	locator.Push(itString("detail.png"));
	m_GroundDetailTexture = matTextureManager::LoadTexture(locator);
	m_GroundMat.AddTextureTop(m_GroundDetailTexture, matMaterial::e_TexLayerAlphaBlend);

	m_GroundMat.SetScale(maVector2d(5.0f, 5.0f), 1);
	m_GroundMat.SetRotation(1.0f, 1);

	locator.Pop();
	locator.Push(itString("green003.png"));
	m_CubeTexture1 = matTextureManager::LoadTexture(locator);
	m_CubeMat1.AddTextureTop(m_CubeTexture1);
	m_CubeMat1.SetScale(maVector2d(0.3f, 0.3f), 0);

	locator.Pop();
	locator.Push(itString("lblue013.png"));
	m_CubeTexture2 = matTextureManager::LoadTexture(locator);
	m_CubeMat2.AddTextureTop(m_CubeTexture2);
	m_CubeMat2.SetScale(maVector2d(0.3f, 0.3f), 0);

	m_TextBlockTexture = matTextureManager::CreatePlainTextureFromText(	m_TextureFont,
																	itString("Text!"),
																	g2dRGBColor(0x30, 0x50, 0xf0),
																	128,
																	64);
	//	First we'll create a uva texture and save it
	//
	const int num_frames = 32;
	const int num_height_frames = 2;
	const int num_width_frames = 2;
	const float frame_rate = 24.0f;
	const int num_pages = 8;

	{
		m_UVABlockTexture = matTextureManager::CreateUVATexture();
		itString texture_filenames[8];

		char tex_name[16];
		strcpy(tex_name, "0jxp201.png");

		int i;
		for( i = 0 ; i < num_pages ; i++ )
		{
			tex_name[6] = '1' + i;
			locator.Pop();
			locator.Push(tex_name);
			texture_filenames[i] = locator.GetLastName();
			m_UVABlockTextures.push_back(matTextureManager::LoadTexture(locator));
			m_UVABlockTexture->AddTexturePage(m_UVABlockTextures[i]);
		}

		m_UVABlockTexture->SetNumFrames(num_frames);
		m_UVABlockTexture->SetNumHeightFrames(num_height_frames);
		m_UVABlockTexture->SetNumWidthFrames(num_width_frames);
		m_UVABlockTexture->SetFrameRate(frame_rate);

//		fsLocator uva_file;
		fsLocator uva_file = gfPaths::GetGamePath(gfPaths::e_ExePath);
		uva_file.Push("data");
		uva_file.Push("g3dtest.tuv");

		if( fsFileUtil::FileExists(uva_file) )
			fsFileUtil::DeleteFile(uva_file);

		fsFileUtil::CreateFile(uva_file);

		matTextureManager::WriteUVATexture(*m_UVABlockTexture, uva_file, texture_filenames);
	}

	//	now we'll destroy it and load it back in again...it should be the same
	//
	{
		int i;
		for( i = 0 ; i < num_pages ; i++ )
			matTextureManager::ReleaseTexture(m_UVABlockTexture->GetPage(i));

		matTextureManager::ReleaseTexture(m_UVABlockTexture);
		m_UVABlockTexture = NULL;
	}

	{
//		fsLocator texture_dir;
//		fsLocator uva_file;
		fsLocator texture_dir = gfPaths::GetGamePath(gfPaths::e_ExePath);
		fsLocator uva_file = gfPaths::GetGamePath(gfPaths::e_ExePath);
		uva_file.Push("data");
		uva_file.Push("g3dtest.tuv");

		m_UVABlockTexture = dynamic_cast<matUVATexture*>(matTextureManager::LoadTexture(uva_file));

		DBG_ASSERT0( m_UVABlockTexture->GetNumFrames() == num_frames, "Failed to save UVA correctly");
		DBG_ASSERT0( m_UVABlockTexture->GetNumWidthFrames() == num_width_frames, "Failed to save UVA correctly");
		DBG_ASSERT0( m_UVABlockTexture->GetNumHeightFrames() == num_height_frames, "Failed to save UVA correctly");
		DBG_ASSERT0( m_UVABlockTexture->GetFrameRate() == frame_rate, "Failed to save UVA correctly");
		DBG_ASSERT0( m_UVABlockTexture->GetNumPages() == num_pages, "Failed to save UVA correctly");
	}

	m_TextBlockMat.AddTextureTop(m_TextBlockTexture);
	m_UVABlockMat.AddTextureTop(m_UVABlockTexture);

	//	set the materials for the fragments
	m_RectFragment->SetMaterial(&m_GroundMat);
	m_CubeFragment->SetMaterial(&m_CubeMat1);
	m_TextBlockFragment->SetMaterial(&m_TextBlockMat);
	m_UVABlockFragment->SetMaterial(&m_UVABlockMat);

	//g3dFragmentManager::SetMaterial(m_RectFragment, &m_GroundMat, 0);
	//g3dFragmentManager::SetMaterial(m_CubeFragment, &m_CubeMat1, 0);
	//g3dFragmentManager::SetMaterial(m_CubeFragment, &m_CubeMat2, 1);
	//g3dFragmentManager::SetMaterialChange(m_CubeFragment, 0, 6);
	//g3dFragmentManager::SetMaterial(m_TextBlockFragment, &m_TextBlockMat, 0);
	//g3dFragmentManager::SetMaterial(m_UVABlockFragment, &m_UVABlockMat, 0);

	//	make models
	m_Ground = new g3dSceneNode();
	m_Ground->SetFragment(m_RectFragment);
	m_Ground->GetTransform().MakeRotateX( -maConstants::c_fPI_Div_2 );
	m_Root->AddChild(m_Ground);

	int i;
	for( i = 0 ; i < 10 ; i++ )
	{
		m_Cubes.push_back(new g3dSceneNode());
		m_Cubes[i]->SetFragment(m_CubeFragment);
		m_Root->AddChild(m_Cubes[i]);
	}

	m_TextBlock = new g3dSceneNode();
	m_TextBlock->SetFragment(m_TextBlockFragment);
	m_TextBlock->GetTransform().MakeTranslate(0, 15, 0);
	m_Root->AddChild(m_TextBlock);

	m_UVABlock = new g3dSceneNode();
	m_UVABlock->SetFragment(m_UVABlockFragment);
	m_UVABlock->GetTransform().MakeTranslate(10, 15, 20);
	m_Root->AddChild(m_UVABlock);

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
void demG3dTestTextures::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	draw some text stuff
/*	const int num_text_lines = 9;
	itString text[num_text_lines];
	text[0] = itString("This slide shows some texturing features of the Terawatt engine.");
	text[1] = itString("The ground plane shows a multitexture material, with a ground texture that looks like dirt and a detail texture");
	text[2] = itString("that looks like grass.  The detail texture is scaled smaller and rotated.  This technique reduces the appearance of");
	text[3] = itString("tiling while still allowing a sense of fine detail.  Many variations are possible.  The Terawatt engine allows layering");
	text[4] = itString("of an unlimited number of textures, though hardware limitations make using more than 2 or 3 impractical.");
	text[5] = itString("The cubes are drawn with multiple materials per polyset.  The green texture is a non-alpha texture but it is given ");
	text[6] = itString("a translucent diffuse alpha so the result is translucent.  Although the cubes are all separate models, they re-use the same vertex data.");
	text[7] = itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)");
	text[8] = itString("Press space to go to the next section");
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
	//	move the cubes around
	float dtheta = maConstants::c_fAngleToRad * 90.0f;
	float dphi = maConstants::c_fAngleToRad * 5.0f;

	int num_cubes = m_Cubes.size();
	int cube_num;
	for( cube_num = 0 ; cube_num < num_cubes ; cube_num++ )
	{
		float x, y, z;

		z = cube_num * 8.0f - 20.0f;
		x = 3.0f * sin(frame_time * dtheta + z * dphi);
		y = 4.0f + cos(frame_time * dtheta + z * dphi);

		g3dSceneNode* cube = m_Cubes[cube_num];
		cube->GetTransform().RotateBy(cube_num * 3.0f * frame_time, maVector3d(0, 1, 0));
		cube->GetTransform().MakeTranslate(x, y, z);
	}

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestTextures::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Ground);
	//g3dRenderer::RemoveModel(m_TextBlock);
	//g3dRenderer::RemoveModel(m_UVABlock);

	//std::for_each(m_Cubes.begin(), m_Cubes.end(), g3dRenderer::RemoveModel);

	//	cleanup fragments
	delete m_RectFragment;
	delete m_CubeFragment;
	delete m_TextBlockFragment;

	//	release textures
	matTextureManager::ReleaseTexture(m_GroundBaseTexture);
	matTextureManager::ReleaseTexture(m_GroundDetailTexture);
	matTextureManager::ReleaseTexture(m_CubeTexture1);
	matTextureManager::ReleaseTexture(m_CubeTexture2);
	matTextureManager::ReleaseTexture(m_TextBlockTexture);

	matTextureManager::ReleaseTexture(m_UVABlockTexture);

	//std::for_each(m_UVABlockTextures.begin(), m_UVABlockTextures.end(), matTextureManager::ReleaseTexture);

	g2dFontUtil::ReleaseFont(m_TextureFont);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
