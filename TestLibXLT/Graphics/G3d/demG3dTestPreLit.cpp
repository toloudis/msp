/*****************************************************************************
**  demG3dTestPreLit.hpp
**
**		This mode displays a demonstration/test of prelit textured and
**		non-textured fragments
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestPreLit.hpp"

#include "Core/app/appTime.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX9/tmesh/tmeshPreLitFrag.hpp"

#include <algorithm>

namespace
{
	unsigned int l_DiffuseColors[3];
	int l_ColorIndices[3] = { 0, 1, 2 };

	//====================================================================
	//====================================================================
	unsigned int convert_color(	int i_Red, int i_Green, int i_Blue, int i_Alpha )
	{
		unsigned int color;
		color =     ( i_Alpha << 24 ) 
				  |	( i_Red << 16 ) 
				  | ( i_Green << 8 )
				  | ( i_Blue );

		return color;
	}
}

//====================================================================
//====================================================================
demG3dTestPreLit::demG3dTestPreLit(g3dViewer &i_Viewer)
:	m_Viewer(i_Viewer), m_Root(NULL), m_Scene(NULL),
	m_Fragment1( 0 ),
	m_Fragment2( 0 )
{
}

//====================================================================
//====================================================================
demG3dTestPreLit::~demG3dTestPreLit()
{
}

//====================================================================
//====================================================================
void demG3dTestPreLit::Initialize()
{
	demG3dTestMode::Initialize();

	m_Root = new g3dSceneNode();
	bool prelit = true;
	g3dLayer *prelit_layer = new g3dLayer(m_Root, g3dLayer::e_ZBuffer,
		g3dLayer::e_World, g3dLayer::e_Multiplicative, true, false, prelit);
	m_Scene = new g3dScene(prelit_layer); // scene owns layer

	m_Viewer.SetScene(m_Scene);
	m_Viewer.SetCamera(&Camera());

	// set up display info
	m_Viewer.SetTextMessage(0, itString("Prelit geometry."));

	// Vertices
	maPoint3d vertices[4];
	vertices[0].Set(  -1.0f,  0.5f, 0.0f );
	vertices[1].Set( -1.58f, -0.5f, 0.0f );
	vertices[2].Set( -0.58f, -0.5f, 0.0f );

	// Normals
	maPoint3d normals[4];
	normals[0].Set( 0, 0, 1 );
	normals[1].Set( 0, 0, 1 );
	normals[2].Set( 0, 0, 1 );

	// Normals
	maPoint2d texture_uvs[4];
	texture_uvs[0].Set( 0, 0 );
	texture_uvs[1].Set( 1, 0 );
	texture_uvs[2].Set( 1, 1 );

	// Indices
	unsigned short indices[3];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// Diffuse Colors
	l_DiffuseColors[0] = convert_color( 255, 0, 0, 255 );
	l_DiffuseColors[1] = convert_color( 0, 255, 0, 255 );
	l_DiffuseColors[2] = convert_color( 0, 0, 255, 255 );

	// Specular Colors
	maFloatRGBA specular_colors[4];
	specular_colors[0].Set( 0, 0, 0, 1 );
	specular_colors[1].Set( 0, 0, 0, 1 );
	specular_colors[2].Set( 0, 0, 0, 1 );

	effPhongData* pData = NULL;
	// Set up material
	pData = dynamic_cast<effPhongData*>(m_Material1.GetEffectData());
	pData->m_ColorAmbient = maFloatRGBA( 0, 0, 0, 1 );
	pData->m_ColorDiffuse = maFloatRGBA( 0.0f, 0.0f, 0.0f, 1 );
	pData->m_ColorEmissive = maFloatRGBA( 0, 0, 0, 1 );
	m_Material1.SetHasSpecular( false );
	matMaterial* pMaterial = &m_Material1;

	// Create the fragment
	m_Fragment1 = new tmeshPreLitFrag( vertices,
					l_DiffuseColors, &(texture_uvs[0]),	 3,
					indices, 3, pMaterial, false );

	// Create a scene node for it
	g3dSceneNode *model1 = new g3dSceneNode();
	model1->SetFragment(m_Fragment1);
	m_Root->AddChild(model1);

	vertices[0].Set(  1.0f,  0.5f, 0.0f );
	vertices[1].Set( 0.58f, -0.5f, 0.0f );
	vertices[2].Set(  1.58f, -0.5f, 0.0f );

	// Set up material
	pData = dynamic_cast<effPhongData*>(m_Material2.GetEffectData());
	pData->m_ColorAmbient = maFloatRGBA( 0, 0, 0, 1 );
	pData->m_ColorDiffuse = maFloatRGBA( 0.0f, 0.0f, 0.0f, 1 );
	pData->m_ColorEmissive = maFloatRGBA( 0, 0, 0, 1 );
	m_Material2.SetHasSpecular( false );
	pMaterial = &m_Material2;

	//	add some textures
	fsLocator locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("data");
	locator.Push(itString("green003.png"));
	m_Texture = matTextureMgr::LoadTexture(locator);
	m_Material2.AddTextureTop(m_Texture);

	// Create the fragment
	m_Fragment2 = new tmeshPreLitFrag( vertices,
											 l_DiffuseColors,
											 &(texture_uvs[0]),
											 3,
											 indices,
											 3,
											 pMaterial,
											 false );

	// Create a scene node for it
	g3dSceneNode *model2 = new g3dSceneNode();
	model2->SetFragment(m_Fragment2);
	m_Root->AddChild(model2);

	// Set camera
	SetRadius( 3 );
	SetPitch( 90.0f * maConstants::c_fAngleToRad );
	SetYaw( 0.0f * maConstants::c_fAngleToRad );

	// Set lights
	m_pLight = g3dLightMgr::CreateDirectionalLight();
	m_pLight->SetDirection( maVector3d( 0, 0, -1.0f ) );
	m_pLight->SetIntensity( maFloatRGBA( 1, 1, 1, 1 ) );
	m_pLight->Enable();

	g3dLightMgr::SetAmbient( maFloatRGBA( 0.25f, 0.25f, 0.25f, 0.25f ));

}

//====================================================================
//	Think
//====================================================================
void demG3dTestPreLit::Think()
{
	demG3dTestMode::Think();

	float frame_time = appTime::GetTime();
	m_Scene->UpdateWorldData();
	m_Viewer.Render(frame_time);
	m_Viewer.Present();

	if( this->GetQuitSignaled() )
		this->SetTerminateCondition( demMode::e_TerminateAndDestroy );
}

//====================================================================
//====================================================================
void demG3dTestPreLit::DeInitialize()
{
	//	cleanup lights
	g3dLightMgr::DestroyLight(m_pLight);

	//	cleanup fragments
	delete m_Fragment1;
	delete m_Fragment2;

	//	release textures
	matTextureMgr::ReleaseTexture(m_Texture);

	m_Viewer.ClearTextMessages();
	delete m_Root;
	delete m_Scene;

	demG3dTestMode::DeInitialize();
}
