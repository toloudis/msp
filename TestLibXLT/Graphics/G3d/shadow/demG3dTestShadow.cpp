/*****************************************************************************
**  demG3dTestShadow.cpp
**
**		This mode displays a demonstration/test of the stencil buffer
**	shadows.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestShadow.hpp"

#include "appCharEvent.hpp"
#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsLocator.hpp"
#include "g2dRGBColor.hpp"
#include "g2dScreen.hpp"
#include "g2dScreenDrawUtil.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dDynamicModel.hpp"
#include "g3dFragmentManager.hpp"
#include "g3dLightManager.hpp"
#include "g3dPackage.hpp"
#include "matPlainTexture.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dRenderer.hpp"
#include "g3dStaticModel.hpp"
#include "g3dStaticProgModel.hpp"
#include "matTextureManager.hpp"
#include "g3dWorldStaticModel.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

namespace
{

const maFloatRGBA l_Light1Color(1, 0, 0, 1);
const maFloatRGBA l_Light2Color(0, 0, 1, 1);

int l_CurLevel = 0;

const int l_NumLevels = 7;
const int l_WidthSections = 128;	// 2^6
const int l_HeightSections = 64;
const float l_Width = 140.0f;
const float l_Height = 140.0f;

}

//====================================================================
//====================================================================
demG3dTestShadow::demG3dTestShadow()
{
}

//====================================================================
//====================================================================
demG3dTestShadow::~demG3dTestShadow()
{
}

//====================================================================
//====================================================================
void demG3dTestShadow::Initialize()
{
	demG3dTestMode::Initialize();

	//	Make the morphable fragment.  It will be a plane with one corner at the origin
	//	(we will center it later).
	std::vector<maPoint3d> vertices;
	std::vector<maVector3d> normals;
	vertices.resize( (l_WidthSections+1) * (l_HeightSections+1) );
	normals.resize(vertices.size());
	std::vector<maPoint2d> texture_coords(vertices.size());

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
			const float xmul = 5.0f;
			const float zmul = 5.0f;
			float y = xmul * cos(cur_width * x_vel) + zmul * sin(cur_height * z_vel);
			vertices[cur_vertex_index].Set(cur_width, y, cur_height);
			
			float xn, yn, zn;
			xn = -xmul * x_vel * -sin(cur_width * x_vel);
			zn = -zmul * z_vel * cos(cur_height * z_vel);
			yn = 1.0f;
			normals[cur_vertex_index].Set(xn, yn, zn);
			normals[cur_vertex_index].Normalize();
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

	m_RectFragment = g3dFragmentManager::CreateCompoundFragment(&(vertices[0]),
																&(normals[0]),
																&(texture_coords[0]),
																vertices.size(),
																meshes);

	//	make fragments
	m_SphereFragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f, 50, 50);
	m_LightVis1Fragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(0.5f, 5, 5);
	m_LightVis2Fragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(0.5f, 5, 5);
	m_AntiRectFragment = g3dPrimitiveFragmentUtil::CreateRectangle(10000.0f, 10000.0f, 2, 2);

	//	setup fragment materials
	m_Mat1.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Mat2.SetDiffuse(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	m_Light1Mat.SetEmissive(l_Light1Color);
	m_Light2Mat.SetEmissive(l_Light2Color);
	m_Light1Mat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_Light2Mat.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_Light1Mat.SetAmbient(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
	m_Light2Mat.SetAmbient(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));

	g3dFragmentManager::SetMaterial(m_LightVis1Fragment, &m_Light1Mat, 0);
	g3dFragmentManager::SetMaterial(m_LightVis2Fragment, &m_Light2Mat, 0);

	// make a texture
	fsLocator locator = gfPaths::GetGamePath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push("lblue0132.png");
	m_Texture1 = matTextureManager::LoadTexture(locator);
	locator.Pop();
	locator.Push("lgrey020.png");
	m_Texture2 = matTextureManager::LoadTexture(locator);

	m_Mat1.AddTextureTop(m_Texture1);
	m_Mat2.AddTextureTop(m_Texture2);
	m_RectMat.AddTextureTop(m_Texture1);

	//	make models
	maMatrix4x4 world_mat;
	world_mat.MakeTranslate(7, 15, 7);
	m_Sphere1 = g3dRenderer::AddStaticModel(m_SphereFragment, world_mat, g3dPackage::e_World);
	m_Sphere1->SetRenderable(true);
	m_Sphere1->SetCastsShadow(true);
	m_Sphere1->SetMaterial(0, &m_Mat1);

	world_mat.MakeTranslate(-7, 15, 7);
	m_Sphere2 = g3dRenderer::AddStaticModel(m_SphereFragment, world_mat, g3dPackage::e_World);
	m_Sphere2->SetRenderable(true);
	m_Sphere2->SetCastsShadow(true);
	m_Sphere2->SetMaterial(0, &m_Mat2);

	g3dHFragment hfrag1(m_LightVis1Fragment);
	m_LightVis1 = g3dRenderer::AddDynamicModel(hfrag1, g3dPackage::e_World);
	m_LightVis1->SetRenderable(true);
	
	g3dHFragment hfrag2(m_LightVis2Fragment);
	m_LightVis2 = g3dRenderer::AddDynamicModel(hfrag2, g3dPackage::e_World);
	m_LightVis2->SetRenderable(true);

	world_mat.MakeTranslate( -l_Width * 0.5f, 0, -l_Height * 0.5f );
	m_Rect = g3dRenderer::AddStaticProgModel(m_RectFragment, world_mat, g3dPackage::e_World);
//	m_Rect->SetCastsShadow(true);
	m_Rect->SetRenderable(true);

	world_mat.MakeRotateX(maConstants::c_fPI_Div_2);
	world_mat.TranslateBy(0, -26.01f, 0);
	m_AntiRect = g3dRenderer::AddStaticModel(m_AntiRectFragment, world_mat, g3dPackage::e_World);
	m_AntiRect->SetCastsShadow(true);
	m_AntiRect->SetRenderable(true);
	m_AntiRect->SetRenderable(false);

	//	set lights
	m_Light1 = g3dLightManager::CreatePointLight();
	m_Light2 = g3dLightManager::CreatePointLight();

	m_Light1->SetIntensity(l_Light1Color);
	m_Light2->SetIntensity(l_Light2Color);

	m_Light1->Enable();
	m_Light2->Enable();

	m_Light1->SetCastsShadow(true);
	m_Light2->SetCastsShadow(true);

	g3dLightManager::SetAmbient(maFloatRGBA(0.3f, 0.3f, 0.3f, 1.0f));
}

//====================================================================
//	Think
//====================================================================
void demG3dTestShadow::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();

	//	flip
	g2dScreen::EndScene();

	g2dScreenDrawUtil::Clear(g2dRGBColor(0x30, 0x80, 0xa0));

	//	move lights
	maPoint3d pos;
	pos.Set(5.0f * cos(frame_time * 0.5f), 25.0f - 5.0f * sin(frame_time * 1.0f), -10.0f + 5.0f * sin(frame_time * 0.5f));
	m_Light1->SetPosition(pos);
	m_LightVis1->GetHead()->GetTransform().MakeTranslate(pos.m_X, pos.m_Y, pos.m_Z);

	pos.Set(10 + 15.0f * cos(frame_time * 0.3f), 23.0f, 15.0f * sin(frame_time * 0.3f));
	m_Light2->SetPosition(pos);
	m_LightVis2->GetHead()->GetTransform().MakeTranslate(pos.m_X, pos.m_Y, pos.m_Z);

	//	adjust rect detail
	m_Rect->SetMeshIndex(l_CurLevel);

	//	render
	g3dRenderer::RenderModels(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestShadow::DeInitialize()
{
	//	cleanup lights
	g3dLightManager::DestroyLight(m_Light1);
	g3dLightManager::DestroyLight(m_Light2);

	//	cleanup models
//	g3dRenderer::RemoveModel(m_Sphere1);
//	g3dRenderer::RemoveModel(m_Sphere2);
	g3dRenderer::RemoveModel(m_Rect);
	g3dRenderer::RemoveModel(m_AntiRect);
	g3dRenderer::RemoveModel(m_LightVis1);
	g3dRenderer::RemoveModel(m_LightVis2);

	//	cleanup fragments
	g3dFragmentManager::DestroyAllFragments();
	
	//	release textures
	matTextureManager::ReleaseTexture(m_Texture1);
	matTextureManager::ReleaseTexture(m_Texture2);

	demG3dTestMode::DeInitialize();
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestShadow::ReceiveCharEvent(appCharEvent& i_Event)
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
