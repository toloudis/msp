/*****************************************************************************
**  demMatTestTextureCompression.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "demMatTestTextureCompression.hpp"

#include "appTime.hpp"
#include "demModeManager.hpp"
#include "fsFileUtil.hpp"
#include "g2dFontUtil.hpp"
#include "g2dRGBColor.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"
#include "g3dViewer.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dLightMgr.hpp"
#include "g3dLayer.hpp"
#include "g3dPointLight.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "matTextureMgr.hpp"
#include "matTextureMgrD3D.hpp"
#include "matUVATexture.hpp"
#include "maConstants.hpp"
#include "gfPaths.hpp"

#include <algorithm>


//============================================================================
//============================================================================
const int c_TEXTURES = 7;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
demMatTestTextureCompression::demMatTestTextureCompression(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
demMatTestTextureCompression::~demMatTestTextureCompression()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void demMatTestTextureCompression::SetUpTexture( int i_Index, fsLocator& i_Loc )
{
	m_TData[i_Index].m_pTexture = matTextureMgrD3D::LoadCompressedTexture(i_Loc);
	m_TData[i_Index].m_Material.AddTextureTop( m_TData[i_Index].m_pTexture );
	m_TData[i_Index].m_Material.SetRotation( maConstants::c_fPI, 0 );
	m_TData[i_Index].m_pFragment->SetMaterial( &(m_TData[i_Index].m_Material) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void demMatTestTextureCompression::Initialize()
{
	demMatTestMode::Initialize();

	m_Root = new g3dSceneNode();
	g3dLayer *screen_layer = new g3dLayer(m_Root, g3dLayer::e_ZSort, g3dLayer::e_Screen);
	m_Scene = new g3dScene(screen_layer); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	m_TData.resize( c_TEXTURES );

	std::vector<fsLocator> textureNames;
	textureNames.resize( c_TEXTURES );

	//	add some texture names
	//
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");

	locator.Push(itString("DXT_1na.dds"));
	textureNames[0] = locator;

	locator.Pop();
	locator.Push(itString("DXT_1a.dds"));
	textureNames[1] = locator;

	locator.Pop();
	locator.Push(itString("DXT_3na.dds"));
	textureNames[2] = locator;

	locator.Pop();
	locator.Push(itString("DXT_5na.dds"));
	textureNames[3] = locator;

	locator.Pop();
	locator.Push(itString("DXT_3a.dds"));
	textureNames[4] = locator;

	locator.Pop();
	locator.Push(itString("DXT_5a.dds"));
	textureNames[5] = locator;

	locator.Pop();
	locator.Push(itString("bonus.dds"));
	textureNames[6] = locator;

	//	make models
	//
	maMatrix4x4 world_mat;

	//	set-up the geometry and materials
	//
	int i;
	for ( i=0; i < c_TEXTURES ; i++ )
	{
		m_TData[i].m_pFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.9f, 0.9f, 1, 1);
		m_TData[i].m_Material.SetDiffuse(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
		m_TData[i].m_Material.SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));

		SetUpTexture( i, textureNames[i] );

		world_mat.MakeTranslate( (-0.20) * (3-i), (0.20) * (3-i), 0);
		m_TData[i].m_pSceneNode = new g3dSceneNode( m_TData[i].m_pFragment, world_mat );
		m_Root->AddChild( m_TData[i].m_pSceneNode );
		m_TData[i].m_pSceneNode->SetRenderable( true );
	}

	//	set lights
	//
	m_pLight1 = g3dLightMgr::CreateDirectionalLight();
	m_pLight2 = g3dLightMgr::CreateDirectionalLight();

	m_pLight1->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	m_pLight2->SetDirection(maVector3d(-0.5f, -0.8f, -0.0f));

	m_pLight1->SetIntensity(maFloatRGBA(0.5f, 0.3f, 0.2f, 1.0f));
	m_pLight2->SetIntensity(maFloatRGBA(0.0f, 0.6f, 0.6f, 1.0f));

	m_pLight1->Enable();
	m_pLight2->Enable();

	g3dLightMgr::SetAmbient(maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
}

//--------------------------------------------------------------------
//	Think
//--------------------------------------------------------------------
void demMatTestTextureCompression::Think()
{
	demMatTestMode::Think();

	float frame_time = appTime::GetTime();

	//	render
	m_Viewer.Render(frame_time);

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void demMatTestTextureCompression::DeInitialize()
{
	//	cleanup lights
	//
	g3dLightMgr::DestroyLight(m_pLight1);
	g3dLightMgr::DestroyLight(m_pLight2);

	//	cleanup fragments
	//
	int i;
	for ( i=0; i < c_TEXTURES ; i++ )
	{
		delete m_TData[i].m_pFragment;

		matTextureMgr::ReleaseTexture( m_TData[i].m_pTexture );

		//delete m_TData[i].m_pSceneNode;
	}

	delete m_Root;
	delete m_Scene;

	demMatTestMode::DeInitialize();
}
