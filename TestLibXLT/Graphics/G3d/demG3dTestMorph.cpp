/*****************************************************************************
**  demG3dTestMorph.hpp
**
**		This mode displays a demonstration/test of the fragment morphing
**	functionality of the Terawatt G3d package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestMorph.hpp"

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightManager.hpp"
#include "matMatAnim.hpp"
#include "g3dLayer.hpp"
#include "matPlainTexture.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "matTextureManager.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

namespace
{

const int l_WidthSections = 40;
const int l_HeightSections = 40;
const float l_Width = 140.0f;
const float l_Height = 140.0f;

}

//====================================================================
//====================================================================
demG3dTestMorph::demG3dTestMorph(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestMorph::~demG3dTestMorph()
{
}

//====================================================================
//====================================================================
void demG3dTestMorph::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	m_Scene = new g3dScene(new g3dLayer(m_Root)); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	this->SetPitch(50.0f * maConstants::c_fAngleToRad);
	this->SetYaw(280.0f * maConstants::c_fAngleToRad);
	this->SetRadius(50.0f);

	//	Make the morphable fragment.  It will be a plane with one corner at the origin
	//	(we will center it later).
	m_Vertices.resize( (l_WidthSections+1) * (l_HeightSections+1) );
	m_Normals.resize(m_Vertices.size());
	std::vector<maPoint2d> texture_coords(m_Vertices.size());

	float width_delta = l_Width / float(l_WidthSections);
	float height_delta = l_Height / float(l_HeightSections);

	float cur_height = 0;

	int width_num, height_num;
	int num_width_divisions = l_WidthSections+1;
	int num_height_divisions = l_HeightSections+1;
	int cur_vertex_index = 0;

	for( height_num = 0 ; height_num < num_height_divisions ; height_num++ )
	{
		float cur_width = 0;
		for( width_num = 0 ; width_num < num_width_divisions ; width_num++ )
		{
			m_Vertices[cur_vertex_index].Set(cur_width, 0, cur_height);
			m_Normals[cur_vertex_index].Set(0, 1, 0);
			texture_coords[cur_vertex_index].Set(	float(width_num) / float(l_WidthSections),
													1.0f - float(height_num) / float(l_HeightSections));
			cur_width += width_delta;
			cur_vertex_index++;

		}

		cur_height += height_delta;
	}

	std::vector<unsigned short> indices(l_WidthSections * l_HeightSections * 6);

	int cur_index_index = 0;
	for( height_num = 0 ; height_num < l_HeightSections ; height_num++ )
	{
		for( width_num = 0 ; width_num < l_WidthSections ; width_num++ )
		{
			int base_vertex_num = (height_num * num_width_divisions + width_num);
			indices[cur_index_index+5] = base_vertex_num;
			indices[cur_index_index+4] = base_vertex_num + 1;
			indices[cur_index_index+3] = base_vertex_num + num_width_divisions;
			indices[cur_index_index+2] = base_vertex_num + num_width_divisions;
			indices[cur_index_index+1] = base_vertex_num + 1;
			indices[cur_index_index+0] = base_vertex_num + num_width_divisions + 1;
			cur_index_index += 6;
		}
	}

	matMaterial* material[1];
	material[0] = &m_RectMat;
	m_RectFragment = g3dFragmentManager::Create(	&(m_Vertices[0]),
													&(m_Normals[0]),
													&(texture_coords[0]),
													&(texture_coords[0]),
													m_Vertices.size(),
													&(indices[0]),
													indices.size(),
													material,
													NULL,
													0,
													true);

	//	setup fragment materials
	m_RectMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_RectMat.SetHasSpecular(true);
	m_RectMat.SetSpecularPower(150);
	m_RectMat.SetSpecular(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	//	add some textures
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("water006.png"));
	m_RectTexture1 = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture1);
	locator.Pop();
	locator.Push(itString("water005.png"));
	m_RectTexture2 = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture2, matMaterial::e_TexLayerAlphaBlend);
	m_RectMat.SetScale(maVector2d(3, 3), 0);
	m_RectMat.SetScale(maVector2d(4.0, 4.0), 1);
	m_RectMat.SetRotation(2.0f, 0);
	m_RectMat.SetRotation(1.0f, 1);


	//	set the materials for the fragments
	m_RectFragment->SetMaterial(&m_RectMat);

	//	make models

	g3dHFragment hfrag(m_RectFragment);
	m_Rect = new g3dSceneNode(hfrag, demG3dLayerSpec::e_World);
	m_Rect->SetRenderable(true);
	m_Rect->GetTransform().MakeTranslate( -l_Width * 0.5f, 0, -l_Height * 0.5f );

	//	add some material animations
	float start_time = appTime::GetTime();

	an2StateAnimation<maVector2d>* texture_pos_anim1 = new an2StateAnimation<maVector2d>(maVector2d(0, 0), maVector2d(4, 0), 180.0f);
	texture_pos_anim1->SetLooping(true);
	m_TexturePosAnim1 = new matMatAnim(	texture_pos_anim1,
										matMatParamIndex::e_TextureTranslation1,
										start_time);
	//m_Rect->GetAnimList().push_back(g3dMatAnimInfo(m_TexturePosAnim1->CreateInstance(start_time), 0));
	m_RectMat.AddMatAnim( m_TexturePosAnim1 );

	an2StateAnimation<maVector2d>* texture_pos_anim2 = new an2StateAnimation<maVector2d>(maVector2d(0, 0), maVector2d(3, 0), 112.0f);
	texture_pos_anim2->SetLooping(true);
	m_TexturePosAnim2 = new matMatAnim(	texture_pos_anim2,
										matMatParamIndex::e_TextureTranslation0,
										start_time);
//	m_Rect->GetAnimList().push_back(g3dMatAnimInfo(m_TexturePosAnim2->CreateInstance(start_time), 0));
	m_RectMat.AddMatAnim( m_TexturePosAnim2 );

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
void demG3dTestMorph::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	draw some text stuff
/*	const int num_text_lines = 6;
	itString text[num_text_lines];
	text[0] = itString("This slide shows the fragment morphing capability of the Terawatt engine.");
	text[1] = itString("Fragments in Terawatt can be either morphable or not.  Morphable fragments can be");
	text[2] = itString("slower in some situations and should only be used when necessary - for applications like this");
	text[3] = itString("water simulation or joint animation systems, for instance.");
	text[4] = itString("('i', 'j', 'k', 'm', '-', and '=' move the camera)");
	text[5] = itString("Press space to end the demo");
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
	//	morph the fragment
	int width_num, height_num;
	int num_width_divisions = l_WidthSections+1;
	int num_height_divisions = l_HeightSections+1;
	int cur_vertex_index = 0;

	float width_delta = l_Width / float(l_WidthSections);
	float height_delta = l_Height / float(l_HeightSections);

	float time_vel = maConstants::c_fAngleToRad * 30.0f;
	float x_vel = maConstants::c_fAngleToRad * 5.0f;
	float z_vel = maConstants::c_fAngleToRad * 5.0f;

	float cur_height = 0;
	for( height_num = 0 ; height_num < num_height_divisions ; height_num++ )
	{
		float cur_width = 0;
		for( width_num = 0 ; width_num < num_width_divisions ; width_num++ )
		{
			m_Vertices[cur_vertex_index].m_Y = 2.0f * cos(frame_time * time_vel + cur_width * x_vel) + 1.5f * sin(frame_time * time_vel + cur_height * z_vel);
			cur_width += width_delta;
			cur_vertex_index++;
		}

		cur_height += height_delta;
	}

	g3dFragmentManager::ModifyVertices( m_RectFragment,
										0,
										g3dFragmentManager::GetNumVertices( m_RectFragment ),
										&(m_Vertices[0]),
										&(m_Normals[0]) );

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestMorph::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
	//g3dRenderer::RemoveModel(m_Rect);

	//	cleanup fragments
	delete m_RectFragment;

	//	release textures
	matTextureManager::ReleaseTexture(m_RectTexture1);
	matTextureManager::ReleaseTexture(m_RectTexture2);

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
