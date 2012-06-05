/*****************************************************************************
**  demG3dTestProgressive.hpp
**
**		This mode displays a demonstration/test of the progressive mesh.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestProgressive.hpp"

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "appCharEvent.hpp"
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
#include "g3dStaticProgModel.hpp"
#include "matTextureManager.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

namespace
{

int l_CurLevel = 0;

const int l_NumLevels = 7;
const int l_WidthSections = 128;	// 2^6
const int l_HeightSections = 64;
const float l_Width = 140.0f;
const float l_Height = 140.0f;

}

//====================================================================
//====================================================================
demG3dTestProgressive::demG3dTestProgressive(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//====================================================================
//====================================================================
demG3dTestProgressive::~demG3dTestProgressive()
{
}

//====================================================================
//====================================================================
void demG3dTestProgressive::Initialize()
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

	float time_vel = maConstants::c_fAngleToRad * 30.0f;
	float x_vel = maConstants::c_fAngleToRad * 5.0f;
	float z_vel = maConstants::c_fAngleToRad * 5.0f;

	for( height_num = 0 ; height_num < num_height_divisions ; height_num++ )
	{
		float cur_width = 0;
		for( width_num = 0 ; width_num < num_width_divisions ; width_num++ )
		{
			float y = 12.0f * cos(cur_width * x_vel) + 7.5f * sin(cur_height * z_vel);
			m_Vertices[cur_vertex_index].Set(cur_width, y, cur_height);
			m_Normals[cur_vertex_index].Set(0, 1, 0);
			texture_coords[cur_vertex_index].Set(	float(width_num) / float(l_WidthSections),
													1.0f - float(height_num) / float(l_HeightSections));
			cur_width += width_delta;
			cur_vertex_index++;

		}

		cur_height += height_delta;
	}

	std::vector<g3dMeshInfo> meshes(l_NumLevels);
	int i;
	for( i = 0 ; i < l_NumLevels ; ++i )
	{
		int cur_level = i;
		int level_offset = 0x01 << cur_level;

		int num_width_sections = l_WidthSections >> cur_level;
		int num_height_sections = l_HeightSections >> cur_level;

		std::vector<unsigned short>& indices = meshes[i].m_Indices;
		indices.resize(num_width_sections * num_height_sections * 6);

		int cur_index_index = 0;
		for( height_num = 0 ; height_num < num_height_sections ; height_num++ )
		{
			for( width_num = 0 ; width_num < num_width_sections ; width_num++ )
			{
				int base_vertex_num = (height_num * num_width_divisions * level_offset + width_num * level_offset);
				indices[cur_index_index+5] = base_vertex_num;
				indices[cur_index_index+4] = base_vertex_num + level_offset;
				indices[cur_index_index+3] = base_vertex_num + num_width_divisions * level_offset;
				indices[cur_index_index+2] = base_vertex_num + num_width_divisions * level_offset;
				indices[cur_index_index+1] = base_vertex_num + level_offset;
				indices[cur_index_index+0] = base_vertex_num + num_width_divisions * level_offset + level_offset;
				cur_index_index += 6;
			}

		}

		meshes[i].m_Materials.push_back(&m_RectMat);
	}

	m_RectFragment = g3dFragmentManager::CreateCompoundFragment(&(m_Vertices[0]),
																&(m_Normals[0]),
																&(texture_coords[0]),
																m_Vertices.size(),
																meshes);

	//	setup fragment materials
	m_RectMat.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	//	add some textures
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("lgrey020.png"));
	m_RectTexture1 = matTextureManager::LoadTexture(locator);
	m_RectMat.AddTextureTop(m_RectTexture1);

	//	make models
	maMatrix4x4 matrix;
	matrix.MakeTranslate( -l_Width * 0.5f, 0, -l_Height * 0.5f );
	m_Rect = g3dRenderer::AddStaticProgModel(m_RectFragment, matrix, demG3dLayerSpec::e_World);
	m_Rect->SetRenderable(true);

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
void demG3dTestProgressive::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	draw some text stuff
	char text[256];
	sprintf(text, "%d / %d", l_CurLevel, l_NumLevels);
	std::vector<itString> text_strings;
	text_strings.push_back(itString(text));
	text_strings.push_back(itString("[, ] change level"));

	g2dRGBColor text_color(0x50, 0x50, 0x90);

	int i;
	for( i = 0 ; i < text_strings.size() ; i++ )
	{
		g2dScreenDrawUtil::DrawText(	10,
										(i * 17) + 10,
										this->GetFont(),
										text_strings[i],
										text_color);

		text_color.SetRed(text_color.GetRed() + 10);
	}


	m_Rect->SetMeshIndex(l_CurLevel);

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestProgressive::DeInitialize()
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

	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestProgressive::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType(']'):
			l_CurLevel++;
			if( l_CurLevel >= (l_NumLevels - 1) )
				l_CurLevel = l_NumLevels - 1;
		break;

		case itString::CharType('['):
			l_CurLevel--;
			if( l_CurLevel < 0 )
				l_CurLevel = 0;
		break;

		default:
			this->demG3dTestMode::ReceiveCharEvent(i_Event);
		break;
	}
}
